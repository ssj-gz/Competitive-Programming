// Simon St James (ssjgz) - 2017-09-22 10:43
#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>

using namespace std;

int main()
{
    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        int N;
        cin >> N;
        vector<int> a(N);
        for (int i = 0; i < N; i++)
        {
            cin >> a[i];
        }

        bool hasNonNegativeValue = false;
        int largestValue = numeric_limits<int>::min();
        for (const auto value : a)
        {
            if (value >= 0)
                hasNonNegativeValue = true;
            largestValue = max(largestValue, value);
        }

        // Contiguous subarray.
        int maxContiguousSubarraySum = 0;
        int sumOfLargestSubarrayEndingHere = 0;
        if(hasNonNegativeValue)
        {
            for (int i = 0; i < N; i++)
            {
                if (sumOfLargestSubarrayEndingHere + a[i] >= 0)
                {
                    sumOfLargestSubarrayEndingHere += a[i];
                }
                else
                {
                    sumOfLargestSubarrayEndingHere = 0;
                }
                maxContiguousSubarraySum = max(maxContiguousSubarraySum, sumOfLargestSubarrayEndingHere);
            }
        }
        else
        {
            maxContiguousSubarraySum = largestValue;
        }


        // Non-contiguous.
        int maxNonContiguousSubarraySum = 0;
        if (hasNonNegativeValue)
        {
            for (const auto value : a)
            {
                if (value > 0)
                    maxNonContiguousSubarraySum += value;
            }
        }
        else
        {
            maxNonContiguousSubarraySum = largestValue;
        }

        cout << maxContiguousSubarraySum << " " << maxNonContiguousSubarraySum << endl;
    }
}
