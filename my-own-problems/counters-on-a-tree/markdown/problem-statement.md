Bob and Alice are playing a board game.  The board is in the form of a tree $T_{\textit{board}}$ with $N$ nodes, numbered from $1$ to $N$. The game involves moving a set of Coins around the board, with each node $v$ having $c_v$ Coins on it at the beginning of the game. For a node $R$ ($1 \le R \le N)$, define $\textit{game}(T_{\textit{board}}, R)$ as the game with the following rules:

1. A copy of $T_{\textit{board}}$ is taken, and for each $v=1,2,\dots N$, we ensure that there are precisely $c_v$ Coins on the node numbered $v$.
2. Bob and Alice now take turns to make a move, with Alice making the first move.
3. A move consists of taking a single Coin ($C$, say) from some node other than $R$ and moving it to an *allowed* node. If $v_C$ is the node that $C$ is currently on, then the set of allowed nodes for this $C$ is the set of $u \ne v_C$ on the shortest path between nodes $v_C$ and $R$.
4. If a player cannot make a move on their turn (i.e. because all the Coins are on node $R$), then the game ends and the other player is declared the winner.

For example, if the tree $T_{\textit{board}}$ currently looks like this and, for this game, we have chosen $R=1$:

![](http://campus.codechef.com/SITJMADM/content/COUNTREE-move-example-1of3.png)

Then the current player can pick a Coin on any of nodes $v = 2, 4$ or $5$ (if there were any Coins on node $3$, picking one of those would also be an option). Let's assume they pick the Coin on $5$, as shown below; then they can move the Coin $X$ steps towards $R=1$, where $X=1,2$ or $3$, ending up on node $4$, $2$ or $1$, respectively:

![](http://campus.codechef.com/SITJMADM/content/COUNTREE-move-example-2of3.png)

Let's assume they pick $X=2$, so they move the Coin to node $2$. Then the tree now looks like:

![](http://campus.codechef.com/SITJMADM/content/COUNTREE-move-example-3of3.png)

and it is the other player's turn to move.

Let $\textit{winner}(\textit{game}(T_{\textit{board}}, R))$ be whichever of Bob or Alice wins the $\textit{game}(T_{\textit{board}}, R)$, assuming both players play perfectly.

Bob knows some elementary Game Theory, so is able to very quickly predict who will win $\textit{game}(T_{\textit{board}}, R)$ for a given $R$. Unimpressed, Alice gives him a harder task: she challenges him to find $\textit{winner}(\textit{game}(T_{\textit{board}}, R))$ for *all of* $R = 1, 2, \ldots , n$.

Can you help Bob figure out the answer? More formally, if $\textit{BobWinR}$ is the set of $R$ such that $\textit{winner}(\textit{game}(T_{\textit{board}}, R)) =$ Bob, then you must calculate the value of

$$
\sum\limits_{R \in \textit{BobWinR}}2^R
$$

Since the result can be very large, please output it modulo $10^9+7$.

### Input

- The first line of the input contains a single integer $T$ denoting the number of test cases. The description of $T$ test cases follows.
- The first line of a testcase contains a single integer $N$.
- Each of the next $N-1$ lines contains two space-separated integers $u$ and $v$ indicating that the nodes numbered $u$ and $v$ are connected by an edge.
- The last line of the testcase contains $N$ space-separated integers $c_1, c_2, \ldots , c_N$, where $c_i$ denotes the number of Coins that should be placed on the the node numbered $i$ in the tree $T_{\textit{board}}$ at the beginning of a game.

### Output

For each testcase, print a single line containing one integer - the sum, modulo $10^9+7$, of $2^R$ over each $R$ such that $\textit{winner}(\textit{game}(T_{\textit{board}}, R)) =$ Bob.

### Constraints 
- $1 \le T \le 1,000$
- $1 \le N \le 200,000$
- $1 \le u,v \le N$
- $0 \le c_i \le 16$ for all valid $i$
- the sum of $N$ over all $T$ testcases does not exceed $200,000$

### Subtasks
**Subtask #1 (5 points):** 

- $T \le 10$
- $N \le 10$
- $c_i \le 6$ for all valid $i$
- the sum of $c_i$ over all nodes $i$ in a testcase does not exceed $10$

**Subtask #2 (10 points)**: 

* $T \le 100$ 
* $N \le 1,000$

**Subtask #3 (85 points)**: original constraints

### Example Input

```
2
3 
1 2
2 3
1 0 1
6
1 2
1 3
4 3
3 5
4 6
0 2 2 1 3 2
```

### Example Output

```
4
14
```

### Explanation

Although the tree $T_{\textit{board}}$ is undirected, *arrows* have been added along edges for clarity, indicating the direction in which Coins must be moved for each $R$. $R$ itself is highlighted in red.

**Example case 1:** The original $T_{\textit{board}}$ looks like this:

![](http://campus.codechef.com/SITJMADM/content/COUNTREE-EX1-1of2.png)

Let's examine each $\textit{game}(T_{\textit{board}},R)$ for $R=1,2,3$.

![](http://campus.codechef.com/SITJMADM/content/COUNTREE-EX1-2of2.png)

For $R = 1$:

Alice turns out to be the winner, here; on the first turn, she has two possible moves:

1. Take the Coin in node $3$ and move it to node $2$; or
1. Take the Coin in node $3$ and move it to node $1$.

If she does the latter, then at the end of her move, all Coins will be in node $R=1$, meaning that Bob will not be able to make a move on his turn, leaving Alice the winner: since she is playing perfectly, she does precisely that :)

For $R = 2$:

Bob gets his revenge this time! Alice has two possible initial moves:

1. Take the Coin in node $1$ and move it to node $2$; or
1. Take the Coin in node $3$ and move it to node $2$.

In either case, at the end of her move there will be exactly one Coin remaining on a node other than $R = 2$.  Bob merely has to move this Coin to $R$ to leave Alice with no possible moves on her turn and leave him the winner!

For $R = 3$:

Alice wins again, using very similar reasoning to the $R = 1$ case (her winning move this time around is to take the Coin in node $1$ and move it to node $3$).

We've figured out $\textit{winner}(\textit{game}(T_{\textit{board}}, R))$ for all $R=1,2,3$, and it turns out that Bob only wins the one game, when $R=2$; thus $\textit{BobWinR}=\{2\}$ and our desired sum is:

$$
\sum\limits_{R \in \textit{BobWinR}}2^R = 2^2 = 4
$$

Taking this modulo $10^9+7$, the answer for this testcase is $4$.

**Example case 2:** The original $T_{\textit{board}}$ looks like:

![](http://campus.codechef.com/SITJMADM/content/COUNTREE-EX2-1of7.png)

If we set $R$ to be the node $1$, then we play on the following tree:

![](http://campus.codechef.com/SITJMADM/content/COUNTREE-EX2-2of7.png)

For $R = 1$, it can be shown that if both players play perfectly, the winner will be Bob - so $\textit{winner}(\textit{game}(T_{\textit{board}}, 1))$ is Bob.

For $R = 2$, we have the following:

![](http://campus.codechef.com/SITJMADM/content/COUNTREE-EX2-3of7.png)

and again, it can be shown that $\textit{winner}(\textit{game}(T_{\textit{board}}, 2))$ is Bob.


For $R = 3$:

![](http://campus.codechef.com/SITJMADM/content/COUNTREE-EX2-4of7.png)

Once again, $\textit{winner}(\textit{game}(T_{\textit{board}}, 3))$ can be shown to be Bob.

$R = 4$:


![](http://campus.codechef.com/SITJMADM/content/COUNTREE-EX2-5of7.png)

This time, Alice will win if both players play perfectly i.e. $\textit{winner}(\textit{game}(T_{\textit{board}}, 4))$ is Alice.

$R = 5$:

![](http://campus.codechef.com/SITJMADM/content/COUNTREE-EX2-6of7.png)

Alice wins for $R = 5$, too; $\textit{winner}(\textit{game}(T_{\textit{board}}, 5))$ is Alice.

Finally, $R = 6$:

![](http://campus.codechef.com/SITJMADM/content/COUNTREE-EX2-7of7.png)

Once more, Alice wins in this case - $\textit{winner}(\textit{game}(T_{\textit{board}}, 6))$ is Alice.

We now know $\textit{winner}(\textit{game}(T_{\textit{board}}, R))$ for all possible values of $R$, and we see that $\textit{BobWinR}=\{1, 2, 3\}$, so the value of our desired sum is:

$$
\sum\limits_{R \in \textit{BobWinR}}2^R = 2^1 + 2^2 + 2^3 = 2 + 4 + 8 = 14
$$

After taking this modulo $10^9+7$, the final answer is $14$.
