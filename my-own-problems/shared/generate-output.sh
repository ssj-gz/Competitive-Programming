#! /bin/bash

EXECUTABLE=./editorial
if [ "$#" -eq 1 ]; then
    EXECUTABLE=$1
fi

time -p for i in testcase-generator/testfile*.in; do 
    echo -n $i
    outfilename=${i//.in/.out}
    cat $i | /usr/bin/time -f %e -o last-testcase-time.txt ${EXECUTABLE} > $outfilename 
    last_testcase_time="$(cat last-testcase-time.txt)"
    echo " (${last_testcase_time} seconds)"
done

