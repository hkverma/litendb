#!/bin/sh
#
# Install docker - Liten is distributed as a docker container
sudo apt-get -y install gnome-terminal
sudo apt-get -y remove docker-desktop
rm -rf $HOME/.docker/desktop
sudo rm -rf /usr/local/bin/com.docker.cli
sudo apt-get -y purge docker-desktop
# set up docker repo
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
# update the packages
sudo apt-get update
sudo apt-get -y install docker-ce docker-ce-cli containerd.io docker-compose-plugin
#Run docker helloworld
sudo systemctl start dockerd
sudo docker run hello-world

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
sudo apt-get -y install libtbb-dev
sudo apt-get -y install valgrind kcachegrind
sudo apt-get -y install graphviz
#Install all these C++ lib dependencies for fizz, folly and wangle
sudo apt-get -y install \
    g++ \
    cmake \
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
    
