#!/bin/bash
# Build all external libraries
#
# If nothing provided build debug
#for dir in glog googletest double-conversion libevent nlohmann folly tbb re2 arrow velox ray
for dir in glog tbb re2 nlohmann arrow
do           
    echo "Building ${dir} ... "
    pushd $dir
    ./build.sh
    popd
done
