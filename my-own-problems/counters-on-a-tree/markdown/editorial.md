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
The game $\textit{game}(T, R)$ is equivalent to the game of Nim, where there is a one-to-one correspondence between coins on the board and piles of stones: for a coin $c$, if $v_c$ is the node containing $c$, then $c$ corresponds to a Nim pile of size $\textit{dist}(R,v_c)$.  Thus, the Sprague-Grundy Theorem can be used. Some simple observations show that the exact value of $c_v$ is not important; only its parity: we set $V_{\textit{coin}}$ to be the set of nodes $v$ such that $c_v$ is odd.

Define

$$
R.\textit{grundy}=\bigoplus_{v\in V_{\textit{coin}}}{\textit{dist}(R,v)}
$$

Then by the Sprague-Grundy Theorem, the second player (Bob) wins $\textit{game}(T, R)$ if and only if $R.\textit{grundy}=0$, so we need only calculate this value for all $R$.

For nodes $u$ and $v$ where $v\in V_{\textit{coin}}$, define the _contribution of $v$ to $u$ as $\textit{dist}(u,v)$_, and the act of updating $u.\textit{grundy}=u.\textit{grundy}\oplus \textit{dist}(u,v)$ as _propagating $v$'s contribution to $u$_.

We form a $\textit{DistTracker}$ data structure with the following API:

```
class DistTracker
    insertDist(distance) { ... }
    addToAllDists(distance) { ... }
    grundyNumber() { ... } // Return the xor sum of all the contained distances 
```

In a naive implementation, at least on of these operations would be $\mathcal{O}(N)$, but by observing how bits in the binary representation of a number change upon incrementing it and using some properties of xor, we can implement all of $\textit{DistTracker}$'s operations in $\mathcal{O}(\log N)$ or better.

We then use Centroid Decomposition plus our $\textit{DistTracker}$ to _collect_ all contributions of $v$ with $v\in V_{\textit{coin}}$ and _propagate_ them to all nodes $R$, thus calculating all required $R.\textit{grundy}$.

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

Consider two coins $c_1$ and $c_2$, both on the same node $v$.  Their contribution to $R.\textit{grundy}$ is $\textit{dist}(R, v) \oplus \textit{dist}(R, v)$.  But $x \oplus x = 0$ for all $x$, so we can remove _both_ coins without affecting $R.\textit{grundy}$ (and so, the outcome of the game).  In general, for all nodes, we can remove pairs of coins from that node until none remain i.e. until the number of coins on the node is either $0$ or $1$ without affecting the outcome of game, so let's do that.  We say that a node $v$ $\textit{hasCoin}$ if the number of coins on $v$ is odd, and set $V_\textit{coin}$ to be the set of all such $v$.  We can now rephrase the formula for $R.\textit{grundy}$ to something node-centric rather than coin-centric:

$$
R.\textit{grundy}=\bigoplus_{v \in V_{\textit{coin}} }{\textit{dist}(R,v)}
$$

Recalling the definitions of _contribution_ and _propagation_ from the Quick Explanation, we see that to solve the problem, we merely need to ensure that for each $v \in V_{\textit{coin}}$ and every $R$, $v$'s contribution to $R$ is propagated to $R$.

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

Imagine further that we proceed along the chain of nodes from left to right performing at each node $v$ the following steps:

* _Propagate_ all the contributions of nodes that $\textit{hasCoin}$ we've seen so far to $v$ via $v.grundy=v.grundy \oplus \textit{distTracker}.\textit{grundyNumber}()$
* If $v\in V_{\textit{coin}}$, _collect_ the contribution of $v$ via $\textit{distTracker}.\textit{insertDist}(0)$
* Move to the next node in the chain, calling $\textit{distTracker}.\textit{addToAllDists}(1)$ to update the contributions we've collected as we go

**TODO - animation showing this**

This way, we _collect_ then  _propagate_ the contribution of each $v\in V_{\textit{coin}}$ to all nodes to $v$'s right.

Let's $\textit{clear}()$ our $\textit{distTracker}$ and repeat the process, this time working in the opposite direction:

**TODO - animation showing this**

Now we've propagated the contribution of each $v \in V_{\textit{coin}}$ to all nodes to $v$'s right _and_ all nodes to its left i.e. to all other nodes.  Thus, after performing these steps, $R.\textit{grundy}$ is set correctly for all $R$, and we've solved the problem.

