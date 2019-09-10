// Simon St James (ssjgz) - 2019-09-10
// 
// Solution to: https://www.codechef.com/SEPT19B/problems/DOOFST
//
// This submission is for Subtask #1 only!
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
//#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <set>
#include <limits>

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

struct HatefulPair
{
    HatefulPair(int person1, int person2)
    {
        if (person2 < person1)
            swap(person1, person2);
        this->person1 = person1;
        this->person2 = person2;
    }
    int person1 = -1;
    int person2 = -1;
};

struct Subset
{
    Subset(const vector<int>& group1, const vector<int>& group2)
        : group1(group1), group2(group2)
    {
    }
    vector<int> group1; 
    vector<int> group2;
    set<HatefulPair> generatedHatefulPairs() const
    {
        set<HatefulPair> generatedHatefulPairs;
        addToHatefulPairs(generatedHatefulPairs);
        return generatedHatefulPairs;
    }
    void addToHatefulPairs(set<HatefulPair>& hatefulPairs) const
    {
        for (const auto group1Person : group1)
        {
            for (const auto group2Person : group2)
            {
                //cout << "blee: " << group1Person << ", " << group2Person << endl;
                hatefulPairs.insert({group1Person, group2Person});
            }
        }
    }

};

bool operator<(const HatefulPair& lhs, const HatefulPair& rhs)
{
    if (lhs.person1 != rhs.person1)
        return lhs.person1 < rhs.person1;
    return lhs.person2 < rhs.person2;
}
bool operator==(const HatefulPair& lhs, const HatefulPair& rhs)
{
    return (lhs.person1 == rhs.person1) && (lhs.person2 == rhs.person2);
}

ostream& operator<<(ostream& os, const Subset& subset)
{
    cout << "{";
    for (const auto x : subset.group1)
    {
        cout << (x + 1) << " ";
    }
    cout << "}  {";
    for (const auto x : subset.group2)
    {
        cout << (x + 1) << " ";
    }
    cout << "}";
    return os;
}

