#include <iostream>
#include <vector>
#include <algorithm>

#include <sys/time.h>
#include <cassert>

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

int64_t optimised(const vector<int>& aOrig, const vector<int>& bOrig, const vector<int>& cOrig)
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
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));


        const int lenA = rand() % 100 + 1;
        const int lenB = rand() % 100 + 1;
        const int lenC = rand() % 100 + 1;

        const int maxA = rand() % 1000 + 1;
        const int maxB = rand() % 1000 + 1;
        const int maxC = rand() % 1000 + 1;

        cout << lenA << " " << lenB << " " << lenC << endl;

        for (int i = 0; i < lenA; i++)
        {
            cout << ((rand() % maxA) + 1) << " ";
        }
        cout << endl;
        for (int i = 0; i < lenB; i++)
        {
            cout << ((rand() % maxB) + 1) << " ";
        }
        cout << endl;
        for (int i = 0; i < lenC; i++)
        {
            cout << ((rand() % maxC) + 1) << " ";
        }
        cout << endl;

        return 0;
    }
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

    const auto solutionOptimised = optimised(a, b, c);
    cout << "solutionOptimised: " << solutionOptimised << endl;
    assert(solutionOptimised == solutionBruteForce);
}
