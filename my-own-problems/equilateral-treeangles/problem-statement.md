In the original [Chef and Gordon Ramsay](https://www.codechef.com/problems/CHGORAM) problem, Chef wanted to close all but three of this restaurants, and used his arch-rival Gordon's Secret Restaurant-Placement Formula to decide which three to leave open to ensure their success.

Chef is now seeking to expand his chain of restaurants to a new town and, would you know it, the dastardly Gordon has already set up his own chain of restaurants there! Once again, Chef conducts a little <strike>Industrial Espionage</strike> Market Research and discovers that Gordon's Secret Restaurant-Placement Formula is different for this new town: the _optimally successful_ way to arrange restaurants is still in _triples_, but this time the best placement for the restaurants in a triple is one where they are _equidistant_ from one another. As before, Chef is wondering how many ways there are to build an optimally successful triple of restaurants in this new town?

More formally, the new town has $N$ _sites_ each labelled with a distinct number from $1$ to $N$, and these sites are connected by $N - 1$ bidirectional _roads_, each of length 1 mile, in such a way that any site $v$ can be reached from any other site $u$ by following roads starting at $u$. For a pair of sites $u$ and $v$, let $\textit{dist}(u, v)$ be the distance, in miles, of the shortest sequence of roads leading from $u$ to $v$.

Chef can't build a restaurant on just any site; only on sites that are deemed _suitable_.

Help Chef find the number of ways of choosing triples $(p, q, r)$ of sites on which to build his restaurants such that the arrangement is _optimally successful_ i.e. such that:

1. $p$, $q$ and $r$ are _pairwise distinct_.
2. $p$, $q$ and $r$ are all _suitable_.
3. $p$, $q$ and $r$ are _equidistant_ from one another i.e. $\textit{dist}(p,q)=\textit{dist}(q,r)=\textit{dist}(r,p)$.

___
###Input

- First line will contain $T$, number of testcases. Then the testcases follow. 
- The first line of a testcase is $N$, the number of sites in the new town.
- The next $N-1$ lines describe how sites are connected by roads: each line consists of two space-separated integers $u$ and $v$ indicating that the sites labelled $u$ and $v$ are connected by a road.
- The last line of the testcase describes the _suitablility_ of sites - the line consists of $N$ space-separated integers $s_1, s_2, \ldots , s_N$: for each valid $i$, if $s_i$ is 1, then the site labelled $i$ is _suitable_; otherwise, the site labelled $i$ is **not** _suitable_.

___
###Output
For each testcase, print a single line containing one integer - the number of optimally successful triples of sites for Chef's new restaurants.

___
###Constraints 
- $1 \leq T \leq 1000$
- $1 \leq N \leq 200000$
- $1 \leq u,v \leq N$
- $s_i \in \{0,1\}$ for all valid $i$
- the sum of $N$ over all $T$ testcases does not exceed $200000$

___
###Subtasks
**Subtask #1 (5 points):** 

* $1 \leq T \leq 100$
* $1 \leq N \leq 100$

**Subtask #2 (10 points)**: 

* $1 \leq T \leq 100$ 
* $1 \leq N \leq 1000$

**Subtask #3 (85 points)**: original constraints

___
###Example Input

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

___
###Example Output

```
0
24
0
18
```

___
###Explanation

Throughout, we only care about the _suitable_ sites: no other site can be part of an optimally successful triple.

**Example case 1:** There are only two _suitable_ sites (the sites labelled $1$ and $2$), so there is no way of forming any triples of distinct suitable sites: the number of optimally successful triples for this testcase is `0`.

**Example case 2:** The suitable sites are those labelled $1$, $2$, $4$ and $5$, and $\textit{dist}(1,2)=\textit{dist}(1,4)=\textit{dist}(1,5)=\textit{dist}(2,4)=\textit{dist}(2,5)=\textit{dist}(4,5)=2$ miles, so _any_ triple consisting of sites drawn from these four suitable sites is optimally successful. In no particular order, the list of all such triples is $(1, 2, 4)$, $(1, 2, 5)$, $(1, 4, 2)$, $(1, 4, 5)$, $(1, 5, 2)$, $(1, 5, 4)$, $(2, 1, 4)$, $(2, 1, 5)$, $(2, 4, 1)$, $(2, 4, 5)$, $(2, 5, 1)$, $(2, 5, 4)$, $(4, 1, 2)$, $(4, 1, 5)$, $(4, 2, 1)$, $(4, 2, 5)$, $(4, 5, 1)$, $(4, 5, 2)$, $(5, 1, 2)$, $(5, 1, 4)$, $(5, 2, 1)$, $(5, 2, 4)$, $(5, 4, 1)$ and $(5, 4, 2)$. There are $24$ of them, so the answer for this testcase is `24`.

**Example case 3:** There are only three suitable sites (those labelled $2$, $4$ and $5$), but while $\textit{dist}(2,4)=\textit{dist}(4,5)=3$ miles, unfortunately $\textit{dist}(5,2)=2$ miles $\neq 3$ miles, so the three suitable sites are not equidistant from each other, and the answer for this testcase is `0`.

**Example case 4:** The suitable sites are those labelled $1$, $3$, $4$, $5$ and $7$. We note that $\textit{dist}(1,5)=\textit{dist}(5,7)=\textit{dist}(7,1)=2$ miles, so $(1,5,7)$ is a successful arrangement of restaurants, as are $(1,7,5)$, $(5,1,7)$, $(5,7,1)$, $(7,1,5)$ and $(7,5,1)$ (that's **6** triples so far).

We note also that $\textit{dist}(1,3)=\textit{dist}(3,4)=\textit{dist}(4,1)=4$ miles, so $(1,3,4)$ is another successful arrangement of restaurants, as are $(1,4,3)$, $(3,1,4)$, $(3,4,1)$, $(4,1,3)$ and $(4,3,1)$ (that's **12** so far).

Finally, we see that $\textit{dist}(3,4)=\textit{dist}(4,7)=\textit{dist}(7,3)=4$ miles, so $(3,4,7)$ is another successful arrangement of restaurants, as are $(3,7,4)$, $(4,3,7)$, $(4,7,3)$, $(7,3,4)$ and $(7,4,3)$, so we are now up to **18** successful triples.

There are no other successful triples, so the answer for this testcase is `18`.



