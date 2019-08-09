// Simon St James (ssjgz) - 2019-08-03
#define SUBMISSION
#ifdef SUBMISSION
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

vector<int64_t> calcRadiationLevels(const vector<int64_t>& radiationPower)
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
    return radiationLevel;
}

bool calcCanKillAllZombies(const vector<int64_t>& radiationPower, const vector<int64_t>& zombieHealth)
{
    const auto radiationLevels = calcRadiationLevels(radiationPower);

    auto sorted = [](const vector<int64_t>& toSort)
    {
        vector<int64_t> sorted = toSort;
        std::sort(sorted.begin(), sorted.end());

        return sorted;
    };

    const auto radiationLevelsSorted = sorted(radiationLevels);
    const auto zombieHealthsSorted = sorted(zombieHealth);

    return (radiationLevelsSorted == zombieHealthsSorted);
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);

    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        const int numCaves = rand() % 11 + 1;
        const int maxHealth = rand() % 100 + 1;
        const int maxRadiationPower = rand() % numCaves + 1;

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
        const auto canKillAllZombies = calcCanKillAllZombies(radiationPower, zombieHealth);
        cout << (canKillAllZombies ? "YES" : "NO") << endl;
    }
}

