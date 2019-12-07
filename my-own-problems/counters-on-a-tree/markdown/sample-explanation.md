The original $T$ looks like this:

![image](https://s3.amazonaws.com/hr-assets/0/1521797383-579a90003c-counters-on-a-tree-example-move-sample-explanation-1of7.png)

If we set the root $R$ to be the vertex $1$, then we play on the following tree - here, $R$ has been marked in red, and arrows have been added to the edges to indicate the path from each vertex to $R$, purely for explanatory purposes:


![image](https://s3.amazonaws.com/hr-assets/0/1521797394-4344db6499-counters-on-a-tree-example-move-sample-explanation-2of7.png)

For $R = 1$, it can be shown that if both players play optimally, the winner will be Bob - so $\textit{winner}(T, 1)$ is Bob.

For $R = 2$, we have the following:

![image](https://s3.amazonaws.com/hr-assets/0/1521797409-85e318c8ad-counters-on-a-tree-example-move-sample-explanation-3of7.png)

and again, it can be shown that $\textit{winner}(T,2)$ is Bob.


For $R = 3$:

![image](https://s3.amazonaws.com/hr-assets/0/1521797424-40f71b48f7-counters-on-a-tree-example-move-sample-explanation-4of7.png)

Once again, $\textit{winner}(T,3)$ can be shown to be Bob.

$R = 4$:


![image](https://s3.amazonaws.com/hr-assets/0/1521797435-618943bb7a-counters-on-a-tree-example-move-sample-explanation-5of7.png)

This time, Alice will win if both players play optimally i.e. $\textit{winner}(T,4)$ is Alice.

$R = 5$

![image](https://s3.amazonaws.com/hr-assets/0/1521797449-b5c6c77cf6-counters-on-a-tree-example-move-sample-explanation-6of7.png)

Alice wins for $R = 5$, too; $\textit{winner}(T,5)$ is Alice.

Finally, $R = 6$:

![image](https://s3.amazonaws.com/hr-assets/0/1521797461-a6eaf767ea-counters-on-a-tree-example-move-sample-explanation-7of7.png)

Once more, Alice wins in this case - $\textit{winner}(T,6)$ is Alice.

So there are $\textit{Bob_w} = 3$ values of $R$ for which Bob wins: $R = 1, R = 2$ and $R = 3$.  So on the first line we output the value of $\textit{Bob_w}$ (that is, $3$), and on the following three lines, we output $1$, $2$ and $3$.


