#! /usr/bin/env bash

set -e

DIR=$1

if [ '$DIR' == '' ]; then
    DIR=$(pwd)
fi;
find $DIR -type f  | egrep ".jpeg|.jpg|.JPEG|.png"
