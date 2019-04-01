The original [Move the Coins](https://www.hackerrank.com/challenges/move-the-coins/problem) was tough from a Game Theory point of view, but after solving the Game Theory part, it was not too difficult.  The reverse is true, here: as we'll see, the Game Theory portion is very easy, but then actually computing the solutions is fairly hard :)

If you've done a few Game Theory challenges, you may immediately recognise that this is the game of Nim in disguise, with a one-to-one correspondence between coins on the tree and piles of stones: each coin corresponds to a pile of stones with $height(v)$ stones in the pile, where $v$ is the vertex the coin is placed on and $\textit{height}(v)$ is the distance from $v$ to vertex $1$.

Thus, we can determine the $\textit{winner}(T)$ for the original tree $T$ very easily using the well-known approach of calculating the Grundy number or nimber, using the following pseudocode:

```
originalTreeGrundyNumber = 0
for each vertex v:
     for each of the c_v coins on v:
           originalTreeGrundyNumber = originalTreeGrundyNumber ^ height(v)
```

Then $\textit{winner}(T)$ is the first player (Alice) if *originalTreeGrundyNumber* is non-zero, and the second player otherwise.

We can simplify slightly by noting that, since $x^x=0$, the precise number of coins on a vertex is not needed: only the *parity* of the number of coins is important.  We say $v$ *hasCoin* if and only if $c_v$ is odd.  Then we only care about whether a vertex *hasCoin* or not: 


```
originalTreeGrundyNumber = 0
for each vertex v:
     if v->hasCoin:
           originalTreeGrundyNumber = originalTreeGrundyNumber ^ height(v)
```

This pseudocode runs in $O(N)$, so the naive approach to solving this problem would be, for each query $q_i$, to construct $T(q_i)$ (which could probably be easily done in $O(1)$), and then just run the pseudocode above on $T(q_i)$ and so find $\textit{winner}(T(q_i))$.  This would, of course, be $O(N \times Q)$, which is far too slow :)

It's fairly easy to see an efficient way of getting a part-way solution: for $q_i=(u_i,v_i)$, $T(q_i)$ is formed by first severing $u_i$ from its parent, then re-parenting $u_i$ to $v_i$.  We can quite easily get the grundy number for the tree formed by just doing the first (severing) operation: for a vertex $v$, define $\textit{descendants(v)}$ to be the set of vertices explored from $v$ during a DFS starting at vertex $1$ i.e. it's the subtree rooted at $v$ in $T$, if we say that $T$  is rooted at vertex $1$.

TODO - example

Severing $v$ would remove the *contributions* of all descendants of $v$ which $hasCoin$ to *originalTreeGrundyNumber*.  We can very easily calculate this *grundyContribForSubtree* for all $v$ in just $O(N)$ - see *findGrundyContribsForNodes* in the Editorial code.

The grundy number for $T(q_i)$ then, is (note that, to remove the contribution $x$ from a xor'd sum $xorSum$, we can do $xorSum \wedge x$):


$\textit{queryGrundyNumber} = \textit{originalTreeGrundyNumber} \wedge u_i.\textit{grundyContribForSubtree} \wedge \textit{contribution from re-adding } u_i \textit{ with new parent}$

Let's write *relocatedSubtreeGrundyContrib* for the last term in that xor sum: if we can figure out how to calculate that efficiently, then we're done!  

When we add $u_i$ back in to $T(q_i)$, now with parent $v_i$, we re-add all $\textit{descendants}(u_i)$ (and so, all coins on nodes that were in $\textit{descendants}(u_i)$), but now their height is probably changed as the height of the re-parented $u_i$ may have changed: if a vertex $x \in \textit{descendants(v)}$ *hasCoin*, then its contribution to the grundy number for $T(q_i)$ will be:

$height(x)+(height(v_i) - height(p(u_i))$

Thus

```
relocatedSubtreeGrundyContrib = 0
for vertex x in descendants(u_i):
    if x->hasCoin:
        relocatedSubtreeGrundyContrib = relocatedSubtreeGrundyContrib ^ (height(x)+(height(v_i) - height(p(u_i))
```        

Obviously, the size of $descendants(u_i)$ can be $O(N)$, so this is not yet good enough - we'd still have a $O(N \times Q)$ solution.

For a query $q_i=(u_i, v_i)$, let $\textit{heightChange}=(height(v_i) - height(p(u_i))$ i.e. *heightChange* is the ... well, height-change for all $x \in \textit{descendants(v)}$ in $T(q_i)$ compared to $x$'s original height in $T$. Let's look at how *relocatedSubtreeGrundyContrib* is built up bitwise: it's hopefully fairly clear that:

```
relocatedSubtreeGrundyContrib = 0
for bitNum = 0 to maxBinaryDigits:
    for vertex x in descendants(u_i):
        if x->hasCoin && bit bitNum is set in (height(x) + heightChange):
            toggle bit bitNum in relocatedSubtreeGrundyContrib 
```        

```
  5 4 3 2 1 0
  ============
0| 0 0 0 0 0 0
1| 0 0 0 0 0 1
2| 0 0 0 0 1 0
3| 0 0 0 0 1 1
4| 0 0 0 1 0 0
5| 0 0 0 1 0 1

```
