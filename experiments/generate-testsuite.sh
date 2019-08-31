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
executablename="${cppfilenamebasename}-testsuite-generator"

if [ bruteforce ]; then
    bruteforceargs1='--testcase-gen-regex-filter=solutionBruteForce: (.*)' 
    bruteforceargs2='--testcase-gen-regex-filter-capture-group=1'
    
fi

clang++ -std=c++14 -stdlib=libc++ "${cppfilename}" -Wall -O3 -g3 -D_LIBCPP_DEBUG=1 -o "${executablename}" && \
    ${DIR}/testcase-generator "${cppfilenamebasename}-testsuite.txt" --stop-after=${stopafter} --executable-name="./${executablename}" "$(echo "${bruteforceargs1}")" ${bruteforceargs2}

