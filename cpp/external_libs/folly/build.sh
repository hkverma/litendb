#!/bin/bash
# Use folly ./build.sh to build and install all dependencies
# Installs boost among other libraries
#
if [ -z "$LITEN_BUILD_TYPE" ]; then
    echo "LITEN_BUILD_TYPE must be set"
    exit
fi
build_type=${LITEN_BUILD_TYPE}
pushd folly
./build.sh
popd
