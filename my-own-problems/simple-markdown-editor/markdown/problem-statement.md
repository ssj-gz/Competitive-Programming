Chef has been tasked with the implemention of part of a simple markdown editor.  The simplification comes from the fact that the markdown document being edited can consist only of two characters: _Non-Formatting Characters_  (represented by `X`) and _Formatting Characters_ (represented by `*`).  The editor also has an _Undo Stack_ containing the list of document _revisions_, and an _Undo Stack Pointer_, pointing to the current revision of the document.

Initially, the document is empty, and the Undo Stack is a list containing a single element: this empty document.  The Undo Stack Pointer initially points at this element.

The document is *parsed* into _formatted ranges_ and _non-formatted ranges_ of characters in the document as follows.

Take the first and second occurrences of formatting chars in the document (if they exist); the positions of these form a formatted range.
Take the third and fourth occurrences of formatting chars in the document (if they exist); the positions of these form a formatted range; etc until no formatted chars remain.

For example, given the document:

```
XXX*XXXX*XX*XXXXXX*XX**XX*XXX
12345678912345678912345678901  <-- This is just a visual aid to help you see the positions :)
```

the first and second formatting chars are at positions 4 and 9, so [4, 9] is a formatted range; the third and fourth are at positions 13 and 21, so [13, 21] is a formatted range; the fifth and sixth are at positions 24 and 25, so [24, 25] is a formatted range.  There is a seventh formatting char at position 28, but no eighth, so there can be no more formatted ranges.

Chef must implement the following queries.  Note that positions of characters in the document are always 1-relative.  The queries are *encrypted* in such a way that Chef can't decrypt the next query until Chef has processed all previous queries i.e. the queries must be processed _online_.

1. $\textit{insertFormattingChar}(\textit{positionToInsert})$.  Chef must first clear all elements in the Undo Stack to the right of the Undo Stack Pointer (if any); then insert a formatting character ('*') at the given positionToInsert in the current document; then append the resulting document to the Undo Stack and increment the Undo Stack Pointer so that it points to the new current document.
2. $\textit{insertNonFormattingChars}(\textit{numCharsToInsert}, \textit{positionToInsert})$.  Chef must first clear all elements in the Undo Stack to the right of the Undo Stack Pointer (if any); then insert numCharsToInsert non-formatting characters ('X') at the given positionToInsert in the current document; then append the resulting document to the Undo Stack and increment the Undo Stack Pointer so that it points to the new current document.
3. $\textit{undo}(\textit{numToUndo})$.  Chef must move the Undo Stack Pointer numToUndo revisions to the left.  The document in the Undo Stack that is pointed to by the updated Undo Stack Pointer (which is guaranteed to exist) becomes the new current document.
4. $\textit{redo}(\textit{numToRedo})$.  Chef must move the Undo Stack Pointer numToRedo revisions to the right.  The document in the Undo Stack that is pointed to by the updated Undo Stack Pointer (which is guaranteed to exist) becomes the new current document.
5. $\textit{numInFormattedRange}(\textit{queryPosition})$. If the character at queryPosition - which is guaranteed to be a non-formatted char - is contained in a formatted range, then Chef's answer to the query is the number of non-formatting chars in this formatted range; otherwise, the answer to the query is 3'141'592.  The result of these queries is used to update the _decryption key_ necessary to unlock further queries.

More formally, Chef has a $\textit{decryptionKey}$ which is used to decrypt queries, which has the initial value $0$.

The 5 types of queries are encoded as follows:

1. Insert formatting char queries take the form:

   ```F encryptedPositionToInsert```  

    To process this query, he must first _decrypt_ `encryptedPositionToInsert` to get $\textit{positionToInsert}$ with his $\textit{decryptionKey}$ using the formula:
    $$
    \textit{positionToInsert} = \textit{decryptionKey} \oplus \textit{encryptedPositionToInsert}
    $$
    He must then process $\textit{insertFormattingChar}(\textit{positionToInsert})$.
2.  Insert non-formatting char queries take the form:

    ```N encryptedNumCharsToInsert encryptedPositionToInsert```

    To process this query, he must first decrypt `encryptedNumCharsToInsert` to get $\textit{numCharsToInsert}$ using the formula:
    $$
    \textit{numCharsToInsert} = \textit{decryptionKey} \oplus \textit{encryptedNumCharsToInsert}
    $$
    and decrypt `encryptedPositionToInsert` to get $\textit{positionToInsert}$ using the formula:
    $$
    \textit{positionToInsert} = \textit{decryptionKey} \oplus \textit{encryptedPositionToInsert}
    $$
    He must then process $\textit{insertNonFormattingChars}(\textit{numCharsToInsert}, \textit{positionToInsert})$.
3.  Undo queries take the form:
  
    ```U encryptedNumToUndo```
    
    To process this query, he must first decrypt `encryptedNumToUndo` to get $\textit{numToUndo}$ using the formula:

    $$
    \textit{numToUndo} = \textit{decryptionKey} \oplus \textit{encryptedNumToUndo}
    $$
    and then process $\textit{undo}(\textit{numToUndo})$.
4.  Redo queries take the form:

    ```R encryptedNumToRedo```

    To process this query, he must first decrypt `encryptedNumToRedo` to get $\textit{numToRedo}$ using the formula:

    $$
    \textit{numToRedo} = \textit{decryptionKey} \oplus \textit{encryptedNumToRedo}
    $$
    and then process $\textit{redo}(\textit{numToRedo})$.
5.  Num in formatted range queries take the form:

    ```Q encryptedQueryPosition``` 

    To process this query, he must first decrypt `encryptedQueryPosition` to get $\textit{queryPosition}$ using the formula:

    $$
    \textit{queryPosition} = \textit{decryptionKey} \oplus \textit{encryptedQueryPosition}
    $$
    and then process $\textit{numInFormattedRange}(\textit{queryPosition})$.

    Once he has computed the answer to this query, $\textit{queryAnswer}$, he must use it to update his $\textit{decryptionKey}$ using the formula:

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
- The character at the decrypted position `queryPosition` in the current revision of the document is always a `X`
- The decrypted position `positionToInsert` is always in the range $1 \dots |d| + 1$, where $|d|$ is the length of the current document
- The decrypted value `numToUndo` will never exceed the value of the current Undo Stack Pointer minus 1
- The decrypted value `numToRedo` will never exceed the current Undo Stack size minus the value of the current Undo Stack Pointer
- the sum of $Q$ over all test cases does not exceed $200,000$


### Subtasks
**Subtask #1 (5 points):** 
- $T \le 10$
- $Q \le 100$
- The length of the document will never exceed $1000$ characters

**Subtask #2 (40 points)**: 
- Original constraints, but there will be no $\textit{undo}$ nor $\textit{redo}$ queries

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



    



