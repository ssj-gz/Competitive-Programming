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
Very detailed explanation ideally with code snippets.
Ideally editorialist should write his own solution
and write editorial strictly following his solution.

For adding mathematical expressions, LaTeX is required, which is nicely explained at https://en.wikibooks.org/wiki/LaTeX/Mathematics

Some examples of latex here. (Latex is always enclosed in between $ sign)

$ans = \sum_{i = 1}^{N} a_i^k \bmod M$

$\frac{num}{den}$

$\leq$, $\geq$, $\neq$, $\pm$

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

