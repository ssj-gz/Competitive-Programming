// Simon St James (ssjgz) - 2019-11-07
// 
// Solution to: https://www.codechef.com/NOV19A/problems/PBOXES
//
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <set>

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

void solutionBruteForceAux(int index, const vector<int64_t>& psByS, vector<std::pair<int64_t, int64_t>>& indexPairsSoFar, vector<bool>& isIndexUsed, vector<std::pair<int64_t, vector<std::pair<int64_t, int64_t>>>>& answers)
{
    if (index == psByS.size())
        return;

    int64_t sum = 0;
    for (const auto& pair : indexPairsSoFar)
    {
        sum += psByS[pair.second] - psByS[pair.first];
    }
    //cout << "# pairs: " << indexPairsSoFar.size() << " sum: " << sum << endl;
    if (answers[indexPairsSoFar.size()].first < sum)
    {
        answers[indexPairsSoFar.size()].first = sum;
        answers[indexPairsSoFar.size()].second = indexPairsSoFar;
    }

    // Don't use this index.
    solutionBruteForceAux(index + 1, psByS, indexPairsSoFar, isIndexUsed, answers);

    if (isIndexUsed[index])
        return;

    isIndexUsed[index] = true;
    for (int otherIndexForPair = index + 1; otherIndexForPair < psByS.size(); otherIndexForPair++)
    {
        if (isIndexUsed[otherIndexForPair])
            continue;

        isIndexUsed[otherIndexForPair] = true;
        indexPairsSoFar.push_back({index, otherIndexForPair});
        solutionBruteForceAux(index + 1, psByS, indexPairsSoFar, isIndexUsed, answers);
        indexPairsSoFar.pop_back();
        isIndexUsed[otherIndexForPair] = false;
    }

    isIndexUsed[index] = false;
}

#if 1
vector<int64_t> solveBruteForce(int N, const vector<int64_t>& SOrig, const vector<int64_t>& POrig)
{
    struct SAndP
    {
        int64_t s = -1;
        int64_t p = -1;
    };

    vector<SAndP> sAndP;
    for (int i = 0; i < N; i++)
    {
        sAndP.push_back({SOrig[i], POrig[i]});
    }
    sort(sAndP.begin(), sAndP.end(), [](const auto& lhs, const auto& rhs) 
            { 
                if (lhs.s != rhs.s)
                    return lhs.s < rhs.s;
                return lhs.p < rhs.p;
            });

    vector<int64_t> psByS;
    for (int i = 0; i < N; i++)
    {
        psByS.push_back(sAndP[i].p);
    }

    vector<bool> isIndexUsed(N, false);
    vector<std::pair<int64_t, vector<std::pair<int64_t, int64_t>>>> answers(N / 2 + 2);
    for (int i = 0; i <= N / 2; i++)
    {
        answers[i].first = numeric_limits<int64_t>::min();
    }

    vector<std::pair<int64_t, int64_t>> indexPairsSoFar;
    solutionBruteForceAux(0, psByS, indexPairsSoFar, isIndexUsed, answers);

    cout << "sAndP:" << endl;
    for (const auto x : sAndP)
    {
        cout << x.p << " ";
    }
    cout << endl;
    for (int i = 0; i <= N / 2; i++)
    {
        cout << "Picking exactly " << i << " sum: " << answers[i].first << " - ";
        for (const auto x : answers[i].second)
        {
            cout << "(" << sAndP[x.first].p << ", " << sAndP[x.second].p << ") ";
        }
        cout << endl;
    }
    
    vector<int64_t> result;
    for (int i = 1; i <= N / 2; i++)
    {
        int64_t bestFori = numeric_limits<int64_t>::min();
        for (int j = 1; j <= i; j++)
        {
            bestFori = max(bestFori, answers[j].first);
        }
        result.push_back(bestFori);
    }
    return result;
}
#endif

