#! /bin/bash
  
EXECUTABLE=./editorial
if [ "$#" -eq 1 ]; then
    EXECUTABLE=$1
fi

time -p for i in testcase-generator/testfile*.in; do 
    echo -n $i 
    cat $i | /usr/bin/time -f %e -o last-testcase-time.txt ${EXECUTABLE} > last-output 
    last_testcase_time="$(cat last-testcase-time.txt)"
    echo " (${last_testcase_time} seconds)"
    diff ${i//.in/.out} last-output 
    if [ $? -eq "0" ]; then 
        echo -e "[\033[0;32mOK\033[0m]"
    else  
        echo -e "[\033[0;31mNO MATCH\033[0m]"
    fi
done

