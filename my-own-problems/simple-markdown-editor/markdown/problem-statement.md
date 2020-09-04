Chef must add some functionality to a simplified Markdown Editor.  The Editor consists of the _current document_, $D$; an _Undo Stack_, $S$ containing the list of document _revisions_; and an _Undo Stack Pointer_ $\textit{SP}$ pointing to the current document in $S$.  Each character in $D$ may only be one of the following: a _Non-Formatting Character_  (`X`) or a _Formatting Character_ (`*`).

The Editor must be able to _parse_ $D$ into a list of _formatted ranges_, as follows:

Take the first and second occurrences of formatting characters in the document (if they exist); the positions of these form a formatted range.
Take the third and fourth occurrences of formatting characters in the document (if they exist); the positions of these form a formatted range; etc until no formatting characters remain.

For example, given the document:

```
XXX*XXXX*XX*XXXXXX*XX**XX*XXX
```

the first and second formatting characters are at positions 4 and 9, so [4, 9] is a formatted range; the third and fourth are at positions 13 and 21, so [13, 21] is a formatted range; the fifth and sixth are at positions 24 and 25, so [24, 25] is a formatted range.  There is a seventh formatting char at position 28, but no eighth, so there can be no more formatted ranges.

Initially, $D$ is the empty string; $S=[D]$, and $\textit{SP}$ points to $D$ in $S$ i.e. $\textit{SP}=1$.

Chef must implement the query types listed below and then process a list ${q_1, q_2, \dots q_Q}$ of $Q$ such queries.  Note that these queries are _encrypted_ so that they must be processed _online_.  The decryption uses a _decryption key_ $d$ which has initial value $0$.

1. `F ep`

    Chef must decrypt $ep$ to give $p$ via $p=d \oplus ep$, where $\oplus$ denotes the Xor operator.
    He must then clear all elements in the $S$ to the right of $SP$ (if any); then insert a formatting character ('*') at the given $p$ in $D$ to give the new current document $D$.
    He must then append the new $D$ to $S$ and adjust $\textit{SP}$ to point to it.
2. `I ec ep`

    Chef must decrypt $ec$ and $ep$ via $e = ec \oplus d$ and $p = ep \oplus d$. He must then handle this query identically to the `F` query, except that instead of adding a formatting character at position $p$ in $D$, he must insert $c$ non-formatting characters ('X').
3. `U eu`

    Chef must decrypt $eu$ to give $u$ via $u = eu \oplus d$.
    He must then move the Undo Stack Pointer $u$ revisions to the left i.e. subtract $u$ from $\textit{SP}$.  It is guaranteed that $u < \textit{SP}$.
    $D$ is then set to the document revision in $S$ that is pointed to by the new $\textit{SP}$.
4. `R er`

    Chef must decrypt $er$ to give $r$ via $r = er \oplus d$.
    He must then handle this query identically to the `U` query, except that adds $v$ to $\textit{SP}$. It is guaranteed that $r < |S| - \textit{SP}$.
5. `Q ep`

    Chef must decrypt $ep$ to give $p$ via $p = \oplus ep$.
    The character at position $p$ is guaranteed to be a non-formatting char.
    He must then find the answer, $A$, to the query as follows: if, after parsing, the character at position $p$ is contained in a formatted range, then $A$ is the number of non-formatting characters in this formatted range; otherwise, $A=3141592$.
    Once he has found $A$, he must update his decryption key via $d = d + A \cdot 2^i \mod{10^9}$, where $i$ is the number of this query.

Help Chef answer the $Q$ queries.  To prove that you can do it, you only need to print the final value of $d$ for each test case!

### Input
- The first line of the input contains a single integer $T$ denoting the number of test cases. The description of $T$ test cases follows.
- The first line of each test case contains a single integer $Q$
- Each of the next $Q$ lines begins with a single character equal to one of `N`, `F`, `U`, `R` or `Q`, denoting the type of the query to process, then a space, and then either two space-separated integers (if the single character was `N`) or two space-separated integers (otherwise), representing the encrypted arguments for that query

### Output
For each test case, print a single line containing one integer ― the final value of $d$ after processing all $Q$ queries.

### Constraints 
- $1 \le T \le 1,000$
- $1 \le Q \le 200,000$
- The length $D$ will never exceed $10^{18}$ characters
- For queries of type `Q`, the character at the decrypted position $p$ in $D$ is always a `X`
- For queries of type `I` or `F`, the decrypted position $p$ is always in the range $1 \dots |D| + 1$
- After processing a query of type `U` or `R`, it is guaranteed that $\textit{SP}$ still satisfies $1 \le \textit{SP} \le |S|$
- the sum of $Q$ over all test cases does not exceed $200,000$

### Subtasks
**Subtask #1 (5 points):** 
- $T \le 10$
- $Q \le 100$
- The length $D$ will never exceed $1000$ characters

**Subtask #2 (40 points)**: 
- Original constraints, except that there will be no queries of type `U` nor `R`

**Subtask #3 (55 points)**: original constraints

