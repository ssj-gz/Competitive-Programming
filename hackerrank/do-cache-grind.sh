#! /bin/bash

if [[ $# -gt 0 ]]; then
  export EXECUTABLE=$1
else
  export EXECUTABLE=./a.out
fi


rm cachegrind.out.*
valgrind --tool=cachegrind $EXECUTABLE
