Inspired by some of the difficulties involved in creating testcases for "Move the Coins 2" (MOVCOIN2).

You are given a tree T over N (2 <= N <= 100'000) nodes. Each nodes has a unique label drawn from  the numbers $1,2, \dots N$.  T is rooted at node 1.   A _reparenting_ $q=(u,v)$ is pair of nodes of T, with $u$ being called the _nodeToReparent_ and $v$ being called the _newParent_.

The _graph resulting from reparenting $q$_, $T(q)$ is constructed as follows:

* Set $T(q)$ to be a copy of T
* Remove the edge between the nodes u and u.parent (T is rooted at node 1, remember, so all nodes other than 1 have a parent)
* Add an edge between the nodes u and v

A reparenting $q$ is said to be _valid_ if the resulting $T(q)$ is still a tree over the N nodes.


Let L be the list of all reparentings, ordered as follows.  Define the _height_ of a node as its distance from the root node, 1.

* If u != u', then (u, v) precedes (u', v') in L if u < u' and comes after it if u > u'
* Else (if u == u') but height(v) != height(v'), (u, v) precedes (u', v') if height(v) < height(v') and comes after it if height(v) > height(v')
* Else (if u == u' and height(v) == height(v')), (u, v) precedes (u', v') if v < v' and comes after it if v > v'

For my MOVCOIN2 testcases, I wanted to find $Q$ distinct valid reparentings.  Do this by forming a sequence $c_i$, $i = 1,2, ..., Q$, $1 <= c_i <= |L| - i$.  Pick the $c_1$th reparenting in L and remove it from L - this is the first chosen reparenting. 
Pick the $c_2$th reparenting in L and remove it from L; this is the 2nd chosen reparenting; etc.

So the aim of the Problem is to calculate (and remove) the $c_i$th item in L, with the added twist that each $c_i$ is _encrypted_ in such a way that the $c_i$'s must be _processed online_.  The requirement that the $c_i$'s must be processed online makes things much more interesting :)

SOLUTION

Here's a brief sketch of the solution.  A documented implementation can be found here: http://campus.codechef.com/SITJMADM/viewsolution/33162695/

First of all, let's figure out how to efficiently tell if a reparenting is _valid_ or not: it's hopefully easy to see that $q=(u,v)$ is a valid reparenting if and only if $v$ is not a descendant of $u$.  Thus, the size of |L| is likely to be $O(N^2)$ i.e. far too large to 

Next, let's dispose of the need to remove the elements from L when we've calculated them: we do this simply by keeping L unchanged throughout, and keeping track of the indices of elements of L that we _should_ have removed; with the use of an balance-tree or some other appropriate datastructure, we can transform each (unencrypted) $c_i$ to its index in the _original_ list $L$ in O(log Q), removing the need to actually remove elements from L.

So: we've reduced the Problem now to finding, for a given index i, the ith element in L, online. This proceeds in three Phases of increasing difficulty.

Phase 1: Finding the nodeToReparent (u) of the reparenting.

This is very easy - note that a node $u$ can be re-parented to precisely $N - numDescendants(u)$ different nodes.  During the precomputation phase, we construct an array numCanReparentToPrefixSum, where numCanReparentToPrefixSum[u] is the number of all valid reparentings $q=(u',v)$ such that $u'<=u$ (this precomputation can be performed in $O(N)$).

Our desired nodeToReparent, then, is the first index such that numCanReparentToPrefixSum[nodeToReparent + 1] exceeds i (there might be a few off-by-one errors, here :)), which we can compute in $O(log |L|)=O(log N)$, concluding Phase 1.

So we now need to find the jth element $q=(u,v)$ in L where $u=nodeToReparent$ (the value of $j$ is easily computed), which is the subject of Phase 2.

Phase 2: Find the newParentHeight (i.e. height(v))

