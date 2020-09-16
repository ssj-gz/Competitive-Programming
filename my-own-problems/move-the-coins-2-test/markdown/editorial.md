# PROBLEM LINK:

[Practice](https://www.codechef.com/problems/MVCN2TST)  
[Div-2 Contest](https://www.codechef.com/SEPT20B/problems/MVCN2TST)  
[Div-1 Contest](https://www.codechef.com/SEPT20A/problems/MVCN2TST)  

**Author and Editorialist:** [Simon St James](https://www.codechef.com/users/ssjgz)  
**Tester:** [Suchan Park](https://www.codechef.com/users/tncks0121)  

# DIFFICULTY:
Medium-Hard

# PREREQUISITES:
Persistence, Graph Theory, AVL Tree, Segment Tree

# PROBLEM:
Given a rooted tree $G$, find the $c_i^{\text{th}}$ element in a list $L$ of pairs of nodes $(u,v)$ that form a valid reparenting, with a specific ordering on $L$, and remove it from $L$, with each $c_i$ being processed online.  A _valid reparenting_ is a $(u,v)$ where severing $u$ from its parent in $G$ and adding an edge between $u$ and $v$ results in a tree.

# QUICK EXPLANATION:
The reparenting $(u,v)$ is _valid_ if and only if $v$ is not a descendant of $u$.  As we find each reparenting, we _simulate_ its removal from $L$ by mapping each $c_i$ to point to the corresponding index $X_i$ in the _original_ $L$ using e.g. an order-statistic tree, so the problem boils down to finding each requested element $(u_i, v_i)$  at a given index $X_i$ in the original $L$, online.

The solution is broken down into three "phases", one for each of the three clauses in the ordering:

**Phase One**: Find $u_i$ (the $\textit{nodeToReparent}$).  For each node $u$ in order, find the number of $v$'s such that $(u,v)$ is a valid reparenting, and form a prefix sum array from this.  The first element in this array strictly greater than $X_i$ gives our $u_i$, and can be found via binary search.

**Phase Two**: Find the $\textit{newParentHeight}$ i.e. $\textit{height}(v_i)$.  We know $u_i$ and need to find the $Y_i^\textit{th}$ element in $L$ that reparents $u_i$, where $Y_i$ is easily computed.  Using some simple observations and pre-computed lookups, we can compute the number of non-descendants of $u_i$ with height less than or to any $h$ ($\textit{findNumNonDescendantsUpToHeight}(u_i, h)$) in $\mathcal{O}(\log N)$.  Similarly to Phase One, we can then perform an Abstract Binary Search on $h$ to find the correct $\textit{height}(v_i)$.

**Phase Three**: Find the final $v_i$ ($\textit{newParent}$).  We know $u_i$ and $\textit{height}(v_i)$, and now just need to find $v_i$ which is the $Z_i^\text{th}$ node at height $\textit{height}(v_i)$ that is not a descendant of $u_i$, where the index $Z_i$ is easily computed.  If we list all nodes with the given height in the order they are visited in a DFS ($\textit{NHDFS}(h)$) in another precomputation step, we see that the descendents of $u_i$ at height $\textit{height}(v_i)$ all lie in an easily-found interval $(l,r)$ in $\textit{NHDFS}(\textit{height}(v_i))$.  By forming _persistent_ AVL Trees of the prefixes and suffixes of $\textit{NHDFS}(h)$ for each $h$ in the precomputation step, we can find in $\mathcal{O}(1)$ a pair of AVL Trees representing all (sorted) nodes to the left of $l$ and all to the right of $r$ in $\textit{NHDFS}(\textit{height}(v_i))$, respectively.  We can then adapt the standard algorithm for finding the $Z_i^\text{th}$ element in two sorted arrays to work with AVL Trees and find the $Z_i^\text{th}$ node not in $(l,r)$ i.e. $v_i$.

# EXPLANATION:
As is often the case, my solution is rather clunkier than other people's (the approach to Phase Three in particular), but I'm going to with my original solution regardless :)

As mentioned in the Brief Explanation, the pair $(u,v)$ is a valid reparenting if and only if $v$ is not a descendent of $u$ in the (rooted) tree $G$.  The size of the list $L$ of valid reparentings is $\mathcal{O}(N^2)$; far too big to construct, let alone to erase from for all $Q$ queries, so we have to be a bit more cunning!

Again as mentioned, we don't actually remove elements from $L$; instead we track the indices of the elements of $L$ that we've removed and use this information to map each new $c_i$ to its corresponding index $X_i$ in the _original_ list $L$.  Most people seem to use gcc's internal `__gnu_pbds::tree` tree for this, though since I was writing a persistent AVL Tree anyway I used that to implement the tracking and mapping; see the $\textit{IndexRemapper}$ class in my code.

Having sidestepped the issue of removing elements from $L$, the problem becomes "find the $X_i^{\textit{th}}$ element in the original $L$, processing each $X_i$ online".  I haven't actually tried it, but I suspect that removing the requirement that the elements be found online would lead to a significantly easier problem.

Anyway, onto the first sub-problem, **Phase One**: finding $u_i$ ($\textit{nodeToReparent}$ in the code) of the remapped $c_i$, $X_i$, in the original list $L$, without constructing $L$!

There are no reparentings with $u=1$, so the first few elements of $L$ are taken up by the valid reparentings that reparent node $2$; then the next few are those that reparent node $3$, etc.  For a given $u$, the number of valid reparentings $(u,v)$ that reparent $u$ is simply the number of $v$ such that $v$ is not a descendent of $u$ i.e. $N-u.\textit{numDescendants}$.  We compute $u.\textit{numDescendants}$ for all $u$ in $\mathcal{O}(N)$ in a precomputation step, and then create a prefix sum array $\textit{CRPS}$ ($\textit{numCanReparentToPrefixSum}$ in the code) such that $\textit{CRPS}(u)$ is the total number of valid reparentings that reparent a node $x$ with $x \le u$.  

As an example, here is the $L$ used in example test case 2 of the Problem, adjusted so that the indices are 0-relative:

[details="$L$ for Example Testcase 2"]

| index      | $u$ | $v$ | $h(v)$ |
| :--------: | :-: | :-: | :----: |
| 0.         | 2   | 1   | 0      |
| 1.         | 2   | 5   | 1      |
| 2.         | 2   | 6   | 1      |
| 3.         | 2   | 7   | 1      |
| 4.         | 2   | 3   | 2      |
| 5.         | 2   | 4   | 2      |
| 6.         | 3   | 1   | 0      |
| 7.         | 3   | 2   | 1      |
| 8.         | 3   | 5   | 1      |
| 9.         | 3   | 6   | 1      |
|10.         | 3   | 7   | 1      |
|11.         | 3   | 4   | 2      |
|12.         | 4   | 1   | 0      |
|13.         | 4   | 2   | 1      |
|14.         | 4   | 5   | 1      |
|15.         | 4   | 6   | 1      |
|16.         | 4   | 7   | 1      |
|17.         | 4   | 3   | 2      |
|18.         | 5   | 1   | 0      |
|19.         | 5   | 2   | 1      |
|20.         | 5   | 6   | 1      |
|21.         | 5   | 7   | 1      |
|22.         | 6   | 1   | 0      |
|23.         | 6   | 2   | 1      |
|24.         | 6   | 5   | 1      |
|25.         | 6   | 7   | 1      |
|26.         | 6   | 3   | 2      |
|27.         | 6   | 4   | 2      |
|28.         | 7   | 1   | 0      |
|29.         | 7   | 2   | 1      |
|30.         | 7   | 5   | 1      |
|31.         | 7   | 6   | 1      |
|32.         | 7   | 3   | 2      |
|33.         | 7   | 4   | 2      |
[/details]

and here is the table of $\textit{CRPS}(u)$ for each $u$ for this example, in order ($u=1$ is omitted):

| $u$ | $\textit{CRPS}(u)$ |
| :-: | :-------------------------------------: |
|2    | 6                                       |
|3    | 12                                      |
|4    | 18                                      |
|5    | 22                                      |
|6    | 28                                      |
|7    | 34                                      |

If we look at the $17^\text{th}$ (0-relative!) element in $L$, we see that it reparents the node $4$.  
If we look at the $18^\text{th}$ element in $L$, we see that it reparents the node $5$.  
If we look at the $28^\text{th}$ element in $L$, we see that it reparents the node $7$.  
In general, hopefully the pattern is clear - _the node reparented by the $X_i^\text{th}$ element of $L$ is the first $u$ such that $\textit{CRPS}(u) > X_i$._  Since $\textit{CRPS}$ is non-increasing, we can easily find this $u$ using a binary search, and so find our $u_i$ in $\mathcal{O}(\log N)$, fulfilling Phase One.

Now that we know $u_i$, we can restrict our attention to the sub-list of $L$ of reparentings that reparent $u_i$; our final desired parenting is at some index $Y_i$ in this sublist.  How do we find $Y_i$? In focussing on this sublist, we are ignoring all the first elements of $L$ that reparent a node $x < u_i$, so we must subtract this number from $X_i$.  By definition, this number is $\textit{CRPS}(u_i - 1)$.  The index $Y_i$ is called $\textit{numOfReparentingThatReparentsNode}$ in the code; I'll be sticking with $Y_i$ here, for obvious reasons :)

So: for **Phase Two**, we need to find the height of $v_i$ ($\textit{newParentHeight}$ in the code), which is the height of $v$ in the $Y_i^\textit{th}$ valid reparenting that reparents $u_i$. In Phase One, we made a tally of all valid reparentings that reparented a node less than or equal to $x$, and found the first $x$ such that this tally exceeded $X_i$; we do a similar trick here in finding the number of reparentings that reparent $u_i$ to a node with height less than or equal to $h$, $\textit{NDUH}(u_i, h)$ ($\textit{findNumNonDescendantsUpToHeight}$ in the code) and find the first $h$ such that $\textit{NDUH}(u_i, h)$ exceeds $Y_i$.  For working out how to compute $\textit{NDUH}(u_i, h)$, consider the following schematic (**click to animate**):

[![](https://etotheipiplusone.com/codechef/editorials/MVCN2TST/images/MVCN2TST_ED_1_THUMB.png)](https://etotheipiplusone.com/codechef/editorials/MVCN2TST/images/MVCN2TST_ED_1_ANIM.gif)

* The set of nodes to which we can reparent $u_i$ consists of those in section $A$ minus a set we'll call $\textit{AD}$: the nodes in $A$ that are descendents of $u_i$
* $\textit{AD}$ is equal to the set $D$ of all descendents of $u_i$ minus the set that we'll call $\textit{DH}$: those nodes $x\in D$ with $x.\textit{height} > h$
* From the schematic, $\textit{DH}$ is precisely the set of _proper descendents_ of all $y$ such that $y\in D$ and $y.\textit{height}=h$. 

Thus, $\textit{NDUH}(u_i, h)=|A|-|AD|=|A|-|D|+|DH|$.  $|A|$ and $|D|$ are trivial to compute so it remains to compute this sum of proper descendents.

Now, in the precomputation step, we perform a DFS and use the common technique of logging, for each node $x$, the "time" at which we first visit $x$ ($x.\textit{dfsBeginVisit}$) and the time at which we finish exploring $x$ ($x.\textit{dfsEndVisit}$). We then form a list, for each $h$, of all nodes $x$ with $x.\textit{height} = h$ ordered by their $\textit{dfsBeginVisit}$, $\textit{NHDFS}(h)$ (called $\textit{nodesAtHeightInDFSOrder}$ in the code). $\textit{NHDFS}$ will come in useful for Phase Three, too.

A well-known fact is that $y$ is a descendant of $x$ if and only if $y.\textit{dfsBeginVisit} > x.\textit{dfsBeginVisit}$ and $y.\textit{dfsEndVisit} < x.\textit{dfsEndVisit}$ and as a consequence, we see that the set of nodes at height $h$ that are descendents of a node $x$ form an _interval_ in $\textit{NHDFS}(h)$, and this interval $[l, r]$ can be found via a binary search on $\textit{NHDFS}(h)$ (see $\textit{descendantRangeFor}$ in the code).  To compute $\textit{DH}$ we need to find the sum of the proper descendents of nodes in an interval of $\textit{NHDFS}(h)$, which can be done by pre-computing prefix arrays ($\textit{numProperDescendantsForNodeAtHeightPrefixSum}$ in the code).

So given a $u_i$ and a height $h$, we can now compute $\textit{NDUH}(u_i, h)$ in $\mathcal{O}(\log N)$, and we need to find the first $h$ such that this value exceeds $Y_i$.  We _could_ list all such $\textit{NDUH}$ for each height $h$, but this would result in an $\mathcal{O}(N^2)$ algorithm, so instead we use the fact that $\textit{NDUH}$ is non-decreasing with $h$ to perform an _Abstract Binary Search_ on $h$, finally finding the object of Phase Two: $\textit{height}(v_i)$.

Now onto **Phase Three**.  We now know $u_i$ and $\textit{height}(v_i)$, and want to find $v_i$ itself (called $\textit{newParent}$ in the code), which is the $Z_i^{\text{th}}$ element in the sublist of $L$ that reparents $u_i$ to a node with height $\textit{height}(v_i)$.  In restricting our attention to this sub-list, we are skipping all elements that reparent $u_i$ to a parent with height less than $\textit{height}(v_i)$, so $Z_i = Y_i - \textit{NDUH}(u_i, \textit{height}(v_i) - 1)$.  If $\textit{height}(v_i)>0$, of course :) $Z_i$ is called $\textit{numOfReparentingForNodeAndNewHeight}$ in the code.

This is a slightly harder sub-problem to solve than Phase Two, but thankfully a bit easier to explain.  Let $H=\textit{NHDFS}(h)$.   Recall that the set of nodes at height $h$ that are descendents of $u_i$ form an interval $[l, r]$ in $H$.  If we formed a list consisting of the first $l - 1$ elements of $H$ and the last $r - 1$ elements of $H$, sorted it, and found the $Z_i^\text{th}$ element in the sorted list, then we'd have our final $v_i$.  This would be far too slow, however.

If we could get a sorted array of the first $l-1$ elements of $H$ and a sorted array of the last $r-1$ elements, we could use the well-known [Find the $k^{\text{th}}$ Element of Two Sorted Arrays](https://www.geeksforgeeks.org/k-th-element-two-sorted-arrays/) algorithm to find the required $Z_i^\text{th}$ element, but obtaining these lists would still be too slow.  We can do something similar, though, using _persistent AVL Trees_.

Recall that _persistent_ (or _versioned_) data structures are structures that are in a certain sense immutable: when we appear to change them, we are in fact bumping the version number and making the change on the _new version_ of the structure, leaving the previous version unchanged and accessible via the prior version number.  For Persistent AVL Trees, we can still insert a value in $\mathcal{O}(\log N)$ but with the added bonus that we can jump to any previous revision (one revision per insertion) of the tree in $\mathcal{O}(1)$.

How does this help? Imagine, in our precomputation step, for each $h$, we created an AVL Tree, $\textit{prefixesForHeight}(h)$.  The first revision (version $0$) of this tree is empty.  We then insert the first value of $H=\textit{NHDFS}(h)$ into the tree; version $1$ contains the first element (the _prefix_ of size one) of $H$, in sorted order.  We then insert the second element of $H$; version $2$ of the tree contains the first two elements (the prefix of size two) of $H$, again in sorted order.  We continue until all elements of $H$ are added, and then create a similar tree, $\textit{suffixesForHeight}(h)$.  Revision $0$ of this tree is empty, and for revision one we add the _last_ element of $H$ (the _suffix_ of size one).  Then we get revision two by adding the last-but-one element of $H$, and so on until all elements have been added.

[![](https://etotheipiplusone.com/codechef/editorials/MVCN2TST/images/MVCN2TST_ED_2_THUMB.png)](https://etotheipiplusone.com/codechef/editorials/MVCN2TST/images/MVCN2TST_ED_2_ANIM.gif)

Now, this doesn't give us the sorted _array_ of the first $l-1$ elements of $H$ or the last $r-1$ elements of $H$, but it does give us, in $\mathcal{O}(1)$, a pair of _trees_ representing this pair of sorted arrays.  We can now adapt the "Find the $k^{\text{th}}$ Element of Two Sorted Arrays" to work with AVL Trees instead of arrays to find the object of Phase Three, $v_i$ (see $\textit{findKthFromPair}$ in the code).

And that's it!

**Complexity Analysis**

* Precomputation: $\textit{nodesAtHeightInDFSOrder}$, $\textit{numProperDescendantsForNodeAtHeightPrefixSum}$ and $\textit{numCanReparentToPrefixSum}$ are $\mathcal{O}(N)$ in total over all $h$; $\textit{prefixesForHeight}$ and $\textit{suffixesForHeight}$ are $\mathcal{O}(N \log N)$
* Phase One - $\mathcal{O}(N)$
* Phase Two - $\mathcal{O}(\log N)$ calls to $\textit{findNumNonDescendantsUpToHeight}$, each of which is $\mathcal{O}(\log N)$ due to the call to $\textit{descendantRangeFor}$
* Phase Three - $\textit{findKthFromPair}$ would be $\mathcal{O}(\log N)$ if it were working with sorted arrays, but AVL Trees give it an extra $\mathcal{O}(\log N)$.

# ALTERNATE EXPLANATION:
Most people solved Phases One and Two in the same way, and most people used some form of _persistence_ for Phase Three, but not many people used AVL Trees: instead, they opted for Persistent Segment Trees (see e.g. the Setter's solution).

There were [one](https://www.codechef.com/viewsolution/37781202) or [two](https://www.codechef.com/viewsolution/37652718) solutions using [Wavelet Trees](https://en.wikipedia.org/wiki/Wavelet_Tree), and a [few](https://www.codechef.com/viewsolution/37738531) using [Merge Sort Trees](https://www.geeksforgeeks.org/merge-sort-tree-smaller-or-equal-elements-in-given-row-range/).

# SOLUTIONS:

[details="Setter's Solution (C++)"]
https://www.codechef.com/viewsolution/37919135
[/details]

[details="Tester's Solution (C++)"]
```cpp
    #include <bits/stdc++.h>
     
    #include <ext/pb_ds/assoc_container.hpp> // Common file 
    #include <ext/pb_ds/tree_policy.hpp> 
      
    typedef __gnu_pbds::tree<long long, __gnu_pbds::null_type, std::less<long long>, __gnu_pbds::rb_tree_tag, 
                 __gnu_pbds::tree_order_statistics_node_update> 
        new_tree; 
     
    namespace Input {
        const int BUFFER_SIZE = int(1.1e5);
     
        char _buf[BUFFER_SIZE + 10];
        int _buf_pos, _buf_len;
     
        char seekChar() {
            if(_buf_pos >= _buf_len) {
                _buf_len = fread(_buf, 1, BUFFER_SIZE, stdin);
                _buf_pos = 0;
            }
            assert(_buf_pos < _buf_len);
            return _buf[_buf_pos];
        }
     
        bool seekEof() {
            if(_buf_pos >= _buf_len) {
                _buf_len = fread(_buf, 1, BUFFER_SIZE, stdin);
                _buf_pos = 0;
            }
            return _buf_pos >= _buf_len;
        }
     
        char readChar() {
            char ret = seekChar();
            _buf_pos++;
            return ret;
        }
     
        long long readLong(long long lb, long long rb) {
            char c = readChar();
            long long mul = 1;
            if(c == '-') {
                c = readChar();
                mul = -1;
            }
            assert(isdigit(c));
     
            long long ret = c - '0';
            char first_digit = c;
            int len = 0;
            while(!seekEof() && isdigit(seekChar()) && ++len <= 19) {
                ret = ret * 10 + readChar() - '0';
            }
            ret *= mul;
     
            if(len >= 2) assert(first_digit != '0');
            assert(len <= 18);
            assert(lb <= ret && ret <= rb);
            return ret;
        }
     
        int readInt(int lb, int rb) {
            return readLong(lb, rb);
        }
     
        void readEoln() {
            char c = readChar();
            assert(c == '\n');
            //assert(c == '\n' || (c == '\r' && readChar() == '\n'));
        }
     
        void readSpace() {
            char c = readChar();
            assert(c == ' ');
        }
    }
    using namespace Input;
     
    const int MAX_N = 200000;
    const int MAX_SUM_N = 200000;
    const int MAX_Q = 200000;
    const int MAX_SUM_Q = 200000;
     
    class DisjointSet {
        std::vector<int> par;
    public:
        DisjointSet(int n): par(n+1) {
            for(int i = 1; i <= n; i++) par[i] = i;
        }
     
        int get(int x) {
            return par[x] == x ? x : (par[x] = get(par[x]));
        }
     
        bool merge(int x, int y) {
            x = get(x);
            y = get(y);
            par[x] = y;
            return x != y;
        }
    };
     
    namespace PersistentSegmentTree {
        struct node {
            int sum, left, right, ts;
        };
     
        node tree[int(1.2e7)]; int NUM_NODES;
        int TIME;
     
        node& get_node(int x) {
            node& ret = tree[x];
            if(ret.ts < TIME) {
                ret = {0, 0, 0, TIME};
            }
            return ret;
        }
     
        node& new_node() {
            return get_node(++NUM_NODES);
        }
     
        int insert(int ni, int nl, int nr, int x) {
            node &nd = get_node(ni);
            int ret = ++NUM_NODES;
            node &new_node = get_node(ret);
     
            new_node = nd;
            if(nl == nr) {
                new_node.sum += 1;
                return ret;
            }
     
            int nm = (nl + nr) >> 1;
            if(x <= nm) {
                new_node.left = insert(nd.left, nl, nm, x);
            }else {
                new_node.right = insert(nd.right, nm+1, nr, x);
            }
     
            new_node.sum = tree[new_node.left].sum + tree[new_node.right].sum;
            return ret;
        }
     
        int get_sum (int ni, int nl, int nr, int x, int y) {
            if(ni == 0 || x > y) return 0;
     
            node &nd = get_node(ni);
            if(nl == x && nr == y) {
                return nd.sum;
            }
            
            int nm = (nl + nr) >> 1;
            int ret = 0;
            if(x <= nm) {
                ret += get_sum(nd.left, nl, nm, x, std::min(y, nm));
            }
            if(nm < y) {
                ret += get_sum(nd.right, nm+1, nr, std::max(x, nm+1), y);
            }
            return ret;
        }
     
        int get_kth(int nl, int nr, std::vector<int> nodes, std::vector<int> coefs, long long &k) {
            const int NUM_CONSIDERING_NODES = nodes.size();
            while(nl < nr) {
                int nm = (nl + nr) >> 1;
     
                int leftsum = 0;
                for (int i = 0; i < NUM_CONSIDERING_NODES; i++) {
                    if (!nodes[i]) continue;
                    auto &nd = get_node(nodes[i]);
                    if (!nd.left) continue;
                    auto &ndl = get_node(nd.left);
                    leftsum += ndl.sum * coefs[i];
                }
     
                if (k < leftsum) {
                    for (int i = 0; i < NUM_CONSIDERING_NODES; i++) {
                        if (!nodes[i]) continue;
                        nodes[i] = get_node(nodes[i]).left;
                    }
                    nr = nm;
                } else {
                    k -= leftsum;
                    for (int i = 0; i < NUM_CONSIDERING_NODES; i++) {
                        if (!nodes[i]) continue;
                        nodes[i] = get_node(nodes[i]).right;
                    }
                    nl = nm+1;
                }
            }
            return nl;
        }
     
        void clear() {
            TIME++;
            NUM_NODES = 0;
        }
    }
     
     
    int sumN = 0, sumQ = 0;
     
    std::vector<int> gph[MAX_N+5];
    int depth[MAX_N+5];
    int subtreeSize[MAX_N+5];
    long long candidatePrefixSum[MAX_N+5];
     
    std::vector<int> ord;
    std::vector<int> nodes_with_depth[MAX_N+5], roots_with_depth[MAX_N+5];
     
    int L[MAX_N+5], R[MAX_N+5];
     
    int dfs (int u, int p) {
        depth[u] = depth[p] + 1;
        ord.push_back(u);
        nodes_with_depth[depth[u]].push_back(u);
        L[u] = ord.size();
        for (int v : gph[u]) if(v != p) subtreeSize[u] += dfs(v, u);
        R[u] = ord.size();
        return ++subtreeSize[u];
    }
     
    int roots[MAX_N+5];
     
    void run() {
        int N = readInt(1, MAX_N);
        readEoln();
        sumN += N;
        assert(sumN <= MAX_SUM_N);
     
        DisjointSet ds(N);
        for (int e = 0; e < N-1; e++) {
            int u = readInt(1, N);
            readSpace();
            int v = readInt(1, N);
            readEoln();
            
            bool merged = ds.merge(u, v);
            assert(merged);
     
            gph[u].push_back(v);
            gph[v].push_back(u);
        }
        
        ord.reserve(N);
        dfs(1, 0);
        for (int i = 1; i <= N; i++) {
            candidatePrefixSum[i] = candidatePrefixSum[i-1] + (N - subtreeSize[i]);
        }
     
        PersistentSegmentTree::clear();
        for (int i = 1; i <= N; i++) {
            roots[i] = PersistentSegmentTree::insert(roots[i-1], 0, N, depth[ord[i-1]]);
        }
        for (int d = 0; d <= N; d++) if(!nodes_with_depth[d].empty()) {
            PersistentSegmentTree::new_node();
            roots_with_depth[d].push_back(PersistentSegmentTree::NUM_NODES);
            for (int u : nodes_with_depth[d]) {
                int new_root = PersistentSegmentTree::insert(roots_with_depth[d].back(), 1, N, u);
                roots_with_depth[d].push_back(new_root);
            }
        }
     
        int Q = readInt(1, MAX_Q);
        readEoln();
        sumQ += Q;
        assert(sumQ <= MAX_SUM_Q);
        
        new_tree usedPositions;
     
        const int MOD = 1'000'000'007;
        long long decryptionKey = 0;
        for (long long pow2 = 1, pow3 = 1, qid = 0; qid < Q; qid++) {
            (pow2 *= 2) %= MOD;
            (pow3 *= 3) %= MOD;
     
            long long encryptedChoice = readLong(0, (1ll << 36));
            readEoln();
     
            long long decryptedChoice = (encryptedChoice ^ decryptionKey) - 1;
            
            long long target = -1;
            {
                long long low = decryptedChoice;
                long long high = std::min(decryptedChoice + qid, candidatePrefixSum[N]);
                while (low <= high) {
                    long long mid = (low + high) / 2;
                    long long currentPosition = mid - usedPositions.order_of_key(mid+1);
                    if (currentPosition < decryptedChoice) {
                        low = mid+1;
                    } else {
                        target = mid;
                        high = mid-1;
                    }
                }
     
                if (target < 0) {
                    printf("qid = %d, dc = %lld %lld\n", qid, decryptedChoice, candidatePrefixSum[N]);
                }
     
                assert(target >= 0);
                usedPositions.insert(target);
            }
            
            // find target-th position from list
     
            // candidatePrefixSum[u-1] <= target < candidatePrefixSum[u]
            int u = std::upper_bound(candidatePrefixSum, candidatePrefixSum + N + 1, target) - candidatePrefixSum;
            target -= candidatePrefixSum[u-1];
     
            int d = PersistentSegmentTree::get_kth(
                0, N, 
                {roots[N], roots[R[u]], roots[L[u]-1]},
                {+1, -1, +1},
                target
            );
     
            int pl = std::distance(
                nodes_with_depth[d].begin(),
                std::lower_bound(
                    nodes_with_depth[d].begin(), nodes_with_depth[d].end(), u,
                    [&](const int &a, const int &b) { return L[a] < L[b]; }
                )
            );
     
            int pr = std::distance(
                nodes_with_depth[d].begin(),
                std::upper_bound(
                    nodes_with_depth[d].begin(), nodes_with_depth[d].end(), ord[R[u]-1],
                    [&](const int &a, const int &b) { return L[a] < L[b]; }
                )
            );
            int v = PersistentSegmentTree::get_kth(
                1, N,
                {roots_with_depth[d].back(), roots_with_depth[d][pr], roots_with_depth[d][pl]},
                {+1, -1, +1},
                target
            );
     
            assert(target == 0);
            assert(v > 0);
            assert(depth[v] == d);
     
            (decryptionKey += pow2 * u + pow3 * v) %= MOD;
        }
        printf("%lld\n", decryptionKey);
        // initialize global vars
        ord.clear();
        for (int u = 0; u <= N; u++) {
            gph[u].clear();
            depth[u] = 0;
            subtreeSize[u] = 0;
            L[u] = R[u] = 0;
            candidatePrefixSum[u] = 0;
            nodes_with_depth[u].clear();
            roots_with_depth[u].clear();
        }
    }
     
    int main() {
    #ifdef IN_MY_COMPUTER
        freopen("example.in", "r", stdin);
    #endif
     
        int T = readInt(1, 1000);
        readEoln();
        while(T--) {
            run();
        }
        assert(feof(stdin));
        return 0;
    } 
```
[/details]
