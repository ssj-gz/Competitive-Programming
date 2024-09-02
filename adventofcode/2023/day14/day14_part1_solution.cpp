#include <iostream>
#include <vector>

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

    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            if (map[row][col] == 'O')
            {
                int newRow = row;
                for (int newCandidateRow = row - 1; newCandidateRow >= 0; newCandidateRow--)
                {
                    if (map[newCandidateRow][col] == '.')
                    {
                        newRow = newCandidateRow;
                    }
                    else
                        break;
                }
                map[row][col] = '.';
                map[newRow][col] = 'O';
            }
        }
    }

    std::cout << "After tilt: " << std::endl;
    for (const auto& row : map) std::cout << row << std::endl;

    int64_t totalLoad = 0;
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            if (map[row][col] == 'O')
            {
                totalLoad += (height - row);
            }
        }
    }
    std::cout << "totalLoad: " << totalLoad << std::endl;
}
