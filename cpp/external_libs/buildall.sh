#!/bin/bash
# Build all external libraries
#
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
# Build arrow
pushd arrow
./build.sh ${build_target}
popd
