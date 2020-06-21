#!/bin/bash
# Build C++ ray libraray and test components
#
# install ray
#
pushd ray/python
pip3 install -e . --verbose
popd
#
# Build ray now
#
pushd ray
# bazel build //:ray_util --verbose_failures
# bazel build //:ray_common --verbose_failures
#sudo bazel build -c debug //:ray_pkg --verbose_failures
#bazel build //:core_worker_test --verbose_failures
sudo ./build.sh --language python --python python3
popd
