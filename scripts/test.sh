#! /usr/bin/env bash

set -e

BUILD_DIR="build-test"
mkdir -p $BUILD_DIR
NCPUS=$(grep -c ^processor /proc/cpuinfo)
(cd $BUILD_DIR;
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j $NCPUS
ctest
)