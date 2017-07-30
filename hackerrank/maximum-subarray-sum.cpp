#include <cmath>
#include <cstdio>
#include <vector>
#include <set>
#include <iostream>
#include <algorithm>
using namespace std;

int main() {
    int T;
    cin >> T;
    for (int t = 0; t < T; t++)
    {
        long n, m;
        cin >> n >> m;
        vector<long> a(n);
        for (int i = 0; i < n; i++)
        {
            cin >> a[i];
        }
        long earlierPrefixSum = 0;
        set<long> earlierPrefixSums;
        long maxSum = 0;
        long sum = 0;
        for (int i = 0; i < n; i++)
        {
            sum = (sum + a[i]) % m;
            maxSum = max(maxSum, sum);
            // For an index i, Let PS(i) be the sum (a[0] + a[1] + ... + a[i]) mod m.
            // If j < i, then (PS(i) - PS(j)) mod m is the sum of the range from j to i, mod m.  Note that the variable "sum" here represents PS(i).
            // We need to find the j such that (PS(i) - PS(j)) mod m gives us the maximum value over all j < i.
            // The best possible value for PS(j) ("bestToSubtract") would be (sum + 1), which would give us m - 1; we probably won't be that 
            // lucky, though, so at least try for the next "best" one after (sum + 1) (note that as the values we attempt
            // to subtract increase, the resulting value after subtraction decreases, so we need only check the 
            // the first value greater than or equal to (sum + 1), if there is one).
            // We don't actually bother to find j itself - we simply use the (sorted) set earlierPrefixSums to find the possible prefix
            // sums for j < i.
            const long bestToSubtract = (sum + 1);
            const set<long>::iterator bestAvailableToSubtractIter = earlierPrefixSums.lower_bound(bestToSubtract);
            if (bestAvailableToSubtractIter != earlierPrefixSums.end())
            {
                const long bestAvailableToSubtract = *bestAvailableToSubtractIter;
                maxSum = max(maxSum, (sum - bestAvailableToSubtract + m) % m);
            }

            earlierPrefixSums.insert(sum);
        }
        cout << maxSum << endl;
    }
    return 0;
}

