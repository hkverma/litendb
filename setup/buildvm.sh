#!/bin/sh

# Install gnome-terminal
sudo apt-get -y install gnome-terminal

# Add the ununtu toolchain the repo
sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test 
sudo apt-get -y update 
sudo apt-get -y dist-upgrade
#
# Install c++ and gdb
sudo apt-get install -y build-essential
sudo apt-get install -y g++-11
sudo apt-get -y install gdb
# Remove the ubuntu toolchain repo
sudo add-apt-repository -y --remove ppa:ubuntu-toolchain-r/test
sudo apt-get -y update
#
# Install libstdc++
# sudo apt-get install libstdc++6
sudo apt-get -y install cmake
sudo apt-get -y install openssl libssl-dev
sudo apt-get install -y m4
# install intel tbb
#sudo apt-get -y install libtbb-dev
sudo apt-get -y install valgrind kcachegrind
sudo apt-get -y install graphviz
#Install all these C++ lib dependencies for fizz, folly and wangle
sudo apt-get -y install \
    g++ \
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
    
#Install docker
# https://docs.docker.com/engine/install/ubuntu/
#Install cmake
#pushd /opt
#sudo wget https://github.com/Kitware/CMake/releases/download/v3.27.6/cmake-3.27.6-linux-x86_64.sh
#sudo chmod +x ./cmake-3.27.6-linux-x86_64.sh
#sudo ./cmake-3.27.6-linux-x86_64.sh
#cd /usr/bin
#sudo ln -s /opt/cmake-3.27.6-linux-x86_64/bin/cmake cmake
#popd

#Install oneapi tbb - remove it when not needed
# download the key to system keyring
#wget -O- https://apt.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB \
#| gpg --dearmor | sudo tee /usr/share/keyrings/oneapi-archive-keyring.gpg > /dev/null
# add signed entry to apt sources and configure the APT client to use Intel repository:
#echo "deb [signed-by=/usr/share/keyrings/oneapi-archive-keyring.gpg] https://apt.repos.intel.com/oneapi all main" | sudo tee /etc/apt/sources.list.d/oneAPI.list
#sudo apt install -y intel-basekit

# Install docker - Liten is distributed as a docker container
sudo apt-get -y remove docker-desktop
rm -rf $HOME/.docker/desktop
sudo rm -rf /usr/local/bin/com.docker.cli
sudo apt-get -y purge docker-desktop
# set up docker repo
sudo apt-get  update
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
# update the packages
sudo apt-get update
sudo apt-get -y install docker-ce docker-ce-cli containerd.io docker-compose-plugin
sudo apt-get -y install docker-compose

# https://desktop.docker.com/linux/main/amd64/docker-desktop-4.17.0-amd64.deb?utm_source=docker&utm_medium=webreferral&utm_campaign=docs-driven-download-linux-amd64
#Run docker helloworld
sudo systemctl start dockerd
sudo docker run hello-world

# Install kubectl
# https://kubernetes.io/docs/tasks/tools/install-kubectl-linux/#install-using-native-package-management
sudo apt-get update
sudo apt-get install -y ca-certificates curl
sudo apt-get install -y apt-transport-https
sudo curl -fsSLo /etc/apt/keyrings/kubernetes-archive-keyring.gpg https://packages.cloud.google.com/apt/doc/apt-key.gpg
echo "deb [signed-by=/etc/apt/keyrings/kubernetes-archive-keyring.gpg] https://apt.kubernetes.io/ kubernetes-xenial main" | sudo tee /etc/apt/sources.list.d/kubernetes.list
sudo apt-get update
sudo apt-get install -y kubectl

#Install minikube
#https://minikube.sigs.k8s.io/docs/start/
curl -LO https://storage.googleapis.com/minikube/releases/latest/minikube-linux-amd64
sudo install minikube-linux-amd64 /usr/local/bin/minikube

#Install node.js
# curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/master/install.sh | bash
# npm install --lts     
# npm install node
# npm install -g typescript
# npm install -g ts-node

# install jdk
sudo apt install -y openjdk-17-jdk openjdk-17-jre
# install python - keep default python (3.10) for Ubuntu22
sudo apt install -y python3-pip
alias python=python3
pip install numpy
pip install cython==3.0.2
# Ensure that this changes if arrow version is changed in opensource submodule
pip install pyarrow==13.0.0
pip install pandas
pip install jupyterlab
pip install graphviz
pip install tbb-devel
#pip install pyspark=3.2.4
#pip install seaborn

#install auditwheel to repair wheels
pip install auditwheel
sudo apt install -y patchelf

#Install arrow dev For now, build it in opensources
#sudo apt update
#sudo apt install -y -V ca-certificates lsb-release wget
#wget https://apache.jfrog.io/artifactory/arrow/$(lsb_release --id --short | tr 'A-Z' 'a-z')/apache-arrow-apt-source-latest-$(lsb_release --codename --short).deb
#sudo apt install -y -V ./apache-arrow-apt-source-latest-$(lsb_release --codename --short).deb
#sudo apt update
#sudo apt install -y -V libarrow-dev # For C++
#sudo apt install -y -V libarrow-glib-dev # For GLib (C)
#sudo apt install -y -V libarrow-dataset-dev # For Apache Arrow Dataset C++
#sudo apt install -y -V libarrow-dataset-glib-dev # For Apache Arrow Dataset GLib (C)
#sudo apt install -y -V libarrow-acero-dev # For Apache Arrow Acero
#sudo apt install -y -V libarrow-flight-dev # For Apache Arrow Flight C++
#sudo apt install -y -V libarrow-flight-glib-dev # For Apache Arrow Flight GLib (C)
#sudo apt install -y -V libarrow-flight-sql-dev # For Apache Arrow Flight SQL C++
#sudo apt install -y -V libarrow-flight-sql-glib-dev # For Apache Arrow Flight SQL GLib (C)
##sudo apt install -y -V libgandiva-dev  For Gandiva C++
##sudo apt install -y -V libgandiva-glib-dev  For Gandiva GLib (C)
#sudo apt install -y -V libparquet-dev # For Apache Parquet C++
#sudo apt install -y -V libparquet-glib-dev # For Apache Parquet GLib (C)
