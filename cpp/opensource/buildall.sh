#!/bin/bash
# Build opensource libraries
#
# If nothing provided build debug
# Build folly - Its various dependencies are listed here - folly/build/fbcode_builder/manifests/folly
# buildvm.sh has the apt-get install command to install all the dependencies
mkdir p folly/build_
pushd folly/build_
cmake ..
make -j $(nproc)
sudo make install
popd
# Build fizz
mkdir -p fizz/build_
pushd fizz/build_
cmake ../fizz
make -j $(nproc)
sudo make install
popd
# Build wangle
mkdir -p wangle/build_
pushd wangle/build_
make -j $(nproc)
sudo make install
popd



#for dir in glog googletest double-conversion libevent nlohmann folly tbb re2 arrow velox ray
#for dir in gflags glog tbb re2 nlohmann arrow
#do           
#    echo "Building ${dir} ... "
#    pushd $dir
#    ./build.sh
#    popd
#done
