// Simon St James (ssjgz) - 2019-09-01
//
// Solution to: https://www.codechef.com/problems/TRIP2/
//
#include <iostream>
#include <vector>
#include <algorithm>

#include <cassert>

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

std::pair<bool, vector<int>> solveOptimised(int N, int K, const vector<int>& aOriginal)
{
    std::pair<bool, vector<int>> result = { false, vector<int>() };

    vector<int> a(aOriginal);
    a.insert(a.begin(), 0);
    a.insert(a.end(), 0);

    int posOfLastDecided = 0;
    for (int i = 1; i < a.size(); i++)
    {
        if (a[i] != -1 && i - posOfLastDecided > 1)
        {

            auto nextDistinct = [&K](int value)
            {
                int newValue = value + 1;
                if (newValue == K + 1)
                    newValue = 1;
                assert(1 <= newValue && newValue <= K);
                assert(value != newValue);

                return newValue;
            };


            if (K == 2)
            {
                if (a[i] != 0 && a[posOfLastDecided] != 0)
                {
                    const bool decidedAreDifferent = (a[i] != a[posOfLastDecided]);
                    const bool distanceIsOdd = (i - posOfLastDecided + 1) % 2 == 1;
                    if (decidedAreDifferent && distanceIsOdd)
                        return { false, vector<int>() };
                    if (!decidedAreDifferent && !distanceIsOdd)
                        return { false, vector<int>() };
                }
            }
            // We know we can succeed - let's just figure out what to use to fill in the gaps!
            int firstNewDecided = 1;
            for (int j = posOfLastDecided + 1; j < i; j++)
            {
                a[j] = firstNewDecided;
                firstNewDecided = nextDistinct(firstNewDecided);
            }
            int dbgNumAdjustments = 0;
            while (a[posOfLastDecided] == a[posOfLastDecided + 1] || a[i - 1] == a[i])
            {
                // This block will run at most twice.
                for (int j = posOfLastDecided + 1; j < i; j++)
                {
                    a[j] = nextDistinct(a[j]);
                }
                dbgNumAdjustments++;
            }
            assert(dbgNumAdjustments <= 2);
            assert(a[posOfLastDecided + 1] != a[posOfLastDecided]);
            assert(a[i - 1] != a[i]);
        }

        if (a[i] != -1)
            posOfLastDecided = i;
    }

    a.pop_back();
    a.erase(a.begin());
    
    return {true, a};
}


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    
    const int T = read<int>();
    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();
        const int K = read<int>();

        vector<int> a(N);

        for (int i = 0; i < N; i++)
        {
            a[i] = read<int>();
        }

        const auto solutionOptimised = solveOptimised(N, K, a);
        cout << (solutionOptimised.first ? "YES" : "NO") << endl;
        if (solutionOptimised.first)
        {
            auto a = solutionOptimised.second;
            assert(unique(a.begin(), a.end()) == a.end());
            for (const auto x : a)
            {
                cout << x << " ";
            }
            cout << endl;

        }
    }

    assert(cin);
}
