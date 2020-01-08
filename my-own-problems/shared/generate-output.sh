#! /bin/bash

EXECUTABLE=./editorial
if [ "$#" -eq 1 ]; then
    EXECUTABLE=$1
fi

time -p for i in testcase-generator/testfile*.in; do echo $i; outfilename=${i//.in/.out} ; cat $i | ${EXECUTABLE} > $outfilename ; done

