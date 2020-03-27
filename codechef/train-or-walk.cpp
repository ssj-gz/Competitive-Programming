// Simon St James (ssjgz) - 2019-12-28
// 
// Solution to: https://www.codechef.com/problems/WALKFAST
//
#include <iostream>
#include <vector>
#include <algorithm>

#include <cassert>

using namespace std;

//#define DIAGNOSTICS

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
#ifdef DIAGNOSTICS
        cout << "TIME JUST WALKING: " << endl;
        cout << "Start position: " << cityPositions[beginIndex] << endl;
        cout << "End position: " << cityPositions[endIndex] << endl;
        cout << "Distance (metres): " << distBeginToEnd << endl;
        cout << "Time taken to walk distance at " << walkSecsPerMetre << " seconds per metre: **" << timeJustWalk << "**" << endl;
        cout << "Start train station position: " << cityPositions[trainBeginIndex] << endl;
        cout << "Distance to walk from journey begin to start train station: " << distBeginToTrainBegin << endl;
        cout << "Time taken to walk to start train station: " << timeWalkToTrain << endl;
        if (timeWalkToTrain <= trainLeaveTime)
        {
            cout << "We can reach the train station on time" << endl;
        }
        else
        {
            cout << "We can't reach the train station on time, so the whole journey must be taken on foot" << endl;
        }
#endif

        if (timeWalkToTrain <= trainLeaveTime)
        {
            const int timeWalkThenTrain = max(trainLeaveTime, timeWalkToTrain) + distTrainBeginToTrainEnd * trainSecsPerMetre + distTrainEndToEnd * walkSecsPerMetre;
            minTime = min(minTime, timeWalkThenTrain);

#ifdef DIAGNOSTICS
            cout << "TIME WALKING TO STATION THEN CATCHING TRAIN THEN WALKING REST OF THE WAY: " << endl;
            cout << "Time waiting for train to leave: " << trainLeaveTime << endl;
            cout << "Start train station position: " << cityPositions[trainBeginIndex] << endl;
            cout << "End train station position: " << cityPositions[trainEndIndex] << endl;
            cout << "Distance between train stations: " << distTrainBeginToTrainEnd << endl;
            cout << "Time taken to travel this distance by train, going at " << trainSecsPerMetre << " seconds per metre: " << distTrainBeginToTrainEnd * trainSecsPerMetre << endl;
            cout << "Final end position: " << cityPositions[endIndex] << endl;
            cout << "Distance between end train station and final end of journey: " << distTrainEndToEnd << endl;
            cout << "Time taken to walk this distance at : " << walkSecsPerMetre << " seconds per meter: "  << (distTrainEndToEnd * walkSecsPerMetre) << endl;
            cout << "Total time: **" << timeWalkThenTrain << "**" << endl;
#endif
        }

        cout << minTime << endl;
    }

    assert(cin);
}
