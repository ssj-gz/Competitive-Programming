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
#if 0
    for (int i = 0; i <= n; i++)
    {
        cout << "i: " << i << " grundy: " << findGrundy(i) << endl;
    }
#endif
    const int grundyCycle[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4 };
    const int grundyCycleSize = sizeof(grundyCycle) / sizeof(grundyCycle[0]);

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

            xorSum ^= grundyCycle[x % grundyCycleSize];
        }

        if (xorSum != 0)
            cout << "Manasa";
        else
            cout << "Sandy";
        cout << endl;
    }
}
