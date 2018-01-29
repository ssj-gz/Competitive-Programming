// Simon St James (ssjgz) - 2018-01-29
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main()
{
    // Ouch - fundamentally easy,  but loads of stupid errors on the way :/
    // Let S(m) be the set of sums <= k (we don't care about sums that exceed k, although
    // we might if the a's could be negative) that can be formed by adding numbers
    // drawn from the set {a[0], a[1], ... a[m - 1]}, where we can draw the
    // same number as many times as we want.  If we can form S(n), then it's a simple matter
    // to give the final result.
    //
    // Calculating S(m) is easy if we know the set S(m - 1): s is in S(m) if and only if a) s is of the form i * a[m - 1] for some i; or
    // b) s is of the form s = s' + i * a[m - 1] for some s' in S(m - 1).
    //
    // However, naively looping, for each m = 0, 1, ... n, over all O(k) such s' and all such i
    // would be O(n) * O(k) * O(k), (there can be O(k) i's for each s'), so this won't do.
    // Let's be crafty, though: set S(m) = S(m-1); add a[m - 1] to S(m); and then perform a single
    // sweep through all possible s from 0 ... k adding s to S(m) if and only if i) s - a[m - 1] >= o
    // and ii) s - a[m - 1] is in S(m)  will give us exactly what we need!
    //
    // To see this, imagine s' is the only number in S(m - 1).  Set S(m) = S(m - 1) (every number in S(m - 1) is also in S(m) - 
    // just don't add any a[m - 1]'s!)
    // s = a[m - 1] should certainly be in S[m], so let's add it; then let's sweep through the array as described and
    // see what other numbers can be formed.  s = a[m - 1] + a[m - 1] certainly satisfies s - a[m - 1] >= 0 and
    // s - a[m - 1] = a[m - 1] is in S(m) (we just added it!), so s = a[m - 1] + a[m - 1] gets added to S(m).  
    // Likewise, s = a[m - 1] + a[m - 1] + a[m - 1] satisfies s - a[m - 1] = a[m - 1] + a[m - 1] >= 0, and s - a[m - 1] = a[m - 1] + a[m - 1] is in S(m) - we just added it!
    // So s = a[m - 1] + a[m - 1] + a[m - 1] is added to S(m).  It's hopefully obvious that after sweeping through
    // all s, all s of the form a[m - 1] * i for some i have been added to S(m).
    //
    // Using identical reasoning, we see that the same sweep also adds all s of the form s' + a[m - 1] * i for some i and in general,
    // that this single sweep gives us precisely the s that satisfy a) or b)!
    //
    // Thus, we can trim the O(n) * O(k) * O(k) solution down to a much more manageable O(n) * O(k) - one sweep (O(K)) for each m.
    //
    // Once we've processed this for all m = 0, ... n - 1 and know the list of numbers <= k that can be formed from a[0], a[1], ... a[n-1],
    // the remainder is trivial.

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
            // we can form s using just a[0], a[1], ... , a[m - 1] i.e. canFormSumFromFirstM is
            // precisely S(m).
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
