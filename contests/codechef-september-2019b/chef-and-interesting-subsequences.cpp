// Simon St James (ssjgz) - 2019-09-09
// 
// Solution to: https://www.codechef.com/SEPT19B/problems/CHEFINSQ
//
#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>

#include <cassert>

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

constexpr auto maxN = 50;

vector<vector<int64_t>> nCrLookup(maxN + 1, vector<int64_t>(maxN + 1, -1));

void buildnCrLookup()
{
    for (auto n = 0; n <= maxN; n++)
    {
        nCrLookup[0][n] = 0;
        nCrLookup[n][n] = 1;
        nCrLookup[n][0] = 1;
    }
    for (auto n = 1; n <= maxN; n++)
    {
        for (auto k = 1; k <= maxN; k++)
        {
            assert(nCrLookup[n - 1][k - 1] != -1);
            assert(nCrLookup[n - 1][k] != -1);

            // The usual recurrence relation for nCr: see e.g. https://en.wikipedia.org/wiki/Binomial_coefficient
            nCrLookup[n][k] = nCrLookup[n - 1][k - 1] + nCrLookup[n - 1][k];
        }
    }
}

int64_t findNumInterestingSubsequences(int N, int K, const vector<int>& aOriginal)
{
    if (K > N)
        return 0;

    auto a = aOriginal;

    sort(a.begin(), a.end());
    const auto kthElement = a[K - 1];
    const auto totalNumOfKthElement = count(a.begin(), a.end(), kthElement);
    auto numOfKthElementInSubset = 0;
    int i = K - 1;
    while (i >= 0 && a[i] == kthElement)
    {
        i--;
        numOfKthElementInSubset++;
    }
    
    // The only sub-sequences we are allowed to pick are those identical to
    // the original sub-sequence (the first K elements of the sorted version
    // of a), with some "wiggle-room" for how we choose the numOfKthElementInSubset
    // values of a equal to kthElement.
    return nCrLookup[totalNumOfKthElement][numOfKthElementInSubset];
}

int main(int argc, char* argv[])
{
    // Let P_1, P_2, ... P _l be the list of distinct primes in A, with P_i < P_(i+1)
    // for all valid i.
    //
    // Then we can easily find the sum of the smallest subset of size K: simply sort A,
    // and pick the smallest K elements and sum them.  By definition of P_i's, a sorted
    // A would look like:
    //
    //    P1, P1, ... P1, P2, P2, ... P2, ... P_(l-1), P_(l-1), ... P_(l-1), P_l, P_l, P_l, .... P_l
    //
    // a[K] is the kthElement (one relative).  Since a[K] must be one of the P_i's,
    // Let m be such that a[K] = P_m.  Then our initial choice of the minimal subset looks like:
    //
    //                                                                       numOfKthElementInSubset
    //          
    //                                                                       ┌─────┴─────┐
    //    P1, P1, ... P1, P2, P2, ... P2, ... P_(m-1), P_(m-1), ... P_(m-1), P_m, P_m, P_m, .... P_m
    //    └─────────────────────────────────────────┬────────────────────────────────────┘ 
    //
    //                                 our initial choice of subset
    //
    // The value numOfKthElementInSubset, as its name suggests, is the number of times P_m = a[K] occurs 
    // in our initial subset.
    //
    // The question asks us to count the number of subsequences with this same sum.  How can we 
    // choose a subset, different to our initial one, that has the same sum as the initial one?
    //
    // Well, in our initial choice, we've chosen all elements equal to P_1, all elements equal to P_2,
    // ... all elements equal to P_(m-1), and numOfKthElementInSubset elements equal to P_m.
    //
    // Can we swap one of these choices (P_c, say, 1 <= c <= m - 1) for a different value (P_d, say, c != d)?
    // d can't be any of 1, 2, ... m - 1 as all such P_1, P_2, ... P_(m-1) are already chosen and we're supposed
    // to be choosing something different! So we'd have to swap P_c for a P_d with d >= m.  But such a P_d would > P_c, 
    // so such a swap woud give us a greater subset sum, so this won't do.  So we must always stick with our choice of all of the
    // P_i's for i = 1, 2, ... m - 1.
    //
    // Can we swap one of our P_m for a different P_d, d != m? Again, d can't be any of 1, 2, ... m - 1 as we've 
    // already chosen all such P_d.  d > m won't do either, as such a P_d would be > P_m, leading to a subset
    // with a greater sum.  What about swapping one of our P_m's for another (as yet unchosen) of the P_m's, if 
    // there is one (i.e. if totalNumOfKthElement > numOfKthElementInSubset)?  This would work: the subset
    // sum would be the same as that of our initial choice i.e. minimal, and in fact we see that:
    //
    //  a subset has minimal sum if and only if it consists of all elements equal to P_1, P_2, ... P_(m-1)
    //  (only one way to do this) plus numOfKthElementInSubset elements equal to P_m
    //
    // There are nCr(totalNumOfKthElement, numOfKthElementInSubset) number of ways of choosing numOfKthElementInSubset
    // P_m's from the totalNumOfKthElement total number of them, and we can easily compute this using a lookup table
    // for nCr, built up using the well-known recurrence for nCr.
    //
    // And that's it!
    ios::sync_with_stdio(false);
    
    buildnCrLookup();

    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const auto N = read<int>();
        const auto K = read<int>();

        vector<int> a(N);
        for (auto& aElement : a)
        {
            aElement = read<int>();
        }

        cout << findNumInterestingSubsequences(N, K, a) << endl;
    }

    assert(cin);
}
