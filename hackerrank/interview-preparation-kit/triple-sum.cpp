#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int64_t bruteForce(const vector<int>& aOrig, const vector<int>& bOrig, const vector<int>& cOrig)
{
    auto a = aOrig;
    auto b = bOrig;
    auto c = cOrig;

    sort(a.begin(), a.end());
    a.erase(std::unique(a.begin(), a.end()), a.end());
    sort(b.begin(), b.end());
    b.erase(std::unique(b.begin(), b.end()), b.end());
    sort(c.begin(), c.end());
    c.erase(std::unique(c.begin(), c.end()), c.end());

    int64_t numTriples = 0;
    for (auto p : a)
    {
        for (auto q : b)
        {
            for (auto r : c)
            {
                if (p <= q && q >= r)
                {
                    numTriples++;
                }
            }
        }
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

    vector<int> a = readVector(lenA);
    vector<int> b = readVector(lenB);
    vector<int> c = readVector(lenC);

    const auto solutionBruteForce = bruteForce(a, b, c);
    cout << "solutionBruteForce: " << solutionBruteForce << endl;

}
