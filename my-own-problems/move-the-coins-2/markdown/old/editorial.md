The original [Move the Coins](https://www.hackerrank.com/challenges/move-the-coins/problem) was tough from a Game Theory point of view, but after solving the Game Theory part, computing the solutions was not too difficult.  The reverse is true, here: as we'll see, the Game Theory portion is very easy, but then actually computing the solutions is fairly hard :)

If you've done a few Game Theory challenges, you may immediately recognise that this is the game of Nim in disguise, with a one-to-one correspondence between coins on the tree and piles of stones: each coin corresponds to a pile of stones with $\textit{height}(v)$ stones in the pile, where $v$ is the vertex the coin is placed on and $\textit{height}(v)$ is the distance from $v$ to vertex $1$.

Thus, we can determine the $\textit{winner}(T)$ for the original tree $T$ very easily using the well-known approach of calculating the Grundy number or nimber, using the following pseudocode:

```
originalTreeGrundyNumber = 0
for each vertex v:
     for each of the c_v coins on v:
           originalTreeGrundyNumber = originalTreeGrundyNumber ^ height(v)
```

Then $\textit{winner}(T)$ is the first player (Alice) if $\textit{originalTreeGrundyNumber}$ is non-zero, and the second player otherwise.

We can simplify slightly by noting that, since $x \wedge x=0$, the precise number of coins on a vertex is not needed: only the *parity* of the number of coins is important.  We say $v$ $\textit{hasCoin}$ if and only if $c_v$ is odd.  Then we only care about whether a vertex $\textit{hasCoin}$ or not: 


```
originalTreeGrundyNumber = 0
for each vertex v:
     if v->hasCoin:
           originalTreeGrundyNumber = originalTreeGrundyNumber ^ height(v)
```

This pseudocode runs in $O(N)$, so the naive approach to solving this problem would be, for each query $q_i$, to construct $T(q_i)$ (which could probably be easily done in $O(1)$), and then just run the pseudocode above on $T(q_i)$ and so find $\textit{winner}(T(q_i))$.  This would, of course, be $O(N \times Q)$, which is far too slow :)

It's fairly easy to see an efficient way of getting a part-way solution: for $q_i=(u_i,v_i)$, $T(q_i)$ is formed by first severing $u_i$ from its parent, then re-parenting $u_i$ to $v_i$.  We can quite easily get the grundy number for the tree formed by just doing the first (severing) operation: for a vertex $v$, define $\textit{descendants(v)}$ to be the set of vertices explored from $v$ during a DFS starting at vertex $1$ i.e. it's the subtree rooted at $v$ in $T$, if we say that $T$  is rooted at vertex $1$.

Severing $v$ would remove the *contributions* of all descendants of $v$ which $\textit{hasCoin}$ to $\textit{originalTreeGrundyNumber}$.  We can very easily calculate this $\textit{grundyContribForSubtree}$ for all $v$ in just $O(N)$ - see *findGrundyContribsForNodes* in the Editorial code.  Note that, to remove the contribution $x$ from a xor'd sum $\textit{xorSum}$, we merely calculate $\textit{xorSum} \wedge x$ - a consequence of the fact that $(a \wedge b \wedge c) \wedge b=a \wedge c$.

The grundy number for $T(q_i)$ then, is 


$\textit{queryGrundyNumber} = \textit{originalTreeGrundyNumber} \wedge u_i.\textit{grundyContribForSubtree} \wedge \textit{contribution from re-adding } u_i \textit{ with new parent}$

Let's write $\textit{relocatedSubtreeGrundyContrib}$ for the last term in that xor sum: if we can figure out how to calculate that efficiently, then we're done!  

When we add $u_i$ back in to $T(q_i)$, now with parent $v_i$, we re-add all $\textit{descendants}(u_i)$ (and so, all coins on nodes that were in $\textit{descendants}(u_i)$), but now their height is probably changed as the height of the re-parented $u_i$ may have changed: if a vertex $x \in \textit{descendants}(v)$ $\textit{hasCoin}$, then its contribution to the grundy number for $T(q_i)$ will be:

