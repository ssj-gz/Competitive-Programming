#! /bin/bash

if [ -z "$AOC_SESSION_COOKIE" ]; then
    echo "Ensure that the AOC_SESSION_COOKIE environment variable is set" >&2
    exit 1
fi

YEAR=$(basename $(realpath "$(pwd)/../"))
DAY_DIR=$(basename $(realpath "$(pwd)"))
if [[ "${DAY_DIR}" =~ day([0-9]+) ]]; then
    DAY=${BASH_REMATCH[1]}

else
    echo "Expected the current directory to be of the form 'dayNNN', not '${DAY_DIR}'" >&2
    exit 1
fi

echo "YEAR: ${YEAR}"
echo "DAY: ${DAY}"

INPUT_URL="https://adventofcode.com/${YEAR}/day/${DAY}/input"
curl -b "session=${AOC_SESSION_COOKIE}" > "day${DAY}_input.txt" "${INPUT_URL}"
touch "day${DAY}_sample_input.txt"
touch "day${DAY}_part1_solution.cpp"
touch "day${DAY}_part2_solution.cpp"
echo -e "all: part1 part2\npart1: day${DAY}_part1_solution.cpp\n\tg++ -std=c++20 -g3 -O3 -Wall -Wextra day${DAY}_part1_solution.cpp -o part1\npart2: day${DAY}_part2_solution.cpp\n\tg++ -std=c++20 -g3 -O3 -Wall -Wextra day${DAY}_part2_solution.cpp -o part2\n" > Makefile
read -r -d '' BLANK_CPP_TEMPLATE << EOM
#include <iostream>

using namespace std;

int main()
{
    return 0;
}
EOM
echo "${BLANK_CPP_TEMPLATE}" > day${DAY}_part1_solution.cpp
echo "${BLANK_CPP_TEMPLATE}" > day${DAY}_part2_solution.cpp




