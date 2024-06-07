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
    bool foundRepetition = false;
    //const auto lcmThing = std::lcm(shapes.size(), jetBlastPatterns.size());
    //const auto lcmThing = 280;
    //const auto lcmThing = 35;
    //std::cout << "lcmThing: " << lcmThing << std::endl;
    std::map<int, int> heightAfterNumRocks;
    int potentialRepetitionAfterRocks = -1;
    while (true)
    {
        // Next shape appears.
        const auto& shape = shapes[nextShapeIndex];
        const int shapeHeight = shape.size();
        const int shapeWidth = shape.front().size();
        int shapeLeftX = 2;
        int shapeTopY = playArea.size() + 3 + shape.size() - 1;
        const int initialShapeTopY = shapeTopY;
        //std::cout << "new shape appears" << std::endl;
        //std::cout << "shapeLeftX: " << shapeLeftX << " shapeTopY: " << shapeTopY << " shapeWidth: " << shapeWidth << " shapeHeight: " << shapeHeight << std::endl;

        auto drawPlayArea = [&]()
        {
            //return;
            //std::cout << "drawPlayArea: shapeLeftX: " << shapeLeftX << " shapeTopY: " << shapeTopY << std::endl;
            vector<string> playAreaView(max<int>(shapeTopY + 1, playArea.size() + 1), string(playAreaWidth, '.'));
            for (int y = 0; y < playArea.size(); y++)
            {
                for (int x = 0; x < playAreaWidth; x++)
                {
                    playAreaView[y][x] = playArea[y][x];
                }
            }
            for (int x = 0; x < shapeWidth; x++)
            {
                for (int y = 0; y < shapeHeight; y++)
                {
                    //std::cout << "y: " << y << std::endl;
                    const int xInPlayAreaView = x + shapeLeftX;
                    const int yInPlayAreaView = shapeTopY - y;
                    //std::cout << "yInPlayAreaView: " << y + yInPlayAreaView << std::endl;

                    assert(0 <= yInPlayAreaView);
                    assert(yInPlayAreaView < playAreaView.size());
                    assert(0 <= xInPlayAreaView);
                    assert(xInPlayAreaView < playAreaView.front().size());

                    if (shape[y][x] == '#')
                        playAreaView[yInPlayAreaView][xInPlayAreaView] = '@';
                    //else
                        //playAreaView[yInPlayAreaView][xInPlayAreaView] = '.';

                }
            }
            std::reverse(playAreaView.begin(), playAreaView.end());
            std::cout << "Current: " << std::endl;
            for (const auto& row : playAreaView)
            {
                std::cout << row << std::endl;
            }
        };

        //std::cout << "Shape appears" << std::endl;
        //drawPlayArea();
        while (true)
        {
            const int origShapeLeftX = shapeLeftX;

            //std::cout << "Jet attempts to blow to " << jetBlastPatterns[jetStreamIndex] << std::endl;
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
                        //std::cout << "y: " << y << std::endl;
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
                //std::cout << " stream didn't push us" << std::endl;
                shapeLeftX = origShapeLeftX;
            }
            //drawPlayArea();
            //std::cout << "Attempting to descend" << std::endl;
            const int origShapeTopY = shapeTopY;
            shapeTopY--;
            if (!isShapePosValid())
            {
                //std::cout << " couldn't settle" << std::endl;
                shapeTopY = origShapeTopY;
                // Inscribe in play area.
                //std::cout << "shapeTopY: " << shapeTopY << std::endl;
                for (int x = 0; x < shapeWidth; x++)
                {
                    for (int y = 0; y < shapeHeight; y++)
                    {
                        //std::cout << "y: " << y << std::endl;
                        const int xInPlayArea = x + shapeLeftX;
                        const int yInPlayArea = shapeTopY - y;
                        while (yInPlayArea >= playArea.size())
                            playArea.push_back(string(playAreaWidth, '.'));
                        if (shape[y][x] != '.')
                            playArea[yInPlayArea][xInPlayArea] = '#';

                    }
                }
                //std::cout << "Inscribed shape" << std::endl;
                //drawPlayArea();
                numRocksLanded++;
                heightAfterNumRocks[numRocksLanded] = playArea.size() - 1;
#if 1
                const int distFell = (initialShapeTopY - shapeTopY);
                //std::cout << "Fell: " << distFell << std::endl; 
                if (longestFallCheck < distFell)
                {
                    longestFallCheck = distFell;
                    std::cout << "new longestFallCheck: " << longestFallCheck << std::endl;
                }
                assert(distFell <= longestFall);
#endif
#if 1
                if (potentialRepetitionAfterRocks == -1 && playArea.size() - 1 >= longestFall)
                {
                    std::string topPattern;
                    auto rowIter = playArea.rbegin();
                    for (int i = 0; i < longestFall; i++)
                    {
                        topPattern += *rowIter;
                        rowIter++;
                    }
                    if (lastOccurenceOfTopPattern.contains(topPattern))
                    {
                        const int diff = (numRocksLanded - lastOccurenceOfTopPattern[topPattern]);
                        std::cout << "Woohoo; numRocksLanded: " << numRocksLanded << " last occurence: " << lastOccurenceOfTopPattern[topPattern] << " diff: " << diff << std::endl;
                        //assert(numRocksLanded - lastOccurenceOfTopPattern[topPattern] == 35);
                        potentialRepetitionAfterRocks = std::lcm(shapes.size(), lcm(diff, jetBlastPatterns.size()));
                        //potentialRepetitionAfterRocks = lcm(shapes.size(), lcm(diff, jetBlastPatterns.size()));
                        std::cout << "potentialRepetitionAfterRocks: " << potentialRepetitionAfterRocks << std::endl;
                        foundRepetition =  true;
#if 0
                        std::sting printTopPatterns = topPattern;
                        while (!printTopPatterns.empty())
                        {
                            assert(printTopPatterns.size() >= playAreaWidth);
                            std::cout << printTopPatterns.substr(0, playAreaWidth) << std::endl;
                            printTopPatterns = topPattern.substr(playAreaWidth);
                        }
#endif
                        std::cout << "Play area: " << std::endl;
                        //drawPlayArea();
                        //return 0;
                    }
                    else
                    {
                        assert(!foundRepetition);
                    }
                    lastOccurenceOfTopPattern[topPattern] = numRocksLanded;
                    std::cout << "# lastOccurenceOfTopPattern: " << lastOccurenceOfTopPattern.size() << std::endl;
                }
#endif

#if 1
                if (potentialRepetitionAfterRocks != -1 && ((numRocksLanded % potentialRepetitionAfterRocks) == 0))
                {
                    int multiplier = 1;
                    while (true)
                    {
                        int64_t numRocksAgo = potentialRepetitionAfterRocks * multiplier;
                        if (numRocksAgo >= numRocksLanded)
                            break;
                        int64_t blockSize = heightAfterNumRocks[numRocksLanded] - heightAfterNumRocks[numRocksLanded - numRocksAgo];
                        std::cout << "blockSize: " << blockSize << " numRocksLanded: " << numRocksLanded << " numRocksAgo: " << numRocksAgo << std::endl;

                        assert(playArea.size() >= blockSize);
                        int recent = playArea.size() - 1;
                        int earlier = recent - blockSize;
                        bool match = true;
                        for (int i = 0; i < blockSize; i++)
                        {
                            //std::cout << "recent:  " << playArea[recent] << std::endl;
                            //std::cout << "earlier: " << playArea[earlier] << std::endl;
                            if (playArea[recent] != playArea[earlier])
                            {
                                match = false;
                                std::cout << "mismatch after: " << i << std::endl;
                                break;
                            }
                            recent--;
                            earlier--;

                        }
                        if (match)
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
#endif

                break;
            }
            //std::cout << "Descent succeeded" << std::endl;
            //drawPlayArea();
        }
#if 0
        if (playArea.back() == "#######")
        {
            break;
        }
#endif
        if (numRocksLanded % 100'000 == 0)
            std::cout << "numRocksLanded % 100'000: " << numRocksLanded << std::endl;
        //if (numRocksLanded > 1'000'000)
            //break;
#if 0
        if (numRocksLanded == 2022)
        {
            std::cout << "answer: " << playArea.size() - 1 /* Ignore "------" */ << std::endl;
            break;
        }
#endif


        nextShapeIndex = (nextShapeIndex + 1) % shapes.size();
        if (nextShapeIndex == 0)
        {
            

        }
    }

}
