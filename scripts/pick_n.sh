#! /usr/bin/env bash

N="$1"
if [ "$N" == "" ]; then
    echo "Usage: $0 N         where N is an integer."
    echo "Picks randomly N lines from stdin"
    exit 1;
fi;
cat <&0 | sort -R | head -n "$N"
