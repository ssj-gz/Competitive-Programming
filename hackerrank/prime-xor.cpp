#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

bool isPrime(int n)
{
    for (int i = 2; i <= sqrt(n) + 1; i++)
    {
        if (i < n && (n % i) == 0)
            return false;
    }
    return true;
}

int main()
{
    const int64_t modulus = 1'000'000'007LL;
    const int maxBitwiseXor = 8191;
    const int minA = 3500;
    const int maxA = 4500;
    int q;
    cin >> q;
    for (int t = 0; t < q; t++)
    {
        int n;
        cin >> n;
        vector<int> numAWithValue(maxA + 1);
        for (int i = 0; i < n; i++)
        {
            int a;
            cin >> a;
            numAWithValue[a]++;
        }
        // Ouch - misinterpreted (and then misjudged) this one quite badly!
        // OK - so to give some clarifying examples of the problem:
        // if a = { 3511, 3511, 3511, 3511 } = { a1, a2, a3, a4},
        // the full set of *unique* sets we can generate is
        // {3511}, {3511, 3511}, {3511, 3511, 3511}, {3511, 3511, 3511, 3511}.
        // (I'd interpreted it, wrongly, as we could have both of {a1, a2} and {a2, a3},
        // but {a1, a2} and {a2, a3} are the *same* set XD).
        // So: do a counting sort of a, and store the number occurrences of a giving value v 
        // in a as numAWithValue[v].  Let m be the number of distinct values in a, and label
        // them b1, b2, ... bm.
        // Then we can describe a set as a vector
        //  (c1, c2, ... , cm), 0 <= ci <= numAWithValue[bi].
        // where ci is the number of bi in the set.  Two sets are identical if and only if the
        // vectors representing them is the same.
        // The xor of such a set is easily computed.
        // Imagine we can count the number of sets xoring to a given value v using only b1, b2 ... bk
        // for some k.
        // We can count the number of sets xoring to v using b1, b2 ... bk+1 very simply:
        // for each value x generated using only b1, b2, ... , bk, compute:
        //  x ^ 0
        //  x ^ (bk+1)
        //  x ^ (bk+1 ^ bk+1)
        //  x ^ (bk+1 ^ bk+1 ^ bk+1)
        //  ..
        //  x ^ (bk+1 ^ bk+1 ^ ... ^ bk+1) i.e. bracketed term is numAWithValue[bk+1] bk+1's all xor'd together
        // and each time we create v, add (num ways of generating v using b1,b2,...bk) to the number of ways
        // we can generate v using b1,b2,...,bk+1.
        // We also need to count the contributions where we use only elements of bk+1; thus, calculate:
        //  bk+1
        //  bk+1
        //  bk+1 ^ bk+1
        //  bk+1 ^ bk+1 ^ bk+1
        //  ..
        //  bk+1 ^ bk+1 ^ ... ^ bk+1
        // and each time we create v, add 1 to the number of ways we can generate v using b1,b2,...,bk+1.
        // So a fairly simple recurrence relation.
        // And that's about it: by the time m gets to n, we know, for each possible value v, the number of 
        // ways of creating v from a.  It's now a simple matter of counting the number of ways of generating
        // v's where v is prime.
        vector<int64_t> numWaysOfFormingX(maxBitwiseXor + 1);
        numWaysOfFormingX[0] = 1;
        for (int value = minA; value <= maxA; value++)
        {
            if (numAWithValue[value] == 0)
                continue; // No contribution.

            const vector<int64_t> numWaysOfFormingXWithoutValue = numWaysOfFormingX;
            // At this point, the code: 
            // 
            //  for (int numOfValue = 1; numOfValue <= numAWithValue[value]; numOfValue++)
            //  {
            //      int valuesXord = 0;
            //      for (int i = 0; i < numOfValue; i++)
            //      {
            //          valuesXord ^= value;
            //      }
            //      numWaysOfFormingX[valuesXord]++;
            //      for (auto prevX = 0; prevX <= maxBitwiseXor; prevX++)
            //      {
            //          auto x = prevX ^ valuesXord;
            //          numWaysOfFormingX[x] += (numWaysOfFormingXWithoutValue[prevX]);
            //          numWaysOfFormingX[x] %= modulus;
            //      }
            //  }
            //  Would work, but is too inefficient.  We can take some shortcuts by exploiting the regularity of
            // the values of valuesXord, though - they are:
            //  value
            //  value^value
            //  value^value^value
            //  ..
            //  value^value^...value numAWithValue[value] times.
            //
            // Every even occurence of this will be 0; every odd occurrence of this will be value i.e. 
            // valuesXord alternates between 0 and value.
            // There are (numAWithValue[value] + 1) / 2 odd occurences.
            // Thus, we can optimise the above block of code to the following.
            const int numXorsOfValueEquallingValue = (numAWithValue[value] + 1) / 2;
            const int numXorsOfValueEqualling0 = numAWithValue[value] - numXorsOfValueEquallingValue;
            //numWaysOfFormingX[value] += numXorsOfValueEquallingValue;
            //numWaysOfFormingX[0] += numXorsOfValueEqualling0;

            for (auto prevX = 0; prevX <= maxBitwiseXor; prevX++)
            {
                {
                    const auto x = prevX ^ value;
                    const int64_t numNewWaysOfMakingX = (numWaysOfFormingXWithoutValue[prevX] * numXorsOfValueEquallingValue) % modulus;
                    numWaysOfFormingX[x] += numNewWaysOfMakingX;
                    numWaysOfFormingX[x] %= modulus;
                }
                {
                    const auto x = prevX ^ 0;
                    const int64_t numNewWaysOfMakingX = (numWaysOfFormingXWithoutValue[prevX] * numXorsOfValueEqualling0) % modulus;
                    numWaysOfFormingX[x] += numNewWaysOfMakingX;
                    numWaysOfFormingX[x] %= modulus;
                }
            }
        }
        // We know the number of ways of generating a given value using subsets of a; calculate the total for those values which
        // are prime.
        int64_t totalValidSets = 0;
        for (int x = 2; x <= maxBitwiseXor; x++)
        {
            if (isPrime(x))
            {
                totalValidSets += numWaysOfFormingX[x];
                totalValidSets %= modulus;
            }
        }
        cout << totalValidSets << endl;
    }
}



