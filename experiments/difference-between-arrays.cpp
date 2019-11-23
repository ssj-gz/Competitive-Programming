// Simon St James (ssjgz) - 2019-XX-XX
// 
// Solution to: TODO - problem link here!
//
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
//#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <limits>
#include <set>
#include <map>
#include <algorithm>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}
int solveBruteForce(const vector<int>& a)
{
    int minMaxDifference = std::numeric_limits<int>::max();

    set<vector<int>> toExplore = { a };
    set<vector<int>> alreadySeen = { a };

    while (!toExplore.empty())
    {

        set<vector<int>> nextToExplore;
        for (const auto& x : toExplore)
        {
            const int difference = *max_element(x.begin(), x.end()) - *min_element(x.begin(), x.end());
            if (difference < minMaxDifference)
            {
                cout << "New best: diff: " << difference << endl;
                for (const auto a : x)
                {
                    cout << a << " ";
                }
                cout << endl;

                minMaxDifference = difference;
            }

            for (int i = 0; i < x.size(); i++)
            {
                vector<int> nextX = x;
                if (nextX[i] % 2 == 0)
                {
                    nextX[i] = nextX[i] / 2;
                }
                else
                {
                    nextX[i] = nextX[i] * 2;
                }

                if (alreadySeen.find(nextX) == alreadySeen.end())
                {
                    alreadySeen.insert(nextX);
                    nextToExplore.insert(nextX);
                }
            }
        }
        toExplore = nextToExplore;
    }
    cout << "#alreadySeen: " << alreadySeen.size() << endl;

    return minMaxDifference;
}

