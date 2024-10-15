#include <iostream>
#include <vector>
#include <map>

#include <cassert>

using namespace std;

int main()
{
    string areaLine;
    vector<string> areaRaw;
    while (getline(cin, areaLine))
    {
        areaRaw.push_back(areaLine);
    }
    const int width = areaRaw.front().size();
    const int height = areaRaw.size();

    vector<vector<char>> area(width, vector<char>(height, ' '));
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            area[x][y] = areaRaw[y][x];
        }
    }

    map<vector<vector<char>>, int> lastSeenAfterMinute;
    int minute = 1;
    while (true)
    {
        std::cout << "Beginning minute # " << minute << std::endl;
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                cout << area[x][y];
            }
            std::cout << std::endl;
        }

        auto areaNext = area;
        for (int x = 0; x < width; x++)
        {
            for (int y = 0; y < height; y++)
            {
                int numAdjacentTrees = 0;
                int numAdjacentLumbers = 0;
                int numAdjacentOpens = 0;
                for (int neighbourX = x - 1; neighbourX <= x + 1; neighbourX++)
                {
                    if (neighbourX < 0 || neighbourX >= width)
                        continue;
                    for (int neighbourY = y - 1; neighbourY <= y + 1; neighbourY++)
                    {
                        if (neighbourY < 0 || neighbourY >= height)
                            continue;
                        if (neighbourX == x && neighbourY == y)
                            continue;
                        const auto oldContent = area[neighbourX][neighbourY];
                        if (oldContent == '.')
                            numAdjacentOpens++;
                        else if (oldContent == '#')
                            numAdjacentLumbers++;
                        else if (oldContent == '|')
                            numAdjacentTrees++;
                        else
                            assert(false);
                    }
                }

                const auto oldContent = area[x][y];
                if (oldContent == '.')
                {
                    if (numAdjacentTrees >= 3)
                        areaNext[x][y] = '|';
                }
                else if (oldContent == '#')
                {
                    if (!(numAdjacentLumbers >= 1 && numAdjacentTrees >=1))
                        areaNext[x][y] = '.';
                }
                else if (oldContent == '|')
                {
                    if (numAdjacentLumbers >= 3)
                        areaNext[x][y] = '#';
                }
                else
                    assert(false);
            }
        }
        area = areaNext;

        if (lastSeenAfterMinute.contains(area))
        {
            std::cout << "After minute " << minute << " repeated configuration last seen after minute: " << lastSeenAfterMinute[area] << std::endl;
            constexpr int64_t desiredMinuteEnd = 1'000'000'000LL;
            //constexpr int64_t desiredMinuteEnd = 1'000LL;
            const int64_t cycleLen = minute - lastSeenAfterMinute[area];
            const int64_t desiredIsRepeatOfMinute = desiredMinuteEnd - ((cycleLen * ((desiredMinuteEnd - minute) / cycleLen)) + minute) + lastSeenAfterMinute[area];
            std::cout << " Should be a repeat of: " << desiredIsRepeatOfMinute << " cycleLen: " << cycleLen << std::endl;
            for (const auto& [repeatedArea, minuteLastSeen] : lastSeenAfterMinute)
            {
                if (minuteLastSeen == desiredIsRepeatOfMinute)
                {
                    std::cout << "Area after minute " << desiredMinuteEnd << " should be: " << std::endl;
                    int numTrees = 0;
                    int numLumbers = 0;
                    for (int y = 0; y < height; y++)
                    {
                        for (int x = 0; x < width; x++)
                        {
                            const auto content = repeatedArea[x][y];
                            cout << content;
                            if (content == '#')
                                numLumbers++;
                            else if (content == '|')
                                numTrees++;
                        }
                        std::cout << std::endl;
                    }
                    std::cout << "numLumbers: " << numLumbers << std::endl;
                    std::cout << "numTrees: " << numTrees << std::endl;
                    std::cout << "result: " << (numTrees * numLumbers) << std::endl;

                    return 0;
                }
            }
        }

        lastSeenAfterMinute[area] = minute;
        minute++;
    }

    return 0;
}