The naive implementation of $\textit{DistTracker}$ given above is too slow to be of use: we'll show how to fix this later.  In the meantime, let's show how we can use Centroid Decomposition with our $\textit{DistTracker}$ to collect and propagate all $v\in V_{coin}$'s on an arbitrary tree $T$.

I won't go into much detail on [Centroid Decomposition](https://www.geeksforgeeks.org/centroid-decomposition-of-tree/) here as there are doubtless many resources about it, but here are the salient properties for our purposes:

**C1:** Centroid Decomposition of $T$ creates $M$ subtrees ($M$ is $\mathcal{O}(N)$) $T_i$ of $T$ each with a node $c_i$ that is the _centre_ of $T_i$  
**C2:** The $\Sigma_{i=1}^M |T_i|$ is $\mathcal{O}(N\log N)$  
**C3:** Let $u,v$ be any distinct pair of nodes, and let $P(u,v)=[u=p_0, p_1, \ldots, p_k=v]$ be the unique path between $u$ and $v$.  Then there is precisely one $i$ such that $u$ and $v$ are in subtree $T_i$ and $c_i \in P(u,v)$  

Let's pick one of the $i$s.  Let $D_i$ be the degree of $c_i$ in $T_i$, and let $b_1, b_2, \dots, b_D$ be the neighbours of $c_i$ in $T_i$.  We partition the nodes other than $c_i$ of $T_i$ into $D_i$ _branches_, where the node $u$ is in branch $l$ if the first node in the unique path from $c_i$ to $u$ is $b_l$.  For example:

**TODO - image here - medium size $T_i$ with $D = 4$**

With this notation, **C3** can be rephrased as:

**C3:** Let $u,v$ be any distinct pair of nodes; then there is precisely one $i$ such that $u$ and $v$ are in subtree $T_i$ and either:

a. $c_i=u$; or
b. $c_i=v$; or
c. $u$ and $v$ are in different _branches_ of $T_i$

If we could solve the problem of, for every $i=1,2,\ldots, M$ performing all of the following:

1. for each $j=1,2,\dots,D_i$, efficiently propagating the contributions of all $v \in V_\textit{coin}\cap B_j$ to the nodes in the other $D_i-1$ branches of $T_i$;
2. propagating the contributions of all $v \in V_\textit{coin}\cap T_i$ to $c_i$; and 
3. (if $c_i.\textit{hasCoin}$) propagating the contribution of $c_i$ to all other nodes in $T_i$

then from **C3**, we would have propagated the contributions of all $v \in V_{coin}$ to all $u \in T$, and will have solved the problem.

**TODO - oops - forgot about the whole "propagate from one branch to another" bit, plus the animation**

Let's go back to optimising our $\textit{DistTracker}$.  Generally with problems involving xoring things together, it helps to take a bitwise approach, and this turns out to be the case here.  Let's have a look at the binary representation of an increasing series of numbers, and see how each bit flips as we go along.  The numbers along the top are the bit number, with bit number $0$ being the least significant bit.

| N | 5 | 4 | 3 | 2 | 1 | 0 |
| - | - | - | - | - | - | - |
| 0 | 0 | 0 | 0 | 0 | 0 | 0 | 
| 1 | 0 | 0 | 0 | 0 | 0 | 1 | 
| 2 | 0 | 0 | 0 | 0 | 1 | 0 | 
| 3 | 0 | 0 | 0 | 0 | 1 | 1 | 
| 4 | 0 | 0 | 0 | 1 | 0 | 0 | 
| 5 | 0 | 0 | 0 | 1 | 0 | 1 | 
| 6 | 0 | 0 | 0 | 1 | 1 | 0 | 
| 7 | 0 | 0 | 0 | 1 | 1 | 1 | 
| 8 | 0 | 0 | 1 | 0 | 0 | 0 | 

We see that bit number 0 oscillates between 0 and 1 on each increment; bit number 1 is $0$ twice, then $1$ twice, etc; bit number $x$ is $0$ $2^x$ times in a row, then $1$ $2^x$ times in a row, etc.  If we can easily tell how many of the distances we are tracking have their $x^{th}$ bit set, we can easily tell if the grundy number has its $x^{th}$ bit set - it's set if and only if the number of such distances is odd.

