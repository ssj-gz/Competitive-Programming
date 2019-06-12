// Simon St James (ssjgz) - 2019-06-12
#include <iostream>
#include <vector>

using namespace std;

int64_t computeMaxNonAdjacentSubsetSum(const vector<int>& a)
{
    // Note: we can always get a sum of at least 0
    // simply by choosing the empty set.
    const int n = a.size();
    if (n == 1)
    {
        return a[0] > 0 ? a[0] : 0;
    }

    // maxUpToNNotUsingN[x] is the max we can form out of all non-adjacent
    // subsets of {a[0], a[1], ... a[x - 1] } *which contain a[x - 1]*.
    vector<int64_t> maxUpToNUsingN(n);
    // maxUpToNNotUsingN[x] is the max we can form out of all non-adjacent
    // subsets of {a[0], a[1], ... a[x - 1] } *which do not contain a[x - 1]*.
    // (equiv: the max we can form out of all non-adjacent subsets of 
    // {a[0], a[1], ... a[x - 2] }).
    vector<int64_t> maxUpToNNotUsingN(n);
    maxUpToNUsingN[0] = a[0];
    maxUpToNNotUsingN[0] = 0;

    for (int i = 1; i < n; i++)
    {
        maxUpToNUsingN[i] = a[i] + maxUpToNNotUsingN[i - 1]; // Need maxUpToN*Not*UsingN so that we are non-adjacent.
        maxUpToNNotUsingN[i] = max(maxUpToNUsingN[i - 1], maxUpToNNotUsingN[i - 1]);
    }
    return max(maxUpToNUsingN[n - 1], maxUpToNNotUsingN[n - 1]);
}

int main(int argc, char* argv[])
{
    // Very easy one: hopefully the code is self-explanatory :)
    int n;
    cin >> n;

    vector<int> a(n);
    for (int i = 0; i < n; i++)
    {
        cin >> a[i];
    }
    const auto result = computeMaxNonAdjacentSubsetSum(a);
    cout << result << endl;
}

