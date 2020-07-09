PROBLEM:

Chef has to implement part of a simple markdown editor.  The simplification comes from the fact that the markdown document being edited can consist only of two characters: Non-formatting characters ('X') and formatting characters ('*').  The editor also has an Undo Stack containing the list of document revisions, and an Undo Stack Pointer, pointing to the current revision of the document.

Initially, the document is empty, and the Undo Stack is a list containing a single element: this empty document.  The Undo Stack Pointer initially points at this element.

The document is *parsed* into _formatted ranges_ and _non-formatted ranges_ of characters in the document as follows.

Take the first and second occurrences of formatting chars in the document (if they exist); the positions of these form a formatted range.
Take the third and fourth occurrences of formatting chars in the document (if they exist); the positions of these form a formatted range; etc until no formatted chars remain: e.g.

For example, given the document:

```
XXX*XXXX*XX*XXXXXX*XX**XX*XXX
12345678912345678912345678901  <-- This is just a visual aid to help you see the positions :)
```

the first and second formatting chars are at positions 4 and 9, so [4, 9] is a formatted range; the third and fourth are at positions 13 and 21, so [13, 21] is a formatted range; the fifth and sixth are at positions 24 and 25, so [24, 25] is a formatted range.  There is a seventh formatting char at position 28, but no eighth, so there are no more formatted ranges.


Chef must implement the following queries.  Note that positions of characters in the document are always 1-relative.  The queries are *encrypted* in such a way that Chef can't decrypt the next query until Chef has processed all previous queries i.e. the queries must be processed _online_.

1. insertFormattingChar(positionToInsert).  Chef must first clear all elements in the Undo Stack to the right of the Undo Stack Pointer (if any); then insert a formatting character ('*') at the given positionToInsert in the current document; then append the resulting document to the Undo Stack and increment the Undo Stack Pointer so that it points to the new current document.
2. insertNonFormattingChars(numCharsToInsert, positionToInsert).  Chef must first clear all elements in the Undo Stack to the right of the Undo Stack Pointer (if any); then insert numCharsToInsert non-formatting characters ('X') at the given positionToInsert in the current document; then append the resulting document to the Undo Stack and increment the Undo Stack Pointer so that it points to the new current document.
3. undo(numToUndo).  Chef must move the Undo Stack Pointer numToUndo to the left.  The document in the Undo Stack that is pointed to by the updated Undo Stack Pointer (which is guaranteed to exist) becomes the new current document.
4. redo(numToReo).  Chef must move the Undo Stack Pointer numToReo to the right.  The document in the Undo Stack that is pointed to by the updated Undo Stack Pointer (which is guaranteed to exist) becomes the new current document.
5. numInFormattedRange(queryPosition).  If the character at queryPosition - which is guaranteed to be a non-formatted char - is contained in a formatted range, then Chef's answer to the query is the number of non-formatting chars in this formatted range; otherwise, the answer to the query is 3'141'592.  The result of these queries is used to update the _decryption key_ necessary to unlock further queries.

Find the final value of the decryption key.

SOLUTION:

An implementation can be found here: http://campus.codechef.com/SITJMADM/viewsolution/35394676/

The Problem is inspired by:

https://www.hackerrank.com/challenges/simple-text-editor/problem

which was a classic wasted opportunity due to its poor testcases; plus some of the real-life problems you'd need to solve if you wanted to write a fast incremental markdown-parser.

It's not very hard algorithmically, and is mostly an Implementation Problem.  I'm quite keen on it as the next Problem of mine - MVCN2TST - might benefit from people having a Persistent Balance Tree implementation handy :)

So - as you've probably guessed, a Persistent Balance Tree is used to solve this problem: the balancing is for processing all queries other than the Undo/ Redo ones; the persistence is used for processing the Undos/ Redos.

Each node of the Balance Tree corresponds to a Formatting Char in the document, and contains (amongst other bits of book-keeping) the number of non-formatting chars to its immediate left.  For non-Undo and Redo queries, the following operations can be performed in O(log F), where F is the number of formatting chars in the document (which is bounded by the number of queries, Q):

* Insert formatted/ non-formatted chars, maintaining the invariants:
** The nodes are "sorted" in the balance tree in order of the position of their corresponding Formatted Char in the current document (useful for finding the first Formatting Char immediately to the right of a given position in the current document)
** The number of Formatting Chars in the current document anywhere to the left of the Formatting Char corresponding to a given node is contained in that node (the parity of this value is used to see if a given position in the current document is within a formatted range)
** The number of Non-Formatting Chars in the current document _immediately to the left of_ the Formatting Char corresponding to a given node is contained in that node (useful for, amongst other things, finding the number of non-formatting chars in a formatted range)
* Find the node corresponding to the first Formatting Char in the  current document to the right of a given position in the document.

This is sufficient to process all non-Undo/ Redo queries.

The addition of persistence allows us to retrive the Balance Tree representing a given "revision" of the document, and is used for processing Undo/ Redo requests in O(1).
