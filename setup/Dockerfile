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

# Make sure locale is sane
ENV LC_ALL=C.UTF-8 \
    LANG=C.UTF-8

# Pre-install Python build tooling for all available CPython installations
RUN set -euo pipefail; \
    if compgen -G "/opt/python/cp3*-cp3*/bin" > /dev/null; then \
        for PYBIN in /opt/python/cp3*-cp3*/bin; do \
            "${PYBIN}/python" -m pip install --upgrade pip setuptools wheel build auditwheel; \
        done; \
    else \
        python3 -m pip install --upgrade pip setuptools wheel build auditwheel; \
    fi

# Default workdir; your project will be mounted here
WORKDIR /work

# Optional: add a helper script into the image
#COPY build_wheels.sh /usr/local/bin/build_wheels.sh
#RUN chmod +x /usr/local/bin/build_wheels.sh

# Default to bash; you can override with build_wheels.sh in `docker run`
ENTRYPOINT ["/bin/bash"]
