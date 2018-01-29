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
        const auto maxSum = n * *max_element(a.begin(), a.end());

        vector<bool> canFormSumFromFirstM(maxSum + 1);
        // Can always form 0 from the first m ai's by just picking none of the ai's :)
        canFormSumFromFirstM[0] = true;

        for (int m = 0; m <= n; m++)
        {
            // After each iteration, canFormSumFromFirstM[s] will be true if and only if
            // we can form s using just a[0], a[1], ... , a[m].
            canFormSumFromFirstM[a[m]] = true;
            for (int sum = 0; sum <= maxSum; sum++)
            {
                if (sum - a[m] >= 0 && canFormSumFromFirstM[sum - a[m]])
                {
                    canFormSumFromFirstM[sum] = true;
                }
            }
        }

        int nearestSumToK = 0;
        for (int i = 0; i <= k; i++)
        {
            if (canFormSumFromFirstM[i])
            {
                nearestSumToK = i;
            }
        }

        cout << nearestSumToK << endl;
    }
}
