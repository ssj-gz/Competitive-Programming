// Simon St James (ssjgz) - 2019-08-03
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
        cout << "zombieHealthPermutation: " << endl;
        for (auto x : zombieHealthPermutation)
        {
            cout << x << " ";
        }
        cout << endl;
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

int main(int argc, char* argv[])
{
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        // TODO - generate randomised test.
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
    }
}

