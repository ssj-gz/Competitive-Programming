// Simon St James (ssjgz) - 2019-12-28
// 
// Solution to: https://www.codechef.com/problems/WALKFAST
//
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
int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int numCities = read<int>();
        const int beginIndex = read<int>() - 1;
        const int endIndex = read<int>() - 1;
        const int trainBeginIndex = read<int>() - 1;
        const int trainEndIndex = read<int>() - 1;
        const int walkSecsPerMetre = read<int>();
        const int trainSecsPerMetre = read<int>();
        const int trainLeaveTime = read<int>();

        vector<int> cityPositions(numCities);
        for (auto& cityPos : cityPositions)
        {
            cityPos = read<int>();
        }

        const int distBeginToEnd = abs(cityPositions[beginIndex] - cityPositions[endIndex]);
        const int distTrainBeginToTrainEnd = abs(cityPositions[trainBeginIndex] - cityPositions[trainEndIndex]);
        const int distBeginToTrainBegin = abs(cityPositions[beginIndex] - cityPositions[trainBeginIndex]);
        const int distTrainEndToEnd = abs(cityPositions[trainEndIndex] - cityPositions[endIndex]);

        const int timeJustWalk = distBeginToEnd * walkSecsPerMetre;
        int minTime = timeJustWalk;

        const int timeWalkToTrain = distBeginToTrainBegin * walkSecsPerMetre;
        if (timeWalkToTrain <= trainLeaveTime)
        {
            const int timeWalkThenTrain = max(trainLeaveTime, timeWalkToTrain) + distTrainBeginToTrainEnd * trainSecsPerMetre + distTrainEndToEnd * walkSecsPerMetre;
            minTime = min(minTime, timeWalkThenTrain);
        }

        cout << minTime << endl;
    }

    assert(cin);
}
