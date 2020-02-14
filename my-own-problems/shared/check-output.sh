#! /bin/bash

usage() {
    echo "Usage: check-output.sh [executable-name] [-d|--diff] [-a|--exe-arg executable-argument] [-t|--tle-ms tle-seconds"
    echo
    echo "Finds all files of the form testfile<suffix>.in below the current directory and pipes them into executable-name (with"
    echo "executable-argument as an argument, if provided) and compares the resulting output with the corresponding testfile<suffix>.out."
    echo
    echo "Options:"
    echo
    echo " * -d|--diff                 In the event that the output from the invocation of executable-name does not match that of the corresponding .out"
    echo "                             file, print the differences between them using the diff command-line utility"                           
    echo " * -a|--executable-argument  The argument to be passed to executable-name when it is invoked"
    echo " * -t|--tle-second     s     If the execution takes more than tle-seconds seconds, it is marked as TLE.  Note: the execution is"
    echo "                             allowed to run its cause, even if it has taken more than tle-seconds seconds.  tle-seconds is permitted to be a"
    echo "                             fractional value"

    echo
    echo "Parameters:"
    echo
    echo " * executable-name           The name of the executable to be tested.  Defaults to ./editorial"
}

params="$(getopt -o d,a:,t: -l diff,exe-arg:,tle-seconds: --name "$cmdname" -- "$@")"

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
TLE_SECONDS=
WA_OCCURRED=false
TLE_OCCURRED=false
LONGEST_TESTFILE_SECONDS=0

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
            TLE_SECONDS=${2-}
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

COMPLETION_NOTIFICATION_PIPE=testfile-completed.pipe

time -p for testfile_name in testcase-generator/testfile*.in; do 
    echo -n $testfile_name 

    if [ -e $COMPLETION_NOTIFICATION_PIPE ]; then
        rm $COMPLETION_NOTIFICATION_PIPE
        mkfifo $COMPLETION_NOTIFICATION_PIPE
    fi


    (cat $testfile_name | /usr/bin/time -f %e -o last-testfile-time.txt ${EXECUTABLE} ${EXECUTABLE_ARG} > last-output 2> last-output-error; echo $? > $COMPLETION_NOTIFICATION_PIPE) &

    seconds_elapsed=1

    EXECUTABLE_RETURN_CODE=""
    while true; do
        read -t 1 EXECUTABLE_RETURN_CODE <> $COMPLETION_NOTIFICATION_PIPE

        seconds_elapsed_message=" ${seconds_elapsed}s"
        seconds_elapsed_message_num_chars=$(echo "$seconds_elapsed_message" | wc -c)

        echo -ne "$seconds_elapsed_message"

        # Move the cursor back to the beginning of the "Seconds elapsed" message we printed
        for dummy in $(seq 1 $(($seconds_elapsed_message_num_chars - 1))); do
            echo -en "\b"
        done
        if [ ! -z "$EXECUTABLE_RETURN_CODE" ]; then
            break
        fi

        seconds_elapsed=$(($seconds_elapsed+1)) # Not strictly accurate, but we can work on that, I guess!
    done

    last_testfile_time="$(cat last-testfile-time.txt | grep -v Command)" # If ${EXECUTABLE} fails, last-testfile-time.txt will contain a "Command exited/ terminated" line; remove it.
    echo " (${last_testfile_time} seconds)"

    diff ${testfile_name//.in/.out} last-output > last-diff-output
    RESULT_OF_DIFF_AGAINST_CORRECT=$?

    if [ ${RESULT_OF_DIFF_AGAINST_CORRECT} -eq "0" ]; then 
        echo -en "[${CHANGE_TO_GREEN}CORRECT${CHANGE_TO_WHITE}]"
    else  
        WA_OCCURRED=true
        if [ ${PRINT_DIFF_ON_MISMATCH} == true ]; then
            cat last-diff-output
        fi
        if [ -s last-output-error ]; then
            echo "stderr:"
            cat last-output-error
        fi
        echo -en "[${CHANGE_TO_RED}WRONG ANSWER${CHANGE_TO_WHITE}]"
    fi

    if [ ${EXECUTABLE_RETURN_CODE} -ne "0" ]; then 
        echo -en "[${CHANGE_TO_RED}NZEC${CHANGE_TO_WHITE}]"
    fi

    if (( $(echo "$last_testfile_time > $LONGEST_TESTFILE_SECONDS" |bc -l) )); then
        LONGEST_TESTFILE_SECONDS=$last_testfile_time
    fi
    if [[ ! -z "${TLE_SECONDS}" &&  "$(echo "$last_testfile_time > $TLE_SECONDS" |bc -l)" -eq 1 ]]; then
        TLE_OCCURRED=true
        echo -en "[${CHANGE_TO_RED}TLE${CHANGE_TO_WHITE}]"
    fi
    echo
done

echo "Longest testfile took ${LONGEST_TESTFILE_SECONDS} seconds"

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
