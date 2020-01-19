In the original [Chef and Gordon Ramsay](https://www.codechef.com/problems/CHGORAM), Chef wanted to close all but three of this Restaurants, and copied his arch-rival Gordon's secret formula for placing his three remaining Restaurants so as to ensure their success.

Chef is now seeking to expand his chain of Restaurants to a new town and, would you know it, the dastardly Gordon has already set up his own chain of Restaurants there! Once again, Chef conducts a little <strike>Industrial Espionage</strike> Market Research and discovers that Gordon's Restaurants have a different secret placement formula in this new town: here again, the most successful way to arrange Restaurants is in _triples_, but this time the optimal placement for success is for all Restaurants in the triple to be _equidistant_ from one another. As before, Chef is wondering how many ways there are to build his triple of Restaurants in this new town in such a way that they are optimally successful?

More formally, the new town has $N$ _sites_ each labelled with a distinct number from $1$ to $N$, and these sites are connected by $N - 1$ bidirectional _roads_, each of length 1 mile, in such a way that any site $v$ can be reached from any other site $u$ by following roads starting at $u$.  For a pair of sites $u$ and $v$, let $\textit{dist}(u, v)$ be the distance, in miles, of the shortest sequence of roads leading from $u$ to $v$.

Chef can't build his Restaurant on just any site he wants: the site $u$ must be _suitable_ if he wishes to build a Restaurant on site $u$.  

Help Chef find the number of ways of choosing triples of distinct sites $p$, $q$ and $r$ to build his Restaurants on such that the arrangement is _optimally successful_ i.e. such that:

* $p$, $q$ and $r$ are all _suitable_; and
* $p$, $q$ and $r$ are _equidistant_ from one another i.e. $\textit{dist}(p,q)=\textit{dist}(q,r)=\textit{dist}(r,p)$

###Input:

- First line will contain $T$, number of testcases. Then the testcases follow. 
- The first line of a testcase is $N$, the number of sites in the new town.
- The next $N-1$ lines describe how sites are connected by roads: each line takes the form of two space-separated integers $u$ and $v$ indicating that the sites labelled $u$ and $v$ should be connected by a road.
- The last line of the testcase describes the _suitablility_ of sites - the line consists of $N$ space-separated integers $s_1, s_2, \dots , s_N$, with each $s_i$ equal to either 0 or 1: if $s_i$ is 1, then the site labelled $i$ is _suitable_; otherwise, the site labelled $i$ is not _suitable_.

###Output:
For each testcase, output in a single line the number of optimally successful triples of sites for his Restaurants.

###Constraints 
- $1 \leq T \leq 1000$
- $1 \leq N \leq 200000$
- $1 \leq u,v \leq N$
- $s_i \in \{0,1\}$ and 
- the sum of $N$ over all $T$ testcases does not exceed $200000$

###Subtasks
**Subtask #1 (5 points):** $1 \leq T \leq 10$ and $1 \leq N \leq 100$

**Subtask #2 (10 points)**: $1 \leq T \leq 10$ and $1 \leq N \leq 1000$

**Subtask #3 (85 points)**: original constraints

###Sample Input:

```
1
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

###Sample Output:

```
18
```

###Explanation

We see that the list of _suitable_ sites consists of the sites labelled $1$, $3$, $4$, $5$ and $7$.  We only need to consider triples formed by choosing three of these suitable sites.

We note that $\textit{dist}(1,5)=2$ miles, and $\textit{dist}(1,7)=2$ miles, and $\textit{dist}(5,7)=2$ miles, so $(1,5,7)$ is a successful arrangement of Restaurants, as are $(1,7,5)$, $(5,1,7)$, $(5,7,1)$, $(7,1,5)$ and $(7,5,1)$ (that's **6** triples so far).

We note also that $\textit{dist}(1,3)=4$ miles, $\textit{dist}(1,4)=4$ miles, and $\textit{dist}(3,4)=4$ miles, so $(1,3,4$) is another successful arrangement of Restaurants, as are $(1,4,3)$, $(3,1,4)$, $(3,4,1)$, $(4,1,3)$ and $(4,3,1)$ (that's **12** so far).

Finally, we see that $\textit{dist}(3,4)=4$ miles, $\textit{dist}(3,7)=4$ miles, and $\textit{dist}(4,7)=4$ miles, so $(3,4,7)$ is another successful arrangement of Restaurants, as are $(3,7,4)$, $(4,3,7)$, $(4,7,3)$, $(7,3,4)$ and $(7,4,3)$, so we are now up to **18** successful triples.

There are no other successful triples, so we print the number `18`.



