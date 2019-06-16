// Simon St James (ssjgz) - 2019-06-16
#include <iostream>
#include <vector>

using namespace std;

int main()
{
    // Completely trivial - repeatedly do a search of all cells for a "1", until
    // we can't find one, in which case we report the size of the largest region found.
    //
    // If we find a 1, we just do a BFS from that 1 to all connected cells containing 1's (and all cells
    // containing 1's connected to these cells, etc), counting all the 1's encountered and
    // changing them into 0's so they are not considered again.  That's about it!
    //
    // A bit weird that the challenge name has "DFS" in it, but oh well.
    int n;
    cin >> n;

    int m;
    cin >> m;

    vector<vector<int>> grid(n, vector<int>(m));

    for (int row = 0; row < n; row++)
    {
        for (int col = 0; col < m; col++)
        {
            cin >> grid[row][col];
        }
    }

    int largestRegion = 0;

    while (true)
    {
        bool regionFound = false;

        for (int startRow = 0; startRow < n; startRow++)
        {
            for (int startCol = 0; startCol < m; startCol++)
            {
                if (grid[startRow][startCol] == 1)
                {
                    regionFound = true;

                    int numInRegion = 1;
                    grid[startRow][startCol] = 0;

                    vector<pair<int, int>> toExplore = { { startRow, startCol }};
                    while (!toExplore.empty())
                    {
                        vector<pair<int, int>> nextToExplore;
                        for (const auto coord : toExplore)
                        {
                            // Find all connected cells.
                            for (int r = coord.first - 1; r <= coord.first + 1; r++)
                            {
                                for (int c = coord.second - 1; c <= coord.second + 1; c++)
                                {
                                    if (r < 0 || r >= n)
                                        continue;
                                    if (c < 0 || c >= m)
                                        continue;
                                    if (grid[r][c] == 1)
                                    {
                                        numInRegion++;
                                        grid[r][c] = 0;
                                        nextToExplore.push_back({r, c});
                                    }
                                }
                            }
                        }
                        toExplore = nextToExplore;
                    }

                    largestRegion = std::max(largestRegion, numInRegion);
                }
            }
        }
        if (!regionFound)
            break;
    }

    cout << largestRegion << endl;
}