#if 1
std::pair<int, vector<string>> solveBruteForce(const int N, const int M, const vector<HatefulPair>& hatefulPairsList)
{
    set<HatefulPair> hatefulPairs(hatefulPairsList.begin(), hatefulPairsList.end());
    cout << "hatefulPairs: " << endl;
    for (const auto& hatefulPair : hatefulPairs)
    {
        cout << hatefulPair.person1 << ", " << hatefulPair.person2 << endl;
    }

    vector<Subset> usefulSubsets;
    {
        vector<bool> includeInSubset(N, false);
        while (true)
        {
            vector<int> group1;
            vector<int> group2;

            for (int i = 0; i < N; i++)
            {
                if (includeInSubset[i])
                    group1.push_back(i);
                else
                    group2.push_back(i);
            }

            const set<HatefulPair> hatefulPairsForSubset = Subset(group1, group2).generatedHatefulPairs();
            bool conflicts = false;
            for (const auto& hatefulPairForSubset : hatefulPairsForSubset)
            {
                if (hatefulPairs.find(hatefulPairForSubset) == hatefulPairs.end())
                {
                    conflicts = true;
                    break;
                }
            }
            bool isUseful = false;
            for (const auto& hatefulPair : hatefulPairs)
            {
                if (hatefulPairsForSubset.find(hatefulPair) != hatefulPairsForSubset.end())
                {
                    isUseful = true;
                }
            }
            if (!conflicts && isUseful)
            {
                usefulSubsets.push_back({group1, group2});
                cout << "Found a useful subset: " << endl;
                cout << Subset(group1, group2) << endl;
            }

            int index = 0;
            while (index < N && includeInSubset[index])
            {
                includeInSubset[index] = false;
                index++;
            }
            if (index == N)
                break;

            includeInSubset[index] = true;
        }
    }
    vector<bool> includeSubset(usefulSubsets.size(), false);
    int result = -1;

    for (int numToChoose = 1; numToChoose <= usefulSubsets.size() && result == -1; numToChoose++)
    {
        cout << "numToChoose: " << numToChoose << " usefulSubsets.size(): " << usefulSubsets.size() << endl;
        vector<int> choices(numToChoose);
        for (int i = 0; i < numToChoose; i++)
        {
            choices[i] = i;
        }
        while (true)
        {
            set<HatefulPair> generatedHatefulPairs;
            vector<Subset> subsets;
            for (int i = 0; i < numToChoose; i++)
            {
                subsets.push_back(usefulSubsets[choices[i]]);
            }
            for (const auto& subset : subsets)
            {
                subset.addToHatefulPairs(generatedHatefulPairs);
            }
            assert(subsets.size() == numToChoose);
            if (generatedHatefulPairs == hatefulPairs)
            {
                cout << "Found: " << subsets.size() << endl;
                for (const auto& subset : subsets)
                {
                    cout << " " << subset << endl;
                }
                result = subsets.size();
                break;

            }

            //string chosen(usefulSubsets.size(), '.');
            //for (int i = 0; i < numToChoose; i++)
            //{
            //chosen[choices[i]] = 'X';
            //}
            //cout << "chosen: " << endl;
            //cout << " " << chosen << endl;

#if 0
            for (int index = 0; index < numToChoose; index++)
            {
                cout << " index: " << index << " choice: " << choices[index] << " usefulSubsets.size: " << usefulSubsets.size() << " blah: "  << (usefulSubsets.size() - 1 - (numToChoose - 1 - index)) << endl;
            }
#endif

            int index = numToChoose - 1;
            while (index >= 0 && choices[index] == usefulSubsets.size() - 1 - (numToChoose - 1 - index))
            {
                index--;
            }
            if (index < 0)
                break;
            int nextChoice = choices[index] + 1;
            for (int i = index; i < numToChoose; i++)
            {
                choices[i] = nextChoice;
                nextChoice++;
            }

        }
    }
#if 0
        for (int numToChoose
        set<HatefulPair> generatedHatefulPairs;
        vector<Subset> subsets;
        for (int i = 0; i < usefulSubsets.size(); i++)
        {
            if (includeSubset[i])
            {
                const auto& subset = usefulSubsets[i];
                subsets.push_back(subset);
                for (const auto group1Person : subset.group1)
                {
                    for (const auto group2Person : subset.group2)
                    {
                        generatedHatefulPairs.insert({group1Person, group2Person});
                    }
                }
            }
        }
        if (generatedHatefulPairs == hatefulPairs)
        {
            if (result == -1 || subsets.size() < result)
            {
                cout << "New best: " << subsets.size() << endl;
                for (const auto& subset : subsets)
                {
                    cout << " " << subset << endl;
                }
                result = subsets.size();
            }

        }
#endif
    
    return {result, vector<string>()};
}
#endif

std::pair<int, vector<string>> solveOptimised(const int64_t N, const int64_t M, const vector<HatefulPair>& hatefulPairsList)
{
    for (const auto& hatefulPair : hatefulPairsList)
    {
        if (hatefulPair.person1 < 0 || hatefulPair.person1 > N - 1)
            return {-1, vector<string>()};
        if (hatefulPair.person2 < 0 || hatefulPair.person2 > N - 1)
            return {-1, vector<string>()};
        if (hatefulPair.person1 == hatefulPair.person2)
            return {-1, vector<string>()};
    }
    // Only deals with Subtask 1, so far.
    if (M == (N * (N - 1)) / 2)
    {
        //cout << "Whoo!" << endl;
        if (N == 1)
            return {-1, vector<string>()};
        int64_t minMoves = 1;
        while ((1 << (minMoves)) <= M - 1)
        {
            minMoves++;
        }
        if (minMoves * N > 10'000'000)
        {
            return {-1, vector<string>()};
        }
        else
        {
            std::pair<int, vector<string>> result;
            result.first = minMoves;
            vector<string>& subsetStrings = result.second;
            string subsetString = string(N / 2, '0') + string(N - N / 2, '1');
            //cout << "subsetString: " << subsetString << endl;
            subsetStrings.push_back(subsetString);
            minMoves--;

            while (minMoves > 0)
            {
                //cout << "Starting subsetString: " << subsetString << endl;
                int beginningOfRun = 0;
                for (int i = 0; i < N; i++)
                {
                    //cout << "i: " << i << " beginningOfRun: " << beginningOfRun << endl;
                    if (i == N - 1 || subsetString[i + 1] != subsetString[i])
                    {
                        const int numInRun = i + 1 - beginningOfRun;
                        //cout << " numInRun: " << numInRun << endl;
                        if (subsetString[i] == '0')
                        {
                            for (int j = i; j >= i - numInRun / 2; j--)
                            {
                                subsetString[j] = '1';
                            }
                        }
                        else
                        {
                            for (int j = i - numInRun / 2; j >= beginningOfRun; j--)
                            {
                                subsetString[j] = '0';
                            }
                        }

                        beginningOfRun = i + 1;
                    }
                }
                //cout << "new subsetString: " << subsetString << endl;
                subsetStrings.push_back(subsetString);
                minMoves--;
            }

            return result;
        }
    }
    else
    {
        return {-1, vector<string>()};
    }
}


const int maxThing = 1'000'000;
vector<int64_t> blahLookup(maxThing + 1, -1);

int64_t calcMinMoves(int64_t n)
{
    blahLookup[1] = 0;
    blahLookup[0] = 0;

    if (blahLookup[n] != -1)
    {
        return blahLookup[n];
    }

    int64_t minMoves = std::numeric_limits<int64_t>::max();
    for (int i = 1; i <= n - 1; i++)
    {
        const int64_t num = 1 + max(calcMinMoves(i), calcMinMoves(n - i));
        minMoves = min(minMoves, num);
    }

    blahLookup[n] = minMoves;
    //cout << "calcMinMoves: " << n << " result: " << minMoves << endl;
    return minMoves;
};

void verifySolution(const set<HatefulPair>& hatefulPairs, const vector<string>& subsetStrings)
{
    set<HatefulPair> generatedHatefulPairs;
    for (const auto& subsetString : subsetStrings)
    {
        vector<int> group1;
        vector<int> group2;
        //cout << "adding from substring: " << subsetString << endl;
        for (int i = 0; i < subsetString.size(); i++)
        {
            if (subsetString[i] == '1')
            {
                group1.push_back(i);
            }
            else
            {
                group2.push_back(i);
            }
        }
        assert(group1.size() + group2.size() == subsetString.size());

        Subset(group1, group2).addToHatefulPairs(generatedHatefulPairs);
    }
#if 0
    cout << "Expected HatefulPairs: " << endl;
    for (const auto& hatefulPair : hatefulPairs)
    {
        cout << " " << hatefulPair.person1 << ", " << hatefulPair.person2 << endl;
    }
    cout << "Actual HatefulPairs: " << endl;
    for (const auto& hatefulPair : generatedHatefulPairs)
    {
        cout << " " << hatefulPair.person1 << ", " << hatefulPair.person2 << endl;
    }
#endif
    assert(generatedHatefulPairs == hatefulPairs);

}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        //const bool generateYes = ((rand() % 4) == 0);
        const bool generateYes = true;

        if (generateYes)
        {
            set<HatefulPair> hatefulPairs;
            int N = -1;
            while (true)
            {
                hatefulPairs.clear();
                N = rand() % 1000 + 1;
#if 0
                const int numSets = rand() % 5 + 1;
                for (int i = 0; i < numSets; i++)
                {
                    vector<int> group1;
                    vector<int> group2;

                    for (int j = 0; j < N; j++)
                    {
                        if (rand() % 2 == 0)
                            group1.push_back(j);
                        else
                            group2.push_back(j);
                    }

                    Subset(group1, group2).addToHatefulPairs(hatefulPairs);

                }
#endif
                for (int i = 0; i < N; i++)
                {
                    for (int j = i + 1; j < N; j++)
                    {
                        hatefulPairs.insert({i, j});
                    }
                }
                //if (!hatefulPairs.empty() && hatefulPairs.size() < (N * (N - 1)) / 2 - 10)
                //if (!hatefulPairs.empty())
                //break;
                if (hatefulPairs.size() == (N * (N - 1))/ 2)
                {
                    //cerr << " Should be solvable in <= " << numSets << " moves" << endl;
                    break;
                }
                cerr << "Generated degenerate testcase; retrying" << endl;
            }
            cout << N << " " << hatefulPairs.size() << endl;
            for (const auto& hatefulPair : hatefulPairs)
            {
                cout << (hatefulPair.person1 + 1) << " " << (hatefulPair.person2 + 1) << endl;
            }

        }
        else
        {
            const int N = rand() % 20 + 1;
            const int numPairsEstimate = rand() % 100 + 1;
            set<HatefulPair> hatefulPairs;
            for (int i = 0; i < numPairsEstimate; i++)
            {
                hatefulPairs.insert({rand() % N + 1, rand() % N + 1});
            }
            cout << N << " " << hatefulPairs.size() << endl;
            for (const auto& hatefulPair : hatefulPairs)
            {
                cout << hatefulPair.person1 << " " << hatefulPair.person2 << endl;
            }
        }


        return 0;
    }

