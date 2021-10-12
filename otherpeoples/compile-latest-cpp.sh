#! /bin/bash

if [ "$1" == "clang" ]; then
    echo "clang"
    export USE_CLANG=true
fi

export LATESTCPP=$(ls -1at *.cpp | head -n 1)
echo "Compiling $LATESTCPP" 
if [ -z "${USE_CLANG}" ]; then
    COMMAND="g++ -std=c++17 $LATESTCPP -O3 -g3 -Wall -Wextra -Wconversion -DONLINE_JUDGE -D_GLIBCXX_DEBUG    -fsanitize=undefined -ftrapv"
else
    COMMAND="clang++ -std=c++17 -stdlib=libc++ $LATESTCPP -O3 -g3 -Wall -Wextra -Wconversion -DONLINE_JUDGE -D_LIBCPP_DEBUG=1 -L/home/simon/tmp/DONTSYNC/clang-git/install/lib/ -fsanitize=undefined,bounds,integer -ftrapv"
fi

echo -e "Executing command:\n  ${COMMAND}"
$($COMMAND)
export COMPILATION_STATUS=$?

if [ "${COMPILATION_STATUS}" -eq "0" ]; then
    echo "Successful"
else
    echo "Failed!!!!!"
fi
