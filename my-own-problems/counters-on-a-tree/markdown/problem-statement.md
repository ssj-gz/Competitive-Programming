Bob and Alice are playing a game with Counters on a Tree T, which has N nodes numbered from 1 to N (1 <= N <= 100'000).  For a node R (1 <= R <= N), define game(T, R) as the game with the following rules:

1. A copy of T is taken, and for each node v of T, we ensure that there are precisely c(v) Counters on v.
2. Bob and Alice now take turns to make a move, with Alice making the first move.
3. A move consists of taking a counter (c, say) from some node other than R and moving it to an *allowed* node.  Let v_c be the node that c is currently on; then the set of allowed nodes for this c is the set of v != v_c on the shortest path between nodes v_c and R.
4. If a player cannot make a move on their turn (i.e. because all the Counters are on node R), then the game ends and the other player is declared the winner.

For example, if the tree $T$ currently looks like this and, for this game, we have chosen $R=1$:

![image](http://campus.codechef.com/SITJMADM/content/COUNTREE-move-example1of3.png)

Then the current player can pick a coin in any of vertices $v = 2, 4$ or $5$.  Let's assume he picks the coin in $5$, as shown below; then he can move the coin $X$ steps towards $R=1$, where $X=1,2$ or $3$, ending up in vertex $4$, $2$ or $1$, respectively:

![image](http://campus.codechef.com/SITJMADM/content/COUNTREE-move-example2of3.png)

Let's assume he picks $X=2$, so he moves the coin to vertex $2$.  Then the tree now looks like:

![image](http://campus.codechef.com/SITJMADM/content/COUNTREE-move-example3of3.png)

and it is the other player's turn to move.

Let winner(game(T, R)) be whichever of Bob or Alice wins the game(T, R), assuming both play perfectly.

Bob knows some elementary Game Theory, so is able to very quickly predict who will win game(T, R) for a given R.  Unimpressed, Alice gives him a harder task: she challenges him to find winner(game(T, R)) for *all of* R = 1, 2, ... , n.

Can you help Bob figure out the answer? More formally, if Bob_win is the set of R such that winner(game(T, R)) == Bob, then you must calculate the value of

$$
\sum\limits_{R \in \textit{Bob_win}}2^R
$$

Since the result can be very large, please output it mod $1000000007$.
