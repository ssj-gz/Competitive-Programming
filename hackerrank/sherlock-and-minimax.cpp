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
    int indexBeforeP = -1;
    int indexAfterQ = -1;
    int largestGapInABetweenPAndQ = -1;
    int indexOfLargestGapInABetweenPAndQ = -1;

    int largestMinPos = -1;
    int largestMin = std::numeric_limits<int>::min();

    auto updateLargestMin = [&largestMin, &largestMinPos](int newMin, int newMinPos)
    {
        if (newMin > largestMin)
        {
            largestMin = newMin;
            largestMinPos = newMinPos;
        }
        else if (newMinPos == largestMin)
        {
            largestMinPos = min(largestMinPos, newMinPos);
        }
    };

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
#if 0
    if (indexOfLargestGapInABetweenPAndQ != -1)
    {
        updateLargestMin(largestGapInABetweenPAndQ / 2, (a[indexOfLargestGapInABetweenPAndQ] + a[indexOfLargestGapInABetweenPAndQ - 1]) / 2);
    }
    if (indexBeforeP == -1)
    {
        assert(a[0] >= P);
        updateLargestMin(a[0] - P, P);
    }
    else if (indexBeforeP != a.size() - 1)
    {
        int PAdjusted = max(P, (a[indexBeforeP + 1] - a[indexBeforeP]) / 2);
        assert(PAdjusted >= a[indexBeforeP] && PAdjusted <= a[indexBeforeP + 1]);
        int PAdjustedDiff = max(a[indexBeforeP + 1] - PAdjustedDiff, PAdjustedDiff - a[indexBeforeP]);
        updateLargestMin(PAdjustedDiff, PAdjusted);
    }
    if (indexAfterQ == -1)
    {
        assert(Q >= a.back());
        updateLargestMin(Q - a.back(), Q);
    }
    else if (indexAfterQ != 0)
    {
        int QAdjusted = min(Q, (a[indexAfterQ] - a[indexAfterQ - 1]) / 2);
        assert(QAdjusted >= a[indexAfterQ - 1] && QAdjusted <= a[indexAfterQ]);
        int QAdjustedDiff = max(a[indexAfterQ] - QAdjustedDiff, QAdjustedDiff - a[indexAfterQ - 1]);
        updateLargestMin(QAdjustedDiff, QAdjusted);
    }
#endif
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
#define ALL_PAIRS
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
        cout << " P: " << P << " Q: " << Q << endl;
        const int optimisedResult = optimised(a, P, Q);
        cout << "optimisedResult: " << optimisedResult << endl;
    
#define BRUTE_FORCE
#ifdef BRUTE_FORCE
    const int bruteForceResult = bruteForce(a, P, Q);
    cout << "bruteForceResult: " << bruteForceResult << endl;
    assert(bruteForceResult == optimisedResult);
#endif
    }
}
