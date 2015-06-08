#! /usr/bin/env bash

if [ "$1" == "--help" ]; then
    echo "Compile with Clang and GCC and then run the tests.";
    echo "This script is used in wercker.yml";
    exit 0;
fi
set -e

function build_and_test {
    export CXX=$1
    BUILD_DIR=$2
    mkdir -p $BUILD_DIR
    (cd $BUILD_DIR;
    cmake -DCMAKE_BUILD_TYPE=Debug ..
    make -j2
    ctest
    )
}

CLANG_BUILD_DIR="build-clang"
GCC_BUILD_DIR="build-gcc"
Clang="/usr/bin/clang++"
GCC="/usr/bin/c++"
build_and_test $Clang $CLANG_BUILD_DIR
build_and_test $GCC $GCC_BUILD_DIR
