#!/bin/bash
# Installs in /usr/local
#
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
cmake ../oneTBB -DCMAKE_BUILD_TYPE="${build_type}" -DTBB_TEST=OFF
cmake --build .
sudo make install
popd
