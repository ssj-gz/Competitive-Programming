#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;


int main() {
    int T;
    cin >> T;
    for (int t = 0; t < T; t++)
    {
        long N;
        cin >> N;
        vector<int> a(N);
        for (int i = 0; i < N; i++)
        {
            cin >> a[i];
        }
        // A nice easy one :) Note that xor is commutative and associative, so we can 
        // re-arrange and re-bracket the elements in the xor'ing any way we choose.
        // Let's re-arrange is so that all occurrences of a[i] for a given i are all xor'd together:
        // (a[i] ^ a[i] ^ .... ^ a[i]) * (rest of xor'ing).
        // Note that x ^ x ^ ... ^ x, where x occurs m times, say, is easy to compute:
        // it is x if m is odd; 0 otherwise.
        // So: if we can find the number of times a[i] occurs in the big xor'ing for each i, we're done.
        // This is easy, though: the number of times a[i] occurs is equal to the number of subarrays
        // containing a[i], which is equal to the total number of possible subarrays minus the ones
        // that don't contain a[i].  The ones that don't contain a[i] are precisely the subarrays of the subarray
        // to the left of a[i] plus the subarrays of the subarray to the right of a[i].
        // Ta-da!
        long result = 0;
        auto numSubArrays = [](long arrayLength) { return arrayLength * (arrayLength + 1) / 2;}; // (arrayLength choose 2) + arrayLength.
        for (int i = 0; i < N; i++)
        {
            const long numSubarraysContaining_i = numSubArrays(N) // All possible subarrays 
                - numSubArrays(i) // Subarrays strictly to the left of i.
                - numSubArrays(N - i - 1); // Subarrays strictly to the right of i.
            if ((numSubarraysContaining_i & 1) == 1)
            {
                result = result ^ a[i];
            }
        }
        cout << result << endl;
    }
    return 0;
}

