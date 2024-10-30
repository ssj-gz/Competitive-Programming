#include <iostream>
#include <vector>
#include <set>

#include <cassert>

using namespace std;

int main()
{
    vector<int> blockInBankCount;
    int blockCount;
    while (cin >> blockCount)
        blockInBankCount.push_back(blockCount);

    const int bankCount = blockInBankCount.size();
    set<vector<int>> seenConfigurations = { blockInBankCount };
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
        if (seenConfigurations.contains(blockInBankCount))
            break;
        seenConfigurations.insert(blockInBankCount);
    }
    std::cout << "cycleCount: " << cycleCount << std::endl;
    return 0;
}
