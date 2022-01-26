#!/bin/bash
# Installs in /usr/local
#
if [ -z "$LITEN_BUILD_TYPE" ]; then
    echo "LITEN_BUILD_TYPE must be set"
    exit
fi
build_type=${LITEN_BUILD_TYPE}
pushd folly
if [[ ${build_type} = "release" ]]; then
    echo "Building release.."
    mkdir -p release
    pushd release
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make
    popd
else
    echo "Building debug.."
    mkdir -p debug
    pushd debug
    cmake .. -DCMAKE_BUILD_TYPE=Debug
    make
    popd
fi
popd
