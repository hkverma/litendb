#!/bin/bash
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
pushd tbb/src
if [[ ${build_target} = "release" ]]; then
    echo "Building release.."
    make release
else
    echo "Building debug.."
    make debug
fi
popd
