// Simon St James (ssjgz) - 2019-08-03
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <algorithm>

#include <cassert>

#include <sys/time.h>

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

bool solveBruteForce(const vector<int64_t>& radiationPower, const vector<int64_t>& zombieHealth)
{
    const int numCaves = radiationPower.size();
    vector<int64_t> radiationLevel(numCaves, 0);
    for (int i = 0; i < numCaves; i++)
    {
        const int radiationRange = radiationPower[i];
        const int radiationRangeLeft = max(0, i - radiationRange);
        const int radiationRangeRight = min(numCaves - 1, i + radiationRange);

        for (int j = radiationRangeLeft; j <= radiationRangeRight; j++)
        {
            radiationLevel[j]++;
        }
    }
    cout << "radiationLevel: " << endl;
    for (auto x : radiationLevel)
    {
        cout << x << " ";
    }
    cout << endl;

    vector<int64_t> zombieHealthPermutation = zombieHealth;
    do
    {
#if 0
        cout << "zombieHealthPermutation: " << endl;
        for (auto x : zombieHealthPermutation)
        {
            cout << x << " ";
        }
        cout << endl;
#endif
        int numZombiesKilled = 0;
        for(int i = 0; i < numCaves; i++)
        {
            if (radiationLevel[i] == zombieHealthPermutation[i])
            {
                numZombiesKilled++;
            }
        }
        if (numZombiesKilled == numCaves)
        {
            return true;
        }
        std::next_permutation(zombieHealthPermutation.begin(), zombieHealthPermutation.end());
    } while (zombieHealthPermutation != zombieHealth);
    return false;
}
bool solveOptimised(const vector<int64_t>& radiationPower, const vector<int64_t>& zombieHealth)
{
    const int numCaves = radiationPower.size();
    vector<int64_t> radiationLevel(numCaves, 0);
    {
        // Rightwards.
        vector<int64_t> changeToAmountToAdd(numCaves, 0);

        for (int i = 0; i < numCaves; i++)
        {
            assert(radiationPower[i] > 0);
            changeToAmountToAdd[i]++;
            const int rangeRightEnd = i + radiationPower[i] + 1;
            if (rangeRightEnd < numCaves)
                changeToAmountToAdd[rangeRightEnd]--;
        }

        int64_t amountToAdd = 0;
        for (int i = 0; i < numCaves; i++)
        {
            amountToAdd += changeToAmountToAdd[i];
            radiationLevel[i] += amountToAdd;
        }
    }
    {
        // Leftwards.
        vector<int64_t> changeToAmountToAdd(numCaves, 0);

        for (int i = 0; i < numCaves; i++)
        {
            assert(radiationPower[i] > 0);
            if (i - 1 >= 0)
            changeToAmountToAdd[i - 1]++;
            const int rangeEndLeft = i - radiationPower[i] - 1;
            if (rangeEndLeft >= 0)
                changeToAmountToAdd[rangeEndLeft]--;
        }

        int64_t amountToAdd = 0;
        for (int i = numCaves - 1; i >= 0; i--)
        {
            amountToAdd += changeToAmountToAdd[i];
            radiationLevel[i] += amountToAdd;
        }
    }
#ifdef BRUTE_FORCE
    {
        vector<int64_t> dbgRadiationLevel(numCaves, 0);
        for (int i = 0; i < numCaves; i++)
        {
            const int radiationRange = radiationPower[i];
            const int radiationRangeLeft = max(0, i - radiationRange);
            const int radiationRangeRight = min(numCaves - 1, i + radiationRange);

            for (int j = radiationRangeLeft; j <= radiationRangeRight; j++)
            {
                dbgRadiationLevel[j]++;
            }
        }
        cout << "radiationLevel: " << endl;
        for (auto x : radiationLevel)
        {
            cout << x << " ";
        }
        cout << endl;
        cout << "dbgRadiationLevel: " << endl;
        for (auto x : dbgRadiationLevel)
        {
            cout << x << " ";
        }
        cout << endl;
        assert(dbgRadiationLevel == radiationLevel);
    }
#endif
    auto sort = [](const vector<int64_t>& toSort)
    {
        vector<int64_t> sorted = toSort;
        std::sort(sorted.begin(), sorted.end());

        return sorted;
    };

    const auto radiationLevelsSorted = sort(radiationLevel);
    const auto zombieHealthsSorted = sort(zombieHealth);

    return (radiationLevelsSorted == zombieHealthsSorted);
}

int main(int argc, char* argv[])
{
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        const int numCaves = rand() % 10 + 1;
        const int maxHealth = rand() % 100 + 1;
        const int maxRadiationPower = rand() % 100 + 1;

        cout << 1 << endl;
        cout << numCaves << endl;

        vector<int64_t> radiationPower;
        for (int i = 0; i < numCaves; i++)
        {
            radiationPower.push_back((rand() % maxRadiationPower) + 1);
        }
        for (const auto& x : radiationPower)
        {
            cout << x << " ";
        }
        cout << endl;


        const bool generateYES = ((rand() % 4) == 0);
        if (generateYES)
        {
            vector<int64_t> radiationLevel(numCaves, 0);
            for (int i = 0; i < numCaves; i++)
            {
                const int radiationRange = radiationPower[i];
                const int radiationRangeLeft = max(0, i - radiationRange);
                const int radiationRangeRight = min(numCaves - 1, i + radiationRange);

                for (int j = radiationRangeLeft; j <= radiationRangeRight; j++)
                {
                    radiationLevel[j]++;
                }
            }
            auto zombieHealth = radiationLevel;
            random_shuffle(zombieHealth.begin(), zombieHealth.end());
            for (const auto& x : zombieHealth)
            {
                cout << x << " ";
            }
            cout << endl;
        }
        else
        {
            for (int i = 0; i < numCaves; i++)
            {
                cout << ((rand() % maxHealth) + 1) << " ";
            }
            cout << endl;
        }
        return 0;
    }

    const int T = read<int>();
    for (int t = 0; t < T; t++)
    {
        const int numCaves = read<int>();
        vector<int64_t> radiationPower(numCaves);
        for (auto& power : radiationPower)
        {
            power = read<int64_t>();
        }
        vector<int64_t> zombieHealth(numCaves);
        for (auto& health : zombieHealth)
        {
            health = read<int64_t>();
        }

        const auto solutionBruteForce = solveBruteForce(radiationPower, zombieHealth);
        cout << "solutionBruteForce: " << (solutionBruteForce ? "YES" : "NO") << endl;
        const auto solutionOptimised = solveOptimised(radiationPower, zombieHealth);
        cout << "solutionOptimised: " << (solutionOptimised ? "YES" : "NO") << endl;

        //assert(solutionOptimised == solutionBruteForce);
    }
}

