#! /bin/bash

stopafter=10000
bruteforce=false

while getopts "s:bh" arg; do
    case $arg in
        h)
            echo "usage: $0 [-s stop-after] [-b] cpp-filename.cpp" 
            ;;
        s)
            stopafter=$OPTARG
            ;;
        b) 
            bruteforce=true
    esac
done

shift $((OPTIND-1))
cppfilename=$@

echo "stopafter: ${stopafter} bruteforce: ${bruteforce} cppfilename: ${cppfilename}"

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
echo "DIR: ${DIR}"

cppfilenamebasename=$(basename "${cppfilename}" .cpp)
executablename="${cppfilenamebasename}-generator"

clang++ -std=c++14 -stdlib=libc++ "${cppfilename}" -Wall -O3 -g3 -D_LIBCPP_DEBUG=1 -o "${executablename}"

if [ $? -ne 0 ]; then
    exit 1
fi


if [ ${bruteforce} == "true" ]; then
    ${DIR}/testcase-generator "${cppfilenamebasename}-testsuite.txt" --stop-after=${stopafter} --executable-name="./${executablename}" --testcase-gen-regex-filter="solutionBruteForce: (.*)" --testcase-gen-regex-filter-capture-group=1
else
    ${DIR}/testcase-generator "${cppfilenamebasename}-testsuite.txt" --stop-after=${stopafter} --executable-name="./${executablename}"
fi

