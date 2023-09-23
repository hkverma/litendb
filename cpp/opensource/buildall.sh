#!/bin/bash
# Build opensource libraries
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
# Build all in build_type directory
mkdir -p ${build_type}
pushd ${build_type}

# Build folly - Its various dependencies are listed here - folly/build/fbcode_builder/manifests/folly
# buildvm.sh has the apt-get install command to install all the dependencies
mkdir -p folly
pushd folly
cmake ../../folly
make -j $(nproc)
sudo make install
popd
# Build fizz
mkdir -p fizz
pushd fizz
cmake ../../fizz/fizz
make -j $(nproc)
sudo make install
popd
# Build wangle
mkdir -p wangle
pushd wangle
cmake ../../wangle/wangle
make -j $(nproc)
sudo make install
popd
#
# Build arrow
rm -rf arrow
mkdir -p arrow
pushd arrow
cmake ../../arrow/cpp -DCMAKE_BUILD_TYPE="${build_type}" -DARROW_CSV=ON -DARROW_FILESYSTEM=ON -DARROW_PYTHON=ON
make -j $(nproc)
sudo make install
popd

# get out of build dir
popd
