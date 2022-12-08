#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

int main()
{
    string treesLine;
    vector<vector<int>> treeGrid;
    while (std::getline(cin, treesLine))
    {
        vector<int> treeRow;
        for (const auto treeHeighChar : treesLine)
        {
            treeRow.push_back(treeHeighChar - '0');
        }

        treeGrid.push_back(treeRow);
    }

    const int numRows = static_cast<int>(treeGrid.size());
    const int numCols = static_cast<int>(treeGrid[0].size());

    int64_t highestScenicScore = 0;

    for (int treeStartX = 0; treeStartX < numCols; treeStartX++)
    {
        for (int treeStartY = 0; treeStartY < numRows; treeStartY++)
        {
            const int treeStartHeight = treeGrid[treeStartY][treeStartX];
            int64_t scenicScore = 1;

            for (const auto& [dx, dy] : std::initializer_list<std::pair<int, int>>{ {-1, 0},
                                         {+1, 0},
                                         {0, -1},
                                         {0, +1}
                                         })
            {
                int treeX = treeStartX + dx;
                int treeY = treeStartY + dy;
                int numTreesCanSee = 0;
                while ((treeX >= 0 && treeX < numCols) &&
                       (treeY >= 0 && treeY < numRows))
                {
                    numTreesCanSee++;

                    if (treeGrid[treeY][treeX] >= treeStartHeight)
                    {
                        //std::cout << " treeX: " << treeX << " treeY: " << treeY << " at least " << treeStartHeight << std::endl;
                        break;
                    }

                    treeX += dx;
                    treeY += dy;
                }

                scenicScore *= numTreesCanSee;

            }

            highestScenicScore = std::max(highestScenicScore, scenicScore);
        }
    }

    cout << "highestScenicScore: " << highestScenicScore << std::endl;


}
