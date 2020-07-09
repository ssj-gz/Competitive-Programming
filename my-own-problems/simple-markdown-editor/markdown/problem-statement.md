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

1. `F encryptedPositionToInsert`.  To process this query, he must first _decrypt_ `encryptedPositionToInsert` to get $\textit{positionToInsert}$ with his $\textit{decryptionKey}$ using the formula:
$$
\textit{positionToInsert} = \textit{decryptionKey} \oplus \textit{encryptedPositionToInsert}
$$
He must then process $\textit{insertFormattingChar}(\textit{positionToInsert})$.
2. `N encryptedNumCharsToInsert encryptedPositionToInsert`. To process this query, he must first decrypt `encryptedNumCharsToInsert` to get $\textit{numCharsToInsert}$ using the formula:
$$
\textit{numCharsToInsert} = \textit{decryptionKey} \oplus \textit{encryptedNumCharsToInsert}
$$
and decrypt `encryptedPositionToInsert` to get $\textit{positionToInsert}$ using the formula:
$$
\textit{positionToInsert} = \textit{decryptionKey} \oplus \textit{encryptedPositionToInsert}
$$


