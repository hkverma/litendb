#!/bin/sh

# Function to install C++ and gdb
install_libs() {
    for arg in "$@"; do
        echo " Installing $arg"
        sudo apt-get install -y $arg
    done
}

# Function to install Docker
install_docker() {
    sudo apt-get -y remove docker-desktop
    rm -rf $HOME/.docker/desktop
    sudo rm -rf /usr/local/bin/com.docker.cli
    sudo apt-get -y purge docker-desktop
    sudo apt-get update
    sudo apt-get -y install \
        ca-certificates \
        curl \
        gnupg \
        lsb-release
    sudo mkdir -p /etc/apt/keyrings
    curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg
    echo \
      "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.gpg] https://download.docker.com/linux/ubuntu \
      $(lsb_release -cs) stable" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null
    sudo apt-get update
    sudo apt-get -y install docker-ce docker-ce-cli containerd.io docker-compose-plugin
    sudo apt-get -y install docker-compose
}

# Function to install kubectl
install_kubectl() {
    sudo apt-get update
    sudo apt-get install -y ca-certificates curl
    sudo apt-get install -y apt-transport-https
    sudo curl -fsSLo /etc/apt/keyrings/kubernetes-archive-keyring.gpg https://packages.cloud.google.com/apt/doc/apt-key.gpg
    echo "deb [signed-by=/etc/apt/keyrings/kubernetes-archive-keyring.gpg] https://apt.kubernetes.io/ kubernetes-xenial main" | sudo tee /etc/apt/sources.list.d/kubernetes.list
    sudo apt-get update
    sudo apt-get install -y kubectl
}

# Function to install minikube
install_minikube() {
    curl -LO https://storage.googleapis.com/minikube/releases/latest/minikube-linux-amd64
    sudo install minikube-linux-amd64 /usr/local/bin/minikube
}

# Function to install JDK and Maven
install_jdk_maven() {
    sudo apt install -y openjdk-17-jdk openjdk-17-jre
    sudo apt install -y maven
}

# Function to install Python packages
install_python_packages() {
    sudo apt install -y python3-pip
    pip install numpy
    pip install cython==3.0.2
    pip install pyarrow==18.1.0
    pip install pandas
    pip install jupyterlab
    pip install graphviz
    pip install tbb-devel
    pip install auditwheel
    sudo apt install -y patchelf
}

# Function to install Arrow development libraries
install_arrow_dev() {
    sudo apt update
    sudo apt install -y -V ca-certificates lsb-release wget
    wget https://apache.jfrog.io/artifactory/arrow/$(lsb_release --id --short | tr 'A-Z' 'a-z')/apache-arrow-apt-source-latest-$(lsb_release --codename --short).deb
    sudo apt install -y -V ./apache-arrow-apt-source-latest-$(lsb_release --codename --short).deb
    sudo apt update
    sudo apt install -y -V libarrow-dev # For C++
    sudo apt install -y -V libarrow-glib-dev # For GLib (C)
    sudo apt install -y -V libarrow-dataset-dev # For Apache Arrow Dataset C++
    sudo apt install -y -V libarrow-dataset-glib-dev # For Apache Arrow Dataset GLib (C)
    sudo apt install -y -V libarrow-acero-dev # For Apache Arrow Acero
    sudo apt install -y -V libarrow-flight-dev # For Apache Arrow Flight C++
    sudo apt install -y -V libarrow-flight-glib-dev # For Apache Arrow Flight GLib (C)
    sudo apt install -y -V libarrow-flight-sql-dev # For Apache Arrow Flight SQL C++
    sudo apt install -y -V libarrow-flight-sql-glib-dev # For Apache Arrow Flight SQL GLib (C)
    sudo apt install -y -V libgandiva-dev  # For Gandiva C++
    sudo apt install -y -V libgandiva-glib-dev  # For Gandiva GLib (C)
    sudo apt install -y -V libparquet-dev # For Apache Parquet C++
    sudo apt install -y -V libparquet-glib-dev # For Apache Parquet GLib (C)
}

#
#Install all these C++ lib dependencies for dev as as well integrated
# open source like arrow, fizz, folly and wangle
install_libs \
    g++ \
    gdb \
    cmake \
    graphviz \
    libasan8 \
    valgrind \
    kcachegrind \
    libboost-all-dev \
    libevent-dev \
    libdouble-conversion-dev \
    libgoogle-glog-dev \
    libgflags-dev \
    libiberty-dev \
    liblz4-dev \
    liblzma-dev \
    libsnappy-dev \
    make \
    zlib1g-dev \
    binutils-dev \
    libjemalloc-dev \
    libssl-dev \
    pkg-config \
    libsodium-dev \
    libfmt-dev \
    libgtest-dev \
    libzstd-dev \
    libgmock-dev \
    libre2-dev \
    libgoogle-perftools-dev \
    nlohmann-json3-dev

# install jdk and maven
install_libs \
    openjdk-17-jdk \
    openjdk-17-jre \
    maven \
    patchelf

# Install docker - Liten is distributed as a docker container
# install_docker
# Install kubectl
# install_kubectl

#Install minikube
# install_minikube

#Install arrow dev libraries if needed
# install_arrow_dev
