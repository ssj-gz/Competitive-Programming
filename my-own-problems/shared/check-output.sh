#! /bin/bash
  
EXECUTABLE=./editorial
if [ "$#" -eq 1 ]; then
    EXECUTABLE=$1
fi

CHANGE_TO_GREEN="\033[0;32m"
CHANGE_TO_WHITE="\033[0m"
CHANGE_TO_RED="\033[0;31m"

time -p for testfile_name in testcase-generator/testfile*.in; do 
    echo -n $testfile_name 
    cat $testfile_name | /usr/bin/time -f %e -o last-testfile-time.txt ${EXECUTABLE} > last-output 
    last_testcase_time="$(cat last-testfile-time.txt)"
    echo " (${last_testcase_time} seconds)"

    diff ${testfile_name//.in/.out} last-output 
    DIFF_AGAINST_CORRECT_RESULT=$?

    if [ ${DIFF_AGAINST_CORRECT_RESULT} -eq "0" ]; then 
        echo -e "[${CHANGE_TO_GREEN}CORRECT${CHANGE_TO_WHITE}]"
    else  
        echo -e "[${CHANGE_TO_RED}WRONG ANSWER${CHANGE_TO_WHITE}]"
    fi
done

