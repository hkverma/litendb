# Start from Ubuntu so we can reuse the buildvm tooling
FROM ubuntu:22.04

# Avoid interactive prompts while installing packages
ENV DEBIAN_FRONTEND=noninteractive

SHELL ["/bin/bash", "-c"]

# Basic OS tools and common build deps; rely on buildvm.sh for the full list
COPY buildvm.sh /tmp/buildvm.sh
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        sudo \
        ca-certificates \
        curl \
        git \
        wget \
        python3 \
        python3-pip \
        python3-venv \
        ninja-build && \
    chmod +x /tmp/buildvm.sh && \
    /tmp/buildvm.sh && \
    rm -f /tmp/buildvm.sh && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Make sure locale is sane and set Liten-specific defaults
ENV LC_ALL=C.UTF-8 \
    LANG=C.UTF-8 \
    LITEN_ROOT_DIR=/work \
    LITEN_BUILD_TYPE=release \
    LITEN_VENV_DIR=/work/venv

# Pre-install Python build tooling for all available CPython installations
RUN set -euo pipefail; \
    if compgen -G "/opt/python/cp3*-cp3*/bin" > /dev/null; then \
        for PYBIN in /opt/python/cp3*-cp3*/bin; do \
            "${PYBIN}/python" -m pip install --upgrade pip setuptools wheel build auditwheel; \
        done; \
    else \
        python3 -m pip install --upgrade pip setuptools wheel build auditwheel; \
    fi

# Include Python requirements so they can be installed at build time
COPY requirements.txt /tmp/requirements.txt

RUN mkdir -p "${LITEN_ROOT_DIR}" && \
    python3 -m venv "${LITEN_VENV_DIR}" && \
    "${LITEN_VENV_DIR}/bin/pip" install --upgrade pip setuptools wheel && \
    "${LITEN_VENV_DIR}/bin/pip" install -r /tmp/requirements.txt && \
    echo "source ${LITEN_VENV_DIR}/bin/activate" >> /etc/bash.bashrc && \
    rm /tmp/requirements.txt
ENV VIRTUAL_ENV=${LITEN_VENV_DIR} \
    PATH=${LITEN_VENV_DIR}/bin:$PATH

# Default workdir; your project will be mounted here
WORKDIR /work

# Keep the container alive even after interactive shell exit
COPY entrypoint.sh /usr/local/bin/entrypoint.sh
RUN chmod +x /usr/local/bin/entrypoint.sh

# Default entrypoint ensures shell access while keeping container running
ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
