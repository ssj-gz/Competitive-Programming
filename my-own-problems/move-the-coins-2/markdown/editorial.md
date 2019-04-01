The original [Move the Coins](https://www.hackerrank.com/challenges/move-the-coins/problem) was tough from a Game Theory point of view, but after solving the Game Theory part, it was not too difficult.  The reverse is true, here: as we'll see, the Game Theory portion is very easy, but then actually computing the solutions is fairly hard :)

If you've done a few Game Theory challenges, you may immediately recognise that this is the game of Nim in disguise, with a one-to-one correspondence between coins on the tree and piles of stones: each coin corresponds to a pile of stones with $h$ stones in the pile, where $height$ is the distance from the vertex the coin is initially placed on to the vertex $1$.  For each vertex $v$, let $\textit{originalHeight}(v)$ be the distance from $v$ to vertex $1$.

Thus, we can determine the $\textit{winner}(T)$ for the original tree $T$ very easily using the well-known approach of calculating the Grundy number or nimber, using the following pseudocode:

```
originalTreeGrundyNumber = 0
for each vertex v:
     for each of the c_v coins on v:
           originalTreeGrundyNumber = originalTreeGrundyNumber ^ height(v)
```

Then $\textit{winner}(T)$ is the first player (Alice) if *originalTreeGrundyNumber* is non-zero, and the second player otherwise.

We can simplify slightly by noting that, since $x^x=0$, the precise number of coins on a vertex is not needed: only the *parity* of the number of coins is important.  We say $v$ *hasCoin* if and only if $c_v$ is odd.  Then: 


```
originalTreeGrundyNumber = 0
for each vertex v:
     if v->hasCoin:
           originalTreeGrundyNumber = originalTreeGrundyNumber ^ height(v)
```

This pseudocode runs in $O(N)$, so the naive approach to solving this problem would be, for each query $q_i$, to construct $T(q_i)$ (which could probably be easily done in $O(1)$), and then just run the pseudocode above on $T(q_i)$ and so find $\textit{winner}(T(q_i))$.  This would, of course, be $O(N \times Q)$, which is far too slow :)
