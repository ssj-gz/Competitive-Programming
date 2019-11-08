// Simon St James (ssjgz) - 2019-11-06
// 
// Solution to: https://www.codechef.com/NOV19A/problems/PHCUL
//
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
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

struct Coord
{
    int64_t x = -1;
    int64_t y = -1;
};

long double findMinDistance(const int64_t x, const int64_t y, const vector<Coord>& phase1Coords, const vector<Coord>& phase2Coords, const vector<Coord>& phase3Coords)
{

    // [The beginning!] (x,y) -> phase1Coords -> phase2Coords -> phase3Coords [the end!]
    //                    └─────┬─────┘└───────┬──────┘└───────┬──────┘
    //                       Phase 1        Phase 2           Phase 3

    // Simple bit of dynamic programming; work backwards from the end to the beginning point (x, y).
    const int numPhases = 3;
    struct CoordAndDistToEnd
    {
        Coord coord;
        long double bestDistToEnd = -1;
    };

    vector<vector<CoordAndDistToEnd>> distToEndForPhaseInfo(numPhases + 1);
    distToEndForPhaseInfo[0].push_back({{x, y}, -1});
    for (int i = 0; i < phase1Coords.size(); i++)
        distToEndForPhaseInfo[1].push_back({phase1Coords[i], -1});
    for (int i = 0; i < phase2Coords.size(); i++)
        distToEndForPhaseInfo[2].push_back({phase2Coords[i], -1});
    for (int i = 0; i < phase3Coords.size(); i++)
        distToEndForPhaseInfo[3].push_back({phase3Coords[i], 0}); // Already at the end, so bestDistToEnd is "0".

    for (int phase = numPhases; phase >=1 ; phase--)
    {
        for (int i = 0; i < distToEndForPhaseInfo[phase - 1].size(); i++)
        {
            const auto phaseCoord = distToEndForPhaseInfo[phase - 1][i].coord;
            auto& currentBestDistToEnd = distToEndForPhaseInfo[phase - 1][i].bestDistToEnd;
            for (int j = 0; j < distToEndForPhaseInfo[phase].size(); j++)
            {
                assert(distToEndForPhaseInfo[phase][j].bestDistToEnd != -1);
                const auto nextPhaseCoord = distToEndForPhaseInfo[phase][j].coord;
                const auto distToNextPhaseCoord = sqrt(static_cast<long double>((phaseCoord.x - nextPhaseCoord.x) * (phaseCoord.x - nextPhaseCoord.x) + (phaseCoord.y - nextPhaseCoord.y) * (phaseCoord.y - nextPhaseCoord.y)));
                const auto distToEndViaNextPhaseCoord = distToNextPhaseCoord + distToEndForPhaseInfo[phase][j].bestDistToEnd ;
                if (currentBestDistToEnd == -1 || distToEndViaNextPhaseCoord < currentBestDistToEnd)
                {
                    currentBestDistToEnd = distToEndViaNextPhaseCoord;
                }

            }
        }
    }

    return distToEndForPhaseInfo[0][0].bestDistToEnd;
}

int main(int argc, char* argv[])
{
    // Very easy - basically Dynamic Programming 101.  No clever  tricks/ observations
    // needed - hopefully the code is self-explanatory!
    //
    // I kind of wish Div 1 had had CAMC added as its replacement scorable problem rather
    // than this - it's much more interesting/ tricky.
    ios::sync_with_stdio(false);

    // We need to output answers with high precision.
    cout << std::setprecision (std::numeric_limits<long double>::digits10 + 1);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const auto x = read<int64_t>();
        const auto y = read<int64_t>();

        const auto N = read<int64_t>();
        const auto M = read<int64_t>();
        const auto K = read<int64_t>();

        auto readCoordVector = [](int numElements)
        {
            vector<Coord> coordVector(numElements);
            for (auto& coord : coordVector)
            {
                coord.x = read<int64_t>();
                coord.y = read<int64_t>();
            }
            return coordVector;

        };

        const vector<Coord> ab = readCoordVector(N);
        const vector<Coord> cd = readCoordVector(M);
        const vector<Coord> ef = readCoordVector(K);
        
        cout << min(
                    // Visit the ab coord before the cd one.
                    findMinDistance(x, y, ab, cd, ef), 
                    // Visit the cd coord before the ab one.
                    findMinDistance(x, y, cd, ab, ef)
                   ) << endl;

    }

    assert(cin);
}
