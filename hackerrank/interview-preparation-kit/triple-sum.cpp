// Simon St James (ssjgz) - 2019-07-04
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int64_t findNumMatchingTriples(const vector<int>& aOrig, const vector<int>& bOrig, const vector<int>& cOrig)
{
    auto a = aOrig;
    auto b = bOrig;
    auto c = cOrig;

    // Sort all three arrays, and remove duplicates.
    sort(a.begin(), a.end());
    a.erase(std::unique(a.begin(), a.end()), a.end());
    sort(b.begin(), b.end());
    b.erase(std::unique(b.begin(), b.end()), b.end());
    sort(c.begin(), c.end());
    c.erase(std::unique(c.begin(), c.end()), c.end());

    int64_t numInANotGreaterThanQ = 0;
    int64_t numInCNotGreaterThanQ = 0;

    int64_t numTriples = 0;

    for (const auto q : b)
    {
        while (numInANotGreaterThanQ < a.size() && a[numInANotGreaterThanQ] <= q)
        {
            numInANotGreaterThanQ++;
        }
        while (numInCNotGreaterThanQ < c.size() && c[numInCNotGreaterThanQ] <= q)
        {
            numInCNotGreaterThanQ++;
        }

        numTriples += numInANotGreaterThanQ * numInCNotGreaterThanQ;
    }

    return numTriples;
}

int main(int argc, char* argv[])
{
    // "Medium"?? Absolutely trivial; for each q, find the number of elements
    // in A and and the number of elements in C which are less than or equal to this q,
    // multiply these two numbers, and add to running tally - if none of A, B
    // or C contain duplicates, then this will give the correct result.
    //
    // Sorting A, B and C makes this process more efficient (and allows us to efficiently
    // remove duplicates): for each increasing q, the set of elements in A less than
    // or equal to q cannot decrease (and similarly for C), so we can just efficiently
    // maintain a count of these as q increases.  Hopefully the code is self-explanatory :)
    int lenA, lenB, lenC;
    cin >> lenA >> lenB >> lenC;

    auto readVector = [](const int vectorSize)
    {
        vector<int> v(vectorSize);
        for (int  i = 0; i < vectorSize; i++)
        {
            cin >> v[i];
        }
        return v;
    };

    const vector<int> a = readVector(lenA);
    const vector<int> b = readVector(lenB);
    const vector<int> c = readVector(lenC);

    const auto numMatchingTriples = findNumMatchingTriples(a, b, c);
    cout << numMatchingTriples << endl;
}
