#define VERIFY_FACTORIAL_TRACKER
#define BRUTE_FORCE
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#undef VERIFY_FACTORIAL_TRACKER
#endif
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;

constexpr int64_t mod = 1'000'000'000;
constexpr int maxNonZeroFactorial = 39;

vector<int64_t> factorialTable;

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
                    //cout << "rangeBegin: " << rangeBegin << endl;
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
            //printMatrix();
#ifdef VERIFY_FACTORIAL_TRACKER
            m_numbers.resize(maxNumber + 1);
#endif
        }
        void blah(int start, int end)
        {
            vector<Cell*> cells;
            collectMinCellsForRange(start, end, 0, m_powerOf2BiggerThanMaxNumber, cells);
            //cout << "collected cells in range " << start << "-" << end << endl;
            for (const auto cell : cells)
            {
                //printCell(cell);
            }
        }
        void addToRange(int number, int start, int end)
        {
            //cout << "addToRange " << number << " " << start << "-" << end << endl;
            vector<Cell*> cells;
            collectMinCellsForRange(start, end, cells);
            //cout << "cells collected by addToRange " << start << "-" << end << endl;
            for (const auto cell : cells)
            {
                //printCell(cell);
            }
            for (auto cell : cells)
            {
                cell->addPendingOperation(number);
                cell->servicePendingOperations();
                if (cell->parent)
                    cell->parent->setNeedsUpdateFromChildren();
            }
            //cout << "cells collected by addToRange after performing operation: " << start << "-" << end << endl;
            for (const auto cell : cells)
            {
                //printCell(cell);
            }
            m_cellMatrix.front().front().updateFromChildren();
#ifdef VERIFY_FACTORIAL_TRACKER
            for (int i = start; i <= end; i++)
            {
                m_numbers[i] += number;
            }
#endif
        }
        int countInRange(int start, int end)
        {
            //cout << " countInRange " << start << "-" << end << endl;
            vector<Cell*> cells;
            collectMinCellsForRange(start, end, cells);
            int numberInRange = 0;
            for (auto cell : cells)
            {
                cell->servicePendingOperations();
                numberInRange += cell->numInRange;
                //printCell(cell);
            }
#ifdef VERIFY_FACTORIAL_TRACKER
            {
                int dbgNumInRange = 0;
                for (int i = start; i <= end; i++)
                {
                    dbgNumInRange += m_numbers[i];
                }
                cout << "countInRange " << start << "-" << end << " result: " << numberInRange << " debug result: " << dbgNumInRange << endl;
                assert(dbgNumInRange == numberInRange);
            }
#endif
            return numberInRange;
        }
    private:
        int m_powerOf2BiggerThanMaxNumber;
        int m_exponentOfPowerOf2BiggerThanMaxNumber;
        int m_maxNumber;
    public:
        void printMatrix()
        {
            // Debug
            for (int cellRow = 0; cellRow < m_cellMatrix.size(); cellRow++)
            {
                //cout << "row: " << cellRow << endl;
                for (int cellCol = 0; cellCol < m_cellMatrix[cellRow].size(); cellCol++)
                {
                    Cell* cell = &(m_cellMatrix[cellRow][cellCol]);
                    printCell(cell);
                }
            }
        }
    private:
        struct Cell
        {
            int numInRange = 0; // TODO - remove.


            int rangeBegin = -1;
            int rangeEnd = -1;
            Cell* parent = nullptr;
            Cell* leftChild = nullptr;
            Cell* rightChild = nullptr;

            int pendingOperatorInfo = 0;
            bool hasPendingOperator = false;
            bool needsUpdateFromChildren = false;

            void addPendingOperation(int operatorInfo)
            {
                if (!hasPendingOperator)
                {
                    hasPendingOperator = true;
                    pendingOperatorInfo = operatorInfo;
                }
                else
                {
                    pendingOperatorInfo += operatorInfo;
                }
            }

            void servicePendingOperations()
            {
                if (hasPendingOperator)
                {
                    numInRange += (rangeEnd - rangeBegin + 1) * pendingOperatorInfo;
                    if (leftChild && rightChild)
                    {
                        leftChild->addPendingOperation(pendingOperatorInfo);
                        rightChild->addPendingOperation(pendingOperatorInfo);
                    }
                    hasPendingOperator = false;
                    //cout << "cell " << this << " updated to numInRange: " << numInRange << " by servicePendingOperations; pendingOperatorInfo: " << pendingOperatorInfo << endl;
                }
            }

            void setNeedsUpdateFromChildren()
            {
                if (needsUpdateFromChildren)
                    return;
                if (!(leftChild && rightChild))
                    return;

                needsUpdateFromChildren = true;
                if (parent)
                    parent->setNeedsUpdateFromChildren();
            }

            void updateFromChildren()
            {
                if (!needsUpdateFromChildren)
                    return;
                if (!(leftChild && rightChild))
                    return;

                leftChild->servicePendingOperations();
                rightChild->servicePendingOperations();
                const int oldValue = numInRange;
                leftChild->updateFromChildren();
                rightChild->updateFromChildren();
                numInRange = leftChild->numInRange + rightChild->numInRange;
                //cout << "updating " << this << " from children; was: " << oldValue << " now: " << numInRange << endl;
                needsUpdateFromChildren = false;
            }

        };
        void printCell(Cell* cell)
        {
            cout << " cell: " << cell << " begin: " << cell->rangeBegin << " end:" << cell->rangeEnd << " numInRange: " << cell->numInRange << " parent: " << cell->parent << " leftChild: " << cell->leftChild << " rightChild: " << cell->rightChild << " hasPendingOperator: " << cell->hasPendingOperator << " pendingOperatorInfo: " << cell->pendingOperatorInfo << " needsUpdateFromChildren: " << cell->needsUpdateFromChildren << endl;
        }
        vector<vector<Cell>> m_cellMatrix;
