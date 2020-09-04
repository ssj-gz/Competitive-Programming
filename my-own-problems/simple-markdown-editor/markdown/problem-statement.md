Chef must add some functionality to a simplified Markdown Editor.  The Editor consists of the _current document_; an _Undo Stack_ containing the list of document _revisions_; and an _Undo Stack Poninter_ pointing to the current document in the Undo Stack.  The document consists of only two characters: _Non-Formatting Characters_  (represented by `X`) and _Formatting Characters_ (represented by `*`).

The Editor must _parse_ the current document to give a list of _formatted ranges_, as follows:

Take the first and second occurrences of formatting chars in the document (if they exist); the positions of these form a formatted range.
Take the third and fourth occurrences of formatting chars in the document (if they exist); the positions of these form a formatted range; etc until no formatted chars remain.

For example, given the document:

```
XXX*XXXX*XX*XXXXXX*XX**XX*XXX
```

the first and second formatting chars are at positions 4 and 9, so [4, 9] is a formatted range; the third and fourth are at positions 13 and 21, so [13, 21] is a formatted range; the fifth and sixth are at positions 24 and 25, so [24, 25] is a formatted range.  There is a seventh formatting char at position 28, but no eighth, so there can be no more formatted ranges.

Initially, the Undo Stack consists of just the initial, empty current document, and the Undo Stack Pointer points to this.

Chef must implement the query types listed below and then process a list ${q_1, q_2, \dots q_Q}$ of $Q$ such queries.  Note that these queries are _encrypted_ so that they must be processed _online_.  The decryption uses a _decryption key_ $d$ which has initial value $0$.

1. `F ep`

    Chef must decrypt $ep$ to give $p$ via $p=d \oplus ep$, where $\oplus$ denotes the Xor operator.
    He must then clear all elements in the Undo Stack to the right of the Undo Stack Pointer (if any); then insert a formatting character ('*') at the given $p$ in the current document to give the new current document.
    He must then append this to the Undo Stack and adjust the  Undo Stack Pointer to point to it.
2. `I ec ep`

    Chef must decrypt $ec$ and $ep$ via $e = ec \oplus d$ and $p = ep \oplus d$. He must then handle this query identically to the `F` query, except that instead of adding a formatting character, he must instead insert $c$ non-formatting characters ('X') at the given $p$ in the current document.
3. `U eu`

    Chef must decrypt $eu$ to give $u$ via $u = eu \oplus d$.
    He must then move the Undo Stack Pointer $u$ revisions to the left.
    The document in the Undo Stack that is pointed to by the new Undo Stack Pointer (which is guaranteed to exist) becomes the new current document.
4. `R er`

    Chef must decrypt $er$ to give $r$ via $r = er \oplus d$.
    He must then handle this query identically to the `U` query, except that he moves the Undo Stack Pointer $r$ revisions to the right.
5. `Q ep`

    Chef must decrypt $ep$ to give $p$ via $p = \oplus ep$.
    The character at position $p$ is guaranteed to be a non-formatted char.
    He must then find the answer, $A$, to the query as follows: if, after parsing, the character at position $p$ is contained in a formatted range, then $A$ is the number of non-formatting chars in this formatted range; otherwise, $A=3141592$.
    Once he has found $A$, he must update his decryption key via $d = d + A \cdot 2^i \mod{10^9}$, where $i$ is the number of this query.


Help Chef answer the $Q$ queries.  To prove that you can do it, you only need to print the final value of $d$ for each testcase!

### Input
- The first line of the input contains a single integer $T$ denoting the number of test cases. The description of $T$ test cases follows.
- The first line of each test case contains a single integer $Q$
- Each of the next $Q$ lines begins with a single character equal to one of `N`, `F`, `U`, `R` or `Q`, denoting the type of the query to process, then a space, and then either two space-separated integers (if the single character was `N`) or two space-separated integers (otherwise), representing the encrypted arguments for that query

### Output
For each test case, print a single line containing one integer ― the final value of $\textit{decryptionKey}$ after processing all $Q$ queries.


