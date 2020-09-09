# PROBLEM LINK:

[Practice](https://www.codechef.com/problems/MVCN2TST)  
[Div-2 Contest](https://www.codechef.com/SEPT20B/problems/MVCN2TST)  
[Div-1 Contest](https://www.codechef.com/SEPT20A/problems/MVCN2TST)  

**Author and Editorialist:** [Simon St James](https://www.codechef.com/users/ssjgz)  
**Tester:** [Suchan Park](https://www.codechef.com/users/tncks0121)  

# DIFFICULTY:
Hard **TODO - revisit this - seems to be easier than MOVCOIN2**

# PREREQUISITES:
Persistence, Graph Theory, AVL Tree, Segment Tree

# PROBLEM:
Given a rooted tree $G$, find the $c_i^{\text{th}}$ element in a list $L$ of pairs of nodes $(u,v)$ that form a valid reparenting, with a specific ordering on $L$, with each $c_i$ being processed online.

# QUICK EXPLANATION:
The reparenting $(u,v)$ is _valid_ if and only if $v$ is not a descendant of $u$.

The solution is broken down into three "phases", one for of the three clauses in the ordering:

Phase One: Find $u$ (the $\textit{nodeToReparent}$).  For each node $u$ in order, find the number of $v$'s such that $(u,v)$ is a valid reparenting, and form a prefix sum array from this.  The first element in this array strictly greater than $c_i$ gives our $nodeToReparent$, and can be found via binary search.

Phase Two: Find the $\textit{newParentHeight}$.  We have our $nodeToReparent$, and now we need to find the $X^\textit{th}$ element in $L$ that reparents $nodeToReparent$, where $X$ is easily computed.  Using some simple observations and pre-computed lookups, we can compute the number of non-descendants of $nodeToReparent$ at any given height $h$ ($\textit{findNumNonDescendantsUpToHeight}(nodeToReparent, h)$) in $\mathcal{O}(\log N)$.  Similarly to Phase One, we can then perform an Abstract Binary Search on $h$ to find the correct $\textit{newParentHeight}=h$.

Phase Three: Find the final $v$ ($\textit{newParent}$).  We have our $\textit{nodeToReparent}$ and $\textit{newParentHeight}$, and now just need to find the $Y^\text{th}$ element at height $newParentHeight$ that is not a descendant of $\textit{nodeToReparent}$, where the index $Y$ is easily found.  If we list all nodes with the given height in the order they are visited in a DFS ($\textit{nodesAtHeightInDFSOrder}(h)$) in another precomputation step, we see that the descendents of $\textit{nodeToReparent}$ all lie in an easily found interval $(l,r)$ in this list, so we want to find the $Y^\text{th}$ element _not_ in this interval.  By forming a _persistent_ AVL Tree of the elements of ($\textit{nodesAtHeightInDFSOrder}$) in the precomputation step, we can find in $\mathcal{O}(1)$ a pair of AVL Trees representing all (sorted) nodes to the left of $l$ and all to the right of $r$, respectively.  We can then adapt the standard algorithm for finding the $Y^\text{th}$ element in two sorted arrays to work with AVL Trees and find the final $\textit{newParent}$.

# EXPLANATION:
Very detailed explanation ideally with code snippets.
Ideally editorialist should write his own solution
and write editorial strictly following his solution.

For adding mathematical expressions, LaTeX is required, which is nicely explained at https://en.wikibooks.org/wiki/LaTeX/Mathematics

Some examples of latex here. (Latex is always enclosed in between $ sign)

$ans = \sum_{i = 1}^{N} a_i^k \bmod M$

$\frac{num}{den}$

$\leq$, $\geq$, $\neq$, $\pm$

# ALTERNATE EXPLANATION:
Could contain more or less short descriptions of possible other approaches.

# SOLUTIONS:

[details="Setter's Solution (C++)"]
indent whole code by 4 spaces
[/details]

[details="Tester's Solution (C++)"]
indent whole code by 4 spaces
[/details]
