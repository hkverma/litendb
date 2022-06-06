#!/bin/bash

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

sudo rm -rf ${build_type}
mkdir -p ${build_type}
pushd ${build_type}
cmake ../re2 -DCMAKE_BUILD_TYPE="${build_type}" -DBUILD_SHARED_LIBS=ON
make
sudo make install
popd
