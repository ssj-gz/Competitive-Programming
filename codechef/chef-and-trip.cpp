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

struct Result
{
    bool hasSolution = false;
    vector<int> solution;
};

Result NoSolution = { false, vector<int>() };

Result findSolution(int N, int K, const vector<int>& aOriginal)
{
    vector<int> a(aOriginal);
    // Add "sentinel" values at the beginning and end to simplify
    // code slightly.
    const auto sentinelValue = -2;
    a.insert(a.begin(), sentinelValue);
    a.insert(a.end(), sentinelValue);

    auto posOfLastDecided = 0;
    for (auto i = 1; i < a.size(); i++)
    {
        if (a[i] != -1 && i - posOfLastDecided > 1)
        {
            // A decided value, preceded by undecideds; figure out how/ if we can fill in the block of
            // undecideds since the last decided.
            auto nextDistinct = [&K](int value)
            {
                auto newValue = value + 1;
                if (newValue == K + 1)
                    newValue = 1;
                assert(1 <= newValue && newValue <= K);
                assert(value != newValue);

                return newValue;
            };

            if (K == 2)
            {
                const bool undecidedsAreSuffixOrPrefix = (a[i] == sentinelValue || a[posOfLastDecided] == sentinelValue);
                if (!undecidedsAreSuffixOrPrefix) // Don't use this logic for prefixes and suffixes - they can always be satisfied!
                {
                    const bool decidedAreDifferent = (a[i] != a[posOfLastDecided]);
                    const bool distanceIsOdd = (i - posOfLastDecided + 1) % 2 == 1;
                    if (decidedAreDifferent && distanceIsOdd)
                        return NoSolution;
                    if (!decidedAreDifferent && !distanceIsOdd)
                        return NoSolution;
                }
            }
            // We know we can succeed - let's just figure out what to use to fill in the gaps!
            auto newDecided = 1; // Arbitrary  - we'll be adjusting all new decided later if they don't work out!
            for (auto j = posOfLastDecided + 1; j < i; j++)
            {
                assert(a[j] == -1);
                a[j] = newDecided;
                newDecided = nextDistinct(newDecided);
            }
            // Adjust if necessary.
            auto numAdjustments = 0;
            while (a[posOfLastDecided] == a[posOfLastDecided + 1] || a[i - 1] == a[i])
            {
                // This block of code inside the "while" will execute at most twice.
                for (auto j = posOfLastDecided + 1; j < i; j++)
                {
                    a[j] = nextDistinct(a[j]);
                }
                numAdjustments++;
            }
            assert(numAdjustments <= 2);
            assert(a[posOfLastDecided + 1] != a[posOfLastDecided]);
            assert(a[i - 1] != a[i]);
        }

        if (a[i] != -1)
            posOfLastDecided = i;
    }

    // Remove sentinels.
    a.pop_back();
    a.erase(a.begin());
    
    return {true, a};
}


int main(int argc, char* argv[])
{
    // Pretty easy - hopefully the code speaks for itself :)
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();
    for (auto t = 0; t < T; t++)
    {
        const auto N = read<int>();
        const auto K = read<int>();

        vector<int> a(N);

        for (auto& aElement : a)
        {
            aElement = read<int>();
        }

        const auto result = findSolution(N, K, a);
        cout << (result.hasSolution ? "YES" : "NO") << endl;
        if (result.hasSolution)
        {
            auto solution = result.solution;
            for (const auto a : solution)
            {
                cout << a << " ";
            }
            cout << endl;
            assert(unique(solution.begin(), solution.end()) == solution.end());

        }
        
    }

    assert(cin);
}
