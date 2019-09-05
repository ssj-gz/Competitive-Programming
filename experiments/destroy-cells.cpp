// Simon St James (ssjgz) - 2019-09-05
// 
// Solution to: https://www.codechef.com/problems/DESTCELL
//
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

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
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        // TODO - generate randomised test.
        //const int T = rand() % 100 + 1;
        const int T = 2;

        cout << T << endl;

        for (int t = 0; t < T; t++)
        {
            cout << (1000 - rand() % 10) << " " << (1000 - rand() % 10) << endl;
        }

        return 0;
    }
    
    // TODO - read in testcase.
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
