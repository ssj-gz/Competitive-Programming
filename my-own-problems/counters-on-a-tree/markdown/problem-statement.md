Bob and Alice are playing a game with _Counters_ on a Tree $T$, which has $N$ nodes each labelled with a distinct number from $1$ to $N$.  At the beginning of a game, the node $v$ has $c(v)$ Counters on it.  For a node $R$ ($1 \leq R \leq N)$, define $\textit{game}(T, R)$ as the game with the following rules:

1. A copy of $T$ is taken, and for each node $v$ of $T$, we ensure that there are precisely $c(v)$ Counters on $v$.
2. Bob and Alice now take turns to make a move, with Alice making the first move.
3. A move consists of taking a counter ($C$, say) from some node other than $R$ and moving it to an *allowed* node.  Let $v_C$ be the node that $C$ is currently on; then the set of allowed nodes for this $C$ is the set of $v \ne v_C$ on the shortest path between nodes $v_C$ and $R$
4. If a player cannot make a move on their turn (i.e. because all the Counters are on node $R$), then the game ends and the other player is declared the winner.

For example, if the tree $T$ currently looks like this and, for this game, we have chosen $R=1$:

![image](http://campus.codechef.com/SITJMADM/content/COUNTREE-move-example1of3.png)

Then the current player can pick a Counter in any of vertices $v = 2, 4$ or $5$ (if there were any Counters in node $3$, they'd be able to pick one of those, too).  Let's assume they pick the Counter in $5$, as shown below; then they can move the Counter $X$ steps towards $R=1$, where $X=1,2$ or $3$, ending up in vertex $4$, $2$ or $1$, respectively:

![image](http://campus.codechef.com/SITJMADM/content/COUNTREE-move-example2of3.png)

Let's assume they pick $X=2$, so they move the Counter to vertex $2$.  Then the tree now looks like:

![image](http://campus.codechef.com/SITJMADM/content/COUNTREE-move-example3of3.png)

and it is the other player's turn to move.

Let $\textit{winner}(\textit{game}(T, R))$ be whichever of Bob or Alice wins the $\textit{game}(T, R)$, assuming both play perfectly.

Bob knows some elementary Game Theory, so is able to very quickly predict who will win $\textit{game}(T, R)$ for a given $R$.  Unimpressed, Alice gives him a harder task: she challenges him to find $\textit{winner}(\textit{game}(T, R))$ for *all of* $R = 1, 2, \ldots , n$.

Can you help Bob figure out the answer? More formally, if $\textit{Bob_win}$ is the set of $R$ such that $\textit{winner}(\textit{game}(T, R)) =$ Bob, then you must calculate the value of

$$
\sum\limits_{R \in \textit{Bob_win}}2^R
$$

Since the result can be very large, please output it modulo $1000000007$.

###Input

- First line will contain $T$, number of testcases. Then the testcases follow. 
- The first line of a testcase is $N$, the number of nodes in the tree $T$.
- The next $N-1$ lines describe how nodes are connected by edges in $T$: each line consists of two space-separated integers $u$ and $v$ indicating that the nodes labelled $u$ and $v$ are connected by an edge.
- The last line of the testcase describes the _number of initial Counters_ on each node - the line consists of $N$ space-separated integers $c_1, c_2, \ldots , c_N$: for each valid $i$, the node labelled $i$ in $T$ should have $c_i$ Counters placed on it at the beginning of a _game_.

###Output

For each testcase, print a single line containing one integer - the sum, modulo $1000000007$, of $2^R$ over each $R$ such that $\textit{winner}(\textit{game}(T, R)) =$ Bob.

###Constraints 
- $1 \leq T \leq 1000$
- $1 \leq N \leq 100000$
- $1 \leq u,v \leq N$
- $0 \leq c_i \leq 16$ for all valid $i$
- the sum of $N$ over all $T$ testcases does not exceed $200000$

###Subtasks
**Subtask #1 (5 points):** 

- $1 \leq T \leq 10$
- $1 \leq N \leq 10$
- $0 \leq c_i \leq 6$ for all valid $i$
- the sum of $c_i$ over all nodes $i$ in a testcase does not exceed $10$

**Subtask #2 (10 points)**: 

* $1 \leq T \leq 100$ 
* $1 \leq N \leq 1000$

**Subtask #3 (85 points)**: original constraints

###Example Input

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

###Example Output

```
4
14
```

###Explanation

Although the graph is undirected, we've added _arrows_ along edges for clarity, indicating the direction that Counters must be moved in for each $R$.  $R$ itself is highlighted in red.

**Example case 1:** TODO

**Example case 2:** The original $T$ looks like this:

![image](http://campus.codechef.com/SITJMADM/content/COUNTREE-EX2-1of7.png)

If we set $R$ to be the vertex $1$, then we play on the following tree:

![image](http://campus.codechef.com/SITJMADM/content/COUNTREE-EX2-2of7.png)

For $R = 1$, it can be shown that if both players play optimally, the winner will be Bob - so $\textit{winner}(\textit{game}(T, 1))$ is Bob.

For $R = 2$, we have the following:

![image](http://campus.codechef.com/SITJMADM/content/COUNTREE-EX2-3of7.png)

and again, it can be shown that $\textit{winner}(\textit{game}(T, 2))$ is Bob.


For $R = 3$:

![image](http://campus.codechef.com/SITJMADM/content/COUNTREE-EX2-4of7.png)

Once again, $\textit{winner}(\textit{game}(T, 3))$ can be shown to be Bob.

$R = 4$:


![image](http://campus.codechef.com/SITJMADM/content/COUNTREE-EX2-5of7.png)

This time, Alice will win if both players play optimally i.e. $\textit{winner}(\textit{game}(T, 4))$ is Alice.

$R = 5$

![image](http://campus.codechef.com/SITJMADM/content/COUNTREE-EX2-6of7.png)

Alice wins for $R = 5$, too; $\textit{winner}(\textit{game}(T, 5))$ is Alice.

Finally, $R = 6$:

![image](http://campus.codechef.com/SITJMADM/content/COUNTREE-EX2-7of7.png)

Once more, Alice wins in this case - $\textit{winner}(\textit{game}(T, 6))$ is Alice.

We now know $\textit{winner}(\textit{game}(T, R))$ all possible values of $R$, and we see that $\textit{Bob_win}={1, 2, 3}$, so the value of our desired sum is:

$$
\sum\limits_{R \in \textit{Bob_win}}2^R = 2^1 + 2^2 + 2^3 = = 2 + 4 + 8 = 14
$$

After taking this modulo $1000000007$, the final answer is $14$.
