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

vector<vector<pair<int, int>>> memoisedResultForHouseIndexAndAvailableHouses;

pair<int, int> findMaxMoney(const int currentHouseIndex, const uint64_t availableHouses, const vector<House>& houses)
{
    //cout << " currentHouseIndex: " << currentHouseIndex << " availableHouses: " << availableHouses << endl;
    if (currentHouseIndex == houses.size())
    {
        return {0, 1};
    }
    const int leadingHousesToSkip = max(static_cast<int>(houses.size() - memoiseLastNHouses), 0);
    const int memoiseHouseIndex = currentHouseIndex - leadingHousesToSkip;
    //cout << "leadingHousesToSkip + memoiseHouseIndex: " << (leadingHousesToSkip + memoiseHouseIndex) << endl;
    const auto memoiseHouseAvailable = availableHouses >> static_cast<uint64_t>(leadingHousesToSkip + memoiseHouseIndex);
    const bool canUseMemoisable = (memoiseHouseIndex >= 0);
    if (canUseMemoisable)
    {
        //cout << "fleep currentHouseIndex: " << currentHouseIndex << " availableHouses: " << availableHouses << " memoiseHouseIndex: " << memoiseHouseIndex << " memoiseHouseAvailable: " << memoiseHouseAvailable << " memoisedResultForHouseIndexAndAvailableHouses[memoiseHouseIndex].size(): " << memoisedResultForHouseIndexAndAvailableHouses[memoiseHouseIndex].size() << endl;
    }
    //assert(!canUseMemoisable || (memoiseHouseIndex < memoiseLastNHouses && memoiseHouseAvailable >= 0 && memoiseHouseAvailable < memoisedResultForHouseIndexAndAvailableHouses[memoiseHouseIndex].size()));
    if (true && canUseMemoisable && memoisedResultForHouseIndexAndAvailableHouses[memoiseHouseIndex][memoiseHouseAvailable].first != -1)
    {
        //cout << "using memo-ised result:" << memoisedResultForHouseIndexAndAvailableHouses[memoiseHouseIndex][memoiseHouseAvailable].first << "," <<  memoisedResultForHouseIndexAndAvailableHouses[memoiseHouseIndex][memoiseHouseAvailable].second << endl;
        return memoisedResultForHouseIndexAndAvailableHouses[memoiseHouseIndex][memoiseHouseAvailable];
    }
    const auto resultIfNotChoseThis = findMaxMoney(currentHouseIndex + 1, availableHouses, houses);

    const bool houseIsAvailable = ((availableHouses & (static_cast<uint64_t>(1) << currentHouseIndex)) != 0);
    if (!houseIsAvailable)
    {
        if (canUseMemoisable)
        {
            //cout << "Storing " << resultIfNotChoseThis.first << "," << resultIfNotChoseThis.second << " at " << " memoiseHouseIndex: " << memoiseHouseIndex << " memoiseHouseAvailable: " << memoiseHouseAvailable << endl;
            memoisedResultForHouseIndexAndAvailableHouses[memoiseHouseIndex][memoiseHouseAvailable] = resultIfNotChoseThis;
        }
        return resultIfNotChoseThis;
    }

    uint64_t availableHousesIfChoseThis = availableHouses;
    for (const auto neighbour : houses[currentHouseIndex].neighbours)
    {
        availableHousesIfChoseThis -= (availableHousesIfChoseThis & (static_cast<uint64_t>(1) << neighbour->houseIndex));
    }
    auto resultIfChoseThis = findMaxMoney(currentHouseIndex + 1, availableHousesIfChoseThis, houses);
    resultIfChoseThis.first += houses[currentHouseIndex].money;

    pair<int, int> result;
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

    //cout << "currentHouseIndex: " << currentHouseIndex << " availableHouses: " << availableHouses << " result: " << result.first << "," << result.second << endl;
    if (canUseMemoisable)
    {
            //cout << "Storing " << result.first << "," << result.second << " at " << " memoiseHouseIndex: " << memoiseHouseIndex << " memoiseHouseAvailable: " << memoiseHouseAvailable << endl;
        memoisedResultForHouseIndexAndAvailableHouses[memoiseHouseIndex][memoiseHouseAvailable] = result;
    }

    return result;
}

int main()
{
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
        memoisedResultForHouseIndexAndAvailableHouses[i] = vector<pair<int, int>>(numPossibleValues, {-1, -1});
    }
    uint64_t availableHouses = (static_cast<uint64_t>(1) << (N + 0)) - 1;
    const auto result = findMaxMoney(0, availableHouses, houses);

    cout << result.first << " " << result.second << endl;

}