### Constraints 
- $1 \le T \le 1,000$
- $1 \le Q \le 200,000$
- The length of the document will never exceed $10^{18}$ characters
- For queries beginning with `Q`, the character at the decrypted position $p$ in the current revision of the document is always a `X`
- For queries beginning with `I` or `F`, the decrypted position $p$ is always in the range $1 \dots D + 1$, where $D$ is the length of the current document
- The decrypted value $\textit{numToUndo}$ will never exceed the value of the current Undo Stack Pointer minus 1
- The decrypted value $\textit{numToRedo}$ will never exceed the current Undo Stack size minus the value of the current Undo Stack Pointer
- the sum of $Q$ over all test cases does not exceed $200,000$


### Subtasks
**Subtask #1 (5 points):** 
- $T \le 10$
- $Q \le 100$
- The length of the document will never exceed $1000$ characters

**Subtask #2 (40 points)**: 
- Original constraints, except that there will be no $\textit{undo}$ nor $\textit{redo}$ queries

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
**Processing query 1**. $\textit{decryptionKey} = 0$.
Need to insert $0 \oplus 9 = 9$ non-formatting chars at position $0\oplus 9 = 1$.
```
document: 
          ↑ insert 9 non-formatting chars here
```
**State after processing query 1**:
```
document: XXXXXXXXX
undo stack:  [ "", "XXXXXXXXX" ]
                   ↑ undo stack pointer = 2
```


**Processing query 2**. $\textit{decryptionKey} = 0$.
Need to insert formatting char at position $0\oplus 4 = 4$.
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


**Processing query 3**. $\textit{decryptionKey} = 0$.
Need to insert formatting char at position $0\oplus 7 = 7$.
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


**Processing query 4**. $\textit{decryptionKey} = 0$.
Need to find the size of the formatted range around the position $0 \oplus 5 = 5$.
```
             ┌──┐     ← Formatted ranges
document: XXX*XX*XXXX
              ↑ query the size of formatted range around this point
```
The queried position is part of a formatted range and the number of non-formatted characters is this range is 2; the answer to query #4 is $2$.


Updating $decryptionKey$: 

$$
decryptionKey = decryptionKey + 2\times  2^{4}  = 0 + 2\times 16 = 32 \mod 10^9+7 = 32
$$
**State after processing query 4**:
```
document: XXX*XX*XXXX
undo stack:  [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX" ]
                                              ↑ undo stack pointer = 4
```


**Processing query 5**. $\textit{decryptionKey} = 32$.
Need to insert $32 \oplus 35 = 3$ non-formatting chars at position $32\oplus 35 = 8$.
```
document: XXX*XX*XXXX
                 ↑ insert 3 non-formatting chars here
```
**State after processing query 5**:
```
document: XXX*XX*XXXXXXX
undo stack:  [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX", 
               "XXX*XX*XXXXXXX" ]
               ↑ undo stack pointer = 5
```


**Processing query 6**. $\textit{decryptionKey} = 32$.
Need to insert formatting char at position $32\oplus 42 = 10$.
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


**Processing query 7**. $\textit{decryptionKey} = 32$.
Need to insert formatting char at position $32\oplus 33 = 1$.
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


**Processing query 8**. $\textit{decryptionKey} = 32$.
Need to find the size of the formatted range around the position $32 \oplus 35 = 3$.
```
          ┌───┐  ┌──┐      ← Formatted ranges
document: *XXX*XX*XX*XXXXX
            ↑ query the size of formatted range around this point
```
The queried position is part of a formatted range and the number of non-formatted characters is this range is 3; the answer to query #8 is $3$.


Updating $decryptionKey$: 

$$
decryptionKey = decryptionKey + 3\times  2^{8}  = 32 + 3\times 256 = 800 \mod 10^9+7 = 800
$$
**State after processing query 8**:
```
document: *XXX*XX*XX*XXXXX
undo stack:  [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX", 
               "XXX*XX*XXXXXXX", "XXX*XX*XX*XXXXX", "*XXX*XX*XX*XXXXX" ]
                             undo stack pointer = 7 ↑
```


**Processing query 9**. $\textit{decryptionKey} = 800$.
Need to re-wind the undo stack pointer by $800\oplus803 = 3$ places.


**State after processing query 9**:
```
document: XXX*XX*XXXX
undo stack:  [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX", 
                                              ↑ undo stack pointer = 4
               "XXX*XX*XXXXXXX", "XXX*XX*XX*XXXXX", "*XXX*XX*XX*XXXXX" ]
```


