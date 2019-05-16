#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

const int64_t maxHeight = 1000;

vector<int64_t> maxGainForP1WithHeightLookup(maxHeight + 1, -1);

void findPartitions(int n, int maxPartitionSize, vector<int>& currentPartition, vector<vector<int>>& partitions)
{
    if (n == 0)
        partitions.push_back(currentPartition);
    if (currentPartition.size() > 5)
        return;
    for (int partitionSize = 1; partitionSize <= min(maxPartitionSize, n); partitionSize++)
    {
        currentPartition.push_back(partitionSize);
        findPartitions(n - partitionSize, partitionSize, currentPartition, partitions);
        currentPartition.pop_back();
    }

}

vector<vector<int>> findPartitions(int n)
{
    vector<vector<int>> partitions;
    vector<int> currentPartition;
    findPartitions(n, n, currentPartition, partitions);
    return partitions;
}

int64_t findMaxGainForP1(int height)
{
    if (maxGainForP1WithHeightLookup[height] != -1)
        return maxGainForP1WithHeightLookup[height];
    if (height == 1)
        return 0;
    const auto partitions = findPartitions(height);

    auto maxGain = std::numeric_limits<int64_t>::min();
    vector<int> bestPartition;
    int bestP2TowerHeight = -1;
    for (const auto towerHeightPartition : partitions)
    {
        if (towerHeightPartition.size() == 1)
            continue;
        auto maxPlayer2Gain = std::numeric_limits<int64_t>::min();
        int p2TowerHeight = -1;
        for (int i = 0; i < towerHeightPartition.size(); i++)
        {
            const int player2ChosenHeight = towerHeightPartition[i];
            assert(player2ChosenHeight < height);
            //cout << "height: " << height << " player2ChosenHeight: " << player2ChosenHeight << endl;
            const int64_t gainForPlayer2 = (i + 1) * (i + 1) - findMaxGainForP1(player2ChosenHeight);
            if (gainForPlayer2 > maxPlayer2Gain)
            {
                maxPlayer2Gain = gainForPlayer2;
                p2TowerHeight = i;
            }
        }
        if (-maxPlayer2Gain >= maxGain)
        {
            maxGain = -maxPlayer2Gain;
            bestPartition = towerHeightPartition;
            bestP2TowerHeight = p2TowerHeight;
        }
    }

    cout << "height: " << height << " maxGain: " << maxGain << endl;
    cout << "Chose partition: " << endl;
    for (const auto x : bestPartition)
    {
        cout << x << " ";
    }
    cout << endl;
    cout << "Chose tower index: " << bestP2TowerHeight << " height: " << bestPartition[bestP2TowerHeight] << endl;
    maxGainForP1WithHeightLookup[height] = maxGain;

    if (height > 0 && maxGainForP1WithHeightLookup[height - 1] != maxGainForP1WithHeightLookup[height])
    {
        cout << "max gain changed at height: " << height << endl;
    }

    return maxGain;
}

int main()
{
    if (false)
    {
        int n = 5;
        cout << "findPartitions " << n << endl;
        const auto partitions = findPartitions(n);
        for (const auto& partition : partitions)
        {
            for (const auto x : partition)
            {
                cout << x << " ";
            }
            cout << endl;
        }
        return 0;

    }
    int T;
    cin >> T;

    int lastMaxGain = -999;

    for (int t = 0; t < T; t++)
    {
        int n;
        cin >> n;

        const auto maxGain = -findMaxGainForP1(n);
        cout << "t: " << t << " maxGain: " << maxGain << endl;

    }
}

