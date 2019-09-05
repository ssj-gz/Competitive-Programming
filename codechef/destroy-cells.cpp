// Simon St James (ssjgz) - 2019-09-05
// 
// Solution to: https://www.codechef.com/problems/DESTCELL
//
#include <iostream>
#include <vector>

#include <cassert>

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

int main(int argc, char* argv[])
{
    // Very easy one - just basically do what it says and simulate the
    // two Hero's Journeys (tee-hee), jumping each block of K "resting steps" in one go.
    // The only tricky part is the asymptotic analysis, but this is fairly
    // well-known: to initialise the Cells takes O(N x M).  Then
    // K = 0 takes ~N x M steps; K = 1 takes ~(N x M) / 2 steps; 
    // K = 2 takes ~(N x M) / 3 steps i.e. enacting a Hero's Journey takes:
    //
    //   ~(N x M)/1 + (N x M)/2 + (N x M)/3 + ...
    //   ~(N x M) * (1/1 + 1/2 + 1/3 + ...)
    //   ~(N x M) * ln(N x M)
    //
    // steps, using the well-known fact that the Harmonic Series over x terms
    // sums to approx ln x.
    //
    // Clearing the Cells would be O(N x M) naively (too slow) so we just
    // track the cells that have been destroyed and un-destroy precisely
    // these, so the cost of restoring the Cells for each K is amortised into
    // the cost of destroying the Cells for each K.

    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int numRows = read<int>();
        const int numCols = read<int>();

        struct Coord
        {
            int rowNum = -1;
            int colNum = -1;
        };

        vector<vector<bool>> isCellDestroyed(numRows, vector<bool>(numCols, false));

        const int numCells = numRows * numCols;
        vector<Coord> hero1VisitedCells;
        hero1VisitedCells.reserve(numCells);
        for (int rowNum = 0; rowNum < numRows; rowNum++)
        {
            for (int colNum = 0; colNum < numCols; colNum++)
            {
                hero1VisitedCells.push_back({rowNum, colNum});
            }
        }
        vector<Coord> hero2VisitedCells;
        hero2VisitedCells.reserve(numCells);
        for (int colNum = 0; colNum < numCols; colNum++)
        {
            for (int rowNum = 0; rowNum < numRows; rowNum++)
            {
                hero2VisitedCells.push_back({rowNum, colNum});
            }
        }

        for (int k = 0; k <= numCells - 1; k++)
        {
            vector<Coord> destroyedCells;
            {
                // Hero 1.
                int cellNum = 0;
                while (cellNum < numCells)
                {
                    const auto& cellToDestroy = hero1VisitedCells[cellNum];
                    destroyedCells.push_back(cellToDestroy);
                    isCellDestroyed[cellToDestroy.rowNum][cellToDestroy.colNum] = true;
                    cellNum += (k + 1);
                }
            }
            {
                // Hero 2.
                int cellNum = 0;
                while (cellNum < numCells)
                {
                    const auto& cellToDestroy = hero2VisitedCells[cellNum];
                    if (!isCellDestroyed[cellToDestroy.rowNum][cellToDestroy.colNum])
                    {
                        // Don't count a Cell destroyed by both Heroes twice!
                        destroyedCells.push_back(cellToDestroy);
                    }
                    cellNum += (k + 1);
                }
            }
            cout << destroyedCells.size() << " ";
            // Clear the destroyed cells - the cost of this is amortised into the cost of marking
            // the cells as destroyed in the first place!
            for (const auto& cell : destroyedCells)
            {
                isCellDestroyed[cell.rowNum][cell.colNum] = false;
            }

        }
        cout << endl;
    }

    assert(cin);
}
