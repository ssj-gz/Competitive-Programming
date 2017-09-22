// Simon St James (ssjgz) - 2017-09-22 10:43
#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>

using namespace std;

int main()
{
    // The non-contiguous case is easy (the sum of all positive values if there are any; else, the
    // single largest value), but the contiguous case is harder.
    //
    // The latter has a "classic"  solution which I've seen before, but the version I saw had no
    // proof/ derivation, so let's have a go at that now.
    //
    // For each i = 0, ... N - 1, we maintain the following:
    //
    //   maxEndingAt[i].beginPos
    //   maxEndingAt[i].sum
    //
    // with the following invariants:
    //   
    //   0 <= maxEndingAt[i].beginPos <= i + 1
    //   maxEndingAt[i].sum = {sum j = maxEndingAt.beginPos .. i { a[j] }} [i.e. setting maxEndingAt[i].beginPos gives us maxEndingAt[i].sum]
    //   Let b' be any index 0 <= i + 1: then {sum j = b' .. i { a[j] }} <= maxEndingAt[i].value.
    //
    // The case where maxEndingAt[i].beginPos = i + 1 corresponds the the empty subarray with 0 sum
    // which is not allowed in this problem but which, as we see, poses no issues.
    //
    // Claim that given maxEndingAt[i - 1].beginPos and maxEndingAt[i - 1].sum, then the following update procedure:
    //
    //   if maxEndingAt[i - 1].sum + a[i] >= 0:
    //     maxEndingAt[i].beginPos = maxEndingAt[i - 1].beginPos
    //     maxEndingAt[i].sum = maxEndingAt[i - 1].sum + a[i] (this is implicit from picking maxEndingAt[i].beginPos, but let's make it explicit)
    ///  else
    //    maxEndingAt[i].sum = 0
    //    maxEndingAt[i].beginPos = i + 1 (i.e. empty subarray).
    //   
    // We use induction on i, so assume that maxEndingAt[i - 1].sum/beginPos fulfils all the requirements.
    // Certainly, if maxEndingAt[i - 1].sum + a[i] (= {sum j = maxEndingAt[i - 1].beginPos .. i - 1 { a[j] }}) >= 0, then 
    // setting maxEndingAt[i].beginPos = maxEndingAt[i - 1].beginPos gives maxEndingAt[i].sum = {sum j = maxEndingAt[i].beginPos .. i { a[j] }}
    // = { sum j = maxEndingAt[i - 1].beginPos .. i { a[j] }} + a[i] = maxEndingAt[i - 1].sum + a[i].
    // i.e. there is a value of maxEndingAt[i].beginPos such that maxEndingAt[i].sum = maxEndingAt[i] + a[i], so
    // maxEndingAt[i].sum >= maxEndingAt[i] + a[i]: but what if there is a better b' such that setting maxEndingAt[i].beginPos = b'
    // gives us a better maxEndingAt[i].sum?
    // Then  there is b' != maxEndingAt[i - 1].beginPos such that:
    //   
    //   maxEndingAt[i].sum - (maxEndingAt[i - 1] + a[i]) > 0 =>
    //   { sum j = b' .. i { a[j] }} - (maxEndingAt[i - 1] + a[i]) > 0 =>
    //   { sum j = b' .. i - 1 { a[j] }} + a[i] - (maxEndingAt[i - 1] + a[i]) > 0 =>
    //   { sum j = b' .. i - 1 { a[j] }} - maxEndingAt[i - 1]  > 0 =>
    //   { sum j = b' .. i - 1 { a[j] }} > maxEndingAt[i - 1]
    //
    // contradicting the inductive hypothesis.
    // Therefore, if maxEndingAt[i - 1].sum + a[i] >= 0, then setting maxEndingAt[i].beginPos = maxEndingAt[i - 1].beginPos and setting
    // maxEndingAt[i].sum appropriately maintains the invariant.
    // If maxEndingAt[i - 1].sum + a[i] < 0, then we must show that the empty array is best for this i i.e. that we must set maxEndingAt[i].beginPos = i + 1,
    // and so set maxEndingAt[i].sum = 0.
    // Assume otherwise i.e. that there is some b' != i + 1 such that setting maxEndingAt[i].beginPos = b' gives maxEndingAt[i].sum > 0.
    // Then 
    //
    //   maxEndingAt[i].sum = { sum j = b' .. i { a[j] }} = { sum j = b' .. i - 1 { a[j] }} + a[i] <= maxEndingAt[i - 1] + a[i] (by inductive hypothesis) < 0 (by assumption).
    //
    // a contradiction.  Thus, in any case, the invariant is maintained.
    // 
    // There's still a problem, though, in that if all values are less than 0, then this will give the max sum of a continuous subarray as 0 (resulting from the empty subarray),
    // which is not allowed.  However, in this case we simply do what we do in the non-continuous case and pick the subarray consisting of just the largest element.
    

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
