// Simon St James (ssjgz) - 2019-11-06
// 
// Solution to: https://www.codechef.com/NOV19A/problems/PHCUL
//
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <cmath>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

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
        long double distToEnd = -1;
    };

    vector<vector<CoordAndDistToEnd>> dp(numPhases + 2);
    dp[0].push_back({{x, y}, -1});
    for (int i = 0; i < phase2Coords.size(); i++)
        dp[1].push_back({phase2Coords[i], -1});
    for (int i = 0; i < phase3Coords.size(); i++)
        dp[2].push_back({phase3Coords[i], -1});
    for (int i = 0; i < phase4Coords.size(); i++)
        dp[3].push_back({phase4Coords[i], 0});

    for (int phase = numPhases; phase >=1 ; phase--)
    {
        for (int i = 0; i < dp[phase - 1].size(); i++)
        {
            const auto phaseCoord = dp[phase - 1][i].coord;
            auto& currentBestDistToEnd = dp[phase - 1][i].distToEnd;
            for (int j = 0; j < dp[phase].size(); j++)
            {
                assert(dp[phase][j].distToEnd != -1);
                const auto nextPhaseCoord = dp[phase][j].coord;
                const auto distToNextPhase = sqrt(static_cast<long double>((phaseCoord.x - nextPhaseCoord.x) * (phaseCoord.x - nextPhaseCoord.x) + (phaseCoord.y - nextPhaseCoord.y) * (phaseCoord.y - nextPhaseCoord.y)));
                cout << "phase: " << phase << " i: " << i << " j: " << j << " dp[phase][j].distToEnd: " << dp[phase][j].distToEnd << " distToNextPhase: " << distToNextPhase << endl;
                if (currentBestDistToEnd == -1 || currentBestDistToEnd >= distToNextPhase + dp[phase][j].distToEnd)
                {
                    currentBestDistToEnd = distToNextPhase + dp[phase][j].distToEnd;
                }

            }
        }
    }

    return dp[0][0].distToEnd;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        // TODO - generate randomised test.
        //const int T = rand() % 100 + 1;
        const int T = 1;
        cout << T << endl;

        for (int t = 0; t < T; t++)
        {
        }

        return 0;
    }
    
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