### Example Input
```
1
17
N 1 9
F 4
F 7
Q 5
N 40 35
F 42
F 33
Q 35
U 803
Q 810
R 216990986
F 216990991
F 216990977
Q 216990978
U 217007368
N 217007374 217007374
Q 217007372
```

### Example Output
```
217924875
```

### Explanation
**Initial status**: 
```
document: 
undo stack:  [ "" ]
               ↑ undo stack pointer = 1
```
**Processing query 1**.
Need to insert $c = d \oplus 9 = 0 \oplus 9 = 9$ non-formatting characters at position $p=0\oplus 9 = 1$.
```
document: 
          ↑ insert 9 non-formatting characters here
```
**State after processing query 1**:
```
document: XXXXXXXXX
undo stack:  [ "", "XXXXXXXXX" ]
                   ↑ undo stack pointer = 2
```

**Processing query 2**.
Need to insert formatting char at position $p = d \oplus 4 = 0\oplus 4 = 4$.
```
document: XXXXXXXXX
             ↑ insert formatting char here
```
**State after processing query 2**:
```
document: XXX*XXXXXX
undo stack:  [ "", "XXXXXXXXX", "XXX*XXXXXX" ]
                                ↑ undo stack pointer = 3
```

**Processing query 3**.
Need to insert formatting char at position $p = d \oplus 7 = 0\oplus 7 = 7$.
```
document: XXX*XXXXXX
                ↑ insert formatting char here
```
**State after processing query 3**:
```
document: XXX*XX*XXXX
undo stack:  [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX" ]
                                              ↑ undo stack pointer = 4
```

**Processing query 4**.
Need to find the size of the formatted range around the position $p = d \oplus 5 = 0 \oplus 5 = 5$.
```
             ┌──┐     ← Formatted ranges
document: XXX*XX*XXXX
              ↑ query the size of formatted range around this point
```
The queried position is part of a formatted range and the number of non-formatted characters is this range is 2; the answer to query #4 is $2$.

Update $d$: $d = d + 2\times  2^{4}  = 0 + 2\times 16 = 32 \mod 10^9+7 = 32$.


**State after processing query 4**:
```
document: XXX*XX*XXXX
undo stack:  [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX" ]
                                              ↑ undo stack pointer = 4
```

**Processing query 5**.
Need to insert $c = d \oplus 35 = 32 \oplus 35 = 3$ non-formatting characters at position $p=32\oplus 35 = 8$.
```
document: XXX*XX*XXXX
                 ↑ insert 3 non-formatting characters here
```
**State after processing query 5**:
```
document: XXX*XX*XXXXXXX
undo stack:  [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX", 
               "XXX*XX*XXXXXXX" ]
               ↑ undo stack pointer = 5
```

**Processing query 6**.
Need to insert formatting char at position $p = d \oplus 42 = 32\oplus 42 = 10$.
```
document: XXX*XX*XXXXXXX
                   ↑ insert formatting char here
```
**State after processing query 6**:
```
document: XXX*XX*XX*XXXXX
undo stack:  [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX", 
               "XXX*XX*XXXXXXX", "XXX*XX*XX*XXXXX" ]
                                 ↑ undo stack pointer = 6
```

**Processing query 7**.
Need to insert formatting char at position $p = d \oplus 33 = 32\oplus 33 = 1$.
```
document: XXX*XX*XX*XXXXX
          ↑ insert formatting char here
```
**State after processing query 7**:
```
document: *XXX*XX*XX*XXXXX
undo stack:  [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX", 
               "XXX*XX*XXXXXXX", "XXX*XX*XX*XXXXX", "*XXX*XX*XX*XXXXX" ]
                             undo stack pointer = 7 ↑
```

**Processing query 8**.
Need to find the size of the formatted range around the position $p = d \oplus 35 = 32 \oplus 35 = 3$.
```
          ┌───┐  ┌──┐      ← Formatted ranges
document: *XXX*XX*XX*XXXXX
            ↑ query the size of formatted range around this point
```
The queried position is part of a formatted range and the number of non-formatted characters is this range is 3; the answer to query #8 is $3$.

Update $d$: $d = d + 3\times  2^{8}  = 32 + 3\times 256 = 800 \mod 10^9+7 = 800$.


**State after processing query 8**:
```
document: *XXX*XX*XX*XXXXX
undo stack:  [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX", 
               "XXX*XX*XXXXXXX", "XXX*XX*XX*XXXXX", "*XXX*XX*XX*XXXXX" ]
                             undo stack pointer = 7 ↑
```

**Processing query 9**.
Need to re-wind the undo stack pointer by $u = d \oplus 803 = 800 \oplus 803 = 3$ places.

**State after processing query 9**:
```
document: XXX*XX*XXXX
undo stack:  [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX", 
                                              ↑ undo stack pointer = 4
               "XXX*XX*XXXXXXX", "XXX*XX*XX*XXXXX", "*XXX*XX*XX*XXXXX" ]
```

**Processing query 10**.
Need to find the size of the formatted range around the position $p = d \oplus 810 = 800 \oplus 810 = 10$.
```
             ┌──┐     ← Formatted ranges
document: XXX*XX*XXXX
                   ↑ query the size of formatted range around this point
```
The queried position is not part of a formatted range; the answer to query #10 is $3141592$.

