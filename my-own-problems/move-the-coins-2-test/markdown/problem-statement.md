Chef is preparing testcases for the ["Move the Coins 2"](https://www.codechef.com/problems/MOVCOIN2) Problem!

For each testcase, he needs to provide a tree $T$ over $N$ nodes labelled $1,2,\dots,N$ and rooted at node $1$, and a list of $Q$ distinct _valid reparentings_ on $T$.

A _reparenting_ $r$ is a pair of nodes $(u,v)$ ($u \ne 1$) of $T$.  The result of applying the reparenting $r$ to $T$ is another graph $T(r)$ over the $N$ nodes formed as follows:

We take a copy of the original $T$, and then remove the edge connecting $u$ to its parent.  We then add a new edge between the nodes $u$ and $v$ to get $T(r)$.

As an example, imagine we have the tree $T$ as shown below:

![image](http://campus.codechef.com/SITJMADM/content/MVCN2TST-EX-reparent1of2.png)

and we wish to apply the reparenting $r=(u,v)=(6,4)$ to it.  The resulting $T(r)$ would look like:

![image](http://campus.codechef.com/SITJMADM/content/MVCN2TST-EX-reparent2of2.png)

A reparenting $r$ is _valid_ if the resulting $T(r)$ is still a _tree_ (that is, a connected graph with no cycles) over the $N$ nodes.

Chef has already decided on this tree $T$, and now just needs to find the $Q$ valid reparentings of $T$.  He does this by forming a list $L$ of all valid reparentings of $T$ and ordering them with a special order.

Let $r=(u,v)$ and $r'=(u',v')$ be valid reparentings.  Then:

- If $u \ne u'$, then $r < r'$ if $u < u'$, and $r > r'$ if $u > u'$
- Otherwise i.e. if $u = u'$, if $\textit{height}(v) \ne \textit{height}(v')$, then $r <
r'$ if $\textit{height}(v) < \textit{height}(v')$ and $r > r'$ if $\textit{height}(v) >
\textit{height}(v')$
- Otherwise i.e. if $u = u'$ and $\textit{height}(v) = \textit{height}(v')$, then $r <
r'$ if $v < v'$ and $r > r'$ if $v > v'$.

where $\textit{height}(x)$ is the distance between the node $x$ and the node $1$.

He then picks $Q$ numbers $c_i$ and chooses the $c_1^\text{th}$ element from $L$, and removes it from $L$; then chooses the $c_2^\text{th}$ remaining element from $L$ and removes it, etc, eventually generating $Q$ distinct 
valid reparentings.  Because Chef doesn't believe in making life easy for himself, these choices are _encrypted_, and he can't find the $\textit{decryptionKey}$ to decrypt a choice 
until he has processed all choices before that one.

More formally, initially $\textit{decryptionKey}=0$.  He reads the first encrypted choice, $\textit{encryptedChoice}_1$ and decrypts it to get $\textit{decryptedChoice}_1$ (i.e. $c_1$ using the earlier notation) using the general formula:

$\textit{decryptedChoice}_i=\textit{decryptionKey} \oplus \textit{encryptedChoice}_i$

(where $\oplus$ denotes the "xor" operator). 

He then finds the $\textit{decryptedChoice}_1^\text{th}$ reparenting in $L$ - let's call it $r_1=(u_1,v_1)$.  He then removes $r_1$ from $L$, and updates his $\textit{decryptionKey}$ using $r_1$ using
the $i=1$ case of the general formula:

$\textit{decryptionKey}=\textit{decryptionKey}+2^i \times u_i + 3^i \times v_i \mod{10^9+7}$

He is now ready for the next encrypted choice, $\textit{encryptedChoice}_2$.  He again decrypts it using the formula above; finds the $\textit{decryptedChoice}_2^\text{th}$ entry in $L$, $r_2=(u_2,v_2)$, removes it, 
updates his $\textit{decryptionKey}$ with it, etc, until all $Q$ encrypted choices have been processed.

Can you help Chef find his $Q$ distinct, valid reparentings corresponding to his encrypted choices? You need not print them: just printing the final value of $decryptionKey$ will suffice to show that you can do it!

### Input
- The first line of the input contains a single integer $T$ denoting the number of test cases. The description of $T$ test cases follows.
- The first line of each test case contains a single integer $N$.
- Each of the next $N-1$ lines contains two space-separated integers $a$ and $b$ denoting that nodes $a$ and $b$ are connected by an edge.
- The next line contains a single integer $Q$
- The $i^\text{th}$ of the next $Q$ lines contains the $i^\text{th}$ encrypted choice, $\textit{encryptedChoice}_i$ for $i=1,2,\dots,Q$.

### Output
For each test case, print a single line containing one integer ― the final value of $\textit{decryptionKey}$ after processing all $Q$ choices.

### Constraints 
- $1 \le T \le 1,000$
- $1 \le N \le 200,000$
- $1 \le Q \le 200,000$
- $1 \le a, b \le N$
- $0 \le \textit{encryptedChoice}_i \le 2^{36}$ for all valid $i$
- $1 \le \textit{decryptedChoice}_i \le |L_{orig}| + 1 - i$ for all valid $i$, where $L_{orig}$ is the original $L$ before any choices have been processed
- the sum of $N$ over all test cases does not exceed $200,000$
- the sum of $Q$ over all test cases does not exceed $200,000$

### Subtasks
**Subtask #1 (5 points):** 
- $T \le 10$
- $N \le 100$
- $Q \le 100$

**Subtask #2 (10 points)**: 
- $T \le 100$
- $N \le 1,000$
- $Q \le 1,000$

**Subtask #3 (85 points)**: original constraints

### Example Input
```
2
5
1 3
1 2
2 4
2 5
3
5
23
72
7
1 7
2 1
6 1
5 4
3 5
1 5
5
9
6
35
313
602
```

### Example Output
```
126
910
```

### Explanation

**Example case 1:** The tree $T$ looks like:

![image](http://campus.codechef.com/SITJMADM/content/MVCN2TST-EX1-T.png)

There are $25$ reparentings $(u,v)$ - too many to draw all $T((u,v)$! - but here is a representative sampling.  Reparentings that are valid are marked with a "✓"; invalid ones with a "✗": 

![image](http://campus.codechef.com/SITJMADM/content/MVCN2TST-EX1-reparentings.png)

By computing all $25$ of the $T((u,v))$, we can form a table of whether $(u,v)$ is a valid reparenting (✓) or not (✗); for this testcase, it is as follows:

```
         v
     1 2 3 4 5 
     ─────────
  1 │✗ ✗ ✗ ✗ ✗ 
  2 │✓ ✗ ✓ ✗ ✗ 
u 3 │✓ ✓ ✗ ✓ ✓ 
  4 │✓ ✓ ✓ ✗ ✓ 
  5 │✓ ✓ ✓ ✓ ✗ 
```

Our initial list $L$, then, is formed by listing all $(u,v)$ with a "✓", and then sorting according to the ordering described above; we get:

```
L: 
 #   u  v  height(v)
 1.  2  1     0
 2.  2  3     1
 3.  3  1     0
 4.  3  2     1
 5.  3  4     2
 6.  3  5     2
 7.  4  1     0
 8.  4  2     1
 9.  4  3     1
10.  4  5     2
11.  5  1     0
12.  5  2     1
13.  5  3     1
14.  5  4     2
```

We need to decrypt each $\textit{encryptedChoice}_i$ for each $i=1,2,\dots,Q$; find the corresponding $(u_i,v_i)$ in the current list $L$; remove it from $L$ and use it to update our $\textit{decryptionKey}$.  The initial value of the $\textit{decryptionKey}$ is $0$.  The first encrypted choice $\textit{encryptedChoice}_1 = 5$.  We decrypt it:

$\textit{decryptedChoice}_1 = \textit{decryptionKey} \oplus \textit{encryptedChoice}_1 = 0 \oplus 5 = 5$

so the reparenting corresponding to the 1st choice is the $5^\text{th}$ entry in the current list $L$, which is $(u_1, v_1) = (3, 4)$.  We update our $\textit{decryptionKey}$ using the formula:

$$\begin{eqnarray}
\textit{decryptionKey}&=&\textit{decryptionKey}+2^1 \times u_1 + 3^1 \times v_1 \mod{10^9+7} \nonumber \\
&=& 0 + 2^1\times3 + 3^1 \times 4\mod{10^9+7} \nonumber \\
&=& 0 + 6 + 12\mod{10^9+7} \nonumber \\
&=& 18 \mod{10^9+7} \nonumber \\
&=&18 \nonumber \\
\end{eqnarray}$$

and remove the $5^\text{th}$ element from $L$.  $L$ is now:

```
L: 
 #   u  v  height(v)
 1.  2  1     0
 2.  2  3     1
 3.  3  1     0
 4.  3  2     1
 5.  3  5     2
 6.  4  1     0
 7.  4  2     1
 8.  4  3     1
 9.  4  5     2
10.  5  1     0
11.  5  2     1
12.  5  3     1
13.  5  4     2
```

On to the $2^\text{nd}$ choice! $\textit{encryptedChoice}_2=23$, so

$\textit{decryptedChoice}_2 = \textit{decryptionKey} \oplus \textit{encryptedChoice}_2 = 18 \oplus 23 = 5$

and so, again, the reparenting $(u_2, v_2)$ corresponding to the $2^\text{nd}$ choice is the $5^\text{th}$ element in our current $L$, which is $(u_2, v_2)=(3,5)$.  We update our $\textit{decryptionKey}$ again:

$$\begin{eqnarray}
\textit{decryptionKey}&=&\textit{decryptionKey}+2^2 \times u_2 + 3^2 \times v_2 \mod{10^9+7} \nonumber \\
&=& 18 + 2^2\times3 + 3^2 \times 5\mod{10^9+7} \nonumber \\
&=& 18 + 12 + 45\mod{10^9+7} \nonumber \\
&=& 75 \mod{10^9+7} \nonumber \\
&=&75 \nonumber \\
\end{eqnarray}$$

and remove the answer from our $L$, leaving $L$ as:

```
 #   u  v  height(v)
 1.  2  1     0
 2.  2  3     1
 3.  3  1     0
 4.  3  2     1
 5.  4  1     0
 6.  4  2     1
 7.  4  3     1
 8.  4  5     2
 9.  5  1     0
10.  5  2     1
11.  5  3     1
12.  5  4     2
```

On to the $3^\text{rd}$ and final choice!  We decrypt $\textit{encryptedChoice}_3 = 72$:

$\textit{decryptedChoice}_3 = \textit{decryptionKey} \oplus \textit{encryptedChoice}_3 = 75 \oplus 72 = 3$

and so the reparenting $(u_3,v_3)$ corresponding to $3^\text{rd}$ choice is the $3^\text{rd}$ entry in $L$, which is $(u_3,v_3)=(3,1)$.  We use this to update $\textit{decryptionKey}$ for the final time:

$$\begin{eqnarray}
\textit{decryptionKey}&=&\textit{decryptionKey}+2^3 \times u_3 + 3^3 \times v_3 \mod{10^9+7} \nonumber \\
&=& 75 + 2^3\times3 + 3^3 \times 1\mod{10^9+7} \nonumber \\
&=& 75 + 24 + 27\mod{10^9+7} \nonumber \\
&=& 126 \mod{10^9+7} \nonumber \\
&=&126 \nonumber \\
\end{eqnarray}$$

We've now processed all $Q$ choices, and the final $\textit{decryptionKey}=126$; thus the answer for this testcase is $126$.

**Example case 2:** The initial tree $T$ is as below:

![image](http://campus.codechef.com/SITJMADM/content/MVCN2TST-EX2-T.png)

There are $49$ possible reparentings; again, we only show a sample of the $49$ $(u,v)$s and their validity:

![image](http://campus.codechef.com/SITJMADM/content/MVCN2TST-EX2-reparentings.png)

The whole table of reparentings and their validity looks like:

```
           v
     1 2 3 4 5 6 7 
     ─────────────
  1 │✗ ✗ ✗ ✗ ✗ ✗ ✗ 
  2 │✓ ✗ ✓ ✓ ✓ ✓ ✓ 
  3 │✓ ✓ ✗ ✓ ✓ ✓ ✓ 
u 4 │✓ ✓ ✓ ✗ ✓ ✓ ✓ 
  5 │✓ ✓ ✗ ✗ ✗ ✓ ✓ 
  6 │✓ ✓ ✓ ✓ ✓ ✗ ✓ 
  7 │✓ ✓ ✓ ✓ ✓ ✓ ✗ 
```

By listing the valid ones and ordering using the ordering described, we get our initial $L$:

```
L: 
 #   u  v  height(v)
 1.  2  1     0
 2.  2  5     1
 3.  2  6     1
 4.  2  7     1
 5.  2  3     2
 6.  2  4     2
 7.  3  1     0
 8.  3  2     1
 9.  3  5     1
10.  3  6     1
11.  3  7     1
12.  3  4     2
13.  4  1     0
14.  4  2     1
15.  4  5     1
16.  4  6     1
17.  4  7     1
18.  4  3     2
19.  5  1     0
20.  5  2     1
21.  5  6     1
22.  5  7     1
23.  6  1     0
24.  6  2     1
25.  6  5     1
26.  6  7     1
27.  6  3     2
28.  6  4     2
29.  7  1     0
30.  7  2     1
31.  7  5     1
32.  7  6     1
33.  7  3     2
34.  7  4     2
```

The initial value of the $\textit{decryptionKey}$ is $0$.  The first encrypted choice $\textit{encryptedChoice}_1 = 9$.  We decrypt it:

$\textit{decryptedChoice}_1 = \textit{decryptionKey} \oplus \textit{encryptedChoice}_1 = 0 \oplus 9 = 9$

so the reparenting $(u_1, v_1)$ corresponding to the first choice is the $9^\text{th}$ element in $L$ i.e. $(u_1, v_1)=(3,5)$.  We update our $\textit{decryptionKey}$:

$$\begin{eqnarray}
\textit{decryptionKey}&=&\textit{decryptionKey}+2^1 \times u_1 + 3^1 \times v_1 \mod{10^9+7} \nonumber \\
&=& 0 + 2^1\times3 + 3^1 \times 5\mod{10^9+7} \nonumber \\
&=& 0 + 6 + 15\mod{10^9+7} \nonumber \\
&=& 21 \mod{10^9+7} \nonumber \\
&=&21 \nonumber \\
\end{eqnarray}$$

and remove $(3,5)$ from the $L$.  To save space, we won't be printing each updated $L$!

On to the next choice. $\textit{encryptedChoice}_2 = 6$; decrypting:

$\textit{decryptedChoice}_2 = \textit{decryptionKey} \oplus \textit{encryptedChoice}_2 = 21 \oplus 6 = 19$

 
so the reparenting $(u_2, v_2)$ corresponding to the $2^\text{nd}$ choice is the $19^\text{th}$ element in $L$ i.e. $(u_2, v_2) = (5, 2)$.  We update our $\textit{decryptionKey}$:

$$\begin{eqnarray}
\textit{decryptionKey}&=&\textit{decryptionKey}+2^2 \times u_2 + 3^2 \times v_2 \mod{10^9+7} \nonumber \\
&=& 21 + 2^2\times5 + 3^2 \times 2\mod{10^9+7} \nonumber \\
&=& 21 + 20 + 18\mod{10^9+7} \nonumber \\
&=& 59 \mod{10^9+7} \nonumber \\
&=&59 \nonumber \\
\end{eqnarray}$$

and remove $(5,2)$ from $L$.

Next: $\textit{encryptedChoice}_3 = 35$, so:

$\textit{decryptedChoice}_3 = \textit{decryptionKey} \oplus \textit{encryptedChoice}_3 = 59 \oplus 35 = 24$

and so $(u_3, v_3) = (6, 7)$.  Update our $\textit{decryptionKey}$:

$$\begin{eqnarray}
\textit{decryptionKey}&=&\textit{decryptionKey}+2^3 \times u_3 + 3^3 \times v_3 \mod{10^9+7} \nonumber \\
&=& 59 + 2^3\times6 + 3^3 \times 7\mod{10^9+7} \nonumber \\
&=& 59 + 48 + 189\mod{10^9+7} \nonumber \\
&=& 296 \mod{10^9+7} \nonumber \\
&=&296 \nonumber \\
\end{eqnarray}$$

and remove $(6,7)$ from $L$.

Next: $\textit{encryptedChoice}_4 = 313$; 

$\textit{decryptedChoice}_4 = \textit{decryptionKey} \oplus \textit{encryptedChoice}_4 = 296 \oplus 313 = 17$

and so $(u_4, v_4)$ is the $17^\text{th}$ element of $L$, which is $(4, 3)$.  Update $\textit{decryptionKey}$:

$$\begin{eqnarray}
\textit{decryptionKey}&=&\textit{decryptionKey}+2^4 \times u_4 + 3^4 \times v_4 \mod{10^9+7} \nonumber \\
&=& 296 + 2^4\times4 + 3^4 \times 3\mod{10^9+7} \nonumber \\
&=& 296 + 64 + 243\mod{10^9+7} \nonumber \\
&=& 603 \mod{10^9+7} \nonumber \\
&=&603 \nonumber \\
\end{eqnarray}$$

and remove $(4,3)$ from $L$.

Last choice! $\textit{encryptedChoice}_5 = 602$; so

$\textit{decryptedChoice}_5 = \textit{decryptionKey} \oplus \textit{encryptedChoice}_5 = 603 \oplus 602 = 1$

The reparenting $(u_5, v_5)$ corresponding to the $5^\text{th}$ choice is the $1^\text{st}$ element of $L$, which is $(u_5, v_5) = (2, 1)$.  Update $\textit{decryptionKey}$:

$$\begin{eqnarray}
\textit{decryptionKey}&=&\textit{decryptionKey}+2^5 \times u_5 + 3^5 \times v_5 \mod{10^9+7} \nonumber \\
&=& 603 + 2^5\times2 + 3^5 \times 1\mod{10^9+7} \nonumber \\
&=& 603 + 64 + 243\mod{10^9+7} \nonumber \\
&=& 910 \mod{10^9+7} \nonumber \\
&=&910 \nonumber \\
\end{eqnarray}$$

We've now processed all $Q$ choices, and the final $\textit{decryptionKey}=910$; thus the answer for this testcase is $910$.




