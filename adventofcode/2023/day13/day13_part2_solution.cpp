#include <iostream>
#include <vector>
#include <algorithm>

#include <cassert>

using namespace std;

vector<int> numColumnsBeforeHorizReflections(const vector<string>& map)
{
    const int width = map.front().size();
    const int height = map.size();

    vector<int> result;
    for (int col = 0; col < width - 1; col++)
    {
        int leftCol = col; 
        int rightCol = col + 1; 
        //std::cout << "Checking for reflection between columns: " << leftCol << " and " << rightCol << std::endl;
        bool isReflection = true;
        while (leftCol >= 0 && rightCol < width)
        {
            for (int row = 0; row < height; row++)
            {
                if (map[row][leftCol] != map[row][rightCol])
                {
                    //std::cout << " nope: row: " << row << " leftCol: " << leftCol << " rightCol: " << rightCol << " are different" << std::endl;
                    isReflection = false;
                }

            }
            leftCol--;
            rightCol++;
        }
        if (isReflection)
        {
            result.push_back(col + 1);
        }
    }
    return result;
}

vector<int> numRowsBeforeVerzReflections(const vector<string>& map)
{
    const int width = map.front().size();
    const int height = map.size();

    vector<int> result;
    for (int row = 0; row < height - 1; row++)
    {
        int topRow = row; 
        int bottomRow = row + 1; 
        //std::cout << "Checking for reflection between rows: " << topRow << " and " << bottomRow << std::endl;
        bool isReflection = true;
        while (topRow >= 0 && bottomRow < height)
        {
            for (int col = 0; col < width; col++)
            {
                if (map[topRow][col] != map[bottomRow][col])
                {
                    //std::cout << " nope: col: " << col << " topRow: " << topRow << " bottomRow: " << bottomRow << " are different" << std::endl;
                    isReflection = false;
                }

            }
            topRow--;
            bottomRow++;
        }
        if (isReflection)
        {
            result.push_back(row + 1);
        }
    }
    return result;
}
int main()
{
    auto readPattern = []()
    {
        vector<string> map;
        string mapRow;
        while (getline(cin, mapRow))
        {
            if (mapRow.empty())
                break;
            map.push_back(mapRow);
        }
        return map;
    };


    int64_t result = 0;
    while(true)
    {
        auto map = readPattern();
        if (map.empty())
            break;

    std::cout << "pattern: " << std::endl;
    for (const auto& x : map) cout << x << endl;

        const int width = map.front().size();
        const int height = map.size();

        {
            const auto initialHorizReflectionInfo = numColumnsBeforeHorizReflections(map);
            std::cout << "initialHorizReflectionInfo: " << std::endl;
            for (const auto x : initialHorizReflectionInfo) std::cout << " " << x;
            std::cout << std::endl;
            bool foundDifferent = false;
            for (int row = 0; row < height; row++)
            {
                for (int col = 0; col < width; col++)
                {
                    const int oldContent = map[row][col];
                    if (oldContent == '.')
                        map[row][col] = '#';
                    else
                        map[row][col] = '.';

                    const auto reflectionInfoAfterSmudge = numColumnsBeforeHorizReflections(map);
                    std::cout << " After smudging row: " << row << " col: " << col << " reflection info: " << std::endl;
                    for (const auto x : reflectionInfoAfterSmudge) std::cout << " " << x;
                    std::cout << std::endl;
                    bool hasNewReflection = false;
                    for (const auto reflection : reflectionInfoAfterSmudge)
                    {
                        if (!foundDifferent && std::find(initialHorizReflectionInfo.begin(), initialHorizReflectionInfo.end(), reflection) == initialHorizReflectionInfo.end())
                        {
                            assert(!hasNewReflection);
                            hasNewReflection = true;
                            std::cout << " adding numCols = " << reflection << " to result" << std::endl;
                            result += reflection;
                            foundDifferent = true;
                        }
                    }

                    map[row][col] = oldContent;
                }
            }
            //assert(foundDifferent);
        }
        {
            const auto initialVertReflectionInfo = numRowsBeforeVerzReflections(map);
            std::cout << " initialVertReflectionInfo: " << std::endl;
            for (const auto x : initialVertReflectionInfo) std::cout << " " << x;
            std::cout << std::endl;

            bool foundDifferent = false;
            for (int row = 0; row < height; row++)
            {
                for (int col = 0; col < width; col++)
                {
                    const int oldContent = map[row][col];
                    if (oldContent == '.')
                        map[row][col] = '#';
                    else
                        map[row][col] = '.';

                    const auto reflectionInfoAfterSmudge = numRowsBeforeVerzReflections(map);
                    std::cout << " After smudging row: " << row << " col: " << col << " reflection info: " << std::endl;
                    for (const auto x : reflectionInfoAfterSmudge) std::cout << " " << x;
                    std::cout << std::endl;
                    bool hasNewReflection = false;
                    for (const auto reflection : reflectionInfoAfterSmudge)
                    {
                        if (!foundDifferent && std::find(initialVertReflectionInfo.begin(), initialVertReflectionInfo.end(), reflection) == initialVertReflectionInfo.end())
                        {
                            assert(!hasNewReflection);
                            hasNewReflection = true;
                            foundDifferent = true;
                            std::cout << " adding numRows = " << reflection << " * 100 to result" << std::endl;
                            result += 100 * reflection;
                        }
                    }

                    map[row][col] = oldContent;
                }
            }
            //assert(foundDifferent);
        }

    }
    std::cout << "result: " << result << std::endl;
}
