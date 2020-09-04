#! /bin/bash

g++ -std=c++17 simple-markdown-editor.cpp -O3 -g3 -Wall -DDIAGNOSTICS && echo "1
17
N 1 9
F 4
F 7
? 5
N 40 35
F 42
F 33
? 35
U 803
? 810
R 216990986
F 216990991
F 216990977
? 216990978
U 217007368
N 217007374 217007374
? 217007372
" | ./a.out > explanation.md && sed -i '/# Explanation/q' markdown/problem-statement.md ; cat explanation.md >> markdown/problem-statement.md

