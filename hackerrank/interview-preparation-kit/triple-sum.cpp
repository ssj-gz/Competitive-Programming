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

    int64_t indexOfGreaterThanQInA = 0;
    int64_t indexOfGreaterThanQInC = 0;

    int64_t numTriples = 0;

    for (const auto q : b)
    {
        while (indexOfGreaterThanQInA < a.size() && a[indexOfGreaterThanQInA] <= q)
        {
            indexOfGreaterThanQInA++;
        }
        while (indexOfGreaterThanQInC < c.size() && c[indexOfGreaterThanQInC] <= q)
        {
            indexOfGreaterThanQInC++;
        }

        numTriples += indexOfGreaterThanQInA * indexOfGreaterThanQInC;
    }

    return numTriples;
}

int main(int argc, char* argv[])
{
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
