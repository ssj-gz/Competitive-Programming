// Simon St James (ssjgz) - 2022-03-31
// 
// Solution to: https://www.codechef.com/MARCH221D/problems/SUB_XOR
//
#include <iostream>
#include <vector>

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

const int64_t mod = 998'244'353;

int64_t solveOptimised(const string& S)
{
    int64_t result = 0;
    const int N = static_cast<int>(S.size());
    int64_t powerOf2 = 1;
    vector<int> num1sAtEvenIndicesUpTo(N);
    int num1sAtEvenIndices = 0;
    for (int i = 0; i < N; i++)
    {
        if (S[i] == '1' && ((i % 2) == 0))
            num1sAtEvenIndices++;
        num1sAtEvenIndicesUpTo[i] = num1sAtEvenIndices;
    }
    // NB: bitIndex counts from the *least significant bit*, so
    // bitIndex == 0 is the "rightmost" bit in the binary representation.
    for (int bitIndex = 0; bitIndex < N; bitIndex++)
    {
        // For a substring T to have its bitIndex'th bit set to 1, T's end 
        // must be no closer than bitIndex to the end of S (at N - 1).
        // Say we found such a substring T, and that its bitIndex'th bit is at
        // index p in S (so S[p] == 1 and 0 <= p <= N - 1 - bitIndex): then that is 1 substring with its
        // bitIndex'th bit set.  If there is a digit in S before the beginning 
        // of T in S, then that is another.  If there is yet another, then that is
        // another, etc.  The number of substrings whose bitIndex'th index is at index
        // p in S is thus exactly (p+1).  If this number of substrings ((p+1)) is even, 
        // i.e. if p is odd, these substrings contribute nothing to the bitIndex'th bit of the final result;
        // else, if p is even, they toggle the bitIndex'th bit of the final result.
        // If we counted all such substrings for each p, (0 <= p <= N - 1 - bitIndex), 
        // we could then determine if the bitIndex'th bit in S was 0 or 1, but this would
        // lead to an O(N^2) algorithm.
        // However, by using the num1sAtEvenIndicesUpTo lookup, we can reduce this to O(N).
        const bool isBitIndexthBitOnInResult = (num1sAtEvenIndicesUpTo[N - 1 - bitIndex] % 2 == 1);
        if (isBitIndexthBitOnInResult) 
        {
            result = (result + powerOf2) % mod;
        }

        powerOf2 = (2 * powerOf2) % mod;
    }
    
    return result;
}

int main()
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {

        const auto strLen [[maybe_unused]] = read<int>() ;
        const auto S = read<string>();
        const auto solutionOptimised = solveOptimised(S);
        cout << solutionOptimised << endl;
    }

    assert(cin);
}
