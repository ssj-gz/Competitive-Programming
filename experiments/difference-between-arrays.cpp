// Simon St James (ssjgz) - 2019-XX-XX
// 
// Solution to: TODO - problem link here!
//
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
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
#if 0
    // Minor adaption of l_return's algorithm.
    int minMaxDifference = std::numeric_limits<int>::max();

    map<int, int> numOfElement;
    for (auto x : a)
    {
        if (x % 2 == 1)
        {
            x = x * 2;
        }
        numOfElement[x]++;
    }

    while (true)
    {
        const int maxElement = std::prev(numOfElement.end())->first;
        const int minElement = numOfElement.begin()->first;

        minMaxDifference = min(minMaxDifference, maxElement - minElement);

        if (maxElement % 2 == 0)
        {
            numOfElement[maxElement]--;
            if (numOfElement[maxElement] == 0)
                numOfElement.erase(std::prev(numOfElement.end()));
            numOfElement[maxElement / 2]++;
        }
        else
        {
            break;
        }
    }

    
    return minMaxDifference;
#else
    // mishraanoopam's algorithm from https://discuss.codechef.com/t/help-needed-in-past-american-express-hiring-challenge/45082/16
    int minMaxDifference = std::numeric_limits<int>::max();
    struct TransformedValueAndIndex
    {
        int value = -1;
        int index = -1;
    };

    vector<TransformedValueAndIndex> valuesAndIndices;
    for (int i = 0; i < a.size(); i++)
    {
        int value = a[i];
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
    int leftIndex = 0;
    map<int, int> numOfIndexInRange;
    numOfIndexInRange[valuesAndIndices[leftIndex].index]++;
    int numIndicesInRange = 1;
    int rightIndex = 0;
    while (leftIndex < valuesAndIndices.size())
    {
        while (rightIndex < valuesAndIndices.size() && numIndicesInRange < a.size())
        {
            rightIndex++;
            numOfIndexInRange[valuesAndIndices[rightIndex].index]++;
            if (numOfIndexInRange[valuesAndIndices[rightIndex].index] == 1)
                numIndicesInRange++;
        }
        if (rightIndex == valuesAndIndices.size())
            break;

        minMaxDifference = min(minMaxDifference, valuesAndIndices[rightIndex].value - valuesAndIndices[leftIndex].value);

        numOfIndexInRange[valuesAndIndices[leftIndex].index]--;
        if (numOfIndexInRange[valuesAndIndices[leftIndex].index] == 0)
            numIndicesInRange--;
        leftIndex++;
    }
    return minMaxDifference;
#endif
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
