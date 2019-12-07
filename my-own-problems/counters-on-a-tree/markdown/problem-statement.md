Alice and Bob are playing a game with coins on a tree $T$ over $N$ vertices numbered $1...N$. The vertex numbered $i$ initially has $c_i$ coins on it.  The game is played as follows:

- At the beginning of the game, one vertex is designated the *root* of $T$ - call this vertex $R$.
- The players then take turns to make a *valid move*, with Alice taking the first turn.
- If the current player cannot make a valid move, then the game ends and the other player is deemed the winner.
- A *valid move* for a player is to pick a vertex $v \ne R$ which has at least one coin in it, and move a single coin in $v$ to a vertex some non-zero number of steps along the path from $v$ to $R$. 

For example, if the tree $T$ currently looks like this and, for this game, we have chosen $R=1$:


![image](counters-on-a-tree-example-move-example-1of2.png)

Then the current player can pick a coin in any of vertices $v = 2, 4$ or $5$.  Let's assume he picks the coin in $5$, as shown below; then he can move the coin $X$ steps towards $R=1$, where $X=1,2$ or $3$, ending up in vertex $4$, $2$ or $1$, respectively:


![image](counters-on-a-tree-example-move-example-2of2.png)

Let's assume he picks $X=2$, so he moves the coin to vertex $2$.  Then the tree now looks like:


![image](counters-on-a-tree-example-move-example-3of3.png)

and it is the other player's turn to move.

If the game is played on a tree $T$, and $R$ is the designated root of $T$ for that game, then let $\textit{winner}(T, R)$ be whichever of Alice or Bob will win if both players play the game optimally on $T$.

Given a tree $T$, find the $R$'s such that $\textit{winner}(T, R)$ is Bob.

