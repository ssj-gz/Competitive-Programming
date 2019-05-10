#include <iostream>
#include <vector>
#include <set>
#include <limits>

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
    cout << "currentMissileNumber: " << currentMissileNumber << " currentTime: " << currentTime  << " currentFrequency: " << currentFrequency << " # remainingMissiles: " << remainingMissiles.size() << endl;
    if (remainingMissiles.empty())
    {
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
        }
        if (numHitByCurrentMissile != 0)
            solution(currentMissileNumber + 1, 0, 0, -1, remainingMissiles, minMissiles);
    }
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