#ifdef VERIFY_FACTORIAL_TRACKER
        vector<int> m_numbers;
#endif

        void collectMinCellsForRange(int start, int end, vector<Cell*>& destCells)
        {
            collectMinCellsForRange(start, end, 0, m_powerOf2BiggerThanMaxNumber, destCells);
            sort(destCells.begin(), destCells.end(), [](const Cell* lhs, const Cell* rhs)
                    {
                        return lhs->rangeBegin < rhs->rangeEnd;
                    });

            for (int i = 1; i < destCells.size(); i++)
            {
                assert(destCells[i]->rangeBegin == destCells[i - 1]->rangeEnd + 1);
            }
            assert(destCells.front()->rangeBegin == start);
            assert(destCells.back()->rangeEnd == end);
        }
        void collectMinCellsForRange(int start, int end, int cellRow, int powerOf2, vector<Cell*>& destCells)
        {
            //cout << "collectMinCellsForRange start: " << start << " end: " << end << " cellRow: " << cellRow << " powerOf2: " << powerOf2 << endl;
            if (cellRow == m_cellMatrix.size())
                return;
            if (cellRow != 0)
            {
                const int parentCellStartIndex = start / (powerOf2 * 2);
                const int parentCellEndIndex = end / (powerOf2 * 2);
                const int parentCellRow = cellRow - 1;
                //cout << "collectMinCellsForRange " << start << "-" << end << " cellRow: " << cellRow << " parentCellStartIndex: " << parentCellStartIndex << " parentCellEndIndex: " << parentCellEndIndex << " parentCells " << endl;
                for (int parentCellCol = parentCellStartIndex; parentCellCol <= parentCellEndIndex; parentCellCol++)
                {
                    Cell *parentCell = &(m_cellMatrix[parentCellRow][parentCellCol]);
                    parentCell->servicePendingOperations();
                }
            }
            if (end < start)
                return;
            if (end == start)
            {
                if (cellRow == m_cellMatrix.size() - 1)
                    destCells.push_back(&(m_cellMatrix.back()[start]));
                else
                    collectMinCellsForRange(start, end, cellRow + 1, powerOf2 >> 1, destCells);
                return;
                //return m_cellMatrix.back()[start].numNumbersInRange;
            }
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
                if (start <= end - powerOf2 + 1)
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
                if (start <= end - powerOf2 + 1)
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

struct Query
{
    int type;
    int64_t n1;
    int64_t n2;
};

vector<int64_t> bruteForce(const vector<int>& originalA, const vector<Query>& queries)
{
    vector<int64_t> results;
    vector<int> A(originalA);

    for (const auto& query : queries)
    {
        switch (query.type)
        {
            case 1:
                {
                    const auto l = query.n1 - 1;
                    const auto r = query.n2 - 1;
                    for (int i = l; i <= r; i++)
                    {
                        A[i]++;
                    }
                    break;
                }
            case 2:
                {
                    const auto l = query.n1 - 1;
                    const auto r = query.n2 - 1;
                    int64_t factorialSum = 0;
                    for (int i = l; i <= r; i++)
                    {
                        factorialSum += factorialTable[A[i]];
                    }
                    results.push_back(factorialSum);
                    break;
                }
            case 3:
                A[query.n1 - 1] = query.n2;
                break;
        }
    }

    return results;
}


int main()
{

    factorialTable.push_back(1); // What's 0 factorial?
    int64_t factorial = 1;
    for (int i = 1; i <= maxNonZeroFactorial; i++)
    {
        factorial = (factorial * i) % mod;
        factorialTable.push_back(factorial);
    }
    ios::sync_with_stdio(false);

    int n, m;
    cin >> n >> m;

    vector<int> A(n);
    for (int i = 0; i < n; i++)
    {
        cin >> A[i];
    }

    vector<Query> queries(m);
    for (int i = 0; i < m; i++)
    {
        cin >> queries[i].type;
        cin >> queries[i].n1;
        cin >> queries[i].n2;
    }
    assert(cin);
#ifdef BRUTE_FORCE
    const auto resultsBruteForce = bruteForce(A, queries);
    for (const auto result : resultsBruteForce)
    {
        cout << result << endl;
    }
#endif

#if 0
    {
        FactorialTracker factorialTracker(10);
        factorialTracker.blah(1,1);
        factorialTracker.blah(0,15);
        factorialTracker.blah(0,7);
        factorialTracker.blah(3,11);
    }
    {
#if 0
        FactorialTracker factorialTracker(5);
        factorialTracker.addToRange(1, 3, 4);
        factorialTracker.printMatrix();
        factorialTracker.addToRange(4, 0, 3);
        factorialTracker.printMatrix();
        factorialTracker.countInRange(3, 3);
#endif
    }

#if 1
    {
        srand(time(0));
        const int maxNum = 100000;
        FactorialTracker factorialTracker(maxNum);
        for (int i = 0; i < 100000; i++)
        {
            const int numberToAdd = rand() % 4;
            int addRangeBegin = rand() % maxNum;
            int addRangeEnd = rand() % maxNum;
            if (addRangeEnd < addRangeBegin)
                swap(addRangeBegin, addRangeEnd);
            factorialTracker.addToRange(numberToAdd, addRangeBegin, addRangeEnd);
            //factorialTracker.printMatrix();
            int countRangeBegin = rand() % maxNum;
            int countRangeEnd = rand() % maxNum;
            if (countRangeEnd < countRangeBegin)
                swap(countRangeBegin, countRangeEnd);
            //cout << factorialTracker.countInRange(countRangeBegin, countRangeEnd) << endl;
        }
    }
#endif

#endif
}

