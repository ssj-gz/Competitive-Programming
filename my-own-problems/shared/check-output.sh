#! /bin/bash
  
EXECUTABLE=./editorial
if [ "$#" -eq 1 ]; then
    EXECUTABLE=$1
fi

time -p for testfile_name in testcase-generator/testfile*.in; do 
    echo -n $testfile_name 
    cat $testfile_name | /usr/bin/time -f %e -o last-testfile-time.txt ${EXECUTABLE} > last-output 
    last_testcase_time="$(cat last-testfile-time.txt)"
    echo " (${last_testcase_time} seconds)"
    diff ${testfile_name//.in/.out} last-output 
    if [ $? -eq "0" ]; then 
        echo -e "[\033[0;32mOK\033[0m]"
    else  
        echo -e "[\033[0;31mNO MATCH\033[0m]"
    fi
done

