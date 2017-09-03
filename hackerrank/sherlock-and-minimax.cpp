// Simon St James (ssjgz) 2017-09-03
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <cassert>

using namespace std;

int optimised(const vector<int>& aOriginal, int64_t P, int64_t Q)
{
    vector<int64_t> a(aOriginal.begin(), aOriginal.end());
    // Insert "sentinel" values of a at the beginning and end - helps to cut down
    // on all the annoying edge cases!
    // We use int64_t for a to avoid difficulties with dealing with arithmetic with
    // min and max integers.
    a.insert(a.begin(), numeric_limits<int>::min());
    a.push_back(numeric_limits<int>::max());

    int largestMinGapPos = -1;
    int largestMinGap = std::numeric_limits<int>::min();

    auto snapToPQ =[P, Q](int64_t pos)
    {
        pos = max(pos, P);
        pos = min(pos, Q);
        return pos;
    };

    for (int i = 1; i < a.size(); i++)
    {
        const int64_t centrePos = a[i - 1] + (a[i] - a[i - 1]) / 2;
        const int64_t centrePosInPQ = snapToPQ(centrePos);
        if (centrePosInPQ < a[i - 1] || centrePosInPQ > a[i])
        {
            // Snapping it means it is no longer between a[i - 1] and a[i] (definitely not a 
            // centre point!) - move on.
            continue;
        }

        int64_t minGap = numeric_limits<int>::max();
        if (a[i - 1] != numeric_limits<int>::min()) // Ignore sentinel at beginning.
        {
            minGap  = min(minGap, centrePosInPQ - a[i - 1]);
        }
        if (a[i] != numeric_limits<int>::max()) // Ignore sentinel at end.
        {
            minGap = min(minGap, a[i] - centrePosInPQ);
        }
        if (minGap > largestMinGap)
        {
            largestMinGapPos = centrePosInPQ;
            largestMinGap = minGap;
        }
    }
    return largestMinGapPos;
}

int main()
{
    // Pretty easy one, though with some annoying edge cases if you are not careful!
    // Firstly, sort a, and note that if a[i - 1] <= K <= a[i], then, in computing
    //   min over all v in a { |v - K| }
    // we can ignore all other v = a[j] besides j = i - 1 j = i; if j < i - 1,
    // then |a[j] - K| > |a[i - 1] - K| so cannot be min, and if j > i, then 
    // |a[j] - K| > |a[i] - K|, so cannot be min.
    // So it seems that we need only examine pairs of successive values a[i - 1] and a[i].
    // The point K that maximises the min distance from a[i - 1] and a[i] is at the centre point
    // of a[i - 1] and a[i] i.e. K = (a[i] + a[i - 1]) / 2; of course, this might lie outside the bounds
    // of P and Q, so we may have to snap K to lie in the range P <= K <= Q and take that as our best for
    // this a[i - 1] and a[i].
    // And that's about it, really: just find the best K over all a[i - 1] and a[i]. 
    // At the time of writing, this challenge is rated "Hard", for some reason!
    int N;
    cin >> N;
    vector<int> a(N);
    for (int i = 0; i < N; i++)
    {
        cin >> a[i];
    }
    sort(a.begin(), a.end());
    int P, Q;
    cin >> P >> Q;

    const int optimisedResult = optimised(a, P, Q);
    cout << optimisedResult << endl;
}
