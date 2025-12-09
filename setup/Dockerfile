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
        build-essential \
        software-properties-common \
        ninja-build && \
    chmod +x /tmp/buildvm.sh && \
    /tmp/buildvm.sh && \
    rm -f /tmp/buildvm.sh && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Add deadsnakes PPA for Python 3.12
RUN add-apt-repository ppa:deadsnakes/ppa -y && \
    apt-get update && apt-get install -y --no-install-recommends \
    python3.12 python3.12-dev python3.12-venv \
    && rm -rf /var/lib/apt/lists/*

# Make python3/python point to 3.12
RUN update-alternatives --install /usr/bin/python3 python3 /usr/bin/python3.12 1 && \
    update-alternatives --install /usr/bin/python python /usr/bin/python3.12 1

# Make sure locale is sane and set Liten-specific defaults
ENV LC_ALL=C.UTF-8 \
    LANG=C.UTF-8 \
    LITEN_ROOT_DIR=/work \
    LITEN_BUILD_TYPE=release \
    LITEN_VENV_DIR=/work/venv

# Include Python requirements so they can be installed at build time
COPY requirements.txt /tmp/requirements.txt

RUN mkdir -p "${LITEN_VENV_DIR}" && \
    python3 -m venv "${LITEN_VENV_DIR}" && \
    source ${LITEN_VENV_DIR}/bin/activate && \
    python3 -m ensurepip --upgrade || \
       curl -sS https://bootstrap.pypa.io/get-pip.py | python3 && \
    python3 -m pip install --upgrade pip && \    
    pip install --upgrade pip setuptools wheel && \
    pip install -r /tmp/requirements.txt && \
    echo "source ${LITEN_VENV_DIR}/bin/activate" >> /etc/bash.bashrc && \
    ln -s ${LITEN_VENV_DIR}/lib/python3.12/site-packages/pyarrow/libarrow.so.2100 ${LITEN_VENV_DIR}/lib/python3.12/site-packages/pyarrow/libarrow.so && \
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
