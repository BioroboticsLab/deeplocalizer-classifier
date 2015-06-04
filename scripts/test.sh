#! /usr/bin/env bash

set -e

BUILD_DIR="build-test"
mkdir -p $BUILD_DIR
(cd $BUILD_DIR;
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
ctest
)