#if 0
    for (int64_t blee = 2; blee <= 100'000; blee++)
    {
        const int64_t minMovesBlee = calcMinMoves(blee);
        //cout << "minMoves: " << blee << " = " << minMovesBlee << endl;
        int64_t exponent = 1;
        while ((1 << (exponent)) <= blee - 1)
        {
            exponent++;
        }

        cout << "blee: " << blee << " minMovesBlee: " << minMovesBlee << " exponent: " << exponent << endl;
    }
    return 0;
#endif

    const int N = read<int>();
    const int M = read<int>();

    vector<HatefulPair> hatefulPairs;
    for (int i = 0; i < M; i++)
    {
        const int person1 = read<int>() - 1;
        const int person2 = read<int>() - 1;
        hatefulPairs.push_back({person1, person2});
    }

#ifdef BRUTE_FORCE
#if 1
    const auto solutionBruteForce = solveBruteForce(N, M, hatefulPairs);
    cout << "solutionBruteForce: " << solutionBruteForce.first << endl;
#endif
#if 0
    const auto solutionOptimised = solveOptimised(N, M, hatefulPairs);
    cout << "solutionOptimised:  " << solutionOptimised << endl;

    assert(solutionOptimised == solutionBruteForce);
#endif
#else
    const auto solutionOptimised = solveOptimised(N, M, hatefulPairs);
    cout << solutionOptimised.first << endl;
    for (const auto& subsetString : solutionOptimised.second)
    {
        cout << subsetString << endl;
    }
    //verifySolution(set<HatefulPair>(hatefulPairs.begin(), hatefulPairs.end()), solutionOptimised.second);
#endif

    assert(cin);
}
