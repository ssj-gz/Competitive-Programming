#include <iostream>
#include <vector>
#include <set>
#include <limits>
#include <cassert>

#include <sys/time.h>

using namespace std;

struct Missile
{
    Missile(int time, int frequency)
        : time(time), frequency(frequency)
    {
    }
    Missile() = default;
    int time;
    int frequency;
    bool operator<(const Missile& other) const
    {
        if (time != other.time)
            return time < other.time;
        return frequency < other.frequency;
    }

};

void solution(int currentMissileNumber, int numHitByCurrentMissile, int currentTime, int currentFrequency, set<Missile>& remainingMissiles, int& minMissiles)
{
    if (currentMissileNumber >= minMissiles)
    {
        return;
    }
    //cout << "currentMissileNumber: " << currentMissileNumber << " currentTime: " << currentTime  << " currentFrequency: " << currentFrequency << " # remainingMissiles: " << remainingMissiles.size() << endl;
    if (remainingMissiles.empty())
    {
        if (currentMissileNumber < minMissiles)
        {
            static int numImprovedresults = 0;
            cout << "New best solution: " << currentMissileNumber << endl;
            numImprovedresults++;
            assert(numImprovedresults < 2);
        }
        minMissiles = std::min(minMissiles, currentMissileNumber);
    }
    for (const Missile& targetMissile : remainingMissiles)
    {
        if (targetMissile.time < currentTime)
            continue;
        const auto frequencyDiff = abs(currentFrequency - targetMissile.frequency);
        if ((targetMissile.time - currentTime >= frequencyDiff) || currentFrequency == -1)
        {
            remainingMissiles.erase(targetMissile);
            solution(currentMissileNumber, numHitByCurrentMissile + 1, targetMissile.time, targetMissile.frequency, remainingMissiles, minMissiles);
            remainingMissiles.insert(targetMissile);
            if (currentMissileNumber == -1)
            {
                // WLOG, missile should pick the earliest target as its first target.
                break;
            }
        }
    }
    if (numHitByCurrentMissile != 0)
        solution(currentMissileNumber + 1, 0, 0, -1, remainingMissiles, minMissiles);
}

int solutionBruteForce(const vector<Missile>& missiles)
{
    int minMissiles = std::numeric_limits<int>::max();
    set<Missile> remainingMissiles(missiles.begin(), missiles.end());
    solution(1, 0, 0, -1, remainingMissiles, minMissiles);
    return minMissiles;
}

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int numMissiles = rand() % 14 + 1;
        const int maxTime = rand() % 300 + 1;
        const int maxFrequency = rand() % 300 + 1;

        cout << numMissiles << endl;
        for (int i = 0; i < numMissiles; i++)
        {
            cout << (rand() % maxTime) << " " << (rand() % maxFrequency) << endl;
        }


        return 0;
    }
    int N;
    cin >> N;

    vector<Missile> missiles(N);
    for (int i = 0; i < N; i++)
    {
        cin >> missiles[i].time;
        cin >> missiles[i].frequency;
    }
    const auto bruteForceResult = solutionBruteForce(missiles);
    cout << "bruteForceResult: " << bruteForceResult << endl;
}
