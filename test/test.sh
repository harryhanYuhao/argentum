#!/bin/sh

set -e 

cd ../src
make test
cd ../test

./a.out samplefile.txt
