#include <iostream>
#include <vector>
#include <map>

#include <cassert>

using namespace std;

int main()
{
    vector<int> blockInBankCount;
    int blockCount;
    while (cin >> blockCount)
        blockInBankCount.push_back(blockCount);

    const int bankCount = blockInBankCount.size();
    map<vector<int>, int> configSeenAtCycleCount = { {blockInBankCount, 0} };
    int cycleCount = 0;
    while (true)
    {
        std::cout << "Beginning cycle: ";
        for (const auto& x : blockInBankCount) std::cout << " " << x;
        std::cout << std::endl;
        int chosenBankIndex = -1;
        int mostBlocks = -1;
        for (int bankIndex = 0; bankIndex < bankCount; bankIndex++)
        {
            if (blockInBankCount[bankIndex] > mostBlocks)
            {
                chosenBankIndex = bankIndex;
                mostBlocks = blockInBankCount[bankIndex];
            }
        }
        int blocksInHand = blockInBankCount[chosenBankIndex];
        blockInBankCount[chosenBankIndex] = 0;
        int bankIndex = chosenBankIndex;
        while (blocksInHand > 0)
        {
            bankIndex = (bankIndex + 1) % bankCount;
            blockInBankCount[bankIndex]++;
            blocksInHand--;
        }

        cycleCount++;
        if (configSeenAtCycleCount.contains(blockInBankCount))
        {
            const int cycleLength = cycleCount - configSeenAtCycleCount[blockInBankCount];
            std::cout << "cycleLength: " << cycleLength << std::endl;
            break;
        }
        configSeenAtCycleCount[blockInBankCount] = cycleCount;
    }
    return 0;
}
