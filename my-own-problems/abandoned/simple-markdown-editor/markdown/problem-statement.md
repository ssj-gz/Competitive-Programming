Chef has been asked to extend a simplified Markdown Editor.  The Editor has Undo/ Redo capability, so has an _Undo Stack_ $S$ containing the _document revisions_.  Each _document revision_ is a string where each character is an `X` (a _non-formatting character_)  or a `*` (a _formatting character_).  The _current document_, denoted $D$, is always equal to $S[\textit{SP}]$, where $\textit{SP}$ is the  _Undo Stack Pointer_.  

The Editor must be able to _parse_ $D$ into a list $P(D)$ of _formatted ranges_, as follows:

* Form a sorted list $L$ of the positions of all formatting characters in $D$, and set $P(D)=[]$
* While $|L| \ge 2$, append the _formatted range_ $[L(1), L(2)]$ to $P(D)$ and pop these first two items from the front of $L$

For example, given $D=$ `XXX*XXXX*XX*XXXXXX*XX**XX*XXX`, we form $L$ = [4, 9, 13, 21, 24, 25, 28].  $|L|=7$, so we append $[L[1], L[2]]=[4,9]$ to $P(D)$ and pop the first two elements from $L$.  Now $|L|=5$, and we append $[13,21]$ to $P(D)$ and pop the first two elements.  $|L|=3$ so we append $[24, 25]$ and pop.  $|L|=1<2$ now so there are no more formatted ranges and the final $P(D)$ is $[[4,9], [13,21], [24,25]]$.

Chef must extend the Editor to handle five specific types of query, and then process a list ${q_1, q_2, \dots, q_Q}$ of $Q$ such queries.  Note that these queries are _encrypted_ so that they must be processed _online_.  The decryption uses a _decryption key_ $d$ which has initial value $0$. Prior to processing the queries, we have $S=[$""$]$, $\textit{SP}=1$ and so $D=$ "".

Each query $q_i$ is of one of the five types `F`, `N`, `U`, `R` and `?` and each types' arguments and instructions for handling are as follows:

1. `F ep`

    Chef must decrypt $\textit{ep}$ to give $p$ via $p=d \oplus \textit{ep}$, where $\oplus$ denotes the Xor operator.
    He must then pop the last $|S| - SP$ elements from $S$ and insert a formatting character (`*`) at the given $p$ in $D$.
    Finally, he must append the updated $D$ to $S$ and increment $\textit{SP}$ so that it points to it.
2. `N ec ep`

    Chef must decrypt $\textit{ec}$ and $\textit{ep}$ via $c = d \oplus \textit{ec}$ and $p = d \oplus \textit{ep}$. He must then handle this query identically to the `F` query, except that instead of adding a `*` at position $p$ in $D$, he must insert $c$ non-formatting characters (`X`).
3. `U eu`

    Chef must decrypt $\textit{eu}$ to give $u$ via $u = d \oplus \textit{eu}$.
    He must then move the Undo Stack Pointer $u$ revisions to the left i.e. subtract $u$ from $\textit{SP}$ and set $D=S[\textit{SP}]$.
4. `R er`

    Chef must decrypt $\textit{er}$ to give $r$ via $r = d \oplus \textit{er}$.
    He must then handle this query identically to the `U` query, except that $\textit{SP}$ has $r$ added to it.
5. `? ep`

    Chef must decrypt $\textit{ep}$ to give $p$ via $p = d \oplus \textit{ep}$.
    He must then update his decryption key via $d = (d + A \cdot 2^i) \,\%\, (10^9+7)$, where $i$ is the number of this query and where $A$ is defined as follows: if there is some formatted range $I$ in $P(D)$ with $p \in I$, then $A$ is the number of ``X``s in the range $I$ in $D$; otherwise, $A=3141592$.

Help Chef process the $Q$ queries.  To prove that you can do it, you need only print the final value of $d$ for each test case!

### Input
- The first line of the input contains a single integer $T$ denoting the number of test cases. The description of $T$ test cases follows.
- The first line of each test case contains a single integer $Q$.
- Each of the next $Q$ lines begins with a single character equal to one of `N`, `F`, `U`, `R` or `?`, denoting the type of the query to process, then a space, and then either two space-separated integers (if the query type is `N`) or a single integer (otherwise) representing the encrypted arguments for that query.

### Output
For each test case, print a single line containing one integer ― the final value of $d$ after processing all $Q$ queries.

### Constraints 
- $1 \le T \le 1,000$
- $1 \le Q \le 500,000$
- The length of $D$ will never exceed $10^{18}$ characters
- For queries of type `?`, the character at the decrypted position $p$ in $D$ is always `X`
- For queries of type `N` or `F`, the decrypted position $p$ is always in the range $1 \dots |D| + 1$
- After processing a query of type `U` or `R`, it is guaranteed that $\textit{SP}$ remains in the range $1 \dots |S|$
- the sum of $Q$ over all test cases does not exceed $500,000$

