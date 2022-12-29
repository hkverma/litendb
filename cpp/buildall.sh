#!/bin/bash
if [ -z "$LITEN_ROOT_DIR" ]; then
    echo "LITEN_ROOT_DIR must be set"
    exit
fi

if [ -z "$LITEN_BUILD_TYPE" ]; then
    echo "LITEN_BUILD_TYPE must be set"
    exit
fi
build_type=${LITEN_BUILD_TYPE}

if [[ "${build_type}" == "debug" || "${build_type}" == "release" ]]; then
    echo "Building ${build_type}"
else
    echo "LITEN_BUILD_TYPE must be release or debug"
    exit
fi

# Create a debug or optimized make file
#
echo "Create makefile for build type ${build_type}"
build_dir=${LITEN_ROOT_DIR}/cpp/build/${build_type}
echo "Building in directory ${build_dir}"
mkdir -p ${build_dir}
cd ${build_dir}
cmake ${LITEN_ROOT_DIR}/cpp -DCMAKE_BUILD_TYPE=${build_type} -DLITEN_BIN_DIR=${build_dir}/bin

# Now build all the targets
#
if [ "$#" -gt 1 ]; then
    for var in "${@:2}"
    do
        echo "Build $var"
        make ${var}
    done
else
    echo "Build all"
    make
fi



