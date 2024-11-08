#! /bin/bash

if [ "$1" != "1" ] && [ "$1" != "2" ]; then
    echo "Expected: $0 <1|2>"
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


if [ "$1" == "1" ]; then
    git add "day${DAY}_part1_solution.cpp" *.txt && git commit . -m "Add AoC ${YEAR} Day ${DAY} Part 1 input and (correct) solution" && git log --name-only .
else
    git add "day${DAY}_part2_solution.cpp" *.txt && git commit . -m "Add AoC ${YEAR} Day ${DAY} Part 2 (correct) solution" && git log --name-only .
fi

