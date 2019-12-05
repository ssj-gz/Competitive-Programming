// Simon St James (ssjgz) - 2019-12-05
// 
// Solution to: https://www.codechef.com/ZCOPRAC/problems/ZCO13001/
//
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

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

int64_t calcAdvertisingRevenue(const vector<int>& teamStrengthsOriginal)
{
    vector<int> teamStrengthsSorted(teamStrengthsOriginal);
    sort(teamStrengthsSorted.begin(), teamStrengthsSorted.end());

    int64_t totalRevenue = 0;
    const int N = teamStrengthsSorted.size();

    // We can generate all pairs of teams by assuming that the
    // first team in a pair always has strength <= the second team
    // in a pair, and considering all first teams.
    //
    // So: if our first team has index in teamStrengthsSorted "firstTeamIndex", and teamStrengthsSorted
    // is sorted, then who does our first team play against? We play against precisely
    // the set of (N - firstTeamIndex - 1) teams whose indices in teamStrengthsSorted are:
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

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);

    const int N = read<int>();

    vector<int> teamStrengths(N);
    for (auto& strength : teamStrengths)
        strength = read<int>();

    cout << calcAdvertisingRevenue(teamStrengths) << endl;

    assert(cin);
}
