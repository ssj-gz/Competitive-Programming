#include <iostream>
#include <vector>
#include <map>

#include <cassert>

using namespace std;

int main()
{
    vector<string> map;
    string mapRow;
    while (getline(cin, mapRow))
    {
        map.push_back(mapRow);
    }
    const int height = map.size();
    const int width = map.front().size();

    std::cout << "initialMap: " << std::endl;
    for (const auto& row : map) std::cout << row << std::endl;

    constexpr int64_t desiredCycleNum = 1'000'000'000;
    std::cout << "desiredCycleNum: " << desiredCycleNum << std::endl;
    //constexpr int64_t desiredCycleNum = 10'001;
    std::map<vector<string>, int> seenMapAtCycle;
    int cycleNum = 1;
    //vector<string> bruteForceMap;
    while (true)
    {
        // Tilt North.
        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                if (map[row][col] == 'O') {
                    int newRow = row;
                    for (int newCandidateRow = row - 1; newCandidateRow >= 0; newCandidateRow--) {
                        if (map[newCandidateRow][col] == '.')
                            newRow = newCandidateRow;
                        else
                            break;
                    }
                    map[row][col] = '.';
                    map[newRow][col] = 'O';
                }
            }
        }
        // Tilt West.
        for (int row = 0; row < height; row++) {
            for (int col = 0; col < width; col++) {
                if (map[row][col] == 'O') {
                    int newCol = col;
                    for (int newCandidateCol = col - 1; newCandidateCol >= 0; newCandidateCol--) {
                        if (map[row][newCandidateCol] == '.')
                            newCol = newCandidateCol;
                        else
                            break;
                    }
                    map[row][col] = '.';
                    map[row][newCol] = 'O';
                }
            }
        }
        // Tilt South.
        for (int row = height - 1; row >= 0; row--) {
            for (int col = 0; col < width; col++) {
                if (map[row][col] == 'O') {
                    int newRow = row;
                    for (int newCandidateRow = row + 1; newCandidateRow < height; newCandidateRow++) {
                        if (map[newCandidateRow][col] == '.')
                            newRow = newCandidateRow;
                        else
                            break;
                    }
                    map[row][col] = '.';
                    map[newRow][col] = 'O';
                }
            }
        }
        // Tilt East.
        for (int row = 0; row < height; row++) {
            for (int col = width - 1; col >= 0; col--) {
                if (map[row][col] == 'O') {
                    int newCol = col;
                    for (int newCandidateCol = col + 1; newCandidateCol < width; newCandidateCol++) {
                        if (map[row][newCandidateCol] == '.')
                            newCol = newCandidateCol;
                        else
                            break;
                    }
                    map[row][col] = '.';
                    map[row][newCol] = 'O';
                }
            }
        }
        //std::cout << "After cycle: " << (cycleNum + 1) << std::endl;
        //for (const auto& row : map) std::cout << row << std::endl;


        if (seenMapAtCycle.contains(map))
        {
            //if (bruteForceMap.empty())
            {
                std::cout << "repetition at cycleNum: " << cycleNum << " - this map was last encountered at cycleNum: " << seenMapAtCycle[map] << std::endl;
                const int64_t cycleBegin = seenMapAtCycle[map];
                const int64_t cycleLen = cycleNum - seenMapAtCycle[map];
                const int64_t lastCycleResetBeforeDesired = cycleNum + ((desiredCycleNum - cycleNum) / cycleLen) * cycleLen;
                std::cout << "lastCycleResetBeforeDesired: " << lastCycleResetBeforeDesired << std::endl;
                const int64_t remainingCycles = desiredCycleNum - lastCycleResetBeforeDesired;
                const int64_t cycleNumDesiredIsCopyOf = remainingCycles + cycleBegin;
                cout << " expecting our desired map to be a copy of the one at cycle: " << cycleNumDesiredIsCopyOf << std::endl;
                vector<string> desiredMap;
                for (const auto& [seenMap, seenAtCycleNum] : seenMapAtCycle)
                {
                    if (seenAtCycleNum == cycleNumDesiredIsCopyOf)
                    {
                        desiredMap = seenMap;
                    }
                }
                int64_t totalLoad = 0;
                for (int row = 0; row < height; row++)
                {
                    for (int col = 0; col < width; col++)
                    {
                        if (desiredMap[row][col] == 'O')
                        {
                            totalLoad += (height - row);
                        }
                    }
                }
                std::cout << "totalLoad: " << totalLoad << std::endl;


                break;
            }
        }
        //if (!seenMapAtCycle.contains(map))
        seenMapAtCycle[map] = cycleNum;

        //if (cycleNum == desiredCycleNum)
        //{
        //    std::cout << "This is the desired one.  It was seen at cycle#: " << seenMapAtCycle[map] << std::endl;
        //    assert(bruteForceMap == map);
        //    break;
        //}
        cycleNum++;
    }


}
