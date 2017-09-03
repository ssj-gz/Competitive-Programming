#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <cassert>

using namespace std;

int bruteForce(const vector<int>& a, int P, int Q)
{
    int largestMin = -1;
    int largestMinIndex = -1;
    for (int i = P; i <= Q; i++)
    {
        int minDiff = numeric_limits<int>::max();
        for (const auto value : a)
        {
            minDiff = min(minDiff, abs(value - i));
        }
        if (minDiff > largestMin)
        {
            largestMin = minDiff;
            largestMinIndex = i;
        }
    }
    assert(largestMinIndex != -1);
    return (largestMinIndex + 1); 
}

int main()
{
    int N;
    cin >> N;
    vector<int> a(N);
    for (int i = 0; i < N; i++)
    {
        cin >> a[i];
    }
    int P, Q;
    cin >> P >> Q;
    // Make P, Q zero-relative.
    P--;
    Q--; 

    sort(a.begin(), a.end());
    
#define BRUTE_FORCE
#ifdef BRUTE_FORCE
    const int bruteForceResult = bruteForce(a, P, Q);
    cout << "bruteForceResult: " << bruteForceResult << endl;
#endif
}
