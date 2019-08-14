// Simon St James (ssjgz) - 2019-08-03
#include <iostream>
#include <vector>
#include <algorithm>

#include <cassert>

using namespace std;

#define gc getchar_unlocked

void scan_integer( int &x )
{
    int c = gc();
    x = 0;
    for( ; ((c<48 || c>57) && c != '-'); c = gc() );
    for( ;c>47 && c<58; c = gc() ) {
        x = (x << 1) + (x << 3) + c - 48;
    }
}


int readInt()
{
    int toRead;
    scan_integer(toRead);
    return toRead;
}

vector<int> calcRadiationLevels(const vector<int>& radiationPower)
{
    const int numCaves = radiationPower.size();
    vector<int> radiationLevel(numCaves, 0);

    vector<int> changeToAmountToAdd(numCaves, 0);

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
    int amountToAdd = 0;
    for (int pos = 0; pos < numCaves; pos++)
    {
        amountToAdd += changeToAmountToAdd[pos];
        radiationLevel[pos] += amountToAdd;
    }
    return radiationLevel;
}

bool calcCanKillAllZombies(const vector<int>& radiationPower, const vector<int>& zombieHealths)
{
    const auto radiationLevels = calcRadiationLevels(radiationPower);

    const auto maxRadiationLevel = *max_element(radiationLevels.begin(), radiationLevels.end());
    const auto maxZombieHealth = *max_element(zombieHealths.begin(), zombieHealths.end());

    // The radiation level for a cave can have at most radiationPower.size() caves contributing
    // "1" to it.
    assert(maxRadiationLevel<= radiationPower.size());

    if (maxZombieHealth > maxRadiationLevel)
    {
        // Zombie healths and Radiation Levels cannot be permutations of each other.
        return false;
    }

    // The maximum zombie health and maximum radiation level are both O(N),
    // so we can easily form the two vectors below.
    assert(maxZombieHealth <= radiationPower.size());

    vector<int> numCavesWithRadiationLevel(maxRadiationLevel + 1);
    for (const auto radiationLevel : radiationLevels)
    {
        numCavesWithRadiationLevel[radiationLevel]++;
    }

    vector<int> numZombiesWithHealth(maxZombieHealth + 1);
    for (const auto zombieHealth : zombieHealths)
    {
        numZombiesWithHealth[zombieHealth]++;
    }

    // Zombie healths are a permutation of radiation levels if and only if
    // the two "histogram" vectors are identical.  We could also have
    // compared the sorted versions of Zombie Health and Radiation Levels,
    // but that would be O(N log2 N) whereas the current way is O(N).
    return (numCavesWithRadiationLevel == numZombiesWithHealth);
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
    //
    // Space: O(N).  Time: O(N).
    ios::sync_with_stdio(false);

    const int T = readInt();
    for (int t = 0; t < T; t++)
    {
        const int numCaves = readInt();
        vector<int> radiationPower(numCaves);
        for (auto& power : radiationPower)
        {
            power = readInt();
        }
        vector<int> zombieHealths(numCaves);
        for (auto& health : zombieHealths)
        {
            health = readInt();
        }
        const auto canKillAllZombies = calcCanKillAllZombies(radiationPower, zombieHealths);
        cout << (canKillAllZombies ? "YES" : "NO") << endl;
    }
}