**Processing query 10**. $\textit{decryptionKey} = 800$.
Need to find the size of the formatted range around the position $800 \oplus 810 = 10$.
```
             ┌──┐     ← Formatted ranges
document: XXX*XX*XXXX
                   ↑ query the size of formatted range around this point
```
The queried position is not part of a formatted range; the answer to query #10 is $3141592$.


Updating $decryptionKey$: 

$$
decryptionKey = decryptionKey + 3141592\times  2^{10}  = 800 + 3141592\times 1024 = 3216991008 \mod 10^9+7 = 216990987
$$
**State after processing query 10**:
```
document: XXX*XX*XXXX
undo stack:  [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX", 
                                              ↑ undo stack pointer = 4
               "XXX*XX*XXXXXXX", "XXX*XX*XX*XXXXX", "*XXX*XX*XX*XXXXX" ]
```


**Processing query 11**. $\textit{decryptionKey} = 216990987$.
Need to move forward the undo stack pointer by $216990987\oplus216990986 = 1$ places.


**State after processing query 11**:
```
document: XXX*XX*XXXXXXX
undo stack:  [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX", 
               "XXX*XX*XXXXXXX", "XXX*XX*XX*XXXXX", "*XXX*XX*XX*XXXXX" ]
               ↑ undo stack pointer = 5
```


**Processing query 12**. $\textit{decryptionKey} = 216990987$.
Need to insert formatting char at position $216990987\oplus 216990991 = 4$.
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


**Processing query 13**. $\textit{decryptionKey} = 216990987$.
Need to insert formatting char at position $216990987\oplus 216990977 = 10$.
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


**Processing query 14**. $\textit{decryptionKey} = 216990987$.
Need to find the size of the formatted range around the position $216990987 \oplus 216990978 = 9$.
```
             ┌┐  ┌─┐       ← Formatted ranges
document: XXX**XX*X*XXXXXX
                  ↑ query the size of formatted range around this point
```
The queried position is part of a formatted range and the number of non-formatted characters is this range is 1; the answer to query #14 is $1$.


Updating $decryptionKey$: 

$$
decryptionKey = decryptionKey + 1\times  2^{14}  = 216990987 + 1\times 16384 = 217007371 \mod 10^9+7 = 217007371
$$
**State after processing query 14**:
```
document: XXX**XX*X*XXXXXX
undo stack:  [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX", 
               "XXX*XX*XXXXXXX", "XXX**XX*XXXXXXX", "XXX**XX*X*XXXXXX" ]
                             undo stack pointer = 7 ↑
```


**Processing query 15**. $\textit{decryptionKey} = 217007371$.
Need to re-wind the undo stack pointer by $217007371\oplus217007368 = 3$ places.


**State after processing query 15**:
```
document: XXX*XX*XXXX
undo stack:  [ "", "XXXXXXXXX", "XXX*XXXXXX", "XXX*XX*XXXX", 
                                              ↑ undo stack pointer = 4
               "XXX*XX*XXXXXXX", "XXX**XX*XXXXXXX", "XXX**XX*X*XXXXXX" ]
```


**Processing query 16**. $\textit{decryptionKey} = 217007371$.
Need to insert $217007371 \oplus 217007374 = 5$ non-formatting chars at position $217007371\oplus 217007374 = 5$.
```
document: XXX*XX*XXXX
              ↑ insert 5 non-formatting chars here
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


**Processing query 17**. $\textit{decryptionKey} = 217007371$.
Need to find the size of the formatted range around the position $217007371 \oplus 217007372 = 7$.
```
             ┌───────┐     ← Formatted ranges
document: XXX*XXXXXXX*XXXX
                ↑ query the size of formatted range around this point
```
The queried position is part of a formatted range and the number of non-formatted characters is this range is 7; the answer to query #17 is $7$.


Updating $decryptionKey$: 

$$
decryptionKey = decryptionKey + 7\times  2^{17}  = 217007371 + 7\times 131072 = 217924875 \mod 10^9+7 = 217924875
$$


That's all the queries processed, and the final value of $\textit{decryptionKey}$ is 217924875; so the answer for this testcase is 217924875.


