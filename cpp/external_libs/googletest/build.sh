#!/bin/bash
pushd googletest
mkdir -p debug
pushd debug
cmake .. -DCMAKE_BUILD_TYPE=Debug
make
sudo make install
popd
popd