Update $d$: $d = d + 3141592\times  2^{10}  = 800 + 3141592\times 1024 = 3216991008 \mod 10^9+7 = 216990987$.


**State after processing query 10**:
```
document: XXX*XX*XXXX
undo stack:  [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX", 
                                              ↑ undo stack pointer = 4
               "XXX*XX*XXXXXXX", "XXX*XX*XX*XXXXX", "*XXX*XX*XX*XXXXX" ]
```

**Processing query 11**.
Need to move forward the undo stack pointer by $r = d \oplus 216990986 = 216990987 \oplus 216990986 = 1$ places.

**State after processing query 11**:
```
document: XXX*XX*XXXXXXX
undo stack:  [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX", 
               "XXX*XX*XXXXXXX", "XXX*XX*XX*XXXXX", "*XXX*XX*XX*XXXXX" ]
               ↑ undo stack pointer = 5
```

**Processing query 12**.
Need to insert formatting char at position $p = d \oplus 216990991 = 216990987\oplus 216990991 = 4$.
```
document: XXX*XX*XXXXXXX
             ↑ insert formatting char here
```
The undo stack has elements to the right of the pointer, which we need to erase; new Undo Stack after erase: 
```
undo stack:  [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX", 
               "XXX*XX*XXXXXXX" ]
               ↑ undo stack pointer = 5
```

We can now perform the insertion, giving the updated document:

```
document: XXX**XX*XXXXXXX
```
**State after processing query 12**:
```
document: XXX**XX*XXXXXXX
undo stack:  [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX", 
               "XXX*XX*XXXXXXX", "XXX**XX*XXXXXXX" ]
                                 ↑ undo stack pointer = 6
```

**Processing query 13**.
Need to insert formatting char at position $p = d \oplus 216990977 = 216990987\oplus 216990977 = 10$.
```
document: XXX**XX*XXXXXXX
                   ↑ insert formatting char here
```
**State after processing query 13**:
```
document: XXX**XX*X*XXXXXX
undo stack:  [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX", 
               "XXX*XX*XXXXXXX", "XXX**XX*XXXXXXX", "XXX**XX*X*XXXXXX" ]
                             undo stack pointer = 7 ↑
```

**Processing query 14**.
Need to find the size of the formatted range around the position $p = d \oplus 216990978 = 216990987 \oplus 216990978 = 9$.
```
             ┌┐  ┌─┐       ← Formatted ranges
document: XXX**XX*X*XXXXXX
                  ↑ query the size of formatted range around this point
```
The queried position is part of a formatted range and the number of non-formatted characters is this range is 1; the answer to query #14 is $1$.

Update $d$: $d = d + 1\times  2^{14}  = 216990987 + 1\times 16384 = 217007371 \mod 10^9+7 = 217007371$.


**State after processing query 14**:
```
document: XXX**XX*X*XXXXXX
undo stack:  [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX", 
               "XXX*XX*XXXXXXX", "XXX**XX*XXXXXXX", "XXX**XX*X*XXXXXX" ]
                             undo stack pointer = 7 ↑
```

**Processing query 15**.
Need to re-wind the undo stack pointer by $u = d \oplus 217007368 = 217007371 \oplus 217007368 = 3$ places.

**State after processing query 15**:
```
document: XXX*XX*XXXX
undo stack:  [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX", 
                                              ↑ undo stack pointer = 4
               "XXX*XX*XXXXXXX", "XXX**XX*XXXXXXX", "XXX**XX*X*XXXXXX" ]
```

**Processing query 16**.
Need to insert $c = d \oplus 217007374 = 217007371 \oplus 217007374 = 5$ non-formatting characters at position $p=217007371\oplus 217007374 = 5$.
```
document: XXX*XX*XXXX
              ↑ insert 5 non-formatting characters here
```
The undo stack has elements to the right of the pointer, which we need to erase; new Undo Stack after erase: 
```
undo stack:  [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX" ]
                                              ↑ undo stack pointer = 4
```

We can now perform the insertion, giving the updated document:

```
document: XXX*XXXXXXX*XXXX
```
**State after processing query 16**:
```
document: XXX*XXXXXXX*XXXX
undo stack:  [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX", 
               "XXX*XXXXXXX*XXXX" ]
               ↑ undo stack pointer = 5
```

**Processing query 17**.
Need to find the size of the formatted range around the position $p = d \oplus 217007372 = 217007371 \oplus 217007372 = 7$.
```
             ┌───────┐     ← Formatted ranges
document: XXX*XXXXXXX*XXXX
                ↑ query the size of formatted range around this point
```
The queried position is part of a formatted range and the number of non-formatted characters is this range is 7; the answer to query #17 is $7$.

Update $d$: $d = d + 7\times  2^{17}  = 217007371 + 7\times 131072 = 217924875 \mod 10^9+7 = 217924875$.



That's all the queries processed, and the final value of $d$ is 217924875; so the answer for this test case is 217924875.
