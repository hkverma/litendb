#!/bin/bash
#
# Install default binaries in libs. Build more as needed in future.
# Go to cpp/external_libs/boost/boost_1_73_0/more/getting_started/unix-variants.html for more install information
# 
# If nothing provided build debug
build_target="debug"
if [[ "$#" -gt 0 ]]; then
    if [[ "$1" == "debug" || "$1" == "release" ]]; then
        build_target=$1
    else
        echo "Usage: build.sh [debug|release]"
        exit
    fi
fi
cd boost_1_73_0
./bootstrap.sh
# ./b2 --help for args infos
./b2 variant=${build_target} link=static threading=multi runtime-link=static install
