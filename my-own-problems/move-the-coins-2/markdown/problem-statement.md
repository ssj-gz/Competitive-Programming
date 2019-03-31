(*Note to Hackerrank - this is a small variation of the original "Move the Coins" Challenge (https://www.hackerrank.com/challenges/move-the-coins/problem) which makes it easier from a Game Theory point of view, but (IMO) make for a much more difficult and interesting Algorithms Challenge!*

*I'll skip adding the story for now as it just follows from the original Move the Coins and just give a brief description of the problem.*
)

Alice and Bob are playing a game with coins on a tree $T=(E,C)$ over $N$ vertices numbered $1...N$. $E$ is the set of *edges* of $T$, and $C$ describes the number of *coins* on each vertex; the vertex numbered $i$ initially has $C(i)$ coins on it.  The rules of the game are as follows:

-  The players take turns to make a *valid move*, with Alice taking the first turn.
- If the current player cannot make a valid move, then the game ends and the other player is deemed the winner.
- A *valid move* for a player is to pick a vertex $v \ne R$ which has at least one coin in it, and move a single coin in $v$ to a vertex some non-zero number of steps along the path from $v$ to the vertex $1$. 


For example, if the tree $T$ currently looks like this:


![image](https://s3.amazonaws.com/hr-assets/0/1521711755-f6308f380d-move-the-coins-2-example-move-example-1of2.png)

Then the current player can pick a coin in any of vertices $v = 2, 4$ or $5$.  Let's assume he picks the coin in $5$, as shown below; then he can move the coin $X$ vertices towards $1$, where $X=1,2$ or $3$, ending up in vertices $4$, $2$ or $1$, respectively:


![image](https://s3.amazonaws.com/hr-assets/0/1521711917-0016648f68-move-the-coins-2-example-move-example-2of2.png)

Let's assume he moves the coin to vertex $2$.  Then the tree now looks like 


![image](https://s3.amazonaws.com/hr-assets/0/1521712073-48296db0ed-move-the-coins-2-example-move-example-3of3.png)

and it is the other player's turn to move.

In the tree $T$, for each vertex $v \ne 1$, let $p(v)$ be the *parent* of $v$ if we did a DFS from vertex $1$; in other words, $p(v)$ is the first vertex we'd encounter on the path from $v$ to vertex $1$.

For a tree $T'=(E',C')$ on vertices $1,2,...,N$, let $\textit{winner}(T')$ be whichever of Alice or Bob will win if both players play the game optimally on $T'$ and Alice moves first.

Alice and Bob soon figure out how to predict $\textit{winner}(T)$ for their original $T$, so Alice proposes that Bob allows her to make a simple transformation to the original $T$ to get a brand-new tree to play the game on.

Alice has $q$ queries numbered $1...q$, and the $i$th query consists of a pair $(u_i, v_i)$ of vertices.  She applies the following transform to create a new tree $T'=(E',C)=\textit{transform}(T, (u_i, v_i))$ (leaving the original $T$ unchanged) as follows:

- Initially, $\textit{transform}(T, (u_i, v_i))$ is set to be a copy of $T=(E,C)$.
- $\textit{transform}(T, (u_i, v_i))$ is then modified by first removing the edge from $p(u_i)$ to $u_i$ and then adding a new edge from $v_i$ to $u_i$ i.e. $\textit{transform}(T, (u_i, v_i))$ is a copy of $T$, but with $u_i$s parent now changed to be $v_i$.

Note that each vertex $i$ in $\textit{transform}(T, (u_i, v_i))$ still has its original $C(i)$ coins in it.

Alice promises that for all her queries, $\textit{transform}(T, (u_i, v_i))$ will be a connected tree on the original $N$ vertices.

You need to output, in sorted order, the $i$s for which $\textit{winner}(\textit{transform}(T, (u_i, v_i)))$ is Bob. 