int solveOptimised(const vector<int>& a)
{
    // mishraanoopam's algorithm from https://discuss.codechef.com/t/help-needed-in-past-american-express-hiring-challenge/45082/16
    int minMaxDifference = std::numeric_limits<int>::max();
    struct TransformedValueAndIndex
    {
        int value = -1;
        int originalIndex = -1;
    };

    vector<TransformedValueAndIndex> valuesAndIndices;
    for (int i = 0; i < a.size(); i++)
    {
        int value = a[i];
        // Build up the full set of values that can be created, using the given rules,
        // from a[i], and add them to valuesAndIndices.
        if (value % 2 == 1)
        {
            value = value * 2;
        }
        while (value % 2 == 0)
        {
            valuesAndIndices.push_back({value, i});
            value = value / 2;
        }
        valuesAndIndices.push_back({value, i});
    }
    sort(valuesAndIndices.begin(), valuesAndIndices.end(), [](const auto& lhs, const auto& rhs) { return lhs.value < rhs.value; });

    // Two pointers - for each leftIndex (in valuesAndIndices array), find the minimum rightIndex such
    // that the set { valuesAndIndices[j].originalIndex | leftIndex <= j <= rightIndex } is the set of n 
    // original indices 0, 1, 2, ... n ( = a.size())
    int leftIndex = 0;
    map<int, int> numOfOriginalIndexInRange;
    numOfOriginalIndexInRange[valuesAndIndices[leftIndex].originalIndex]++;
    int numOriginalIndicesInRange = 1;
    int rightIndex = 0;

    int bestLeftIndex = -1;
    int bestRightIndex = -1;
    while (leftIndex < valuesAndIndices.size())
    {
        while (rightIndex < valuesAndIndices.size() && numOriginalIndicesInRange < a.size())
        {
            rightIndex++;
            numOfOriginalIndexInRange[valuesAndIndices[rightIndex].originalIndex]++;
            if (numOfOriginalIndexInRange[valuesAndIndices[rightIndex].originalIndex] == 1)
                numOriginalIndicesInRange++;
        }
        if (rightIndex == valuesAndIndices.size())
            break;

        const int maxDifference = valuesAndIndices[rightIndex].value - valuesAndIndices[leftIndex].value;
        if (maxDifference < minMaxDifference)
        {
            bestLeftIndex = leftIndex;
            bestRightIndex = rightIndex;

            minMaxDifference = maxDifference;
        }

        numOfOriginalIndexInRange[valuesAndIndices[leftIndex].originalIndex]--;
        if (numOfOriginalIndexInRange[valuesAndIndices[leftIndex].originalIndex] == 0)
            numOriginalIndicesInRange--;
        leftIndex++;
    }

    {
        // Diagnostics.
        vector<int> newValueAtOriginalIndex(a.size());
        set<int> originalIndicesUsed = { valuesAndIndices[bestLeftIndex].originalIndex, valuesAndIndices[bestRightIndex].originalIndex };
        newValueAtOriginalIndex[valuesAndIndices[bestLeftIndex].originalIndex] = valuesAndIndices[bestLeftIndex].value;
        newValueAtOriginalIndex[valuesAndIndices[bestRightIndex].originalIndex] = valuesAndIndices[bestRightIndex].value;
        assert(valuesAndIndices[bestLeftIndex].originalIndex != valuesAndIndices[bestRightIndex].originalIndex);
        for (int i = bestLeftIndex + 1; i <= bestRightIndex - 1; i++)
        {
            const int originalIndex = valuesAndIndices[i].originalIndex;
            if (originalIndicesUsed.find(originalIndex) == originalIndicesUsed.end())
            {
                originalIndicesUsed.insert(originalIndex);
                newValueAtOriginalIndex[originalIndex] = valuesAndIndices[i].value;
            }
        }
        assert(originalIndicesUsed.size() == a.size());

        vector<int> copyOfA(a);
        auto printArray = [&copyOfA](int highlightIndex) { 
            for (int i = 0; i < copyOfA.size(); i++)
            {
                if (i == highlightIndex)
                    cout << "[";
                cout << copyOfA[i];
                if (i == highlightIndex)
                    cout << "]";
                cout << " ";
            }
            cout << endl;
        };
        cout << "Starting array: " << endl;
        printArray(-1);
        cout << endl;
        for (int i = 0; i < a.size(); i++)
        {
            while (newValueAtOriginalIndex[i] > copyOfA[i])
            {
                assert(copyOfA[i] % 2 == 1);
                cout << "Double the odd value " << copyOfA[i] << " at index " << i << " giving " << copyOfA[i] * 2 << endl;
                copyOfA[i] *= 2;
                printArray(i);
                cout << endl;
            }
            while (newValueAtOriginalIndex[i] < copyOfA[i])
            {
                assert(copyOfA[i] % 2 == 0);
                cout << "Halve the even value " << copyOfA[i] << " at index " << i << " giving " << copyOfA[i] / 2 << endl;
                copyOfA[i] /= 2;
                printArray(i);
                cout << endl;
            }
            assert(copyOfA[i] == newValueAtOriginalIndex[i]);

        }
        const int difference = *max_element(copyOfA.begin(), copyOfA.end()) - *min_element(copyOfA.begin(), copyOfA.end());
        assert(difference == minMaxDifference);
    }


    return minMaxDifference;
}


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        const int T = 1;
        cout << T << endl;

        for (int t = 0; t < T; t++)
        {
            const int N = 2 + rand() % 10;
            const int maxA = rand() % 1000 + 1;

            cout << N << endl;
            for (int i = 0; i < N; i++)
            {
                cout << (1 + rand() % maxA);
                if (i != N - 1)
                    cout << " ";
            }
            cout << endl;

        }

        return 0;
    }
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();
        vector<int> a(N);
        for (auto& value : a)
            value = read<int>();

#ifdef BRUTE_FORCE
#if 1
        const auto solutionBruteForce = solveBruteForce(a);
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
#endif
#if 1
        const auto solutionOptimised = solveOptimised(a);
        cout << "solutionOptimised:  " << solutionOptimised << endl;

        assert(solutionOptimised == solutionBruteForce);
#endif
#else
        const auto solutionOptimised = solveOptimised(a);
        cout << solutionOptimised << endl;
#endif
    }

    assert(cin);
}
