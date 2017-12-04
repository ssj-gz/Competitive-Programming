// Simon St James (ssjgz) 2017-12-04
#include <iostream>
#include <vector>

using namespace std;

#if 0
const int n = 1000;
vector<int> grundyLookup(n + 1, -1);

int findGrundy(int i)
{
    assert(i < grundyLookup.size());
    if (i < 0)
        return 0;
    if (grundyLookup[i] != -1)
        return grundyLookup[i];
    const int primes[] = {2, 3, 5, 7, 11, 13};

    vector<int> nextGrundies;
    for (const auto prime : primes)
    {
        if (i - prime < 0)
            continue;
        nextGrundies.push_back(findGrundy(i - prime));
    }

    sort(nextGrundies.begin(), nextGrundies.end());
    int mex = 0;
    if (!nextGrundies.empty())
    {
        for (int j = 0; j <= nextGrundies.back() + 1; j++)
        {
            if (find(nextGrundies.begin(), nextGrundies.end(), j) == nextGrundies.end())
            {
                mex = j;
                break;
            }
        }
    }

    grundyLookup[i] = mex;
    return mex;
}
#endif

int main()
{
    // Easy one if you just experiment, but pretty hard if you try to Logic your way into a solution :)
    // So we basically just consider one pile for the time being, and try to find the Grundy number
    // for a single pile of n elements; this is easily done via the #if 0'd code below and the "findGrundy"
    // function above.  We see that the grundy numbers for n = 0, 1, ... etc are just the cycle:
    //
    //   0, 0, 1, 1, 2, 2, 3, 3, 4 
    //
    // repeated over and over again (which I wouldn't have Logic'd), so the grundy number for a single pile of size n is easily computed.
    //
    // Once we know the Grundy number for a pile of size n, then the final answer, by Sprague-Grundy, is 
    // just the Grundy numbers of all the pile sizes xor'd together.
#if 0
    for (int i = 0; i <= n; i++)
    {
        cout << "i: " << i << " grundy: " << findGrundy(i) << endl;
    }
#endif
    const int grundyCycle[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4 };
    const int grundyCycleLength = sizeof(grundyCycle) / sizeof(grundyCycle[0]);

    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        int n;
        cin >> n;

        int64_t xorSum = 0;
        for (int i = 0; i < n; i++)
        {
            int64_t x;
            cin >> x;

            xorSum ^= grundyCycle[x % grundyCycleLength];
        }

        if (xorSum != 0)
            cout << "Manasa";
        else
            cout << "Sandy";
        cout << endl;
    }
}
