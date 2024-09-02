#include <iostream>
#include <vector>

using namespace std;

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
        const auto map = readPattern();
        if (map.empty())
            break;

        const int width = map.front().size();
        const int height = map.size();


        for (int col = 0; col < width - 1; col++)
        {
            int leftCol = col; 
            int rightCol = col + 1; 
            std::cout << "Checking for reflection between columns: " << leftCol << " and " << rightCol << std::endl;
            bool isReflection = true;
            while (leftCol >= 0 && rightCol < width)
            {
                for (int row = 0; row < height; row++)
                {
                    if (map[row][leftCol] != map[row][rightCol])
                    {
                        std::cout << " nope: row: " << row << " leftCol: " << leftCol << " rightCol: " << rightCol << " are different" << std::endl;
                        isReflection = false;
                    }

                }
                leftCol--;
                rightCol++;
            }
            if (isReflection)
            {
                result += col + 1;
            }
        }
        for (int row = 0; row < height - 1; row++)
        {
            int topRow = row; 
            int bottomRow = row + 1; 
            std::cout << "Checking for reflection between rows: " << topRow << " and " << bottomRow << std::endl;
            bool isReflection = true;
            while (topRow >= 0 && bottomRow < height)
            {
                for (int col = 0; col < width; col++)
                {
                    if (map[topRow][col] != map[bottomRow][col])
                    {
                        std::cout << " nope: col: " << col << " topRow: " << topRow << " bottomRow: " << bottomRow << " are different" << std::endl;
                        isReflection = false;
                    }

                }
                topRow--;
                bottomRow++;
            }
            if (isReflection)
            {
                result += 100 * (row + 1);
            }
        }
    }
    std::cout << "result: " << result << std::endl;
}
