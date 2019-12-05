// Simon St James (ssjgz) - 2019-12-05
// 
// Solution to: https://www.codechef.com/ZCOPRAC/problems/ZCO13001/
//
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

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

int64_t solveBruteForce(const vector<int>& teamStrengths)
{
    int64_t totalRevenue = 0;
    const int N = teamStrengths.size();

    for (int team1Index = 0; team1Index < N; team1Index++)
    {
        for (int team2Index = team1Index + 1; team2Index < N; team2Index++)
        {
            totalRevenue += abs(teamStrengths[team1Index] - teamStrengths[team2Index]);
        }
    }
    
    return totalRevenue;
}

#if 1
int64_t solveOptimised(const vector<int>& teamStrengthsOriginal)
{
    vector<int> teamStrengthsSorted(teamStrengthsOriginal);
    sort(teamStrengthsSorted.begin(), teamStrengthsSorted.end());

    int64_t totalRevenue = 0;
    const int N = teamStrengthsSorted.size();

    // We can generate all pairs of teams by assuming that the
    // first team in a pair always has strength <= the second team
    // in a pair, and considering all first teams.
    //
    // So: if our first team has index in teamStrengthsSorted firstTeamIndex, and teamStrengthsSorted
    // is sorted, then who do we play against? We play against precisely
    // the set of (N - firstTeamIndex - 1)  teams whose indices in teamStrengthsSorted are:
    //
    //  firstTeamIndex + 1
    //  firstTeamIndex + 2
    //     ....
    //  N - 2
    //  N - 1
    //
    // What revenue do these pairings generate? Precisely:
    //
    //  abs(teamStrengthsSorted[firstTeamIndex + 1] - teamStrengthsSorted[firstTeamIndex]) +
    //  abs(teamStrengthsSorted[firstTeamIndex + 2] - teamStrengthsSorted[firstTeamIndex]) +
    //      ...
    //  abs(teamStrengthsSorted[N - 1] - teamStrengthsSorted[firstTeamIndex]) +
    //  abs(teamStrengthsSorted[N - 2] - teamStrengthsSorted[firstTeamIndex])
    //
    // Since teamStrengthsSorted is sorted, we can ditch the "abs" - each term will be >= 0:
    //
    //  teamStrengthsSorted[firstTeamIndex + 1] - teamStrengthsSorted[firstTeamIndex] +
    //  teamStrengthsSorted[firstTeamIndex + 2] - teamStrengthsSorted[firstTeamIndex] +
    //  ...
    //  teamStrengthsSorted[N - 1] - teamStrengthsSorted[firstTeamIndex] +
    //  teamStrengthsSorted[N - 2] - teamStrengthsSorted[firstTeamIndex]
    //
    //  But re-arranging the terms, this is just:
    //
    //  teamStrengthsSorted[firstTeamIndex + 1] + teamStrengthsSorted[firstTeamIndex + 2] + ... +
    //  teamStrengthsSorted[N - 1] + teamStrengthsSorted[N - 2] - (N - firstTeamIndex - 1) * teamStrengthsSorted[firstTeamIndex].
    // 
    // i.e. the sum of all teams "to the right of " firstTeamIndex, minus (N - firstTeamIndex - 1) * teamStrengthsSorted[firstTeamIndex].
    //
    // Adding up the revenues for all pairings where the first team has index firstTeamIndex over all firstTeamIndex = 0 ... N - 1
    // gives us the result.
    int64_t sumOfAllTeamsToTheRightOf = std::accumulate(teamStrengthsSorted.begin(), teamStrengthsSorted.end(), 0);
    for (int firstTeamIndex = 0; firstTeamIndex < N; firstTeamIndex++)
    {
        sumOfAllTeamsToTheRightOf -= teamStrengthsSorted[firstTeamIndex];
        totalRevenue += sumOfAllTeamsToTheRightOf - (N - firstTeamIndex - 1) * teamStrengthsSorted[firstTeamIndex];
    }



    return totalRevenue;
}
#endif


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

        const int N = rand() % 100 + 1;
        const int maxA = rand() % 1000 + 1;

        cout << N << endl;
        for (int i = 0; i < N; i++)
        {
            cout << (1 + rand() % maxA);
            if (i != N)
                cout << " ";
        }
        cout << endl;

        return 0;
    }

    const int N = read<int>();

    vector<int> teamStrengths(N);
    for (auto& strength : teamStrengths)
        strength = read<int>();


#ifdef BRUTE_FORCE
#if 1
    const auto solutionBruteForce = solveBruteForce(teamStrengths);
    cout << "solutionBruteForce: " << solutionBruteForce << endl;
#endif
#if 1
    const auto solutionOptimised = solveOptimised(teamStrengths);
    cout << "solutionOptimised:  " << solutionOptimised << endl;

    assert(solutionOptimised == solutionBruteForce);
#endif
#else
    const auto solutionOptimised = solveOptimised(teamStrengths);
    cout << solutionOptimised << endl;
#endif

    assert(cin);
}
