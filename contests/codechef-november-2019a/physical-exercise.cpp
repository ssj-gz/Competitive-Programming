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

long double findMinDistance(const int64_t x, const int64_t y, const vector<Coord>& phase2Coords, const vector<Coord>& phase3Coords, const vector<Coord>& phase4Coords)
{
    const int numPhases = 3;
    struct CoordAndDistToEnd
    {
        Coord coord;
        long double bestDistToEnd = -1;
    };

    vector<vector<CoordAndDistToEnd>> distToEndForPhaseInfo(numPhases + 2);
    distToEndForPhaseInfo[0].push_back({{x, y}, -1});
    for (int i = 0; i < phase2Coords.size(); i++)
        distToEndForPhaseInfo[1].push_back({phase2Coords[i], -1});
    for (int i = 0; i < phase3Coords.size(); i++)
        distToEndForPhaseInfo[2].push_back({phase3Coords[i], -1});
    for (int i = 0; i < phase4Coords.size(); i++)
        distToEndForPhaseInfo[3].push_back({phase4Coords[i], 0}); // Already at the end, so "0".

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
                const auto distToNextPhase = sqrt(static_cast<long double>((phaseCoord.x - nextPhaseCoord.x) * (phaseCoord.x - nextPhaseCoord.x) + (phaseCoord.y - nextPhaseCoord.y) * (phaseCoord.y - nextPhaseCoord.y)));
                if (currentBestDistToEnd == -1 || currentBestDistToEnd >= distToNextPhase + distToEndForPhaseInfo[phase][j].bestDistToEnd)
                {
                    currentBestDistToEnd = distToNextPhase + distToEndForPhaseInfo[phase][j].bestDistToEnd;
                }

            }
        }
    }

    return distToEndForPhaseInfo[0][0].bestDistToEnd;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);

    cout << std::setprecision (std::numeric_limits<long double>::digits10 + 1);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const auto x = read<int64_t>();
        const auto y = read<int64_t>();

        const auto N = read<int64_t>();
        const auto M = read<int64_t>();
        const auto K = read<int64_t>();

        vector<Coord> a(N);
        for (auto& coord : a)
        {
            coord.x = read<int64_t>();
            coord.y = read<int64_t>();
        }
        vector<Coord> b(M);
        for (auto& coord : b)
        {
            coord.x = read<int64_t>();
            coord.y = read<int64_t>();
        }
        vector<Coord> c(K);
        for (auto& coord : c)
        {
            coord.x = read<int64_t>();
            coord.y = read<int64_t>();
        }

        cout << min(findMinDistance(x, y, a, b, c), findMinDistance(x, y, b, a, c)) << endl;

    }

    assert(cin);
}
