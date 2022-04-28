# Competitive Programming Solutions

My solutions to over 350+ Competitive Programming (CP) Problems, drawn from:

* [Hackerrank](hackerrank/) (my [account](https://www.hackerrank.com/ssjgz?hr_r=1); top 0.4%)
* [Codechef](codechef/) (my [account](https://www.codechef.com/users/ssjgz/); 6\*-Rating); and 
* [Advent of Code](adventofcode/)

For the uninitiated, CP Problems require you to write code to perform some computation on a range of _test inputs_ and output the results.  Submitting your Solution (constrained to a single source file) uploads it to the _Online Judge_, which automatically compiles it and feeds it the prepared test inputs.  A Solution is given a perfect score only if each outputted result is correct, _and_ (the tricky bit!) completes its processing within the allotted time (typically, 2 seconds per test input).

All but the most trivial Problems adhere to the following pattern: "there is an algorithm that solves the Problem that is _simple but too slow_, and to succeed you must create an algorithm that is _(worst-case asymptotically) optimal_."  The difficulty of creating this algorithm ranges from "easy" to "_very_ hard!" :)

All (non-Contest) Solutions in this repo received perfect scores.  Most are *reasonably* well-written, with [some](contests/codechef-october-2019a/bacterial-reproduction.cpp) having "Editorial"-style high level overviews (the asymptotically-optimal algorithms can be decidedly non-obvious).  Many are ... not as high quality, though :)

Some were written during live [contests](contests/), but the majority are Practice problems that can be done at your own pace.

This repo also contains some of [my own problems](my-own-problems/), including ones that have been published ([Chef and Gordon Ramsay 2](my-own-problems/equilateral-treeangles/) (née "Equilateral Treeangles"), [Move the Coins 2!](my-own-problems/counters-on-a-tree/) and [Move the Coins - Creating Tests](my-own-problems/move-the-coins-2-test/)), and others that (sadly) ended up [not being workable](my-own-problems/abandoned/).  Most also contain Testcase Generators (and shared helper utils), Problem Statements, Editorials, Python-based [manim](https://github.com/3b1b/manim) [animations](my-own-problems/counters-on-a-tree/markdown/animation-manim-source/) etc so you can see how much work goes into creating the things (2426 commits worth, apparently! ;))

The whole repo contains **~74k lines of C++** code (after stripping whitespace and comments), according to [tokei](https://github.com/XAMPPRocky/tokei).


