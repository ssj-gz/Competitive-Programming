// Simon St James (ssjgz) - 2018-01-29
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main()
{
    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        int n, k;
        cin >> n >> k;

        vector<int> a(n);
        for (int i = 0; i < n; i++)
        {
            cin >> a[i];
        }
        const auto maxRelevantSum = k; // We don't care about sums that exceed k.

        vector<bool> canFormSumFromFirstM(maxRelevantSum + 1);
        // Can always form 0 from the first m ai's by just picking none of the ai's :)
        canFormSumFromFirstM[0] = true;

        for (int m = 0; m < n; m++)
        {
            // After each iteration, canFormSumFromFirstM[s] will be true if and only if
            // we can form s using just a[0], a[1], ... , a[m].
            if (a[m] > maxRelevantSum)
                continue;
            canFormSumFromFirstM[a[m]] = true;
            for (int sum = 0; sum <= maxRelevantSum; sum++)
            {
                if (sum - a[m] >= 0 && canFormSumFromFirstM[sum - a[m]])
                {
                    canFormSumFromFirstM[sum] = true;
                }
            }
        }

        int nearestSumToK = 0;
        for (int sum = 0; sum <= k; sum++)
        {
            if (canFormSumFromFirstM[sum])
            {
                nearestSumToK = sum;
            }
        }

        cout << nearestSumToK << endl;
    }
}
