#!/bin/bash
build_type="debug"
if [[ "$#" -gt 0 ]]; then
    if [[ "$1" == "debug" || "$1" == "release" ]]; then
        build_type=$1
    else
        echo "Usage: build.sh <debug|release> [targets ... ]"
        exit
    fi
fi

# Create a debug or optimized make file
#
echo "Create makefile for build type ${build_type}"
build_dir=build/${build_type}
mkdir -p ${build_dir}
cd ${build_dir}
cmake ../.. -DCMAKE_BUILD_TYPE=${build_type}

# Now build all the targets
#
if [[ "$#" -gt 1 ]]; then
    for var in "${@:2}"
    do
        echo "Build $var"
        make ${var}
    done
else
    echo "Build all"
    make
fi



