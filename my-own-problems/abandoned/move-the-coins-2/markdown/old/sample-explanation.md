The initial tree $T$ looks like:

![image](https://s3.amazonaws.com/hr-assets/0/1521719250-e6299fc6bd-move-the-coins-2-example-move-sample-explanation-1of4.png)

Query  $q_1=(4, 1)$ changes the parent of vertex $4$ from its original parent $3$ to its new parent $1$, which results in $T(q_1)$ below:


![image](https://s3.amazonaws.com/hr-assets/0/1521719410-5ff07f5091-move-the-coins-2-example-move-sample-explanation-2of4.png)

On the transformed tree resulting from this query, Alice wins if both players play optimally.

Query  $q_2=(6, 2)$ changes the parent of vertex $6$ from its original parent $4$ to its new parent $2$, which results in $T(q_2)$:


![image](https://s3.amazonaws.com/hr-assets/0/1521719536-a6ad87530d-move-the-coins-2-example-move-sample-explanation-3of4.png)

On the transformed tree resulting from this query, Bob wins.

Query $q_3=(3,2)$ changes the parent of vertex $3$ from its original parent $1$ to its new parent $2$, resulting in $T(q_3)$:


![image](https://s3.amazonaws.com/hr-assets/0/1521719611-bd72daca1c-move-the-coins-2-example-move-sample-explanation-4of4.png)

On the transformed tree resulting from this query, Bob again is the winner if both players play optimally.

Only the $2$ queries $q_2$ and $q_3$ result in a win for Bob, so we output $2$ (the number of winning queries for Bob) on one line, then the winning query numbers ($2$ and $3$) on the next two lines.


