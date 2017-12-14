#include <iostream>
#include <limits>

using namespace std;

int main()
{
    int t;
    cin >> t;

    int minimumTime = 0;
    for (int streetIndex = 0; streetIndex < t; streetIndex++)
    {
        int numHouses;
        cin >> numHouses;

        int minHouse = std::numeric_limits<int>::max();
        int maxHouse = std::numeric_limits<int>::min();

        for (int i = 0; i < numHouses; i++)
        {
            int housePos;
            cin >> housePos;

            minHouse = min(minHouse, housePos);
            maxHouse = max(maxHouse, housePos);
        }

        cout << (maxHouse - minHouse) << endl;
    }
}
