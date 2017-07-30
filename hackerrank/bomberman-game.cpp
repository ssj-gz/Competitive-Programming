#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

void printCells(const vector<vector<int>>& cells)
{
    if (cells.empty())
        return;
    const int R = cells.size();
    const int C = cells[0].size();
    string cellsOut(C, '.');
    for (int row = 0; row < R; row++)
    {
        std::fill(cellsOut.begin(), cellsOut.end(), '.');
        for (int column = 0; column < C; column++)
        {
            if (cells[row][column] != -1)
            {
                cellsOut[column] = 'O';
            }

        }
        cout << cellsOut;
        if (row != R - 1)
            cout << endl;
    }
}

bool compareAsPrinted(const vector<vector<int>>& cells1, const vector<vector<int>>& cells2)
{
    if (cells1.size() != cells2.size())
    {
        return false;
    }
    const int R = cells1.size();
    const int C = cells1[0].size();
    for (int row = 0; row < R; row++)
    {
        for (int column = 0; column < C; column++)
        {
            if (cells1[row][column] * cells2[row][column] < 0)
            {
                return false;
            }

        }
    }
    return true;
}


int main() {
    /* Enter your code here. Read input from STDIN. Print output to STDOUT */   
    int R, C, N;
    cin >> R >> C >> N;
    vector<vector<int> > cellDetonationTime(R, vector<int>(C, -1));
    int currentTime = 0;
    // Step 1.
    for (int row = 0; row < R; row++)
    {
        string cellsForRow;
        cin >> cellsForRow;
        for (int column = 0; column < C; column++)
        {
            if (cellsForRow[column]  == 'O')
            {
                cellDetonationTime[row][column] = currentTime + 3;
            }
        }
    }
    // Step 2.
    currentTime++;
    vector<vector<int> > previous = cellDetonationTime;
    vector<vector<int> > previousPrevious;
    bool skipIfCycleDetected = true;
    while (currentTime < N)
    {
        // Step 3.
        // Plant bombs - none will detonate this second.
        currentTime++;
        for (int row = 0; row < R; row++)
        {
            for (int column = 0; column < C; column++)
            {
                if (cellDetonationTime[row][column] == -1)
                {
                    cellDetonationTime[row][column] = currentTime + 3;
                }
            }

        }
        if (currentTime == N)
            break;
        // Step 4.
        currentTime++;
        // Explode bombs.
        for (int row = 0; row < R; row++)
        {
            for (int column = 0; column < C; column++)
            {
                if (cellDetonationTime[row][column] != currentTime)
                    continue;
                // Detonate bomb at cell.
                cellDetonationTime[row][column] = -1;
                int adjacentCellOffsets[][2]  = { 
                    {-1, 0},
                    {1, 0},
                    {0, -1},
                    {0, 1},
                };
                for (int adjacentCell = 0; adjacentCell < 4; adjacentCell++)
                {
                    int i = row + adjacentCellOffsets[adjacentCell][0];
                    int j = column + adjacentCellOffsets[adjacentCell][1];
                    if (i < 0 || i >= R || j < 0 || j >= C)
                        continue;

                    int& impactedCellDetonationTime = cellDetonationTime[i][j];
                    if (impactedCellDetonationTime != currentTime )
                    {
                        impactedCellDetonationTime = -1;
                    }

                }
            }

        }
        if (currentTime == N)
            break;

        if (skipIfCycleDetected && compareAsPrinted(cellDetonationTime, previousPrevious))
        {
            skipIfCycleDetected = false;
            const int timeAdjustment = N - (N % 4 + 1) - currentTime;
            currentTime += timeAdjustment;
            for (int row = 0; row < R; row++)
            {
                for (int column = 0; column < C; column++)
                {
                    if (cellDetonationTime[row][column] != -1)
                        cellDetonationTime[row][column] += timeAdjustment;
                }
            }
        }
        previousPrevious = previous;
        previous = cellDetonationTime;


    }
    printCells(cellDetonationTime);
    return 0;
}




