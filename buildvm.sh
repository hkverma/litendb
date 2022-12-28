#!/bin/sh
#
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
