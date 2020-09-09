# PROBLEM LINK:

[Practice](https://www.codechef.com/problems/MOVCOIN2)  
[Div-2 Contest](https://www.codechef.com/SEPT20B/problems/MOVCOIN2)  
[Div-1 Contest](https://www.codechef.com/SEPT20A/problems/MOVCOIN2)  

**Author and Editorialist:** [Simon St James](https://www.codechef.com/users/ssjgz)  
**Tester:** [Suchan Park](https://www.codechef.com/users/tncks0121)

# DIFFICULTY:
Medium-Hard **TODO - revisit this - seems harder than MVCN2TST**

# PREREQUISITES:
Sprague-Grundy, Centroid Decomposition, Bit Manipulation, Ad-hoc

# PROBLEM:
Bob and Alice are playing a game on a board which is a tree, $T$.  Each node of $T$ has some number of coins placed on it.  For a node $R$, define $\textit{game}(T, R)$ to be the game played on $T$ in which players take turns to move a coin from some node other than $R$ strictly towards $R$.  The first player unable to make a move loses the game.  For each $R$, find the winner of $game(T,R)$, assuming both players play perfectly.

# QUICK EXPLANATION:
The game $\textit{game}(T, R)$ is equivalent to the game of Nim, where there is a one-to-one correspondence between coins on the board and piles of stones: for a coin $c$, if $v_c$ is the node containing $c$, then $c$ corresponds to a Nim pile of size $\textit{dist}(R,v_c)$.  Thus, the Sprague-Grundy Theorem can be used. Noting that $x \oplus x=0$ (where $\oplus$ denotes the xor operator), we see that a pair of coins on the same node contribute nothing to the outcome of the game, so only the parity matters: we say that the node $v$ $\textit{hasCoin}$ if the number of coins on $v$ is odd.

Define

$$
R.\textit{grundy}=\bigoplus_{v|v.\textit{hasCoin}}{\textit{dist}(R,v)}
$$

Then by the Sprague-Grundy Theorem, the second player (Bob) wins $\textit{game}(T, R)$ if and only if $R.\textit{grundy}=0$, so we need only calculate this value for all $R$.

For nodes $u$ and $v$ where $v.\textit{hasCoin}$, define the _contribution of $v$ to $u$ as $\textit{dist}(u,v)$_, and the act of updating $u.\textit{grundy}=u.\textit{grundy}\oplus \textit{dist}(u,v)$ as _propagating $v$'s contribution to $u$_.

We form a $\textit{DistTracker}$ data structure with the following API:

```
class DistTracker
    insertDist(distance) { ... }
    addToAllDists(distance) { ... }
    grundyNumber() { ... } // Return the xor sum of all the contained distances 
```

A naive implementation would have $\textit{addToAllDists}$ and $\textit{grundyNumber}$ taking $\mathcal{O}(N)$ each, but by observing the way bits in the binary representation of a number change when we increment it, and using some properties of xor, we can get all of $\textit{DistTracker}$'s operations down to $\mathcal{O}(\log N)$ or better.

We then use Centroid Decomposition plus our $\textit{DistTracker}$ to _collect_ all contributions of $v$ with $v.\textit{hasCoin}$ and _propagate_ the contributions of these $v$s to all other nodes $R$, thus calculating all $R.\textit{grundy}$, as required.

# EXPLANATION:

As mentioned, this is the game of [Nim](https://en.wikipedia.org/wiki/Nim) in disguise: in Nim, we start with some number $M$ of piles, the $i^\textit{th}$ of which contains $p_i$ stones, and players take turns to choose a non-empty pile and take at least one stone from it, until a player cannot make a move, in which case they lose.  In the game $\textit{game}(T, R)$, let $v_c$ be the node containing the coin $c$; then the correspondence between the two games is as follows:

* Each coin $c$ corresponds to a pile of size $\textit{dist}(R,v_c)$ in the game of Nim
* Choosing a coin $c$ from a node not in $R$ and moving it strictly towards $R$ corresponds to choosing a non-empty pile of stones and taking at least one stone from it
* The losing state - where all coins are in $R$ - corresponds to the losing state in Nim where all piles are empty.

The [Sprague-Grundy Theorem](https://en.wikipedia.org/wiki/Sprague-Grundy) makes a few interesting statements including the remarkable result that in the game of Nim, the second player wins if and only if the _Grundy Number_  for the game is $0$, where the grundy number is the xor-sum of all the pile sizes i.e. $p_1 \oplus p_2 \oplus \dots \oplus p_M$. Translating this into our game, we see that Bob wins if and only if the grundy number for the game $\textit{game}(T, R)$, $R.\textit{grundy}$ defined by:

$$
R.\textit{grundy}=\bigoplus_{c}{\textit{dist}(R,v_c)}
$$

is $0$.

Consider two coins $c_1$ and $c_2$, both on the same node $v$.  Their contribution to $R.\textit{grundy}$ is $\textit{dist}(R, v) \oplus \textit{dist}(R, v)$.  But $x \oplus x = 0$ for all $x$, so we can remove _both_ coins without affecting $R.\textit{grundy}$ (and so, the outcome of the game).  In general, for all nodes, we can remove pairs of coins from that node until none remain i.e. until the number of coins on the node is either $0$ or $1$ without affecting the outcome of game, so let's do that.  We say that a node $v$ $\textit{hasNode}$ if the number of coins on $v$ is odd.  We can now rephrase the formula for $R.\textit{grundy}$ to something node-centric rather than coin-centric:

$$
R.\textit{grundy}=\bigoplus_{v|v.\textit{hasCoin}}{\textit{dist}(R,v)}
$$

Recalling the definitions of _contribution_ and _propagation_ from the Quick Explanation, we see that to solve the problem, we merely need to ensure that for each $v$ that $\textit{hasCoin}$ and every $R$, $v$'s contribution to is propagated to $R$.

Let's consider for the moment the special case where $T$ is simply a long chain of nodes.  Imagine we had a $\textit{DistTracker}$ data structure with the below API (a simple implementation is also provided):

```
    class DistTracker
    public:
        insertDist(distance)
        {
            allDistances.append(distance)
        }
        addToAllDists(toAdd) 
        { 
            for each distance in allDistances:
                distance += toAdd
        }
        grundyNumber() 
        { 
            result = 0
            for each distance in allDistances:
                result = result ^ distance
            return result
        }
        clear()
        {
            allDistances = []
        }
    private:
        allDistances = []
    
```

Imagine further that we proceed along the chain of nodes from left to right, at each node $v$ performing the following steps:

* _Propagate_ all the contributions of nodes that $\textit{hasCoin}$ we've seen so far to $v$ via $v.grundy=v.grundy \oplus \textit{distTracker}.\textit{grundyNumber}()$
* If $v.hasCoin$, _collect_ the contribution of $v$ via $\textit{distTracker}.\textit{insertDist}(0)$
* Move to the next node in the chain, calling $\textit{distTracker}.\textit{addToAllDists(1)$} as we go

**TODO - animation showing this**

This way, we _propagate_ the contribution of each $v$ that $\textit{hasCoin}$ to all nodes to $v$'s right.

Let's $\textit{clear}()$ our $\textit{distTracker}$ and repeat the process, this time working in the opposite direction:

**TODO - animation showing this**

Now we've propagated the contribution of each $v$ that $\textit{hasCoin}$ to all nodes to $v$'s right _and_ all nodes to its left i.e. to all other nodes.  Thus, after performing these steps, $R.\textit{grundy}$ is set correctly for all $R$, and we've solved the problem.

The naive implementation of $\textit{DistTracker}$ given above is too slow to be of use: we'll show how to fix this later.  In the meantime, let's show how we can use Centroid Decomposition with our $\textit{DistTracker}$ to collect and propagate all $v$'s that $\textit{hasCoin}$.


# ALTERNATE EXPLANATION:
Could contain more or less short descriptions of possible other approaches.

# SOLUTIONS:

[details="Setter's Solution (C++)"]
**TODO - add publically-accessible link to the following: http://campus.codechef.com/SEP20TST/viewsolution/36836393/ . I can't inline the code here as it would exceed the forum post size limit!**
[/details]

[details="Tester's Solution (Kotlin)"]
```kotlin
   class MOVCOIN2_NAIVE (val br: java.io.BufferedReader, val bw: java.io.BufferedWriter) {
       val MOD = 1000000007L
       fun run() {
           val N = br.readLine()!!.toInt()
           require(N in 1..1000)
   
           val gph = List(N) { mutableListOf<Int>() }
           repeat(N-1) {
               val (u, v) = br.readLine()!!.split(' ').map(String::toInt)
               gph[u-1].add(v-1)
               gph[v-1].add(u-1)
           }
   
           val C = br.readLine()!!.split(' ').map{ it.toInt() % 2 == 1 }
   
           fun getGrundyWithDFS (u: Int, d: Int, p: Int): Int {
               var ret = if(C[u]) d else 0
               for(v in gph[u]) if(v != p) ret = ret xor getGrundyWithDFS(v, d+1, u)
               return ret
           }
   
           var pow2 = 1L
           var ans = 0L
           for (i in 0 until N) {
               pow2 = (pow2 * 2) % MOD
               if (getGrundyWithDFS(i, 0, -1) == 0) {
                   ans += pow2
               }
           }
           ans %= MOD
           bw.write("${ans}\n")
       }
   }
   
   fun main (args: Array<String>) {
       val br = java.io.BufferedReader(java.io.InputStreamReader(System.`in`))
       val bw = java.io.BufferedWriter(java.io.OutputStreamWriter(System.`out`))
   
       val T = br.readLine()!!.toInt()
       require(T in 1..100)
   
       repeat(T) {
           val solver = MOVCOIN2_NAIVE(br, bw)
           solver.run()
       }
   
       bw.flush()
       require(br.readLine() == null)
   }
```
[/details]

