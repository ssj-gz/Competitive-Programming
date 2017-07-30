#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <array>
#include <cassert>

using namespace std;

int main()
{
    int n;
    cin >> n;
    vector<unsigned int> a(n);
    for (int i = 0; i < n; i++)
    {
        cin >> a[i];
    }
    const int twoTo16 = 65536;
    // Hard one! 
    // =
    // UPDATE: having successfully completed (100%) this one myself, I thought I'd look 
    // at the other solutions.  "Fast Fourier Transform", seriously?? And of the 
    // people who used Python, only 3(!)  got full marks: two of whom plagiarised
    // each other, and the 3rd of whom seems to have just translated the Editorial solution
    // into Python.  Not very impressed with this one, to be honest :/
    // =
    // UPDATE2: just tried a solution that was listed as getting 100% (it's very similar to my own
    // original one, before I added the "nonZeroPrefixFrequencies" optimisation), and, as mine
    // did, it times out on the first test case.  Something screwy going on, here.
    // =
    // UPDATE3: test cases are very weak.  The following generates an array which causes a 
    // timeout errors as it makes the nonZeroPrefixFrequencies ineffectual (or worse, really):
    //  n = 100000;
    //  a.resize(n);
    //  {
    //      unsigned int prefixXorValue = 0;
    //      for (int i = 0; i < n; i++)
    //      {
    //          a[i] = ((prefixXorValue ^ i) % (1 << 16));
    //          prefixXorValue ^= a[i];
    //          assert(prefixXorValue == (i % (1 << 16)));
    //      }
    // }
    //
    // The following generates an array where "0" is the most frequent value of xor'd subarrays,
    // which my algorithm can't handle:
    //  n = 8;
    //  a.resize(n);
    //  {
    //      for (int i = 0; i < n; i++)
    //      {
    //          if (i < n / 2)
    //              a[i] = i % 2;
    //          else
    //              a[i] = 2 * (i % 2);
    //      }
    //  }
    // =


    // As with most "perform an operation on all subarrays" kind of 
    // problems, computing a prefix lookup is useful, so 
    // compute the xor of each possible prefix.  The range of possible
    // values for the xor of a  prefix is limited, so store the frequency
    // of this: prefixXorFrequency[x] is the number of prefixes whose xor
    // is x.
    array<int, twoTo16 + 1> prefixXorFrequency = {};
    unsigned int prefixXorValue = 0;
    prefixXorFrequency[0] = 1; // Empty prefix.
    for (int i = 0; i < n; i++)
    {
        prefixXorValue = (prefixXorValue ^ a[i]);
        assert(prefixXorValue < prefixXorFrequency.size());
        prefixXorFrequency[prefixXorValue]++;
    }
    // If i is an index 0 <= i < n, then define P(i) to be the xor
    // of that prefix.
    // Now: note that if i and j are two indices, i < j,
    // then P(i) ^ P(j) is equal to the xor'd range of a for [i+1 .. j];  to see this, note
    // that xor is commutative and associative, so 
    //  P((i) ^ P(j) ==
    //  (a[0] ^ a[1] ^ ... ^ a[i]) ^ (a[0] ^ a[1] ^ ... ^ a[j]) ==
    //  (a[0] ^ a[0] ^ a[1] ^ a[1] ^ ... a[i] ^ a[i] ^ a[i+1] ^ a[i+2] ^ ... ^a[j]).
    // Since x ^ x == 0, this is just
    //  a[i+1] ^ a[i+2] ^ ... ^a[j].
    // i.e. the xor'd range of a for [i+1 .. j].
    // Pick any pair of values x and y, with x < y.  Let k = prefixXorFrequency[x], 
    // m = prefixXorFrequency[y]; then let i_1, i_2 ... i_k be the set of indices for
    // which P(i_l) = x and j_1, j_2 ... j_m be the set of indices for which P(j_l) = y.
    // Now: take any pair i_p, j_q 1 <= p <= k, 1 <= q <= m.
    // Since x does not equal y, i_p cannot equal j_q.  So either
    //  [i_p+1 ... j_q] or
    //  [j_q+1... i_p]
    // is a non-empty range.  The xor'd value of this is (from logic above),  respectively:
    //  P(i_p) ^ P(j_q) == x ^ y or
    //  P(j_q) ^ P(i_p) == y ^ x
    // i.e. the same value either way: x ^ y.  The number of such pairs i_p and i_q is 
    // prefixXorFrequency[x] * prefixXorFrequency[y] i.e. the contribution of the pair
    // x,y to the frequency of x ^ y in all xor'd subsequences is prefixXorFrequency[x] * prefixXorFrequency[y].
    // If we do this for all pairs x,y, x < y, then we can tally the frequency of each value
    // for all subsequences of a, which is what we want.
    // TODO - this does not correctly count the 0's, since insisting that x < y
    // will ignore some of these.

    // Instead of trying all 65536 * 65536 values, filter out those ones
    // that will give 0 so they are never even attempted: with the test
    // case provided, this always makes a huge difference, though I
    // imagine it's possible to contrive unpleasant arrays in which
    // *every* value can occur as the xor'd value of a subarray :/
    vector<int> nonZeroPrefixFrequencies;
    for (unsigned int i = 0; i < prefixXorFrequency.size(); i++)
    {
        if (prefixXorFrequency[i] > 0)
            nonZeroPrefixFrequencies.push_back(i);
    }
    array<int, twoTo16 * 2> frequency = {};
    for (unsigned int ix = 0; ix < nonZeroPrefixFrequencies.size(); ix++)
    {
        const int x = nonZeroPrefixFrequencies[ix];
        const int prefixXorFrequencyX = prefixXorFrequency[x];
        for (unsigned int iy = ix + 1; iy < nonZeroPrefixFrequencies.size(); iy++)
        {
            const int y = nonZeroPrefixFrequencies[iy];
            frequency[x ^ y] += prefixXorFrequencyX * prefixXorFrequency[y];
        }
    }
    int mostFrequentValue = -1;
    int highestFrequency = 0;
    for (int value = 0; value <= twoTo16; value++)
    {
        if (frequency[value] > highestFrequency)
        {
            mostFrequentValue = value;
            highestFrequency = frequency[value];
        }
    }
    cout << mostFrequentValue << " " << highestFrequency << endl;
}



