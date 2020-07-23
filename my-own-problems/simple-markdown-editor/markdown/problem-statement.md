Chef has been tasked with the implemention of part of a simple markdown editor.  The simplification comes from the fact that the markdown document being edited can consist only of two characters: _Non-Formatting Characters_  (represented by `X`) and _Formatting Characters_ (represented by `*`).  The editor also has an _Undo Stack_ containing the list of document _revisions_, and an _Undo Stack Pointer_, pointing to the current revision of the document.

Initially, the document is empty, and the Undo Stack is a list containing a single element: this empty document.  The Undo Stack Pointer initially points at this element.

The document is _parsed_ to give the list of _formatted ranges_ of characters in the document as follows:

Take the first and second occurrences of formatting chars in the document (if they exist); the positions of these form a formatted range.
Take the third and fourth occurrences of formatting chars in the document (if they exist); the positions of these form a formatted range; etc until no formatted chars remain.

For example, given the document:

```
XXX*XXXX*XX*XXXXXX*XX**XX*XXX
```

the first and second formatting chars are at positions 4 and 9, so [4, 9] is a formatted range; the third and fourth are at positions 13 and 21, so [13, 21] is a formatted range; the fifth and sixth are at positions 24 and 25, so [24, 25] is a formatted range.  There is a seventh formatting char at position 28, but no eighth, so there can be no more formatted ranges.

Chef must implement the following queries.  Note that positions of characters in the document are always 1-relative.  

1. $\textit{insertFormattingChar}(\textit{positionToInsert})$.  Chef must first clear all elements in the Undo Stack to the right of the Undo Stack Pointer (if any); then insert a formatting character ('*') at the given $\textit{positionToInsert}$ in the current document; then append the resulting document to the Undo Stack and increment the Undo Stack Pointer so that it points to the new current document.
2. $\textit{insertNonFormattingChars}(\textit{numCharsToInsert}, \textit{positionToInsert})$.  Chef must handle this in an identical way to $\textit{insertFormattingChar}$, except instead of adding a formatting character, he must instead insert $\textit{numCharsToInsert}$ non-formatting characters ('X') at the given $\textit{positionToInsert}$ in the current document.
3. $\textit{undo}(\textit{numToUndo})$.  Chef must move the Undo Stack Pointer $\textit{numToUndo}$ revisions to the left.  The document in the Undo Stack that is pointed to by the updated Undo Stack Pointer (which is guaranteed to exist) becomes the new current document.
4. $\textit{redo}(\textit{numToRedo})$.  Chef must move the Undo Stack Pointer $\textit{numToRedo}$ revisions to the right.  The document in the Undo Stack that is pointed to by the updated Undo Stack Pointer (which is guaranteed to exist) becomes the new current document.
5. $\textit{numInFormattedRange}(\textit{queryPosition})$. If the character at $\textit{queryPosition}$ - which is guaranteed to be a non-formatted char - is contained in a formatted range, then Chef's answer to the query is the number of non-formatting chars in this formatted range; otherwise, the answer to the query is 3'141'592.

More formally, Chef must answer $Q$ queries ${q_1, q_2, \dots q_Q}$, with each query being one of the above types. The queries are *encrypted* in such a way that Chef can't decrypt the next query until he has processed all previous queries i.e. the queries must be processed _online_.  Chef has a $\textit{decryptionKey}$ (with initial value $0$) which is used to decrypt these queries and which he updates after handling each $\textit{numInFormattedRange}$ query.  Each query $q_i$ is encoded in one of the following forms:

1. $\textit{insertFormattingChar}$ queries take the form:

   ```F encryptedPositionToInsert```  

    To process this query, Chef must first _decrypt_ `encryptedPositionToInsert` to get $\textit{positionToInsert}$ with his $\textit{decryptionKey}$ using the formula:
    $$
    \textit{positionToInsert} = \textit{decryptionKey} \oplus \textit{encryptedPositionToInsert}
    $$
    He must then process $\textit{insertFormattingChar}(\textit{positionToInsert})$.
2.  $\textit{insertNonFormattingChars}$ queries take the form:

    ```N encryptedNumCharsToInsert encryptedPositionToInsert```

    To process this query, Chef must first decrypt `encryptedNumCharsToInsert` to get $\textit{numCharsToInsert}$ using the formula:
    $$
    \textit{numCharsToInsert} = \textit{decryptionKey} \oplus \textit{encryptedNumCharsToInsert}
    $$
    and decrypt `encryptedPositionToInsert` to get $\textit{positionToInsert}$ using the formula:
    $$
    \textit{positionToInsert} = \textit{decryptionKey} \oplus \textit{encryptedPositionToInsert}
    $$
    He must then process $\textit{insertNonFormattingChars}(\textit{numCharsToInsert}, \textit{positionToInsert})$.
3.  $\textit{undo}$ queries take the form:
  
    ```U encryptedNumToUndo```
    
    To process this query, Chef must first decrypt `encryptedNumToUndo` to get $\textit{numToUndo}$ using the formula:

    $$
    \textit{numToUndo} = \textit{decryptionKey} \oplus \textit{encryptedNumToUndo}
    $$
    and then process $\textit{undo}(\textit{numToUndo})$.
4.  $\textit{redo}$ queries take the form:

    ```R encryptedNumToRedo```

    To process this query, Chef must first decrypt `encryptedNumToRedo` to get $\textit{numToRedo}$ using the formula:

    $$
    \textit{numToRedo} = \textit{decryptionKey} \oplus \textit{encryptedNumToRedo}
    $$
    and then process $\textit{redo}(\textit{numToRedo})$.
5.  $\textit{numInFormattedRange}$ queries take the form:

    ```Q encryptedQueryPosition``` 

    To process this query, Chef must first decrypt `encryptedQueryPosition` to get $\textit{queryPosition}$ using the formula:

    $$
    \textit{queryPosition} = \textit{decryptionKey} \oplus \textit{encryptedQueryPosition}
    $$
    and then process $\textit{numInFormattedRange}(\textit{queryPosition})$.

    Once Chef has computed the answer to this query, $\textit{queryAnswer}$, he must use it to update his $\textit{decryptionKey}$ using the formula:

    $$
    \textit{decryptionKey} = \textit{queryAnswer} \times 2^i + \textit{decryptionKey} \mod{10^9}
    $$

    where $i$ is the number of this query.

Help Chef answer the $Q$ queries.  To prove that you can do it, you only need to print the final value of $\textit{decryptionKey}$ for each testcase!

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
- The character at the decrypted position $\textit{queryPosition}$ in the current revision of the document is always a `X`
- The decrypted position $\textit{positionToInsert}$ is always in the range $1 \dots |d| + 1$, where $|d|$ is the length of the current document
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

