In the original [Chef and Gordon Ramsay](https://www.codechef.com/problems/CHGORAM) problem, Chef wanted to close all but three of this restaurants, and used his arch-rival Gordon's Secret Restaurant Placement Formula to decide which three to leave open to ensure their success.

Chef is now seeking to expand his chain of restaurants to a new town and, would you know it, the dastardly Gordon has already set up his own chain of restaurants there! Once again, Chef conducts a little <strike>Industrial Espionage</strike> Market Research and discovers that Gordon's Secret Restaurant Placement Formula is different for this new town: the optimal way to arrange restaurants is still in triples, but this time, the restaurants in any optimal triple have to be *equidistant* from each other. Just like before, Chef is wondering about the number of ways to build an optimally successful triple of restaurants in this new town.

More formally, the new town has $N$ *sites* (numbered $1$ through $N$) connected by $N - 1$ bidirectional roads (the length of each road is $1$ mile) in such a way that any site $v$ can be reached from any other site $u$ using these roads. For any pair of sites $u$ and $v$, let $\textit{dist}(u, v)$ be the length, in miles, of the shortest sequence of roads that connects $u$ and $v$.

Some sites have been deemed *suitable* for building a restaurant; other sites are unsuitable. Help Chef find the number of ways to choose an ordered triple of sites $(p, q, r)$ on which he could build his restaurants such that this arrangement of restaurants would be *optimally successful*, i.e.:
1. $p$, $q$ and $r$ are pairwise distinct.
2. $p$, $q$ and $r$ are all suitable sites.
3. $p$, $q$ and $r$ are equidistant from one another, i.e. $\textit{dist}(p,q) = \textit{dist}(q,r) = \textit{dist}(r,p)$.

### Input
- The first line of the input contains a single integer $T$ denoting the number of test cases. The description of $T$ test cases follows.
- The first line of each test case contains a single integer $N$.
- Each of the next $N-1$ lines contains two space-separated integers $u$ and $v$ denoting that sites $u$ and $v$ are connected by a road.
- The last line contains $N$ space-separated integers $s_1, s_2, \ldots, s_N$, where $s_i = 1$ denotes that site $i$ is suitable, while $s_i = 0$ denotes that it is not suitable.

### Output
For each test case, print a single line containing one integer ― the number of triples of sites which result in optimally successful arrangements of restaurants.

### Constraints 
- $1 \le T \le 1,000$
- $1 \le N \le 200,000$
- $1 \le u, v \le N$
- $s_i \in \{0,1\}$ for each valid $i$
- the sum of $N$ over all test cases does not exceed $200,000$

### Subtasks
**Subtask #1 (5 points):** 
- $T \le 100$
- $N \le 100$

**Subtask #2 (10 points)**: 
- $T \le 100$
- $N \le 1,000$

**Subtask #3 (85 points)**: original constraints

### Example Input
```
4
4 
3 2
4 3
1 3
1 1 0 0
5
3 1
2 3
4 3
5 3
1 1 0 1 1
5
1 3
4 3
2 1
1 5
0 1 0 1 1
9
2 6
1 2
7 2
5 8
3 9
8 4
5 9
2 5
1 0 1 1 1 0 1 0 0
```

### Example Output
```
0
24
0
18
```

### Explanation
Throughout, we only care about the suitable sites, since no other site can be part of an optimally successful triple.

**Example case 1:** There are only two suitable sites (the sites $1$ and $2$), so there is no way to form any triple of distinct suitable sites.

**Example case 2:** The suitable sites are $1$, $2$, $4$ and $5$, and the distance between any two of them is always $2$ miles, so any triple consisting of sites drawn from these four suitable sites results in an optimally successful arrangement.

**Example case 3:** There are only three suitable sites (those labelled $2$, $4$ and $5$), but while $\textit{dist}(2,4)=\textit{dist}(4,5)=3$ miles, unfortunately $\textit{dist}(5,2)=2$ miles, so the three suitable sites are not equidistant from each other.

**Example case 4:** The suitable sites are $1$, $3$, $4$, $5$ and $7$.
- Since $\textit{dist}(1,5)=\textit{dist}(5,7)=\textit{dist}(7,1)=2$ miles, the triple $(1,5,7)$ results in an optimally successful arrangement, as do $(1,7,5)$, $(5,1,7)$, $(5,7,1)$, $(7,1,5)$ and $(7,5,1)$.
- Also, since $\textit{dist}(1,3)=\textit{dist}(3,4)=\textit{dist}(4,1)=4$ miles, $(1,3,4)$ is another triple that results in an optimally successful arrangement, as are $(1,4,3)$, $(3,1,4)$, $(3,4,1)$, $(4,1,3)$ and $(4,3,1)$.
- Finally, we see that $\textit{dist}(3,4)=\textit{dist}(4,7)=\textit{dist}(7,3)=4$ miles, so $(3,4,7)$ is another triple that results in an optimally successful arrangement, as are $(3,7,4)$, $(4,3,7)$, $(4,7,3)$, $(7,3,4)$ and $(7,4,3)$. That's $18$ triples in total.

