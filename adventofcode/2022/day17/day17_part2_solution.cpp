#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <numeric>
#include <cassert>

#define _GLIBCXX_DEBUG       // Iterator safety; out-of-bounds access for Containers, etc.
#pragma GCC optimize "trapv" // abort() on (signed) integer overflow.

using namespace std;

int main()
{
    string jetBlastPatterns;
    cin >> jetBlastPatterns;

    std::cout << "jetBlastPatterns: " << jetBlastPatterns << std::endl;
    std::cout << "jetBlastPatterns.size(): " << jetBlastPatterns.size() << std::endl;

    const vector<vector<string>> shapes = {
        { "####" },

        { ".#.",
          "###",
          ".#." },

        { "..#",
          "..#",
          "###" },

        { "#",
          "#",
          "#",
          "#" },

        { "##",
          "##" }
    };

    constexpr int playAreaWidth = 7;
    constexpr int playAreaMinX = 0;
    constexpr int playAreaMaxX = playAreaMinX + playAreaWidth - 1;
    // The "back" of the playArea is the "topmost" row;
    // the front, the bottommost.
    vector<string> playArea = { string(playAreaWidth, '-') };
    int jetStreamIndex = 0;
    int nextShapeIndex = 0;

    int numRocksLanded = 0;
    int longestFallCheck = -1;
    constexpr int longestFall = 42; // Empiricially derived.
    map<string, int> lastOccurenceOfTopPattern;
    std::map<int, int> heightAfterNumRocks;
    int potentialCycleAfterRocks = -1;
    while (true)
    {
        // Next shape appears.
        const auto& shape = shapes[nextShapeIndex];
        const int shapeHeight = shape.size();
        const int shapeWidth = shape.front().size();
        int shapeLeftX = 2;
        int shapeTopY = playArea.size() + 3 + shape.size() - 1;
        const int initialShapeTopY = shapeTopY;

        while (true)
        {
            const int origShapeLeftX = shapeLeftX;

            if (jetBlastPatterns[jetStreamIndex] == '<')
            {
                shapeLeftX--;
            }
            else
            {
                shapeLeftX++;
            }
            jetStreamIndex = (jetStreamIndex + 1) % jetBlastPatterns.size();

            auto isShapePosValid = [&]()
            {
                bool isValid = true;
                for (int x = 0; x < shapeWidth; x++)
                {
                    for (int y = 0; y < shapeHeight; y++)
                    {
                        const int xInPlayAreaView = x + shapeLeftX;
                        const int yInPlayAreaView = shapeTopY - y;
                        if (xInPlayAreaView < 0 || xInPlayAreaView >= playAreaWidth)
                        {
                            return false;
                        }
                        if (yInPlayAreaView < 0)
                        {
                            return false;
                        }
                        if (yInPlayAreaView >= playArea.size())
                            continue; // Fine - shape is too high up to touch anything.
                        if (shape[y][x] != '.' && playArea[yInPlayAreaView][xInPlayAreaView] != '.')
                        {
                            return false;
                        }
                    }
                }
                return true;
            };
            if (!isShapePosValid())
            {
                shapeLeftX = origShapeLeftX;
            }
            const int origShapeTopY = shapeTopY;
            shapeTopY--;
            if (!isShapePosValid())
            {
                shapeTopY = origShapeTopY;
                // Inscribe in play area.
                for (int x = 0; x < shapeWidth; x++)
                {
                    for (int y = 0; y < shapeHeight; y++)
                    {
                        const int xInPlayArea = x + shapeLeftX;
                        const int yInPlayArea = shapeTopY - y;
                        while (yInPlayArea >= playArea.size())
                            playArea.push_back(string(playAreaWidth, '.'));
                        if (shape[y][x] != '.')
                            playArea[yInPlayArea][xInPlayArea] = '#';

                    }
                }
                numRocksLanded++;
                heightAfterNumRocks[numRocksLanded] = playArea.size() - 1;

                {
                    // This was the code used to create longestFall; we now use it
                    // to check that longestFall is correct.
                    const int distFell = (initialShapeTopY - shapeTopY);
                    if (longestFallCheck < distFell)
                    {
                        longestFallCheck = distFell;
                        std::cout << "new longestFallCheck: " << longestFallCheck << std::endl;
                    }
                    assert(distFell <= longestFall);
                }

                if (potentialCycleAfterRocks == -1 && playArea.size() - 1 >= longestFall)
                {
                    // Calculate a string representing the top longestFall rows of the current play area,
                    // and how many rocks ago we last saw it (if at all).
                    std::string topPattern;
                    auto rowIter = playArea.rbegin();
                    for (int i = 0; i < longestFall; i++)
                    {
                        topPattern += *rowIter;
                        rowIter++;
                    }
                    if (lastOccurenceOfTopPattern.contains(topPattern))
                    {
                        const int numRocksLandedSinceRepeat = (numRocksLanded - lastOccurenceOfTopPattern[topPattern]);
                        // For a true cycle, we require all of:
                        //    * # shapes
                        //    * # jet blast patterns
                        //    * this the number of rocks landed since we last saw this topPattern
                        // all to coincide i.e. find the lcm of these three values.
                        potentialCycleAfterRocks = std::lcm(shapes.size(), lcm(numRocksLandedSinceRepeat, jetBlastPatterns.size()));
                    }
                    lastOccurenceOfTopPattern[topPattern] = numRocksLanded;
                }

                if (potentialCycleAfterRocks != -1 && ((numRocksLanded % potentialCycleAfterRocks) == 0))
                {
                    // Check for a cycle ending at the top of the current/ playArea, checking back <some multiple of>
                    // potentialCycleAfterRocks rocks ago.
                    int multiplier = 1;
                    while (true)
                    {
                        int64_t numRocksAgo = potentialCycleAfterRocks * multiplier;
                        if (numRocksAgo >= numRocksLanded)
                            break;
                        const int64_t numRowsToCheck = heightAfterNumRocks[numRocksLanded] - heightAfterNumRocks[numRocksLanded - numRocksAgo];

                        assert(playArea.size() >= numRowsToCheck);
                        int recent = playArea.size() - 1;
                        int earlier = recent - numRowsToCheck;
                        bool cycleDetected = true;
                        for (int i = 0; i < numRowsToCheck; i++)
                        {
                            if (playArea[recent] != playArea[earlier])
                            {
                                cycleDetected = false;
                                break;
                            }
                            recent--;
                            earlier--;

                        }
                        if (cycleDetected)
                        {
                            // We've found our cycle! Use this to quickly figure out the result of applying the remaining
                            // required rock landings.
                            const int64_t cycleLength = numRocksAgo;
                            const int64_t heightGainPerCycle = (heightAfterNumRocks[numRocksLanded] - heightAfterNumRocks[numRocksLanded - numRocksAgo]);

                            int64_t answer = heightAfterNumRocks[numRocksLanded];
                            const int64_t remainingRockLandings = 1'000'000'000'000 - numRocksLanded;
                            const int64_t remainingRockLandingsNumCycles = remainingRockLandings / cycleLength;
                            answer += remainingRockLandingsNumCycles * heightGainPerCycle;
                            const int64_t rockLandingsAfterFinalCycle = remainingRockLandings % cycleLength;
                            const int64_t beginOfRepetitionRockLanding = numRocksLanded - numRocksAgo;
                            answer += heightAfterNumRocks[beginOfRepetitionRockLanding + rockLandingsAfterFinalCycle] - heightAfterNumRocks[beginOfRepetitionRockLanding];
                            std::cout << "answer: " << answer << std::endl;
                            return 0;

                        }
                        multiplier++;
                    }


                }

                break;
            }
        }

        nextShapeIndex = (nextShapeIndex + 1) % shapes.size();
    }

}
