// Simon St James (ssjgz) 2017-09-03
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
    return (largestMinIndex); 
}

int optimised(const vector<int>& aOriginal, int64_t P, int64_t Q)
{
    vector<int64_t> a(aOriginal.begin(), aOriginal.end());
    a.insert(a.begin(), numeric_limits<int>::min());
    a.push_back(numeric_limits<int>::max());

    int largestMinPos = -1;
    int largestMin = std::numeric_limits<int>::min();

    for (int i = 1; i < a.size(); i++)
    {
        int64_t centrePos = a[i - 1] + (a[i] - a[i - 1]) / 2;
        //cout << " i: " << i << " a[i - 1] " << a[i - 1] << endl;
        //cout << " centrePos unadjusted: " << centrePos << endl;
        centrePos = max(centrePos, P);
        centrePos = min(centrePos, Q);
        //cout << " i: " << i << " a[i]: " << a[i] << " centrePos: " << centrePos << endl;
        if (centrePos < a[i - 1] || centrePos > a[i])
        {
            //cout << " out of range" << endl;
            continue;
        }
        int64_t gap = numeric_limits<int>::max();
        assert(centrePos >= a[i - 1]);
        assert(centrePos <= a[i]);
        if (a[i - 1] != numeric_limits<int>::min())
        {
            gap  = min(gap, centrePos - a[i - 1]);
        }
        if (a[i] != numeric_limits<int>::max())
        {
            gap = min(gap, a[i] - centrePos);
        }
        //cout << " gap: " << gap << endl;
        if (gap > largestMin)
        {
            //cout << " found new best: " << gap << " at " << centrePos << endl;
            largestMinPos = centrePos;
            largestMin = gap;
        }

    }
    return largestMinPos;
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
    sort(a.begin(), a.end());
//#define ALL_PAIRS
#ifdef ALL_PAIRS
    for (int P = 1; P < a.back() + 100; P++)
        for (int Q = P; Q < a.back() + 100; Q++)
#else
    int P, Q;
    cin >> P >> Q;
    // Make P, Q zero-relative.
    //P--;
    //Q--; 
#endif

    {
        //cout << " P: " << P << " Q: " << Q << endl;
        const int optimisedResult = optimised(a, P, Q);
        //cout << "optimisedResult: " << optimisedResult << endl;
        cout << optimisedResult << endl;
    
//#define BRUTE_FORCE
#ifdef BRUTE_FORCE
    const int bruteForceResult = bruteForce(a, P, Q);
    cout << "bruteForceResult: " << bruteForceResult << endl;
    assert(bruteForceResult == optimisedResult);
#endif
    }
}
