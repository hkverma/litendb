#!/bin/bash
# If nothing provided build debug
build_target="Debug"
if [[ "$#" -gt 0 ]]; then
    if [[ "$1" == "Debug" || "$1" == "Release" ]]; then
        build_target=$1
    else
        echo "Usage: build.sh [Debug|Release]"
        exit
    fi
fi
# Build these on top of default components
pushd glog
if [[ ${build_target} = "Release" ]]; then
    echo "Building release.."
    mkdir -p release
    pushd release
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make
    sudo make install
    popd
else
    echo "Building debug.."
    mkdir -p debug
    pushd debug
    cmake .. -DCMAKE_BUILD_TYPE=Debug
    make
    sudo make install
    popd
fi
popd
