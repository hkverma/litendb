#!/bin/bash
pushd glog
mkdir -p debug
pushd debug
cmake .. -DCMAKE_BUILD_TYPE=Debug
make
sudo make install
popd
popd
