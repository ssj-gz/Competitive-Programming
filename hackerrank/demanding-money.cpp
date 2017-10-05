#include <iostream>
#include <vector>

using namespace std;

struct House
{
    int money = 0;
    int houseIndex = 0;
    vector<House*> neighbours;
};

const int memoiseLastNHouses = 12;

pair<int, int> findMaxMoney(const int currentHouseIndex, const int64_t availableHouses, const vector<House>& houses)
{
    if (currentHouseIndex == houses.size())
    {
        return {0, 1};
    }
    const auto resultIfNotChoseThis = findMaxMoney(currentHouseIndex + 1, availableHouses, houses);

    const bool houseIsAvailable = ((availableHouses & (static_cast<int64_t>(1) << currentHouseIndex)) != 0);
    if (!houseIsAvailable)
    {
        return resultIfNotChoseThis;
    }

    int64_t availableHousesIfChoseThis = availableHouses;
    for (const auto neighbour : houses[currentHouseIndex].neighbours)
    {
        availableHousesIfChoseThis -= (availableHousesIfChoseThis & (static_cast<int64_t>(1) << neighbour->houseIndex));
    }
    auto resultIfChoseThis = findMaxMoney(currentHouseIndex + 1, availableHousesIfChoseThis, houses);
    resultIfChoseThis.first += houses[currentHouseIndex].money;

    if (resultIfNotChoseThis.first > resultIfChoseThis.first)
    {
        return resultIfNotChoseThis;
    }
    else if (resultIfNotChoseThis.first < resultIfChoseThis.first)
    {
        return resultIfChoseThis;
    }
    else 
    {
        assert(resultIfChoseThis.first == resultIfNotChoseThis.first);
        return { resultIfChoseThis.first, resultIfChoseThis.second + resultIfNotChoseThis.second};
    }
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

    int64_t availableHouses = (static_cast<int64_t>(1) << (N + 1)) - 1;
    const auto result = findMaxMoney(0, availableHouses, houses);

    cout << result.first << " " << result.second << endl;

}
