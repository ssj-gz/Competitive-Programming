#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <stack>
#include <cassert>

using namespace std;

class FactorialTracker
{
    public:
        FactorialTracker(int maxNumber)
            : m_maxNumber{maxNumber}
        {
            int exponentOfPowerOf2 = 0;
            int powerOf2 = 1;
            while (maxNumber > powerOf2)
            {
                exponentOfPowerOf2++;
                powerOf2 *= 2;
            }
            m_powerOf2BiggerThanMaxNumber = powerOf2;
            m_exponentOfPowerOf2BiggerThanMaxNumber = exponentOfPowerOf2;
            // Build cell matrix.
            int numCellsInThisRow = 1;
            powerOf2 = m_powerOf2BiggerThanMaxNumber;
            while (powerOf2 > 0)
            {
                m_cellMatrix.push_back(vector<Cell>(numCellsInThisRow));
                int rangeBegin = 0;
                for (int cellCol = 0; cellCol < numCellsInThisRow; cellCol++)
                {
                    m_cellMatrix.back()[cellCol].rangeBegin = rangeBegin;
                    m_cellMatrix.back()[cellCol].rangeEnd = rangeBegin + powerOf2 - 1;
                    rangeBegin += powerOf2;
                }
                numCellsInThisRow *= 2;
                powerOf2 /= 2;
            }
            // Cell matrix parent/child.
            for (int cellRow = 0; cellRow < m_cellMatrix.size() - 1; cellRow++)
            {
                int childCellIndex = 0;
                for (int cellCol = 0; cellCol < m_cellMatrix[cellRow].size(); cellCol++)
                {
                    m_cellMatrix[cellRow][cellCol].leftChild = &(m_cellMatrix[cellRow + 1][childCellIndex]);
                    m_cellMatrix[cellRow + 1][childCellIndex].parent = &(m_cellMatrix[cellRow][cellCol]);
                    childCellIndex++;
                    m_cellMatrix[cellRow][cellCol].rightChild = &(m_cellMatrix[cellRow + 1][childCellIndex]);
                    m_cellMatrix[cellRow + 1][childCellIndex].parent = &(m_cellMatrix[cellRow][cellCol]);
                    childCellIndex++;
                }
            }
            printMatrix();
        }
        void blah(int start, int end)
        {
            vector<Cell*> cells;
            collectMinCellsForRange(start, end, 0, m_powerOf2BiggerThanMaxNumber, cells);
            cout << "collected cells in range " << start << "-" << end << endl;
            for (const auto cell : cells)
            {
                printCell(cell);
            }
        }
    private:
        int m_powerOf2BiggerThanMaxNumber;
        int m_exponentOfPowerOf2BiggerThanMaxNumber;
        int m_maxNumber;
        void printMatrix()
        {
            // Debug
            for (int cellRow = 0; cellRow < m_cellMatrix.size() - 1; cellRow++)
            {
                cout << "row: " << cellRow << endl;
                for (int cellCol = 0; cellCol < m_cellMatrix[cellRow].size(); cellCol++)
                {
                    Cell* cell = &(m_cellMatrix[cellRow][cellCol]);
                    printCell(cell);
                }
            }
        }
        struct Cell
        {
            int rangeBegin = -1;
            int rangeEnd = -1;
            Cell* parent = nullptr;
            Cell* leftChild = nullptr;
            Cell* rightChild = nullptr;

            int pendingOperatorInfo = 0;
            bool hasPendingOperator = false;
        };
        void printCell(Cell* cell)
        {
            cout << " cell: " << cell << " begin: " << cell->rangeBegin << " end:" << cell->rangeEnd << " parent: " << cell->parent << " leftChild: " << cell->leftChild << " rightChild: " << cell->rightChild << " hasPendingOperator: " << cell->hasPendingOperator << " pendingOperatorInfo: " << cell->pendingOperatorInfo << endl;
        }
        vector<vector<Cell>> m_cellMatrix;
        void collectMinCellsForRange(int start, int end, int cellRow, int powerOf2, vector<Cell*>& destCells)
        {
            if (end < start)
                return;
            if (end == start)
            {
                destCells.push_back(&(m_cellMatrix.back()[start]));
                return;
                //return m_cellMatrix.back()[start].numNumbersInRange;
            }
            if (cellRow == m_cellMatrix.size())
                return;
            if (end + 1 - start < powerOf2)
            {
                // Gap between start and end is too small; recurse with next row.
                collectMinCellsForRange(start, end, cellRow + 1, powerOf2 >> 1, destCells);
                return;
                //return countNumbersInRange(start, end, cellRow + 1, powerOf2 >> 1);
            }
            if ((start % powerOf2) == 0)
            {
                // Advance start by one complete cell at a time, summing the contents of that cell, then recurse on the rest.
                if (start <= end - powerOf2)
                {
                    destCells.push_back(&(m_cellMatrix[cellRow][start/powerOf2]));
                    start += powerOf2;
                }
                collectMinCellsForRange(start, end, cellRow + 1, powerOf2 >> 1, destCells);
                return;
            }
            if (((end + 1) % powerOf2) == 0)
            {
                // Unadvance end by one complete cell at a time, summing the contents of that cell, then recurse on the rest.
                if (start <= end - powerOf2)
                {
                    destCells.push_back(&(m_cellMatrix[cellRow][end/powerOf2]));
                    end -= powerOf2;
                }
                collectMinCellsForRange(start, end, cellRow + 1, powerOf2 >> 1, destCells);
                return;
            }
            // Neither start nor end is a multiple of powerOf2 ... sum up the complete cells in between (if any) ...
            const int powerOf2AfterStart = (start / powerOf2 + 1) * powerOf2;
            const int powerOf2BeforeEnd =  (end / powerOf2) * powerOf2;
            // ... add the complete cell in between (if there is one) ...
            if (powerOf2AfterStart < powerOf2BeforeEnd)
            {
                destCells.push_back(&(m_cellMatrix[cellRow][powerOf2AfterStart / powerOf2]));
            }
            // ... and then split into two, and recurse: for each of two split regions, at least one of the start or end will be a multiple of powerOf2, so they
            // will not split further.
            collectMinCellsForRange(start, powerOf2AfterStart - 1, cellRow + 1, powerOf2 >> 1, destCells);
            collectMinCellsForRange(powerOf2BeforeEnd, end, cellRow + 1, powerOf2 >> 1, destCells);
            return;
        }
};


int main()
{
    {
    FactorialTracker factorialTracker(10);
    factorialTracker.blah(1,1);
    factorialTracker.blah(0,15);
    factorialTracker.blah(0,7);
    factorialTracker.blah(3,11);
    }
    {
        FactorialTracker factorialTracker(1000);
        factorialTracker.blah(30, 530);
    }
}

