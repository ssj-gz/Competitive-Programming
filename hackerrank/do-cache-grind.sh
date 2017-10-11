#! /bin/bash

rm cachegrind.out.*
valgrind --tool=cachegrind ./a.out 
