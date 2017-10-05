// Simon St James (ssjgz) 2017-10-05 10:24
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

struct House
{
    int money = 0;
    int houseIndex = 0;
    vector<House*> neighbours;
};

const int memoiseLastNHouses = 12;

vector<vector<pair<int64_t, int64_t>>> memoisedResultForHouseIndexAndAvailableHouses;

pair<int64_t, int64_t> findMaxMoney(const int currentHouseIndex, const uint64_t availableHouses, const vector<House>& houses)
{
    if (currentHouseIndex == houses.size())
    {
        return {0, 1};
    }
    // Work out the memo-isation parameters.
    const int leadingHousesToAlwaysSkip = max(static_cast<int>(houses.size() - memoiseLastNHouses), 0);
    const int memoiseHouseIndex = currentHouseIndex - leadingHousesToAlwaysSkip;
    const auto memoiseHouseAvailable = availableHouses >> static_cast<uint64_t>(leadingHousesToAlwaysSkip + memoiseHouseIndex);
    const bool canUseMemoisable = (memoiseHouseIndex >= 0);
    assert(!canUseMemoisable || (memoiseHouseIndex < memoiseLastNHouses && memoiseHouseAvailable >= 0 && memoiseHouseAvailable < memoisedResultForHouseIndexAndAvailableHouses[memoiseHouseIndex].size()));
    if (canUseMemoisable && memoisedResultForHouseIndexAndAvailableHouses[memoiseHouseIndex][memoiseHouseAvailable].first != -1)
    {
        return memoisedResultForHouseIndexAndAvailableHouses[memoiseHouseIndex][memoiseHouseAvailable];
    }

    // Don't choose this house, and recurse.
    const auto resultIfNotChoseThis = findMaxMoney(currentHouseIndex + 1, availableHouses, houses);
    const bool houseIsAvailable = ((availableHouses & (static_cast<uint64_t>(1) << currentHouseIndex)) != 0);
    if (!houseIsAvailable)
    {
        if (canUseMemoisable)
            memoisedResultForHouseIndexAndAvailableHouses[memoiseHouseIndex][memoiseHouseAvailable] = resultIfNotChoseThis;
        return resultIfNotChoseThis;
    }

    // Choose this house, and recurse.
    uint64_t availableHousesIfChoseThis = availableHouses;
    for (const auto neighbour : houses[currentHouseIndex].neighbours)
    {
        availableHousesIfChoseThis -= (availableHousesIfChoseThis & (static_cast<uint64_t>(1) << neighbour->houseIndex));
    }
    auto resultIfChoseThis = findMaxMoney(currentHouseIndex + 1, availableHousesIfChoseThis, houses);
    resultIfChoseThis.first += houses[currentHouseIndex].money;

    pair<int64_t, int64_t> result;
    if (resultIfNotChoseThis.first > resultIfChoseThis.first)
    {
        result = resultIfNotChoseThis;
    }
    else if (resultIfNotChoseThis.first < resultIfChoseThis.first)
    {
        result = resultIfChoseThis;
    }
    else 
    {
        assert(resultIfChoseThis.first == resultIfNotChoseThis.first);
        result = { resultIfChoseThis.first, resultIfChoseThis.second + resultIfNotChoseThis.second};
    }

    if (canUseMemoisable)
    {
        memoisedResultForHouseIndexAndAvailableHouses[memoiseHouseIndex][memoiseHouseAvailable] = result;
    }

    return result;
}

int main()
{
    // Fundamentally easy: it's basically a brute-force search with some memo-isation on the last memoiseLastNHouses houses.
    // We use an int64_t to denote a bitmask of the houses that Kilgrave can still visit (availableHouses).
    // If currentHouseIndex is amongst the last memoiseLastNHouses houses, the we take the binary representation of the availability
    // of the last (houses.size() - currentHouseIndex <= memoiseLastNHouses) houses (memoiseHouseAvailable), and store the result
    // that we eventually calculate in memoisedResultForHouseIndexAndAvailableHouses[memoiseHouseIndex][memoiseHouseAvailable], where memoiseHouseIndex
    // is the index from 0 ... memoiseLastNHouses - 1 of currentHouseIndex in the last memoiseLastNHouses to memo-ise.
    int N, M;
    cin >> N >> M;

    vector<House> houses(N);

    for (int i = 0; i < N; i++)
    {
        cin >> houses[i].money;
        houses[i].houseIndex = i;
    }

    for (int i = 0; i < M; i++)
    {
        int a, b;
        cin >> a >> b;
        a--;
        b--;

        houses[a].neighbours.push_back(&(houses[b]));
        houses[b].neighbours.push_back(&(houses[a]));
    }

    memoisedResultForHouseIndexAndAvailableHouses.resize(memoiseLastNHouses);
    for (int i = 0; i < memoiseLastNHouses; i++)
    {
        const uint64_t numPossibleValues = 1 << (memoiseLastNHouses - i);
        memoisedResultForHouseIndexAndAvailableHouses[i] = vector<pair<int64_t, int64_t>>(numPossibleValues, {-1, -1});
    }
    uint64_t availableHouses = (static_cast<uint64_t>(1) << (N + 0)) - 1;
    const auto result = findMaxMoney(0, availableHouses, houses);

    cout << result.first << " " << result.second << endl;
}
