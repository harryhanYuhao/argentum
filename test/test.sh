#!/bin/sh

set -e 

cd ../src
make test
cd ../test

gcc -o write_file write_file.c
./write_file
./argentum samplefile.txt
