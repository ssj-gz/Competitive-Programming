#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;


int main() {
    int n;
    cin >> n;
    vector<unsigned long> a(n);
    for (int i = 0; i < n; i++)
    {
        cin >> a[i];
    }
    // Tricky one :) First of all: pick the highest power of 2 occurring
    // in any of the a's (i.e. appearing in their binary representation).
    // If *all* of the a's have this power of 2, then that power of 2
    // cannot possibly be in the min score (every permutation eliminates it)
    // i.e. the min score is less than that power of 2, so we can remove that power of 
    // 2 from all a's.
    // Then look at the new highest power of 2 occuring in any of  the a's: if 
    // *that* occurs in all a's, etc.
    // Keep going until the highest power of 2 occurring in any of the a's does not
    // occur in all of the a's (call it P): then P *must* occur in the min score:
    // any permutation must place a value with P next to a value 
    // without P.  So min score >= P.
    // Let x be the number of a's with P (so n - x of them do not have that P).
    // Now, re-order a so that the first x elements are those with P,
    // and the next n - x are those without it.  Note that none of the first x among 
    // themselves can give rise to a score >= P (they all contain P, so xor'ing any 
    // pair of them together gives a value without P i.e. < P).  Likewise, none of the latter 
    // n - x can give rise to a score with P: none of them has  P, so xor'ing any pair 
    // of them together cannot give a value with P.
    // In fact, the highest contributor to the score is at the single place where a value
    // with P is placed next to the value without P i.e. it is, in our newly-ordered a, the value
    //   a[x - 1] ^ a[x]
    // (a[x - 1] has P; a[x] does not; a[x - 1] ^ a[x] >= P).
    // We can then re-order a slightly so that the values occurring in a[x - 1] and a[x] have 
    // the minimum value after xor'ing; this permutation of a will give the minimum score.
    // Note that a potentially very high amount of permutations will give the same minimum score
    // since further permuting the first x - 1 and latter n - x - 1 once we've chosen the minimal permutation
    // described above has no effect on the score.
    uint64_t powerOf2 = static_cast<uint64_t>(1) << 63;
    while (powerOf2 > 0)
    {
        int numWithPowerOf2 = 0; 
        for (auto value : a)
        {
            if ((value & powerOf2) != 0)
            {
                numWithPowerOf2++;
            }
        }
        if (numWithPowerOf2 == n)
        {
            // This power of 2 cannot occur in the min score, so
            // remove it from all a's for simplicity.
            for (auto& value : a)
            {
                value -= powerOf2;
            }
        }
        else if (numWithPowerOf2 != 0)
        {
            // We've found the highest power of 2 occurring in at 
            // least one of the a's, but not in all of them.
            // This is the "P" described above.
            break;
        }
        powerOf2 >>= 1;
    }
    vector<unsigned int> withPowerOf2;
    vector<unsigned int> withoutPowerOf2;
    for (auto value : a)
    {
        if ((value & powerOf2) != 0)
        {
            withPowerOf2.push_back(value);
        }
        else
        {
            withoutPowerOf2.push_back(value);
        }
    }
    unsigned int minScore = std::numeric_limits<unsigned int>::max();
    for (auto value1 : withPowerOf2)
    {
        for (auto value2 : withoutPowerOf2)
        {
            const unsigned int score = value1 ^ value2;    
            minScore = min(minScore, score);
        }
    }
    if (withPowerOf2.empty() || withoutPowerOf2.empty())
    {
        // Original array must have consisted of all equal values;
        // any permutation gives score of 0.
        minScore = 0;
    }
    cout << minScore << endl;
    return 0;
}

