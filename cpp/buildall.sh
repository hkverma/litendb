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
if [ -z "$LITEN_ROOT_DIR" ]; then
    echo "LITEN_ROOT_DIR must be set"
    exit
fi

# Create a debug or optimized make file
#
echo "Create makefile for build type ${build_type}"
build_dir=${LITEN_ROOT_DIR}/cpp/build/${build_type}
echo "Building in directory ${build_dir}"
mkdir -p ${build_dir}
cd ${build_dir}
cmake ${LITEN_ROOT_DIR}/cpp -DCMAKE_BUILD_TYPE=${build_type} -DLITEN_BUILD_DIR=/${build_dir}

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