#if 1
vector<int64_t> solveOptimised(int N, const vector<int64_t>& SOrig, const vector<int64_t>& POrig)
{
    struct SAndP
    {
        int64_t s = -1;
        int64_t p = -1;
    };

    vector<SAndP> sAndP;
    for (int i = 0; i < N; i++)
    {
        sAndP.push_back({SOrig[i], POrig[i]});
    }
    sort(sAndP.begin(), sAndP.end(), [](const auto& lhs, const auto& rhs) 
            { 
            if (lhs.s != rhs.s)
            return lhs.s < rhs.s;
            return lhs.p < rhs.p;
            });

    vector<int64_t> psByS;
    for (int i = 0; i < N; i++)
    {
        psByS.push_back(sAndP[i].p);
    }
    cout << "psByS:" << endl;
    for (const auto x : psByS)
    {
        cout << x << " ";
    }
    cout << endl;

    multiset<int64_t> unused;
    multiset<int64_t> uppers;
    vector<std::pair<int64_t, int64_t>> blah;
    int64_t bestSum = 0;
    for (const auto& x : psByS)
    {
        cout << "Iteration: x: " << x << " bestSum: " << bestSum << " pairs: " << endl;
        for (const auto x : blah)
        {
            cout << "{" << x.first << ", " << x.second << "} ";
        }
        cout << "unused: " << endl;
        for (const auto x : unused)
        {
            cout << x << " ";
        }
        cout << endl;
        cout << "uppers: " << endl;
        for (const auto x : uppers)
        {
            cout << x << " ";
        }
        cout << endl;
        const auto newSumIfIgnoreX = bestSum;
        auto newSumIfAdd = std::numeric_limits<int64_t>::min();
        if (!unused.empty())
        {
            newSumIfAdd = bestSum + x - *unused.begin();
        }
        auto newSumIfReplace = std::numeric_limits<int64_t>::min();
        if (!uppers.empty())
        {
            newSumIfReplace = bestSum + x - *uppers.begin();
        }
        cout << "newSumIfIgnoreX: " << newSumIfIgnoreX << " newSumIfAdd: " << newSumIfAdd << " newSumIfReplace: " << newSumIfReplace << endl;

        if (newSumIfIgnoreX >= newSumIfAdd && newSumIfIgnoreX >= newSumIfReplace)
        {
            unused.insert(x);
            continue;
        }

        if (newSumIfReplace > newSumIfAdd)
        {
            const auto toReplace = *uppers.begin();
            uppers.erase(uppers.begin());
            uppers.insert(x);
            unused.insert(toReplace);
            int lower = 0;
            for (auto blahIter = blah.begin(); blahIter != blah.end(); blahIter++)
            {
                if (blahIter->second == toReplace)
                {
                    lower = blahIter->first;
                    blah.erase(blahIter);
                    break;
                }
            }
            assert(lower != 0);
            blah.push_back({lower, x});
            bestSum = newSumIfReplace;
        }
        else
        {
            const int newLower = *unused.begin();
            unused.erase(unused.begin());
            uppers.insert(x);
            blah.push_back({newLower, x});

            bestSum = newSumIfAdd;
        }
    }
    cout << "Final sum: " << bestSum << " from pairs: " << endl;
    for (const auto x : blah)
    {
        cout << "{" << x.first << ", " << x.second << "} ";
    }
    cout << endl;

    return vector<int64_t>();
}
#endif


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        // TODO - generate randomised test.
        const int N = 1 + rand() % 13;
        const int maxS = 1 + rand() % 100;
        const int maxP = 1 + rand() % 100;

        cout << N << endl;
        for (int i = 0; i < N; i++)
        {
            cout << (1 + rand() % maxS) << " " << (1 + rand() % maxP) << endl;
        }

        return 0;
    }

    const int N = read<int>();

    vector<int64_t> S(N);
    vector<int64_t> P(N);

    for (int i = 0; i < N; i++)
    {
        S[i] = read<int64_t>();
        P[i] = read<int64_t>();
    }

#ifdef BRUTE_FORCE
#if 1
    const auto solutionBruteForce = solveBruteForce(N, S, P);
    cout << "solutionBruteForce--: " << endl;
    for (const auto x : solutionBruteForce)
    {
        cout << "solutionBruteForce: " << x << endl;
    }
#endif
#if 1
    const auto solutionOptimised = solveOptimised(N, S, P);
    cout << "solutionOptimised--: " << endl;
    for (const auto x : solutionOptimised)
    {
        cout << "solutionOptimised: " << x << endl;
    }

    //assert(solutionOptimised == solutionBruteForce);
#endif
#else
    const auto solutionOptimised = solveOptimised();
    cout << solutionOptimised << endl;
#endif

    assert(cin);
}
