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

    int numVisibleTrees = 0;

    for (int treeStartX = 0; treeStartX < numCols; treeStartX++)
    {
        for (int treeStartY = 0; treeStartY < numRows; treeStartY++)
        {
            bool isVisibleFromAnyEdge = false;
            const int treeStartHeight = treeGrid[treeStartY][treeStartX];

            for (const auto& [dx, dy] : std::initializer_list<std::pair<int, int>>{ {-1, 0},
                                         {+1, 0},
                                         {0, -1},
                                         {0, +1}
                                         })
            {
                int treeX = treeStartX + dx;
                int treeY = treeStartY + dy;
                bool isVisibleFromThisEdge = true;
                while ((treeX >= 0 && treeX < numCols) &&
                       (treeY >= 0 && treeY < numRows))
                {
                    if (treeGrid[treeY][treeX] >= treeStartHeight)
                    {
                        //std::cout << " treeX: " << treeX << " treeY: " << treeY << " at least " << treeStartHeight << std::endl;
                        isVisibleFromThisEdge = false;
                        break;
                    }

                    treeX += dx;
                    treeY += dy;
                }

                if (isVisibleFromThisEdge)
                {
                    isVisibleFromAnyEdge = true;
                    break;
                }
            }
            //std::cout << "treeStartX: " << treeStartX << " treeStartY: " << treeStartY << " isVisibleFromAnyEdge: " << isVisibleFromAnyEdge << " treeStartHeight: " <<  treeStartHeight << std::endl;
            if (isVisibleFromAnyEdge)
                numVisibleTrees++;
        }
    }

    cout << "numVisibleTrees: " << numVisibleTrees << endl;

}
