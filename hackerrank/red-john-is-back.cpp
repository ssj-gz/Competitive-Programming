// Simon St James (ssjgz) - 2018-01-29
#include <iostream>
#include <vector>

using namespace std;

int findNumWaysToMakeN(int N, vector<int>& numWaysToMakeNLookup)
{
    if (N == 0)
        return 1;
    if (N == 1)
        return 1;
    if (numWaysToMakeNLookup[N] != -1)
        return numWaysToMakeNLookup[N];

    auto result = 0;
    if (N >= 1)
        result += findNumWaysToMakeN(N - 1, numWaysToMakeNLookup);
    if (N >= 4)
       result += findNumWaysToMakeN(N - 4, numWaysToMakeNLookup); 

    return result;
}

int main()
{
    // Easy one: the problem reduces to finding "M" (maxNumWaysToMakeN), which is the number
    // of ways of forming N by adding a sequence of 1's and 4's i.e. the number of sequences
    // of 1's and 4's that sum to N.
    // This has a simple recurrence relation: the number of ways of making N is the number of ways of making N - 1 (we 
    // just append "1") to each sequence that yields N - 1) plus the number of ways of making N - 4 (we just append "4"
    // to each sequence that yields N - 4), with the number of ways of forming 0 or 1 equal to 1 as the recursion
    // base case(s).
    // It's then a simple matter to count the primes up to numWaysToMakeN - I use the Sieve of Eratosthenes here.    
    int T;
    cin >> T;

    constexpr auto maxN = 40;

    vector<int> numWaysToMakeNLookup(maxN + 1, -1);

    auto maxNumWaysToMakeN = 0;
    for (auto i = 1; i <= maxN; i++)
    {
        // Populate lookup table and find maxNumWaysToMakeN.
        maxNumWaysToMakeN = max(maxNumWaysToMakeN, findNumWaysToMakeN(i, numWaysToMakeNLookup));
    }

    vector<bool> isPrime(maxNumWaysToMakeN + 1, true);
    isPrime[1] = false;
    for (auto factor = 2; factor < isPrime.size(); factor++)
    {
        for (auto composite = 2 * factor; composite < isPrime.size(); composite += factor)
        {
            isPrime[composite] = false;
        }
    }

    for (auto t = 0; t < T; t++)
    {
        int N;
        cin >> N;

        const auto numWaysToMakeN = findNumWaysToMakeN(N, numWaysToMakeNLookup);

        auto numPrimes = 0;
        for (auto i = 1; i <= numWaysToMakeN; i++)
        {
            if (isPrime[i])
                numPrimes++;
        }
        cout << numPrimes << endl;
    }
}