### Subtasks
**Subtask #1 (5 points):** 
- $T \le 10$
- $Q \le 100$
- The length of $D$ will never exceed $1000$ characters

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
```

### Example Output
```
217924875
```

### Explanation
**Initial status**: 
```
D: 
S: [ "" ]
     ↑ SP = 1
```
**Processing query 1**.
Need to insert $c = d \oplus 9 = 0 \oplus 9 = 9$ ``X``s at position $p=0\oplus 9 = 1$.
```
D: 
   ↑ insert 9 Xs here
```
**State after processing query 1**:
```
D: XXXXXXXXX
S: [ "", "XXXXXXXXX" ]
         ↑ SP = 2
```

**Processing query 2**.
Need to insert a `*` at position $p = d \oplus 4 = 0\oplus 4 = 4$.
```
D: XXXXXXXXX
      ↑ insert * here
```
**State after processing query 2**:
```
D: XXX*XXXXXX
S: [ "", "XXXXXXXXX", "XXX*XXXXXX" ]
                      ↑ SP = 3
```

**Processing query 3**.
Need to insert a `*` at position $p = d \oplus 7 = 0\oplus 7 = 7$.
```
D: XXX*XXXXXX
         ↑ insert * here
```
**State after processing query 3**:
```
D: XXX*XX*XXXX
S: [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX" ]
                                    ↑ SP = 4
```

**Processing query 4**.
Need to find the size of the formatted range around the position $p = d \oplus 5 = 0 \oplus 5 = 5$.
```
      ┌──┐     ← Formatted ranges (P(D))
D: XXX*XX*XXXX
       ↑ check for formatted range around this point
```
The position $p$ is part of a formatted range and the number of ``X``s is this range is 2; so $A = 2$.

Update $d$: $d = (0 + 2 \cdot  2^{4}) \,\%\, (10^9+7)  = 32$.

**State after processing query 4**:
```
D: XXX*XX*XXXX
S: [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX" ]
                                    ↑ SP = 4
```

**Processing query 5**.
Need to insert $c = d \oplus 35 = 32 \oplus 35 = 3$ ``X``s at position $p=32\oplus 35 = 8$.
```
D: XXX*XX*XXXX
          ↑ insert 3 Xs here
```
**State after processing query 5**:
```
D: XXX*XX*XXXXXXX
S: [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX", "XXX*XX*XXXXXXX" ]
                                                   ↑ SP = 5
```

**Processing query 6**.
Need to insert a `*` at position $p = d \oplus 42 = 32\oplus 42 = 10$.
```
D: XXX*XX*XXXXXXX
            ↑ insert * here
```
**State after processing query 6**:
```
D: XXX*XX*XX*XXXXX
S: [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX", "XXX*XX*XXXXXXX", 
     "XXX*XX*XX*XXXXX" ]
     ↑ SP = 6
```

**Processing query 7**.
Need to insert a `*` at position $p = d \oplus 33 = 32\oplus 33 = 1$.
```
D: XXX*XX*XX*XXXXX
   ↑ insert * here
```
**State after processing query 7**:
```
D: *XXX*XX*XX*XXXXX
S: [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX", "XXX*XX*XXXXXXX", 
     "XXX*XX*XX*XXXXX", "*XXX*XX*XX*XXXXX" ]
                        ↑ SP = 7
```

**Processing query 8**.
Need to find the size of the formatted range around the position $p = d \oplus 35 = 32 \oplus 35 = 3$.
```
   ┌───┐  ┌──┐      ← Formatted ranges (P(D))
D: *XXX*XX*XX*XXXXX
     ↑ check for formatted range around this point
```
The position $p$ is part of a formatted range and the number of ``X``s is this range is 3; so $A = 3$.

Update $d$: $d = (32 + 3 \cdot  2^{8}) \,\%\, (10^9+7)  = 800$.

**State after processing query 8**:
```
D: *XXX*XX*XX*XXXXX
S: [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX", "XXX*XX*XXXXXXX", 
     "XXX*XX*XX*XXXXX", "*XXX*XX*XX*XXXXX" ]
                        ↑ SP = 7
```

**Processing query 9**.
Need to re-wind the undo stack pointer by $u = d \oplus 803 = 800 \oplus 803 = 3$ places.

**State after processing query 9**:
```
D: XXX*XX*XXXX
S: [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX", "XXX*XX*XXXXXXX", 
                                    ↑ SP = 4
     "XXX*XX*XX*XXXXX", "*XXX*XX*XX*XXXXX" ]
```

**Processing query 10**.
Need to find the size of the formatted range around the position $p = d \oplus 810 = 800 \oplus 810 = 10$.
```
      ┌──┐     ← Formatted ranges (P(D))
D: XXX*XX*XXXX
            ↑ check for formatted range around this point
```
The position $p$ is not part of a formatted range; $A = 3141592$.

Update $d$: $d = (800 + 3141592 \cdot  2^{10}) \,\%\, (10^9+7)  = 216990987$.

**State after processing query 10**:
```
D: XXX*XX*XXXX
S: [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX", "XXX*XX*XXXXXXX", 
                                    ↑ SP = 4
     "XXX*XX*XX*XXXXX", "*XXX*XX*XX*XXXXX" ]
```

**Processing query 11**.
Need to move forward the undo stack pointer by $r = d \oplus 216990986 = 216990987 \oplus 216990986 = 1$ places.

**State after processing query 11**:
```
D: XXX*XX*XXXXXXX
S: [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX", "XXX*XX*XXXXXXX", 
                                                   ↑ SP = 5
     "XXX*XX*XX*XXXXX", "*XXX*XX*XX*XXXXX" ]
```

**Processing query 12**.
Need to insert a `*` at position $p = d \oplus 216990991 = 216990987\oplus 216990991 = 4$.
```
D: XXX*XX*XXXXXXX
      ↑ insert * here
```
The undo stack has elements to the right of the pointer, which we need to erase; new Undo Stack after erase: 
```
S: [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX", "XXX*XX*XXXXXXX" ]
                                                   ↑ SP = 5
```

We can now perform the insertion, giving the updated document:

```
D: XXX**XX*XXXXXXX
```
**State after processing query 12**:
```
D: XXX**XX*XXXXXXX
S: [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX", "XXX*XX*XXXXXXX", 
     "XXX**XX*XXXXXXX" ]
     ↑ SP = 6
```

**Processing query 13**.
Need to insert a `*` at position $p = d \oplus 216990977 = 216990987\oplus 216990977 = 10$.
```
D: XXX**XX*XXXXXXX
            ↑ insert * here
```
**State after processing query 13**:
```
D: XXX**XX*X*XXXXXX
S: [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX", "XXX*XX*XXXXXXX", 
     "XXX**XX*XXXXXXX", "XXX**XX*X*XXXXXX" ]
                        ↑ SP = 7
```

**Processing query 14**.
Need to find the size of the formatted range around the position $p = d \oplus 216990978 = 216990987 \oplus 216990978 = 9$.
```
      ┌┐  ┌─┐       ← Formatted ranges (P(D))
D: XXX**XX*X*XXXXXX
           ↑ check for formatted range around this point
```
The position $p$ is part of a formatted range and the number of ``X``s is this range is 1; so $A = 1$.

Update $d$: $d = (216990987 + 1 \cdot  2^{14}) \,\%\, (10^9+7)  = 217007371$.

**State after processing query 14**:
```
D: XXX**XX*X*XXXXXX
S: [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX", "XXX*XX*XXXXXXX", 
     "XXX**XX*XXXXXXX", "XXX**XX*X*XXXXXX" ]
                        ↑ SP = 7
```

**Processing query 15**.
Need to re-wind the undo stack pointer by $u = d \oplus 217007368 = 217007371 \oplus 217007368 = 3$ places.

**State after processing query 15**:
```
D: XXX*XX*XXXX
S: [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX", "XXX*XX*XXXXXXX", 
                                    ↑ SP = 4
     "XXX**XX*XXXXXXX", "XXX**XX*X*XXXXXX" ]
```

**Processing query 16**.
Need to insert $c = d \oplus 217007374 = 217007371 \oplus 217007374 = 5$ ``X``s at position $p=217007371\oplus 217007374 = 5$.
```
D: XXX*XX*XXXX
       ↑ insert 5 Xs here
```
The undo stack has elements to the right of the pointer, which we need to erase; new Undo Stack after erase: 
```
S: [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX" ]
                                    ↑ SP = 4
```

We can now perform the insertion, giving the updated document:

```
D: XXX*XXXXXXX*XXXX
```
**State after processing query 16**:
```
D: XXX*XXXXXXX*XXXX
S: [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX", "XXX*XXXXXXX*XXXX" ]
                                                   ↑ SP = 5
```

**Processing query 17**.
Need to find the size of the formatted range around the position $p = d \oplus 217007372 = 217007371 \oplus 217007372 = 7$.
```
      ┌───────┐     ← Formatted ranges (P(D))
D: XXX*XXXXXXX*XXXX
         ↑ check for formatted range around this point
```
The position $p$ is part of a formatted range and the number of ``X``s is this range is 7; so $A = 7$.

Update $d$: $d = (217007371 + 7 \cdot  2^{17}) \,\%\, (10^9+7)  = 217924875$.

That's all the queries processed, and the final value of $d$ is $217924875$; so the answer for this test case is $217924875$.
