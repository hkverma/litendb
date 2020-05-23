#!/bin/bash
#
# Install default binaries in libs. Build more as needed in future.
# Go to cpp/external_libs/boost/boost_1_73_0/more/getting_started/unix-variants.html for more install information
# 
cd boost_1_73_0
./bootstrap.sh
# ./b2 --help for args infos
./b2 variant=debug link=static threading=multi runtime-link=static install
