In the original [Chef and Gordon Ramsey](https://www.codechef.com/problems/CHGORAM), Chef wanted to close all but three of this Restaurants, and copied his arch-rival Gordon's secret formula to placing his three remaining Restaurants so as to ensure their success.

Chef is now seeking to expand his chain of Restaurants to a new town and, would you know it, the dastardly Gordon has already set up his own chain of Restaurants there! Once again, Chef conducts a little <strike>Industrial Espionage</strike> Market Research and discovers that Gordon's Restaurants have a different secret formula in this new town: here again, the most successful way to arrange Restaurants is in _triples_, but this time the optimal placement for success is for all Restaurants in the triple be _equidistant_ from one another. As before, Chef is wondering how many ways there are to build his triple of Restaurants in this new town in such a way that they are optimally successful?

More formally, the new town has $N$ _sites_ each labelled with a distinct number from $1$ to $N$, and these sites are connected by $N - 1$ _roads_, each of length 1 mile, in such a way that any site $v$ can be reached from any other site $u$ by following roads starting at $u$.  For a pair of sites $u$ and $v$, let $dist(u, v)$ be the distance, in miles, of the shortest sequence of roads leading from $u$ to $v$.

Chef can't just build his Restaurant on just any site he wants: the site $u$ must be _suitable_ if he wishes to build a Restaurant on site $u$.  

Help Chef find the number of ways of choosing triples of sites $p$, $q$ and $r$ such that:

* $p$, $q$ and $r$ are all _suitable_; and
* $p$, $q$ and $r$ are _equidistant_ from one another i.e. $dist(p,q)=dist(q,r)=dist(r,p)$

###Input:

- First line will contain $T$, number of testcases. Then the testcases follow. 
- The first line of a testcase is $N$, the number of sites in the new town.
- The next $N-1$ lines describe how sites are connected by roads: each line takes the form of two space-separated integers $u$ and $v$ indicating that the sites labelled $u$ and $v$ should be connected by a road.
- The next $N$ lines describe the _suitablility_ of sites - each line consists of a single integer which is either 0 or 1: if the $i^{\textit{th}}$ line consists of the number 1, then the site labelled $i$ is _suitable_; otherwise, the site labelled $i$ is not _suitable_.

###Constraints 
- $1 \leq T \leq 1000$
- $1 \leq N$
- the sum of $N$ over all $T$ testcases does not exceed $200000$

###Subtasks
- 5 points : $1 \leq T \leq 10$ and $1 \leq N \leq 100$
- 10 points : $1 \leq T \leq 10$ and $1 \leq N \leq 1000$
- 85 points: original constraints



