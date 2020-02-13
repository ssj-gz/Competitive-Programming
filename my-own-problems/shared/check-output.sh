#! /bin/bash

usage() {
    echo "Usage: check-output.sh [executable-name] [-d|--diff] [-a|--exe-arg executable-argument] [-t|--tle-ms tle-milliseconds"
    echo
    echo "Finds all files of the form testfile<suffix>.in below the current directory and pipes them into executable-name (with"
    echo "executable-argument as an argument, if provided) and compares the resulting output with the corresponding testfile<suffix>.out."
    echo
    echo "Options:"
    echo
    echo " * -d|--diff                 In the event that the output from the invocation of executable-name does not match that of the corresponding .out"
    echo "                             file, print the differences between them using the diff command-line utility"                           
    echo " * -a|--executable-argument  The argument to be passed to executable-name when it is invoked"
    echo " * -t|--tle-milliseconds     If the execution takes more than tle-milliseconds milliseconds, it is marked as TLE.  Note: the execution is"
    echo "                             allowed to run its cause, even if it has taken more than tle-milliseconds milliseconds"

    echo
    echo "Parameters:"
    echo
    echo " * executable-name           The name of the executable to be tested.  Defaults to ./editorial"
}

params="$(getopt -o d,a:,t: -l diff,exe-arg:,tle-milliseconds: --name "$cmdname" -- "$@")"

if [ $? -ne 0 ]
then
    usage
    exit 1
fi

eval set -- "$params"
unset params
  
EXECUTABLE=./editorial
PRINT_DIFF_ON_MISMATCH=false
EXECUTABLE_ARG=
TLE_MILLISECONDS=
WA_OCCURRED=false
TLE_OCCURRED=false
LONGEST_TESTCASE_SECONDS=0

while true
do
    case $1 in
        -d|--diff)
            PRINT_DIFF_ON_MISMATCH=true
            shift
            ;;
        -a|--exe_arg)
            EXECUTABLE_ARG=${2-}
            shift 2
            ;;
        -t|--tle-milliseconds)
            TLE_MILLISECONDS=${2-}
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

time -p for testfile_name in testcase-generator/testfile*.in; do 
    echo -n $testfile_name 
    cat $testfile_name | /usr/bin/time -f %e -o last-testfile-time.txt ${EXECUTABLE} ${EXECUTABLE_ARG} > last-output 
    last_testcase_time="$(cat last-testfile-time.txt)"
    echo " (${last_testcase_time} seconds)"

    diff ${testfile_name//.in/.out} last-output > last-diff-output
    RESULT_OF_DIFF_AGAINST_CORRECT=$?

    if [ ${RESULT_OF_DIFF_AGAINST_CORRECT} -eq "0" ]; then 
        echo -en "[${CHANGE_TO_GREEN}CORRECT${CHANGE_TO_WHITE}]"
    else  
        WA_OCCURRED=true
        if [ ${PRINT_DIFF_ON_MISMATCH} == true ]; then
            cat last-diff-output
        fi
        echo -en "[${CHANGE_TO_RED}WRONG ANSWER${CHANGE_TO_WHITE}]"
    fi

    if (( $(echo "$last_testcase_time > $LONGEST_TESTCASE_SECONDS" |bc -l) )); then
        LONGEST_TESTCASE_SECONDS=$last_testcase_time
    fi

    last_testcase_time_ms=$(echo $last_testcase_time*1000 | bc)
    if [[ ! -z "${TLE_MILLISECONDS}" &&  "$(echo "$last_testcase_time_ms > $TLE_MILLISECONDS" |bc -l)" -eq 1 ]]; then
        TLE_OCCURRED=true
        echo -en "[${CHANGE_TO_RED}TLE${CHANGE_TO_WHITE}]"
    fi
    echo
done

echo "Longest testcase took ${LONGEST_TESTCASE_SECONDS} seconds"

PASS=true
if [ ${WA_OCCURRED} == true ]; then
    PASS=false
    echo -e "[${CHANGE_TO_RED}FAILED${CHANGE_TO_WHITE}] due to WA"
fi
if [ ${TLE_OCCURRED} == true ]; then
    PASS=false
    echo -e "[${CHANGE_TO_RED}FAILED${CHANGE_TO_WHITE}] due to TLE"
fi

if [ ${PASS} == true ]; then
    echo -e "[${CHANGE_TO_GREEN}PASSED${CHANGE_TO_WHITE}]"
    exit 0
else
    exit 1
fi