Let's re-visit the original example, and this time track whether a distance representing the contribution of a coin has its $x^\text{th}$ bit set by scrolling it through the grid as shown: if a coin enters the red zone (called the "red-one-zone") on the $x^{th}$ row (zero-relative), then that distance has its $x^{th}$ bit set; if it leaves then that bit is unset.

**TODO - animation showing this**

Note that the grid doesn't have a **TODO** th row as, since it only has **TODO** nodes, the max distance between two nodes is **TODO**, and so a tracked distance can never enter the 1-red-zone for that row so we can ignore that row.  Similar logic is used to reduce $\textit{m\_numBits}$ in the $\textit{DistTracker}$ implementation.

The computation of the grundy number (the xor of all the tracked distances) has been rolled into $\textit{addToAllDists()}$, so $\textit{grundyNumber}()$ is now $\mathcal{O}(1)$ instead of $\mathcal{O}(N)$ as it was before, so this is an improvement; however, we still have to move all coins on each call to $\textit{addToAllDists}()$ leaving it as $\mathcal{O}(N)$, so this appears to have gained us little.

However, what if, instead of adjusting all distances by to the right on a call to $\textit{addToAllDists}(1)$ and tracking which distances enter the 1-red-zone for each bit number $x$, why don't we scroll the $x$ 1-red-zones by $1$ in the opposite direction and count how many coins they hit or leave? Since $x$ is $\mathcal{O}(\log N)$, this turns an $\mathcal{O}(N)$ operation into a $\mathcal{O}(\log N)$ one, so all operations on $\textit{DistTracker}$ are now $\mathcal{O}(\log N)$ in the worst case.

**TODO - animation showing this**

And that's it!

**TODO - end less lamely, and do the complexity analysis, as well.  Should mention somewhere that reducing m_numBits gives _asymptotic_ gains i.e. without it, _creating_ a $\textit{DistTracker}$ for each Centroid $T_i$ would contribute $O(N^2)$ to the runtime**


# ALTERNATE EXPLANATION:
Could contain more or less short descriptions of possible other approaches.

# SOLUTIONS:

[details="Setter's Solution (C++)"]
**TODO - add publically-accessible link to the following: http://campus.codechef.com/SEP20TST/viewsolution/36836393/ . I can't inline the code here as it would exceed the forum post size limit!**
[/details]

