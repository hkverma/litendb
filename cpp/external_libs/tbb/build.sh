#!/bin/bash
# Installs in /usr/local
#
if [ -z "$LITEN_BUILD_TYPE" ]; then
    echo "LITEN_BUILD_TYPE must be set"
    exit
fi
build_type=${LITEN_BUILD_TYPE}
pushd oneTBB
mkdir build
pushd build
if [[ ${build_type} = "release" ]]; then
    echo "Building release.."
    cmake -DCMAKE_BUILD_TYPE=Release -DTBB_TEST=OFF ..
else
    echo "Building debug.."
    cmake -DCMAKE_BUILD_TYPE=Debug -DTBB_TEST=OFF ..
fi
cmake --build .
make install
popd
