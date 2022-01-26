#!/bin/bash
# Build all external libraries
#
# If nothing provided build debug
for dir in glog googletest double-conversion libevent nlohmann tbb ray re2 folly arrow velox
do           
    pushd $dir
    ./build.sh
    popd
done
