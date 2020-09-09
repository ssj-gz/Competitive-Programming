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
Should contain concise explanation, clear to advanced coders.

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

[details="Setter's Solution"]
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

