(*Note to Hackerrank - this is Yet Another small variation of the original "Move the Coins" Challenge (https://www.hackerrank.com/challenges/move-the-coins/problem ; the other variation is https://www.hackerrank.com/administration/challenges/edit/64022 , which I submitted a short while ago) which Yet Again makes it easier from a Game Theory point of view, but (IMO) make for a much more difficult and interesting Algorithms Challenge!*

*I'll skip adding the story for now as it just follows from the original Move the Coins and just give a brief description of the problem.*
)

Alice and Bob are playing a game with coins on a tree $T$ over $N$ vertices numbered $1...N$. The vertex numbered $i$ initially has $c_i$ coins on it.  One node, $R$, is designated the *root* of the tree. The rules of the game are as follows:

- Alice moves first.
- If the current player cannot make a move, then that player loses the game.
- A valid move for a player is to pick a vertex $v \ne R$ which has at least one coin in it, and move a coin in $v$ $X$ vertices along the shortest path from $v$ to vertex R, where $X > 0$.

For example, if the tree $T$ currently looks like this and $R=1$:


![image](https://s3.amazonaws.com/hr-assets/0/1521711755-f6308f380d-move-the-coins-2-example-move-example-1of2.png)

Then the current player can pick a coin in any of vertices $v = 2, 4$ or $5$.  Let's assume he picks the coin in $5$, as shown below; then he can move the coin $X$ vertices towards $1$, where $X=1,2$ or $3$, ending up in vertices $4$, $2$ or $1$, respectively:


![image](https://s3.amazonaws.com/hr-assets/0/1521711917-0016648f68-move-the-coins-2-example-move-example-2of2.png)

Let's assume he moves the coin to vertex $2$.  Then the tree now looks like 


![image](https://s3.amazonaws.com/hr-assets/0/1521712073-48296db0ed-move-the-coins-2-example-move-example-3of3.png)

and it is the other player's turn to move.

For a tree $T$ on vertices $1,2,...,N$ with root $R$, let $\textit{winner}(T, R)$ be whichever of Alice or Bob will win if both players play the game optimally on $T'$ and Alice moves first.

Given a tree $T$, find the $R$'s such $\textit{winner}(T, R)$ is Bob.

