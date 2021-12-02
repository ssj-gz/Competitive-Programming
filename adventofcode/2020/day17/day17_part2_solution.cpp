#include <iostream>
#include <vector>

using namespace std;

int countAliveNeighbours(int w, int depth, int row, int col, const vector<vector<vector<vector<bool>>>>& isCellAlive)
{
    int count = 0;
    for (int neighbourW = w - 1; neighbourW <= w + 1; neighbourW++)
    {
        if (neighbourW < 0 || neighbourW >= isCellAlive.size())
            continue;
        const auto& neighbourCube = isCellAlive[neighbourW];
        for (int neighourDepth = depth - 1; neighourDepth <= depth + 1; neighourDepth++)
        {
            if (neighourDepth < 0 || neighourDepth >= neighbourCube.size())
                continue;
            const auto& neighbourGrid = neighbourCube[neighourDepth];
            for (int neighourRowIndex = row - 1; neighourRowIndex <= row + 1; neighourRowIndex++)
            {
                if (neighourRowIndex < 0 || neighourRowIndex >= neighbourGrid.size())
                    continue;
                const auto& neighbourRow = neighbourGrid[neighourRowIndex];
                for (int neighbourCol = col - 1; neighbourCol <= col + 1; neighbourCol++)
                {
                    if (neighbourCol < 0 || neighbourCol >= neighbourRow.size())
                        continue;
                    if (neighbourW == w && neighourDepth == depth && neighourRowIndex == row && neighbourCol == col)
                        continue;
                    if (neighbourRow[neighbourCol])
                        count++;
                }
            }
        }
    }
    return count;
}

#if 0
void printState(const vector<vector<vector<vector<bool>>>>& isCellAlive)
{
    const int totalDepth = isCellAlive.size();
    const int middleDepth = totalDepth / 2;
    for (int depth = 0; depth < totalDepth; depth++)
    {
        cout << "z=" << depth - middleDepth << endl;
        int rowIndex = 0;
        for (const auto& row : isCellAlive[depth])
        {
            int colIndex = 0;
            for (const auto& cell : row)
            {
                if (cell)
                    cout << '#';
                else
                    cout << '.';
                //cout << countAliveNeighbours(depth, rowIndex, colIndex, isCellAlive);
                colIndex++;
            }
            cout << endl;
            rowIndex++;
        }
        cout << endl;
    }

}
#endif


int main()
{
    vector<string> initialConfig;
    string configRow;
    while (getline(cin, configRow))
    {
        initialConfig.push_back(configRow);
    }
    const int initialConfigHeight = static_cast<int>(initialConfig.size());
    const int initialConfigWidth = static_cast<int>(initialConfig.front().size());

    // Assume that the initial config is in the middle of the pocket.
    const int numCycles = 6;
    const int pocketHeight = initialConfigHeight + 2 * numCycles;
    const int pocketWidth = initialConfigWidth + 2 * numCycles;
    const int pocketDepth = 1 + 2 * numCycles;
    const int pocketW = 1 + 2 * numCycles;

    vector<vector<vector<vector<bool>>>> isCellAlive(pocketW, vector<vector<vector<bool>>>(pocketDepth, vector<vector<bool>>(pocketWidth, vector<bool>(pocketHeight, false))));
    const int rowOffset = numCycles;
    const int middleDepth = numCycles;
    const int middleW = numCycles;
    for (int row = rowOffset; row < rowOffset + initialConfigHeight; row++)
    {
        const int colOffset = numCycles;
        for (int col = colOffset; col < colOffset + initialConfigWidth; col++)
        {
            isCellAlive[middleW][middleDepth][row][col] = (initialConfig[row - rowOffset][col - colOffset] == '#');
            cout << "initialConfig[row - rowOffset][col - colOffset]: " << initialConfig[row - rowOffset][col - colOffset]  << endl;
        }
    }
    //printState(isCellAlive);
    for (int cycle = 0; cycle < numCycles; cycle++)
    {
        vector<vector<vector<vector<bool>>>> nextIsCellAlive(isCellAlive);
        for (int w = 0; w < pocketW; w++)
        {
            for (int depth = 0; depth < pocketDepth; depth++)
            {
                for (int row = 0; row < pocketHeight; row++)
                {
                    for (int col = 0; col < pocketWidth; col++)
                    {
                        const int numAliveNeighbours = countAliveNeighbours(w, depth, row, col, isCellAlive);
                        if (isCellAlive[w][depth][row][col])
                        {
                            if (numAliveNeighbours != 2 && numAliveNeighbours != 3)
                                nextIsCellAlive[w][depth][row][col] = false;
                        }
                        else
                        {
                            if (numAliveNeighbours == 3)
                                nextIsCellAlive[w][depth][row][col] = true;
                        }
                    }
                }
            }
        }
        isCellAlive = nextIsCellAlive;
        cout << "After cycle " << (cycle + 1) << endl;
        //printState(isCellAlive);

    }

    int64_t aliveCellsAfterSim = 0;
    for (int w = 0; w < pocketW; w++)
    {
        for (int depth = 0; depth < pocketDepth; depth++)
        {
            for (int row = 0; row < pocketHeight; row++)
            {
                for (int col = 0; col < pocketWidth; col++)
                {
                    if (isCellAlive[w][depth][row][col])
                        aliveCellsAfterSim++;
                }
            }
        }
    }
    cout << aliveCellsAfterSim << endl;
}
