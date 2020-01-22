Bob and Alice are playing a game with _Counters_ on a Tree $T$, which has $N$ nodes each labelled with a distinct number from $1$ to $N$.  At the beginning of a game, the node $v$ has $c(v)$ Counters on it.  For a node $R$ ($1 \leq R \leq N)$, define $\textit{game}(T, R)$ as the game with the following rules:

1. A copy of $T$ is taken, and for each node $v$ of $T$, we ensure that there are precisely $c(v)$ Counters on $v$.
2. Bob and Alice now take turns to make a move, with Alice making the first move.
3. A move consists of taking a counter ($C$, say) from some node other than $R$ and moving it to an *allowed* node.  Let $v_C$ be the node that $C$ is currently on; then the set of allowed nodes for this $C$ is the set of $v \ne v_C$ on the shortest path between nodes $v_C$ and $R$
4. If a player cannot make a move on their turn (i.e. because all the Counters are on node $R$), then the game ends and the other player is declared the winner.

For example, if the tree $T$ currently looks like this and, for this game, we have chosen $R=1$:

![image](http://campus.codechef.com/SITJMADM/content/COUNTREE-move-example1of3.png)

Then the current player can pick a coin in any of vertices $v = 2, 4$ or $5$.  Let's assume he picks the coin in $5$, as shown below; then he can move the coin $X$ steps towards $R=1$, where $X=1,2$ or $3$, ending up in vertex $4$, $2$ or $1$, respectively:

![image](http://campus.codechef.com/SITJMADM/content/COUNTREE-move-example2of3.png)

Let's assume he picks $X=2$, so he moves the coin to vertex $2$.  Then the tree now looks like:

![image](http://campus.codechef.com/SITJMADM/content/COUNTREE-move-example3of3.png)

and it is the other player's turn to move.

Let $\textit{winner}(\textit{game}(T, R))$ be whichever of Bob or Alice wins the $\textit{game}(T, R)$, assuming both play perfectly.

Bob knows some elementary Game Theory, so is able to very quickly predict who will win $\textit{game}(T, R)$ for a given R.  Unimpressed, Alice gives him a harder task: she challenges him to find $\textit{winner}(\textit{game}(T, R))$ for *all of* $R = 1, 2, \ldots , n$.

Can you help Bob figure out the answer? More formally, if $\textit{Bob_win}$ is the set of R such that $\textit{winner}(\textit{game}(T, R)) =$ Bob, then you must calculate the value of

$$
\sum\limits_{R \in \textit{Bob_win}}2^R
$$

Since the result can be very large, please output it mod $1000000007$.

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
