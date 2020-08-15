A sequel to ["Move the Coins"](https://www.hackerrank.com/challenges/move-the-coins/problem), with a tiny twist :)

Bob and Alice are playing a board game.  The board is in the form of a tree $T$ with $N$ nodes, numbered from $1$ to $N$, and _rooted_ at node $1$. The game involves moving a set of *Coins* around the board, with each node $v$ having $c_v$ Coins on it at the beginning of the game. Define $\textit{game}(T)$ as the game with the following rules:

1. A copy of $T$ is taken, and for each $v=1,2,\dots N$, we ensure that there are precisely $c_v$ Coins on the node numbered $v$.
2. Bob and Alice now take turns to make a move, with Alice making the first move.
3. A move consists of taking a single Coin ($C$, say) from some node other than $1$ and moving it to an *allowed* node. If $v_C$ is the node that $C$ is currently on, then the set of allowed nodes for this $C$ is the set of $u \ne v_C$ on the shortest path between nodes $v_C$ and $1$ (**this is the "tiny twist" from the original Problem**).
4. If a player cannot make a move on their turn (i.e. because all the Coins are on node $1$), then the game ends and the other player is declared the winner.

For example, if the tree $T$ currently looks like this:

![image](http://campus.codechef.com/SITJMADM/content/MOVCOIN2-move-example-1of3.png)

Then the current player can pick a Coin on any of nodes $v = 2, 4$ or $5$ (if there were any Coins on node $3$, picking one of those would also be an option). Let's assume they pick the Coin on $5$, as shown below; then they can move the Coin $X$ steps towards $1$, where $X=1,2$ or $3$, ending up on node $4$, $2$ or $1$, respectively:

![image](http://campus.codechef.com/SITJMADM/content/MOVCOIN2-move-example-2of3.png)

Let's assume they pick $X=2$, so they move the Coin to node $2$. Then the tree now looks like:

![image](http://campus.codechef.com/SITJMADM/content/MOVCOIN2-move-example-3of3.png)

and it is the other player's turn to move.

Let $\textit{winner}(\textit{game}(T))$ be whichever of Bob or Alice wins the $\textit{game}(T)$, assuming both players play perfectly.

Bob knows some elementary Game Theory, so given the _original tree_ $T_\textit{orig}$ and the initial distribution $c_v$ of Coins at the start of the game, he can easily decide $\textit{winner}(\textit{game}(T_\textit{orig}))$ without even having to play!  But as in the original Problem, Alice now gives him a set of _reparenting queries_ $q_i=(u_i, v_i)$, $i=1,2,\ldots\,Q$, and challenges him, for each query $q_i$, to find $\textit{winner}(\textit{game}(T(q_i)))$, where $T(q_i)$ is the _transformed tree_ created from applying the reparenting query $q_i=(u_i, v_i)$ to the original tree $T_{\textit{orig}}$ as follows:

1. Take a copy of the original tree $T_{\textit{orig}}$, along with the $c_v$ Coins on each node $v$.
2. Since this tree is _rooted_ at 1, it makes sense to refer to a _parent_ of a node.  In our copy of $T_{\textit{orig}}$, sever the node $u_i$ from its original parent i.e. remove the edge between $u_i$ and its original parent.
3. _Re-parent_ the node $u_i$ to its _new parent_, $v_i$ i.e. add an edge between $u_i$ and $v_i$.

The resulting tree is our $T(q_i)$.

As an example, assume that the below is our $T_\textit{orig}$ and that our list of $c_v$ describing the number of Coins on a node is as shown (i.e. $c_1=0$, $c_2=2$, etc):

![image](http://campus.codechef.com/SITJMADM/content/MOVCOIN2-EX-reparent1of2.png)

Assume we want to apply the reparenting query $q=(u,v)=(6,3)$.  Then we compute $T((6,3))$ as follows:

![image](http://campus.codechef.com/SITJMADM/content/MOVCOIN2-EX-reparent2of2.png)

Can you help Bob compute, for each of Alice's reparenting queries $q$, the winner of the game played on the transformed board $T(q)$?

More formally, Bob is given the following:

1. An original tree $T_\textit{orig}$
2. A list $c_i$ for $i=1,2,\ldots,N$ describing the number of Coins initially placed on node $v$ of $T_\textit{orig}$
3. A list of reparenting queries $q_i=(u_i, v_i)$ for $i=1,2,\ldots,Q$

Let $\textit{BobWinQ}$ be the set of $i$ such that $\textit{winner}(\textit{game}(T(q_i)))=\text{Bob}$; then you must compute the value:

$$
\sum\limits_{i \in \textit{BobWinQ}}2^i
$$

Since this value can be very large, please output it mod $10^9+7$.

### Input

- The first line of the input contains a single integer $T$ denoting the number of test cases. The description of $T$ test cases follows.
- The first line of a testcase contains a single integer $N$.
- Each of the next $N-1$ lines contains two space-separated integers $a$ and $b$ indicating that the nodes numbered $a$ and $b$ are connected by an edge.
- The next line of the testcase contains $N$ space-separated integers $c_1, c_2, \ldots , c_N$, where $c_i$ denotes the number of Counters that should be placed on the the node numbered $i$ in the tree $T_{\textit{board}}$ at the beginning of a game.
- The next line contains a single integer $Q$ denoting the number of reparenting queries.
- For each $i=1,2,\dots,Q$, the $i^\text{th}$ of the next $Q$ lines consists of two space-separated integers $u_i$ and $v_i$ denoting the reparenting query $q_i=(u_i, v_i)$.

### Output

For each testcase, print a single line containing one integer - the sum, modulo $10^9+7$, of $2^i$ over each $i$ such that $\textit{winner}(\textit{game}(T(q_i))) =$ Bob.

### Constraints 
- $1 \le T \le 1,000$
- $1 \le N \le 200,000$
- $1 \le Q \le 200,000$
- $1 \le a,b \le N$
- $1 \le u_i,v_i \le N$
- $u_i \ne 1$ for all valid $i$
- the graph $T(q_i)$ will always be a _tree_ over the $N$ vertices for all valid $i$
- $0 \le c_i \le 16$ for all valid $i$
- the sum of $N$ over all $T$ testcases does not exceed $200,000$
- the sum of $Q$ over all $T$ testcases does not exceed $200,000$

### Subtasks
**Subtask #1 (5 points):** 

- $T \le 10$
- $N \le 10$
- $Q \le 10$
- $c_i \le 6$ for all valid $i$
- the sum of $c_i$ over all nodes $i$ in a testcase does not exceed $10$

**Subtask #2 (10 points)**: 

* $T \le 100$ 
* $N \le 1,000$
* $Q \le 1,000$

**Subtask #3 (85 points)**: original constraints

### Example Input

```
2
4
1 2
4 3
4 1
1 2 1 1
3
2 3
3 1
3 2
6
1 2
3 1
1 4
5 3
6 4
0 2 0 1 1 0
3
4 2
3 4
5 1
```

### Example Output

```
4
10
```

### Explanation

**Example case 1:** The original tree $T_{\textit{orig}}$ looks like this (the initial Coins specified by the $c_i$ have been added for clarity):

![image](http://campus.codechef.com/SITJMADM/content/MOVCOIN2-EX1-1of4.png)

The first query is $q_1=(u_1, v_1)=(2,3)$, and we construct the corresponding $T(q_1)$ as show below:

![image](http://campus.codechef.com/SITJMADM/content/MOVCOIN2-EX1-2of4.png)

We see that $\textit{winner}(\textit{game}(T(q_1)))$ is Alice.  For example, a winning strategy for Alice would be to move the coin in node $3$ 
to node $4$; then we note that, whatever move Bob makes, Alice can "mirror" his move i.e. if on the next move Bob moves a coin from node $2$ to node $4$,
there will still be a coin left on node $2$, so Alice can copy this move and also move a coin from node $2$ to node $4$.  It's easily verified that Alice
can *always* perform this mirroring move no matter what Bob does, so Alice can always make a move after Bob.  Thus, Bob must be the first player who cannot
move, and so loses $\textit{game}(T(q_1))$

The second query is $q_2=(u_2, v_2)=(3,1)$, and we construct the corresponding $T(q_2)$ as show below:

![image](http://campus.codechef.com/SITJMADM/content/MOVCOIN2-EX1-3of4.png)

We see that $\textit{winner}(\textit{game}(T(q_2)))$ is Bob.   To see this, consider the number of coins on a node other than 1 at the end of each turn.  It's 
hopefully clear that this number will always reduce by exactly one after each turn, no matter what move Alice or Bob make; therefore, since there are initially $4$
coins on nodes other than node $1$, precisely $4$ turns will take place, and the player whose turn it is on the $5^{\text{th}}$ move (i.e. Alice) will lose.  

Thus, $\textit{winner}(\textit{game}(T(q_2)))=$ Bob, and we add $2$ to $\textit{BobWinQ}$,

The third and final query is $q_3=(u_3, v_3)=(3,2)$, and we construct the corresponding $T(q_3)$ as show below:

![image](http://campus.codechef.com/SITJMADM/content/MOVCOIN2-EX1-4of4.png)

Alice wins the game played on $T(q_3)$: if she moves the coin in node $2$ to node $3$ on her first move, then we have a similar situation to that of $q_2$ -
yet again, there will be $4$ coins on nodes other than node $1$, and no matter what move each player makes on their turn, this number will reduce by exactly one each turn.  
Thus, after Alice makes this first move, the game will continue for exactly $4$ more turns, and the player whose turn it is to move on the $6^{\text{th}}$ turn (i.e. Bob)
will be unable to make a move, and so will lose.

So we have $\textit{BobWinQ}=\{2\}$, so the sum we need to compute is

$$
\sum\limits_{i \in \textit{BobWinQ}}2^i = 2^2=4
$$

Taking this modulo $10^9+7$, the final answer for this testcase is $4$.

**Example case 2:** The original tree $T_{\textit{orig}}$ and the transformed trees for each of the three queries are as follows:

![image](http://campus.codechef.com/SITJMADM/content/MOVCOIN2-EX2.png)

It can be shown that:

$$
\textit{winner}(\textit{game}(T(q_1))) = \text{Bob}
$$
$$
\textit{winner}(\textit{game}(T(q_2))) = \text{Alice}
$$
$$
\textit{winner}(\textit{game}(T(q_3))) = \text{Bob}
$$

Thus, only $q_1$ and $q_3$ lead to Bob wins, so $\textit{BobWinQ}=\{1,3\}$, and the sum we need to compute is:

$$
\sum\limits_{i \in \textit{BobWinQ}}2^i = 2^1+2^3=2+8=10
$$

Taking this modulo $10^9+7$, the final answer for this testcase is $10$.
