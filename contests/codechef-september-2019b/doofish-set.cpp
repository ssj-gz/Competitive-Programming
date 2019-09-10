// Simon St James (ssjgz) - 2019-09-10
// 
// Solution to: https://www.codechef.com/SEPT19B/problems/DOOFST
//
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
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
int solveBruteForce(const int N, const int M, const vector<HatefulPair>& hatefulPairsList)
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

            set<HatefulPair> hatefulPairsForSubset;
            for (const auto group1Person : group1)
            {
                for (const auto group2Person : group2)
                {
                    hatefulPairsForSubset.insert({group1Person, group2Person});
                }
            }
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
                for (const auto group1Person : subset.group1)
                {
                    for (const auto group2Person : subset.group2)
                    {
                        generatedHatefulPairs.insert({group1Person, group2Person});
                    }
                }
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
    
    return result;
}
#endif

#if 0
int solveOptimised()
{
    int result;
    
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

        const bool generateYes = ((rand() % 4) == 0);

        if (generateYes)
        {
            set<HatefulPair> hatefulPairs;
            int N = -1;
            while (true)
            {
                N = rand() % 20 + 1;
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

                    for (const auto group1Person : group1)
                    {
                        for (const auto group2Person : group2)
                        {
                            hatefulPairs.insert({group1Person, group2Person});
                        }
                    }

                }
                if (hatefulPairs.size() != (N * (N - 1)) / 2)
                    break;
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
    cout << "solutionBruteForce: " << solutionBruteForce << endl;
#endif
#if 0
    const auto solutionOptimised = solveOptimised();
    cout << "solutionOptimised:  " << solutionOptimised << endl;

    assert(solutionOptimised == solutionBruteForce);
#endif
#else
    const auto solutionOptimised = solveOptimised();
    cout << solutionOptimised << endl;
#endif

    assert(cin);
}
