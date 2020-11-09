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
if [[ ${build_target} = "release" ]]; then
    echo "Building release.."
    pushd tbb/src
    make release
    ln -s tbb/build/linux_intel64_gcc_cc9.3.0_libc2.27_kernel5.4.0_release bin
    popd
else
    echo "Building debug.."
    pushd tbb/src
    make debug
    ln -s tbb/build/linux_intel64_gcc_cc9.3.0_libc2.27_kernel4.19.128_debug bin
    popd
fi

