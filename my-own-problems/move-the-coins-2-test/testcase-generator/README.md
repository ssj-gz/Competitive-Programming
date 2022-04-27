The testcase generator consumes quite a lot of stack space; enough to cause a crash when run with most Linux distro's default stack space.

In Ubuntu 20.04, calling this first:

```
ulimit -s 65536
```

(i.e. set the stack space to 64MB) appears sufficient to allow the testcase generator to complete successfully. 
