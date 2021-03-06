// Simon St James (ssjgz) - 2019-11-07
// 
// Solution to: https://www.codechef.com/NOV19A/problems/PBOXES
//
// This is O(N^2); just aiming for Subtask #1 & #2 for this submission :)
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
//#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <set>
#include <iomanip>

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
    answers[0].first = 0; // Assuming a choice of 0 boxes is OK?

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
        for (int j = 0; j <= i; j++)
        {
            bestFori = max(bestFori, answers[j].first);
        }
        result.push_back(bestFori);
    }
    return result;
}
#endif

struct PValueAndIndex
{
    int64_t p = -1;
    int index = -1;
};
bool operator<(const PValueAndIndex& lhs, const PValueAndIndex& rhs)
{
    if (lhs.p != rhs.p)
        return lhs.p < rhs.p;
    return lhs.index < rhs.index;
}

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
#if 0
    cout << "psByS:" << endl;
    for (const auto x : psByS)
    {
        cout << x << " ";
    }
    cout << endl;
#endif

    vector<int64_t> result(N / 2 + 1);
    multiset<PValueAndIndex> unused;
    multiset<PValueAndIndex> uppers;
    multiset<PValueAndIndex> lowers;
    int64_t bestSum = 0;
    for (int i = 0; i < N; i++)
    {
        const auto x = psByS[i];
        //cout << "Iteration: x: " << x << " bestSum: " << bestSum << " pairs: " << endl;
        //for (const auto x : blah)
        //{
            //cout << "{" << x.first << ", " << x.second << "} ";
        //}
        //cout << endl;
#if 0
        cout << "unused: " << endl;
        for (const auto x : unused)
        {
            cout << x.p << " ";
        }
        cout << endl;
        cout << "uppers: " << endl;
        for (const auto x : uppers)
        {
            cout << x.p << " ";
        }
        cout << endl;
        cout << "lowers: " << endl;
        for (const auto x : lowers)
        {
            cout << x.p << " ";
        }
        cout << endl;
#endif
        const auto newSumIfIgnoreX = bestSum;
        auto newSumIfAdd = std::numeric_limits<int64_t>::min();
        if (!unused.empty())
        {
            newSumIfAdd = bestSum + x - unused.begin()->p;
        }
        auto newSumIfReplace = std::numeric_limits<int64_t>::min();
        if (!uppers.empty())
        {
            newSumIfReplace = bestSum + x - uppers.begin()->p;
        }
        //cout << "newSumIfIgnoreX: " << newSumIfIgnoreX << " newSumIfAdd: " << newSumIfAdd << " newSumIfReplace: " << newSumIfReplace << endl;

        if (newSumIfIgnoreX >= newSumIfAdd && newSumIfIgnoreX >= newSumIfReplace)
        {
            unused.insert({x, i});
            continue;
        }

        if (newSumIfReplace > newSumIfAdd)
        {
            const auto toReplace = *uppers.begin();
            uppers.erase(uppers.begin());
            uppers.insert({x, i});
            unused.insert(toReplace);
            bestSum = newSumIfReplace;
        }
        else
        {
            const auto newLower = *(unused.begin());
            unused.erase(unused.begin());
            uppers.insert({x, i});
            lowers.insert(newLower);

            bestSum = newSumIfAdd;
        }
    }
    //cout << "Final sum: " << bestSum << " from pairs: " << endl;
#if 0
    for (const auto x : blah)
    {
        cout << "{" << x.first << ", " << x.second << "} ";
    }
    cout << endl;
#endif
    assert(lowers.size() == uppers.size());
    for (int i = uppers.size(); i <= N / 2; i++)
    {
        result[i] = bestSum;
    }
#if 1
    string s(N, '.');
    for (const auto& x : lowers)
    {
        s[x.index] = '-';
    }
    for (const auto& x : uppers)
    {
        s[x.index] = '+';
    }
    //cout << s << endl;
