#!/bin/bash
# Installs in /usr/local
#
if [ -z "$LITEN_BUILD_TYPE" ]; then
    echo "LITEN_BUILD_TYPE must be set"
    exit
fi
build_type=${LITEN_BUILD_TYPE}
pushd velox
if [[ ${build_type} = "release" ]]; then
    echo "Building release.."
    make release
else
    echo "Building debug.."
    make debug
fi
make 
popd