$\textit{height}(x)+(\textit{height}(v_i) - \textit{height}(p(u_i))$

Thus

```
relocatedSubtreeGrundyContrib = 0
for vertex x in descendants(u_i):
    if x->hasCoin:
        relocatedSubtreeGrundyContrib = relocatedSubtreeGrundyContrib ^ (height(x)+(height(v_i) - height(p(u_i))
```        

Obviously, the size of $descendants(u_i)$ can be $O(N)$, so this is not yet good enough - we'd still have a $O(N \times Q)$ solution.

For a query $q_i=(u_i, v_i)$, let $\textit{heightChange}=(\textit{height}(v_i) - \textit{height}(p(u_i))$ i.e. $\textit{heightChange}$ is the ... well, height-change for all $x \in \textit{descendants}(v)$ in $T(q_i)$ compared to $x$'s original height in $T$. Let's look at how $\textit{relocatedSubtreeGrundyContrib}$ is built up bitwise: it's hopefully fairly clear that:

```
relocatedSubtreeGrundyContrib = 0
for bitNum = 0 to maxBinaryDigits:
    for vertex x in descendants(u_i):
        if x->hasCoin && bit bitNum is set in (height(x) + heightChange):
            toggle bit bitNum in relocatedSubtreeGrundyContrib 
```        
($\textit{maxBinaryDigits}$ is chosen so that we can represent the largest possible height (which is $N$) - its value is approx $log_{2}N$). So it looks like we need to compute, for each $\textit{bitNum}$, the *number* of descendants $x$ of $u_i$ which $\textit{hasCoin}$ and for which bit $\textit{bitNum}$ is set in the value of $\textit{height}(x) + \textit{heightChange}$; the bit $\textit{bitNum}$ of  $\textit{relocatedSubtreeGrundyContrib}$ will be set to 1 if and only if this number is odd.

When does $\textit{height}(x) + \textit{heightChange}$ have its $\textit{bitNum}$ th bit set to 1? Let's look at a simpler case: when does a number $m$ have its $\textit{bitNum}$ th bit set to 1? The pattern is hopefully clear from the table below: $m$'s $\textit{bitNum}$ th bit is set to 1 if and only if $2^{\textit{bitNum}} \le m \pmod{2^{\textit{bitNum} + 1}} \le 2^{\textit{bitNum} + 1}-1$.

```
   5 4 3 2 1 0 <-- bitNum
  ============
0| 0 0 0 0 0 0
1| 0 0 0 0 0 1
2| 0 0 0 0 1 0
3| 0 0 0 0 1 1
4| 0 0 0 1 0 0
5| 0 0 0 1 0 1
6| 0 0 0 1 1 0
7| 0 0 0 1 1 1
8| 0 0 1 0 0 0
```

So, for example, a number $m$ will have its $3$rd bit set if and only if, the value $m \pmod{2^{3 + 1}}=m \pmod{2^{4}} = m \pmod{16}$ is in the (inclusive) range $2^3=8$ to $2^{3+1}-1=2^{4}-1=15$.

It's hopefully clear that considering $m+p$ for some $p$ *scrolls* this range by $p$ units.  Thus, the values of $m$ for which $m+5$ has its $3$rd  bit set to 1 are precisely those for which $m \pmod{16}$ is in the (inclusive) range $8-5=3$ to $15-5=10$.

Finding the set of values of $m$ for which $m-5$ has its $3$rd  bit set to 1 is a little trickier: here, that bit is set to 1 if and only if $m \pmod{16}$ falls into one of a *pair* of (inclusive) ranges: $8+5=13$ to $15$; and $0$ to $4$, but in general, we see that (setting $m=\textit{height}(x)$ and $p=\textit{heightChange}$):

Calculating $\textit{relocatedSubtreeGrundyContrib}$ for $u_i$ and $\textit{heightChange}$ merely requires us to count, for each $\textit{bitNum}$, the number of descendants $x$ of $u_i$ which $\textit{hasCoin}$ and whose $\textit{height}(x) \pmod{2^{\textit{bitNum}+1}}$ lies in either one or two ranges.  The range(s) are easily deduced from the value of $\textit{heightChange}$.

This is now sounding much more tractable - why don't we, as we perform our DFS, tally, for each $\textit{bitNum}$, the heights of vertices (well - just those which $\textit{hasCoin}$), $\pmod{2^{\textit{bitNum} + 1}}$? Even better: let's use a SegmentTree for each $\textit{bitNum}$, allowing us to count the number of tallies in a range very efficiently! See the *numNodesWithHeightModuloPowerOf2* array of SegmentTrees in the code.

Then, when our DFS has finished processing the node $u_i$, each descendant $x$ of $u_i$ which $hasCoin$ will have been tallied in *numNodesWithHeightModuloPowerOf2*, so we can find the number of $x$'s satisfying bit $\textit{bitNum}$ of $\textit{height}(x)+\textit{heightChange}$ is 1, and so compute $\textit{relocatedSubtreeGrundyContrib}$!

Except that, no - $\textit{numNodesWithHeightModuloPowerOf2}$ will indeed contain the number of such $x$'s, but it will also contain all such $y$'s, where the $y$'s are vertices our DFS explored *before* we reached $u_i$ mixed in!  Rats! Foiled again!

Or are we? What if, when our DFS first encounters $u_i$, we store the number of vertices satisfying bit $\textit{bitNum}$ of $\textit{height}(x)+\textit{heightChange}$ is 1 (this is the contribution of the $y$s - let's call it $\textit{originalCoinsThatMakeDigitOneAfterHeightChange}$) and then, when we finish processing $u_i$, we subtract this from the current number of vertices satisfying bit $\textit{bitNum}$ of $\textit{height}(x)+\textit{heightChange}$ is 1 (the contributions of the $x$'s and the $y$'s)? That's it - this will give us precisely the number of descendants $x$ of $u_i$ for which bit $\textit{bitNum}$ is set in the value of $\textit{height}(x)+\textit{heightChange}$, and we're done!

We store the $\textit{maxBinaryDigits}$ values $\textit{originalCoinsThatMakeDigitOneAfterHeightChange}$ alongside the query that makes use of it.  This means that we do a couple of bits of processing for the query $q_i=(u_i,v_i)$: the first when we encounter $u_i$ in our DFS, and the other when we have finished processing $u_i$ in our DFS - therefore, it makes sense to process the queries in an order that is not necessarily the same in which we were given the queries - we attach $q_i$ to $u_i$) (see *queriesForNode* in the code) and handle it when our DFS handles $u_i$, then figure out how to put the queries back in their original order at the end.

What's the time complexity? We are doing a DFS, and for each vertex, for each of the $\textit{maxBinaryDigits}$ ($O(log_{2}N)$), we (if the vertex $\textit{hasCoin}$) add the height to the tally in $\textit{numNodesWithHeightModuloPowerOf2}$ - this involves adding to a SegmentTree ($O(log_{2}N)$), so in total this part takes  $O(N \times \textit{maxBinaryDigits} \times log_{2}N) = O(N \times log_{2}N \times log_{2}N)$.  

We also, for each *query*, have to compute $\textit{originalCoinsThatMakeDigitOneAfterHeightChange}$ and then $\textit{descendantCoinsThatMakeDigitOneAfterHeightChange}$ (these computations are rolled into the DFS, so it might looks like we do this for each node, but no: it is done per *query*, not *node*).  Both require interrogating the $\textit{maxBinaryDigits}$ SegmentTrees of $\textit{numNodesWithHeightModuloPowerOf2}$ for the number of values in a given range, and each interrogation of a SegmentTree is $O(log_{2}N)$.  Thus, this part takes up $O(Q \times \textit{maxBinaryDigits} \times log_{2}N) = O(Q \times log_{2}N \times log_{2}N)$.

There's various other bits of book-keeping, but these two parts are the bulk of the work, so in total, the algorithm is $O((N+Q) \times log_{2}N \times log_{2}N)$.


