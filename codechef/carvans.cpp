// Simon St James (ssjgz) - 2020-08-22
// 
// Solution to: https://www.codechef.com/problems/CARVANS
//
#include <iostream>
#include <vector>
#include <limits>

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

int findNumCarsAtTheirMaxSpeed(const vector<int>& carMaxSpeeds)
{
    int numCarsTravellingAtMaxSpeed = 0;
    int maxSpeedWithoutCollision = std::numeric_limits<int>::max();

    for (const auto carMaxSpeed : carMaxSpeeds)
    {
        if (carMaxSpeed <= maxSpeedWithoutCollision)
            numCarsTravellingAtMaxSpeed++;
        maxSpeedWithoutCollision = min(maxSpeedWithoutCollision, carMaxSpeed);
    }
    
    return numCarsTravellingAtMaxSpeed;
}

int main()
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const auto numCars = read<int>();
        vector<int> carMaxSpeeds(numCars);

        for (auto& carMaxSpeed : carMaxSpeeds)
            carMaxSpeed = read<int>();

        cout << findNumCarsAtTheirMaxSpeed(carMaxSpeeds) << endl;
    }

    assert(cin);
}
