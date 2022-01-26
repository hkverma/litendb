#!/bin/bash
# If nothing provided build debug
#
if [ -z "$LITEN_BUILD_TYPE" ]; then
    echo "LITEN_BUILD_TYPE must be set"
    exit
fi
build_type=${LITEN_BUILD_TYPE}
pushd double-conversion
if [[ ${build_type} = "release" ]]; then
    echo "Building release.."
    mkdir -p release
    pushd release
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make
    sudo make install
    popd
else
    mkdir -p debug
    pushd debug
    cmake .. -DCMAKE_BUILD_TYPE=Debug
    make
    sudo make install
    popd
fi
popd
