#! /bin/bash

for cheat_name in CHEAT_NO_PROPER_DESCENDANT_PREFIX_SUM CHEAT_PHASE_THREE; do
     g++ -std=c++14 move-the-coins-2-test-cheat.cpp -o move-the-coins-2-test-cheat-${cheat_name}  -O3 -g3 -Wall -Wextra -D${cheat_name}
done
