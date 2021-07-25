#!/bin/bash
# Build arrow
# more information at https://arrow.apache.org/docs/developers/cpp/building.html
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
# Build these on top of default components
cmake_command="cmake ../nlohmann/ -DCMAKE_BUILD_TYPE="${build_target}""
echo "cmake command= "$cmake_command

if [[ ${build_target} = "release" ]]; then
    echo "Building release.."
    mkdir release
    cd release
    ${cmake_command}
    make arrow
    make install
else
    echo "Building debug.."
    mkdir debug
    cd debug
    ${cmake_command}
    make arrow
    make install    
fi
