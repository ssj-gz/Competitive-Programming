#! /bin/bash

usage() {
    echo "Usage: TODO"
}

params="$(getopt -o n,a: -l no-diff,exe-arg: --name "$cmdname" -- "$@")"

if [ $? -ne 0 ]
then
    usage
    exit 1
fi

eval set -- "$params"
unset params
  
EXECUTABLE=./editorial
NO_DIFF=false
EXECUTABLE_ARG=

while true
do
    case $1 in
        -n|--no-diff)
            NO_DIFF=true
            shift
            ;;
        -a|--exe_arg)
            EXECUTABLE_ARG=${2-}
            shift 2
            ;;
        -h|--help)
            usage
            exit
            ;;
        --)
            shift
            if [ ! -z "${1:-}" ]
            then
                EXECUTABLE=${1:-}
            fi
            break
            ;;
        *)
            usage
            ;;
    esac
done

CHANGE_TO_GREEN="\033[0;32m"
CHANGE_TO_WHITE="\033[0m"
CHANGE_TO_RED="\033[0;31m"

echo "Executable name: $EXECUTABLE NO_DIFF: ${NO_DIFF} EXECUTABLE_ARG ${EXECUTABLE_ARG}"

time -p for testfile_name in testcase-generator/testfile*.in; do 
    echo -n $testfile_name 
    cat $testfile_name | /usr/bin/time -f %e -o last-testfile-time.txt ${EXECUTABLE} ${EXECUTABLE_ARG} > last-output 
    last_testcase_time="$(cat last-testfile-time.txt)"
    echo " (${last_testcase_time} seconds)"

    diff ${testfile_name//.in/.out} last-output > last-diff-output
    DIFF_AGAINST_CORRECT_RESULT=$?

    if [ ${DIFF_AGAINST_CORRECT_RESULT} -eq "0" ]; then 
        echo -e "[${CHANGE_TO_GREEN}CORRECT${CHANGE_TO_WHITE}]"
    else  
        if [ ${NO_DIFF} != true ]; then
            cat last-diff-output
        fi
        echo -e "[${CHANGE_TO_RED}WRONG ANSWER${CHANGE_TO_WHITE}]"
    fi
done

