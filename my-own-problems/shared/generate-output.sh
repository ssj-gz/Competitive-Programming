#! /bin/bash

EXECUTABLE=./editorial
if [ "$#" -eq 1 ]; then
    EXECUTABLE=$1
fi

time -p for testfile_name in testcase-generator/testfile*.in; do 
    echo -n $testfile_name
    outfilename=${testfile_name//.in/.out}
    cat $testfile_name | /usr/bin/time -f %e -o last_testfile_time.txt ${EXECUTABLE} > $outfilename 
    last_testfile_time="$(cat last_testfile_time.txt)"
    echo " (${last_testfile_time} seconds)"
done