This is a bit trickier, and fundamentally relies on an binary search over all O(N) possible heights $h$ and the efficient computation of numNonDescendantsUpToHeight(nodeToReparent, h).  numNonDescendantsUpToHeight(nodeToReparent, h) is defined as:

    the number of nodes x such that height(x) <= h and x is not a descendant of nodeToReparent

With a few observations and precomputations, we can compute a given numNonDescendantsUpToHeight(nodeToReparent, h) in O(log N).

A brief description of how: nodeToReparent can always be reparented to *any* node whose height is < height(nodeToReparent), so we start off with this value (easily computed with a bit of precomputation).

Counted the number of nodes x such that height(x) >= height(nodeToReparent) and height(x) <= h and x not a descendant of nodeToReparent is trickier.  Observe that calculating  the number of descendants d of nodeToReparent subject to height(nodeToReparent) <= height(d) <= h is sufficient to compute this, and call this value Y.  Let X be the set of descendants of nodeToReparent whose height is precisely h.  Then Y = numDescendants(nodeToReparent) - sum over all nodes z in X of the number of *proper descendants* of z.

In our precomputation step, we compute nodesAtHeightInDfsOrder, such that nodesAtHeightInDfsOrder[h] is precisely the listed of nodes at height h, listed in the order in which we performed a DFS from the root node.  The set X mentioned above will be a contiguous *subarray* of nodesAtHeightInDfsOrder[h], and the left and right indices of this array can be computed in O(log N).  We also precompute numProperDescendantsForNodeAtHeightPrefixSum, where numProperDescendantsForNodeAtHeightPrefixSum[h] is essentially the prefix sum array of the number of proper descendants of nodesAtHeightInDfsOrder[h].  We now have everything we need to compute numNonDescendantsUpToHeight(nodeToReparent, h).

We then do a binary search over all values of h until we find the smallest h such that numNonDescendantsUpToHeight(nodeToReparent, h) > j.  Thus, Phase 2 finds newParentHeight in $O(N \times log N \times log N)$.

We now need to find the kth element $q=(u,v)$ in L such that $u=nodeToReparent$ and $height(v) = newParentHeight$, where k is easily computed, and this is the subject of Phase 3.

Phase 3: Finally finding newParent

There's probably plenty of ways of skinning this cat :) To re-cap: for some k, we need to find the kth node (in order of node  x at height h which is *not* a descendant of nodeToReparent.  Once again, the set X of possible x's is a contiguous subarray of nodesAtHeightInDfsOrder[h], and the left and right indices of this range can be computed in O(log N).  Thus, we need to find the kth node in this set.

The way I approached this is as follows.  In the precomputation step, compute, for each h, prefixesForHeight[h] and suffixesForHeight[h].  These are _persistent_ balance-trees, sorted by node label, computed by adding each element in nodesAtHeightInDfsOrder[h] in turn (for prefixesForHeight[h]) and adding each element in nodesAtHeightInDfsOrder[h] _in reverse order_ for suffixesForHeight[h].

We can then use these persistent balance trees to find, in O(1), a pair of balance trees representing the set of elements at height $h$ that are _not_ descendants of nodeToReparent simply by switching prefixesForHeight to the revision representing the balance-tree when we added all elements in nodesAtHeightInDfsOrder[h] to the *left* of the subarray of nodesAtHeightInDfsOrder[h] representing X and suffixesForHeight to the revision representing the balance-tree when we added all elements in nodesAtHeightInDfsOrder[h] to the _right_ of the subarray of nodesAtHeightInDfsOrder[h] representing X.

We can then find the kth node at height h that it not a descendant of nodeToReparent, in order of node id, by using the well-known algorithm for finding the kth element in a pair of sorted arrays (adapted to work with balance-trees instead of arrays).

Thus, we can compute the final Phase and find the final newParent in $O(N \times log N \times log N)$.

We've now calculated the $c_i$ remaining reparenting, and we can use this to update the encryption key and decrypt $c_{i+1}$.




