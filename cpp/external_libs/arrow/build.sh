#!/bin/bash
# Build arrow
# more information at https://arrow.apache.org/docs/developers/cpp/building.html
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

# Build these on top of default components
# -DARROW_CSV=ON -DARROW_FILESYSTEM=ON -DARROW_PYTHON=ON -DARROW_GANDIVA=ON -DARROW_BUILD_TESTS=ON"

sudo rm -rf ${build_type}
mkdir -p ${build_type}
pushd ${build_type}
cmake ../arrow/cpp -DCMAKE_BUILD_TYPE="${build_type}" -DARROW_CSV=ON -DARROW_FILESYSTEM=ON -DARROW_PYTHON=ON
make
sudo make install
popd
