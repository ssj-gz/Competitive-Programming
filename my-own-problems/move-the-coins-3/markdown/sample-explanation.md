The original $T$ looks like this:

![image](https://s3.amazonaws.com/hr-assets/0/1521797383-579a90003c-move-the-coins-2-example-move-sample-explanation-1of7.png)

If we set the root $R$ to be the vertex $1$, then we play on the following board - here, $R$ has been marked in red and the shortest path from a vertex to $R$ has been shown with arrows, purely for explanatory purposes (***TODO*** - although, maybe it makes sense to phrase the whole question in terms of directed graphs?):


![image](https://s3.amazonaws.com/hr-assets/0/1521797394-4344db6499-move-the-coins-2-example-move-sample-explanation-2of7.png)

For $R = 1$, Bob has a winning strategy.

For $R = 2$, we have the following:

![image](https://s3.amazonaws.com/hr-assets/0/1521797409-85e318c8ad-move-the-coins-2-example-move-sample-explanation-3of7.png)

and again, for $R = 2$ Bob has a winning strategy.


For $R = 3$:

![image](https://s3.amazonaws.com/hr-assets/0/1521797424-40f71b48f7-move-the-coins-2-example-move-sample-explanation-4of7.png)

Bob once again has a winning strategy.

$R = 4$:


![image](https://s3.amazonaws.com/hr-assets/0/1521797435-618943bb7a-move-the-coins-2-example-move-sample-explanation-5of7.png)

This time, for $R = 4$, Alice has a winning strategy.

$R = 5$

![image](https://s3.amazonaws.com/hr-assets/0/1521797449-b5c6c77cf6-move-the-coins-2-example-move-sample-explanation-6of7.png)

Alice wins for $R = 5$, too.

Finally, $R = 6$:

![image](https://s3.amazonaws.com/hr-assets/0/1521797461-a6eaf767ea-move-the-coins-2-example-move-sample-explanation-7of7.png)

Once more, Alice wins in this case.

So there are $\textit{Bob_w} = 3$ values of $R$ for which Bob wins: $R = 1, R = 2$ and $R = 3$.  So on the first line we output the value of $\textit{Bob_w}$ (that is, $3$), and on the following three lines, we output $1$, $2$ and $3$.