[details="Tester's Solution (Kotlin)"]
```kotlin
package MOVCOIN2

class Movcoin2Solver(private val N: Int, private val gph: List<List<Int>>, private val hasToken: List<Boolean>) {
    private class XorOfElementPlusConstant (elements: List<Int>, val constantMax: Int) {
        private val MAX_BITS = 17

        val xored = IntArray(constantMax+1)

        init {
            for(b in 0 until MAX_BITS) {
                var l = (1 shl b)
                var r = (1 shl (b+1)) - 1

                var cnt = 0

                val freq = IntArray(constantMax+1)
                for(it in elements) {
                    val target = it and ((1 shl (b+1)) - 1)
                    freq[target] = (freq[target] ?: 0) + 1
                    if (target in l..r) cnt++
                }

                for (d in 0..constantMax) {
                    if (cnt % 2 == 1) xored[d] += 1 shl b

                    cnt -= freq.getOrElse(r) { 0 }

                    l--
                    if(l < 0) l = (1 shl (b+1)) - 1
                    r--
                    if(r < 0) r = (1 shl (b+1)) - 1

                    cnt += freq.getOrElse(l) { 0 }
                }
            }
        }

        fun getXorOfElementsPlus(constant: Int) = xored[constant]
    }

    private val marked = BooleanArray(N)

    private val subtreeSize = IntArray(N)
    private val getMaxSubtreeSizeWhenUIsRemoved = IntArray(N)

    private fun getCentroidInComponentOf(root: Int): Int {
        val queue: java.util.Queue<Pair<Int,Int>> = java.util.ArrayDeque<Pair<Int,Int>>()
        val order = mutableListOf<Pair<Int,Int>>()

        queue.add(Pair(root, -1))
        while(!queue.isEmpty()) {
            order.add(queue.peek())
            val (u, p) = queue.poll()
            subtreeSize[u] = 1
            getMaxSubtreeSizeWhenUIsRemoved[u] = 0
            for(v in gph[u]) if(!marked[v] && v != p) queue.add(Pair(v, u))
        }

        order.reverse()

        for((u, p) in order) {
            if(p >= 0) subtreeSize[p] += subtreeSize[u]
        }

        val numNodes = subtreeSize[root]

        for((u, p) in order) {
            getMaxSubtreeSizeWhenUIsRemoved[u] = maxOf(getMaxSubtreeSizeWhenUIsRemoved[u], numNodes - subtreeSize[u])
            if (p >= 0) {
                getMaxSubtreeSizeWhenUIsRemoved[p] = maxOf(getMaxSubtreeSizeWhenUIsRemoved[p], subtreeSize[u])
            }
            if (getMaxSubtreeSizeWhenUIsRemoved[u] <= numNodes / 2) {
                return u
            }
        }

        return -1
    }

    private fun getGrundys(): List<Int> {
        val grundy = IntArray(N)

        fun process(root: Int, initialD: Int) {
            val order = mutableListOf<Pair<Int,Int>>()

            val queue: java.util.Queue<Triple<Int,Int,Int>> = java.util.ArrayDeque<Triple<Int,Int,Int>>()

            queue.add(Triple(root, -1, initialD))
            while(!queue.isEmpty()) {
                val (u, p, d) = queue.poll()
                order.add(Pair(u, d))
                for(v in gph[u]) if(!marked[v] && v != p) queue.add(Triple(v, u, d+1))
            }

            val distances = mutableListOf<Int>()
            for((u, d) in order) if(hasToken[u]) distances.add(d)

            val maxDistance = order.maxBy(Pair<Int,Int>::second)!!.second

            val ds = XorOfElementPlusConstant(distances, maxDistance)
            for((u, d) in order) grundy[u] = grundy[u] xor ds.getXorOfElementsPlus(d)
        }

        val queue: java.util.Queue<Int> = java.util.ArrayDeque<Int>()

        queue.add(0)
        process(0, 1)

        while(!queue.isEmpty()) {
            val q = queue.poll()

            process(q, 1)

            val u = getCentroidInComponentOf(q)
            marked[u] = true
            process(u, 0)

            for(v in gph[u]) if(!marked[v]) queue.add(v)
        }

        //println(grundy.toList())
        return grundy.toList()
    }

    private fun getAnswer(grundy: List<Int>): Long {
        val MOD = 1000000007
        var pow2 = 1
        var ans = 0L
        for(value in grundy) {
            pow2 *= 2
            pow2 %= MOD
            if(value == 0) ans += pow2
        }
        return ans % MOD
    }

    fun run() = getAnswer(getGrundys())
}

class Movcoin2Connector(private val br: java.io.BufferedReader, private val bw: java.io.BufferedWriter) {
    var sumN = 0

    fun checkConstraints() {
        require(sumN <= 200000)
    }

    fun run() {
        val N = br.readLine()!!.toInt()
        require(N in 1..200000)

        val grp = IntArray(N) { it }
        fun getGroup(x: Int): Int{
            val parents = generateSequence(x, { grp[it] }).takeWhile { grp[it] != it }
            val r = grp[parents.lastOrNull() ?: x]
            parents.forEach{ grp[it] = r }
            return r
        }

        val gph = List(N) { mutableListOf<Int>() }
        repeat(N-1) {
            val (a, b) = br.readLine()!!.split(' ').map{ it.toInt() - 1 }
            gph[a].add(b)
            gph[b].add(a)

            val p = getGroup(a)
            val q = getGroup(b)
            require(p != q)
            grp[p] = q
        }

        val C = br.readLine()!!.split(' ').map(String::toInt)
        require(C.all{ it in 0..16 })

        val solver = Movcoin2Solver(N, gph, C.map{ it % 2 == 1 })
        bw.write("${solver.run()}\n")
    }
}

fun main (args: Array<String>) {
    val br = java.io.BufferedReader(java.io.InputStreamReader(System.`in`))
    val bw = java.io.BufferedWriter(java.io.OutputStreamWriter(System.`out`))

    val T = br.readLine()!!.toInt()
    require(T in 1..1000)

    val connector = Movcoin2Connector(br, bw)
    repeat(T) {
        connector.run()
    }
    connector.checkConstraints()

    bw.flush()
    require(br.readLine() == null)
}
```
[/details]