#endif
    set<int> openUppers;
    set<PValueAndIndex> allowedUppers;
    int numUnattachedLowers = 0;
    vector<int64_t> bestUpperForHere(N, -1);
    vector<int64_t> prevBestUpperForHere;
    while (!uppers.empty())
    {
        set<int> lockIndices;
        int bestLowerToRemove = -1;
        int bestUpperToRemove = -1;
        int64_t scoreForBestRemovedUpperAndLower = std::numeric_limits<int64_t>::min();
        for (auto upper : uppers)
        {
            allowedUppers.insert(upper);
        }
        for (int i = N - 1; i >= 0; i--)
        {
            if (s[i] == '+')
            {
                openUppers.insert(i);
            }
            if (s[i] == '-')
            {
                // Consequences of removing this?
                // Can remove:
                //   i) Any open upper.
                //  ii) Any upper to our left.
                int64_t scoreFromRemovingThisLower = bestSum + psByS[i];
#if 0
                int64_t lowestUpperVal = std::numeric_limits<int64_t>::max();
                int lowestUpperIndex = -1;
                for (int j = 0; j < i; j++)
                {
                    if (s[j] == '+')
                    {
                        if (psByS[j] < lowestUpperVal)
                        {
                            lowestUpperVal = psByS[j];
                            lowestUpperIndex = j;
                        }
                    }
                }

                for (const auto upper : openUppers)
                {
                    if (psByS[upper] < lowestUpperVal)
                    {
                        lowestUpperVal = psByS[upper];
                        lowestUpperIndex = upper;
                    }
                }
#endif
                const auto lowestUpper = *allowedUppers.begin();
                const auto lowestUpperVal = lowestUpper.p;
                const auto lowestUpperIndex = lowestUpper.index;
                scoreFromRemovingThisLower -= lowestUpperVal;
                if (scoreFromRemovingThisLower > scoreForBestRemovedUpperAndLower)
                {
                    scoreForBestRemovedUpperAndLower = scoreFromRemovingThisLower;
                    bestUpperToRemove = lowestUpperIndex;
                    bestLowerToRemove = i;
                }
                bestUpperForHere[i] = lowestUpperVal;
            }

            if (s[i] == '-')
            {
                assert(!openUppers.empty());
                numUnattachedLowers++;
                assert(numUnattachedLowers <= openUppers.size());
                if (numUnattachedLowers == openUppers.size())
                {
                    while (!openUppers.empty())
                    {
                        allowedUppers.erase({psByS[*openUppers.begin()], *openUppers.begin()});
                        openUppers.erase(openUppers.begin());

                    }
                    numUnattachedLowers = 0;
                    lockIndices.insert(i);
                }
            }

        }
        assert(bestLowerToRemove != -1 && bestUpperToRemove != -1);
        s[bestUpperToRemove] = '.';
        s[bestLowerToRemove] = '.';
        bestSum = scoreForBestRemovedUpperAndLower;
        //cout << "s: " << s << " bestSum: " << bestSum << endl;
        uppers.erase({psByS[bestUpperToRemove], bestUpperToRemove});
        lowers.erase({psByS[bestLowerToRemove], bestLowerToRemove});

#if 0
        cout << "bestUpperForHere: " << endl;
        for (const auto& x : bestUpperForHere)
        {
            cout << setw(4) << x << " ";
        }
        cout << endl;
#endif
#if 0
        if (!prevBestUpperForHere.empty())
        {
            cout << "diff bestUpperForHere from last: " << endl;
            for (int i = 0; i < N; i++)
            {
                if (s[i] == '-')
                {
                    cout << setw(3) << bestUpperForHere[i] - prevBestUpperForHere[i] << " ";
                }
                else
                {
                    cout << setw(3) << '*' << " ";
                }
            }
            cout << endl;

        }
#endif
        prevBestUpperForHere = bestUpperForHere;

        cout << "lockIndices: " << endl;
        for (const auto x : lockIndices)
        {
            cout << " " << x;
        }
        cout << endl;


        result[uppers.size()] = bestSum;


        assert(uppers.size() == lowers.size());
    }

    
    result.erase(result.begin());

    return result;
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
        //const int N = 2 + rand() % 20;
        const int N = 1000;
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

    //assert((solutionOptimised.empty() && solutionBruteForce.empty()) ||  solutionOptimised.back() == solutionBruteForce.back());
    assert(solutionBruteForce == solutionOptimised);
#endif
#else
    const auto solutionOptimised = solveOptimised(N, S, P);
    for (const auto x : solutionOptimised)
    {
        cout << x << endl;
    }
#endif

    assert(cin);
}
