#!/usr/bin/env bash
# generate release.zip in the script directory

if [ $# -ne 1 ]; then
	echo "usage: $0 generator_path" 2>&1
	exit 1
fi

cd `dirname $0`
rm release.zip

#clean up
rm -r release
mkdir release

cp -r "$1" release/
cp install.command release/
cp ../add_dynUTI.sh release/
cp ../testv4.apd  release/

zip -r release.zip release/*
rm -r release
