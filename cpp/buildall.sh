#!/bin/bash
build_target="Debug"
if [[ "$#" -gt 0 ]]; then
    if [[ "$1" == "Debug" || "$1" == "Release" ]]; then
        build_target=$1
    else
        echo "Usage: build.sh [Debug|Release]"
        exit
    fi
fi
#
echo "Building ${build_target}"
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=${build_target}
make
