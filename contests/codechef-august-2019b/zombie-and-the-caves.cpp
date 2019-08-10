// Simon St James (ssjgz) - 2019-08-03
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

vector<int64_t> calcRadiationLevels(const vector<int64_t>& radiationPower)
{
    const int numCaves = radiationPower.size();
    vector<int64_t> radiationLevel(numCaves, 0);

    vector<int64_t> changeToAmountToAdd(numCaves, 0);

    for (int i = 0; i < numCaves; i++)
    {
        assert(radiationPower[i] > 0);
        // Notify the Sweeper that a range has just begun, so it needs
        // to add an extra 1 to each cell it encounters in its travel from
        // left to right  until further notice.
        // If the range begins to the left of the 0th pos, treat the 0th
        // pos as the beginning of the range.
        const int radiationRangeBeginPos = max<int>(0, i - radiationPower[i]);
        changeToAmountToAdd[radiationRangeBeginPos]++;
        // Notify the Sweeper that a range finished, so it no longer has to add
        // the extra 1 to each cell it encounters.
        const int afterRadiationRangeEndPos = i + radiationPower[i] + 1;
        if (afterRadiationRangeEndPos < numCaves)
            changeToAmountToAdd[afterRadiationRangeEndPos]--;

    }

    // Sweep from left to right, obeying the "breadcrumbs" left in the code above.
    int64_t amountToAdd = 0;
    for (int pos = 0; pos < numCaves; pos++)
    {
        amountToAdd += changeToAmountToAdd[pos];
        radiationLevel[pos] += amountToAdd;
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

    // Well-known result: two arrays are anagrams/ permutations of each other
    // if and only if their sorted versions are equal.
    const auto radiationLevelsSorted = sorted(radiationLevels);
    const auto zombieHealthsSorted = sorted(zombieHealth);

    return (radiationLevelsSorted == zombieHealthsSorted);
}

int main(int argc, char* argv[])
{
    // Very easy one - the only potentially tricky part is computing the Radiation Levels, 
    // but there are a bunch of easy ways to deal with this: one way would be to use the
    // well-known "sweep from left to right, counting how many ranges the current position
    // is enclosed by" which takes O(N x log2N).  We can actually do better, though (O(N))
    // as the width of the ranges is at most N - hopefully the code in calcRadiationLevels
    // is reasonably self-explanatory :)
    //
    // Having computed the Radiation Levels, checking if all Zombies can be killed is trivial;
    // again, hopefully the code in calcCanKillAllZombies is self-explanatory :)
    ios::sync_with_stdio(false);

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

