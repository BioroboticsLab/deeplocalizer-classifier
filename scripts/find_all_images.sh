#! /usr/bin/env bash

set -e

DIR=$1

if [ "$DIR" == "" ]; then
    DIR=$(pwd)
fi;

find `realpath $DIR` -type f  | egrep "(.jpeg|.jpg|.JPEG|.png|.PNG)$"
