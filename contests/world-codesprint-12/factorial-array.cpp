#define VERIFY_FACTORIAL_TRACKER
#define BRUTE_FORCE
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#undef BRUTE_FORCE
#undef VERIFY_FACTORIAL_TRACKER
#endif
#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <cassert>

using namespace std;

constexpr int64_t mod = 1'000'000'000UL;
constexpr int maxNonZeroFactorial = 39;

vector<int64_t> factorialTable;

struct FactorialHistogram
{
    std::array<int64_t, maxNonZeroFactorial + 1> numWithFactorial = {};
};

class FactorialTracker
{
    public:
        FactorialTracker(int maxNumber)
            : m_maxNumber{maxNumber}
        {
            int exponentOfPowerOf2 = 0;
            int64_t powerOf2 = 1;
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
                int64_t rangeBegin = 0;
                for (int cellCol = 0; cellCol < numCellsInThisRow; cellCol++)
                {
                    m_cellMatrix.back()[cellCol].rangeBegin = rangeBegin;
                    m_cellMatrix.back()[cellCol].rangeEnd = rangeBegin + powerOf2 - 1;
                    assert(m_cellMatrix.back()[cellCol].rangeEnd >= m_cellMatrix.back()[cellCol].rangeBegin);
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
            m_dbgFactorialHistogram.resize(maxNumber + 1);
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
            //for (int i = start; i <= end; i++)
            //{
                //m_numbers[i] += number;
            //}
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
                //numberInRange += cell->numInRange;
                //printCell(cell);
            }
#ifdef VERIFY_FACTORIAL_TRACKER
            {
                //int dbgNumInRange = 0;
                //for (int i = start; i <= end; i++)
                //{
                    //dbgNumInRange += m_numbers[i];
                //}
                //cout << "countInRange " << start << "-" << end << " result: " << numberInRange << " debug result: " << dbgNumInRange << endl;
                //assert(dbgNumInRange == numberInRange);
            }
#endif
            return numberInRange;
        }
        void setInitialValues(const vector<int64_t>& A)
        {
            for (int i = 0; i < A.size(); i++)
            {
                const int factorialIndex = (A[i] <= maxNonZeroFactorial ? A[i] : -1);
                if (factorialIndex != -1)
                {
                    auto& cell = m_cellMatrix.back()[i];
                    cell.factorialHistogram.numWithFactorial[factorialIndex] = 1;
                    if (cell.parent)
                        cell.parent->setNeedsUpdateFromChildren();
                }
            }
            m_cellMatrix.front().front().updateFromChildren();
#ifdef VERIFY_FACTORIAL_TRACKER
            for (int i = 0; i < A.size(); i++)
            {
                const int factorialIndex = (A[i] <= maxNonZeroFactorial ? A[i] : -1);
                if (factorialIndex != -1)
                {
                    m_dbgFactorialHistogram[i].numWithFactorial[factorialIndex] = 1;
                }
            }
#endif
        }
        void addOneToAllInRange(int left, int right)
        {
            vector<Cell*> cells;
            collectMinCellsForRange(left, right, cells);
            for (auto cell : cells)
            {
                cell->addPendingOperation(1);
                cell->servicePendingOperations();
                if (cell->parent)
                    cell->parent->setNeedsUpdateFromChildren();
            }
            m_cellMatrix.front().front().updateFromChildren();
        }
        void setValue(int pos, int64_t value)
        {
            //cout << "setValue: " << pos << " to " << value << endl;
            //cout << "original matrix:" << endl;
            //printMatrix();
            vector<Cell*> cells;
            collectMinCellsForRange(pos, pos, cells);
            assert(cells.size() == 1);
            auto cell = cells.front();
            cell->servicePendingOperations();
            //printCell(cell);
            const int valueFactorialIndex = (value <= maxNonZeroFactorial ? value : -1);
            bool foundNonZero = false;
            for (int i = 1; i <= maxNonZeroFactorial; i++)
            {
                //cout << "i: " << i << " numWithFactorial[i]: " << cell->factorialHistogram.numWithFactorial[i] << endl;
                if (cell->factorialHistogram.numWithFactorial[i] != 0)
                {
                    assert(!foundNonZero);
                    assert(cell->factorialHistogram.numWithFactorial[i] == 1);
                    cell->factorialHistogram.numWithFactorial[i] = 0;
                    //cout << "Found non-zero: " << i << endl;
                    foundNonZero = true;
                    //break;
                }
            }
            //cout << " valueFactorialIndex: " << valueFactorialIndex << endl;
            if (valueFactorialIndex != -1)
            {
                cell->factorialHistogram.numWithFactorial[valueFactorialIndex] = 1;
            }
            //cout << "after set value: " << endl;
            //printCell(cell);
            for (int i = 1; i <= maxNonZeroFactorial; i++)
            {
                //cout << " (after set value) i: " << i << " numWithFactorial[i]: " << cell->factorialHistogram.numWithFactorial[i] << endl;
            }

            if (cell->parent)
                cell->parent->setNeedsUpdateFromChildren();
            m_cellMatrix.front().front().updateFromChildren();

            //cout << "matrix after setValue " << pos << " to " << value << endl;
            //printMatrix();

        }
        int64_t factorialSum(int left, int right)
        {
            vector<Cell*> cells;
            collectMinCellsForRange(left, right, cells);
            int64_t factorialSum = 0;
            for (auto cell : cells)
            {
                cell->servicePendingOperations();
                for (int i = 1; i <= maxNonZeroFactorial; i++)
                {
                    factorialSum = (factorialSum + (cell->factorialHistogram.numWithFactorial[i] * factorialTable[i]) % mod) % mod;
                }
            }
            assert(factorialSum >= 0 && factorialSum < mod);
            return factorialSum;
        }
    private:
        int64_t m_powerOf2BiggerThanMaxNumber;
        int m_exponentOfPowerOf2BiggerThanMaxNumber;
        int m_maxNumber;
    public:
        void printMatrix()
        {
            // Debug
            for (int cellRow = 0; cellRow < m_cellMatrix.size(); cellRow++)
            {
                cout << "row: " << cellRow << endl;
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
            FactorialHistogram factorialHistogram;

            int64_t rangeBegin = -1;
            int64_t rangeEnd = -1;
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
#if 0
                    const int numInRange = (rangeEnd - rangeBegin + 1);
                    numInRange += (rangeEnd - rangeBegin + 1) * pendingOperatorInfo;
#endif
                    if (leftChild && rightChild)
                    {
                        leftChild->addPendingOperation(pendingOperatorInfo);
                        rightChild->addPendingOperation(pendingOperatorInfo);
                    }
                    //cout << "Cell: " << this << " " << rangeBegin << "-" << rangeEnd << " original factorial histogram:" << endl;
                    for (int i = 0; i <= maxNonZeroFactorial; i++)
                    {
                        //cout << "i: " << i << " numWithFactorial[i]: " << factorialHistogram.numWithFactorial[i] << endl;
                    }
                    for (int i = maxNonZeroFactorial; i >= pendingOperatorInfo; i--)
                    {
                        factorialHistogram.numWithFactorial[i] = factorialHistogram.numWithFactorial[i - pendingOperatorInfo];
                    }
                    for (int i = 0; i < min(pendingOperatorInfo, maxNonZeroFactorial + 1) ; i++)
                    {
                        factorialHistogram.numWithFactorial[i] = 0;
                    }
                    //cout << "Cell: " << this << " after updating with " << pendingOperatorInfo << " factorial histogram:" << endl;
                    for (int i = 0; i <= maxNonZeroFactorial; i++)
                    {
                        //cout << "i: " << i << " numWithFactorial[i]: " << factorialHistogram.numWithFactorial[i] << endl;
                    }
                    hasPendingOperator = false;
                    if (pendingOperatorInfo != 1)
                    {
                        //cout << "Merged multiple factorials" << endl;
                    }
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
                //const int oldValue = numInRange;
                leftChild->updateFromChildren();
                rightChild->updateFromChildren();
                //numInRange = leftChild->numInRange + rightChild->numInRange;
                //cout << "updating " << this << " from children; was: " << oldValue << " now: " << numInRange << endl;
                for (int i = 0; i <= maxNonZeroFactorial; i++)
                {
                    factorialHistogram.numWithFactorial[i] = leftChild->factorialHistogram.numWithFactorial[i] + rightChild->factorialHistogram.numWithFactorial[i];
                }
                needsUpdateFromChildren = false;
            }

        };
        void printCell(Cell* cell)
        {
            cout << " cell: " << cell << " begin: " << cell->rangeBegin << " end:" << cell->rangeEnd << " parent: " << cell->parent << " leftChild: " << cell->leftChild << " rightChild: " << cell->rightChild << " hasPendingOperator: " << cell->hasPendingOperator << " pendingOperatorInfo: " << cell->pendingOperatorInfo << " needsUpdateFromChildren: " << cell->needsUpdateFromChildren << endl;
            for (int i = 1; i <= maxNonZeroFactorial; i++)
            {
                if (cell->factorialHistogram.numWithFactorial[i] != 0)
                    cout << "  i: " << i << " numWithFactorial[i]: " << cell->factorialHistogram.numWithFactorial[i] << endl;
            }
        }
        vector<vector<Cell>> m_cellMatrix;
#ifdef VERIFY_FACTORIAL_TRACKER
        vector<FactorialHistogram> m_dbgFactorialHistogram;
#endif

        void collectMinCellsForRange(int start, int end, vector<Cell*>& destCells)
        {
            collectMinCellsForRange(start, end, 0, m_powerOf2BiggerThanMaxNumber, destCells);
            sort(destCells.begin(), destCells.end(), [](const Cell* lhs, const Cell* rhs)
                    {
                        return lhs->rangeBegin < rhs->rangeBegin;
                    });

            for (int i = 1; i < destCells.size(); i++)
            {

                if (!(destCells[i]->rangeBegin == destCells[i - 1]->rangeEnd + 1))
                {
                    //cout << "destCells[i]->rangeBegin: " << destCells[i]->rangeBegin << " destCells[i - 1]->rangeEnd + 1: " << (destCells[i - 1]->rangeEnd + 1) << endl;
                    cout << "Cell error" << endl;
                    for (const auto cell : destCells)
                    {
                        printCell(cell);
                    }
                }
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
                {
                    destCells.push_back(&(m_cellMatrix.back()[start]));
                //cout << "Added cell 1: " << destCells.back() << endl;
                }
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
                //cout << "Added cell 2: " << destCells.back() << " row: " << cellRow << " index: " << (start/ powerOf2) << " cells in row: " << m_cellMatrix[cellRow].size() << endl;
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
                //cout << "Added cell 3: " << destCells.back() << endl;
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
                //cout << "Added cell 4: " << destCells.back() << endl;
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

vector<int64_t> bruteForce(const vector<int64_t>& originalA, const vector<Query>& queries, const vector<int64_t>& resultsOptimised)
{
    vector<int64_t> results;
    vector<int64_t> A(originalA);

    int queryIndex = 0;
    for (const auto& query : queries)
    {
        cout << "queryIndex: " << queryIndex;
        switch (query.type)
        {
            case 1:
                {
                    const auto l = query.n1 - 1;
                    const auto r = query.n2 - 1;
                    cout << " type - add " << l << "-" << r << endl;
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
                    cout << " type - sum " << l << "-" << r << endl;
                    for (int i = l; i <= r; i++)
                    {
                        if (A[i] <= maxNonZeroFactorial)
                            factorialSum = (factorialSum + factorialTable[A[i]]) % mod;
                    }
                    if (resultsOptimised[results.size()] != factorialSum)
                    {
                        cout << "Error at query: " << queryIndex << " sum over range: " << l << "-" << r << " optimised: " << resultsOptimised[results.size()] << " brute force: " << factorialSum << endl;
                        if (queryIndex != 0)
                            cout << "Previous query type: " << queries[queryIndex - 1].type << " n1: " << queries[queryIndex - 1].n1 << " n2: " << queries[queryIndex - 1].n2 << endl;
                    }
                    results.push_back(factorialSum);
                    break;
                }
            case 3:
                A[query.n1 - 1] = query.n2;
                cout << " type - setvalue " << query.n1 - 1 << "-" << query.n2 << endl;
                break;
        }
#if 0
        cout << "After query " << queryIndex << endl;
        for (const auto x : A)
        {
            cout << x << " ";
        }
        cout << endl;
#endif
        queryIndex++;
    }

    return results;
}

vector<int64_t> results(const vector<int64_t>& A, const vector<Query>& queries)
{
    FactorialTracker factorialTracker(A.size());
    factorialTracker.setInitialValues(A);
    vector<int64_t> results;

    int queryIndex = 0;
    for (const auto& query : queries)
    {
        //cout << "Matrix before query index " << queryIndex << endl;
        //factorialTracker.printMatrix();
        switch (query.type)
        {
            case 1:
                {
                    const auto l = query.n1 - 1;
                    const auto r = query.n2 - 1;
                    factorialTracker.addOneToAllInRange(l, r);
                    break;
                }
            case 2:
                {
                    const auto l = query.n1 - 1;
                    const auto r = query.n2 - 1;
                    const auto factorialSum = factorialTracker.factorialSum(l, r);
                    results.push_back(factorialSum);
                    break;
                }
            case 3:
                factorialTracker.setValue(query.n1 - 1, query.n2);
                break;
        }
        //cout << "Matrix after query index " << queryIndex << endl;
        //factorialTracker.printMatrix();
        queryIndex++;
    }
    return results;
}


int main(int argc, char** argv)
{
    if (argc == 2)
    {
        srand(time(0));
        //const int n = rand() % 100'000 + 1;
        //const int m = rand() % 100'000 + 1;
        const int n = 100'000;
        const int m = 100'000;
        cout << n << " " << m << endl;
        for (int i = 0; i < n; i++)
        {
            cout << rand() % 50 + 1 << " ";
        }
        cout << endl;
        for (int i = 0; i < m; i++)
        {
            const int type = rand() % 3 + 1;
            cout << type << " ";
            if (type == 1 || type == 2)
            {
                int l = rand() % n + 1;
                int r = rand() % n + 1;
                if (l > r)
                    swap(l, r);
                cout << l << " " << r;
            }
            else
            {
                const int pos = rand() % n + 1;
                const int value = rand() % 50 + 1;
                cout << pos << " " << value;
            }
            cout << endl;
        }
        return 0;
    }

    factorialTable.push_back(0); // What's 0 factorial?
    int64_t factorial = 1;
    for (int64_t i = 1; i <= maxNonZeroFactorial; i++)
    {
        factorial = (factorial * i) % mod;
        factorialTable.push_back(factorial);
    }
    //for (int i = 0; i <= maxNonZeroFactorial; i++)
    //{
        //cout << "i: " << i << " factorialTable[i]: " << factorialTable[i] << endl;
    //}
    ios::sync_with_stdio(false);

    int64_t n, m;
    cin >> n >> m;

    vector<int64_t> A(n);
    for (int i = 0; i < n; i++)
    {
        cin >> A[i];
    }

    vector<Query> queries(m);
    for (int64_t i = 0; i < m; i++)
    {
        cin >> queries[i].type;
        cin >> queries[i].n1;
        cin >> queries[i].n2;
    }
    assert(cin);
    const auto resultsOptimised = results(A, queries);
    //cout << "results: " << endl;
    for (const auto result : resultsOptimised)
    {
        cout << result << endl;
    }
#ifdef BRUTE_FORCE
    cout << "results brute force:" << endl;
    const auto resultsBruteForce = bruteForce(A, queries, resultsOptimised);
    for (const auto result : resultsBruteForce)
    {
        cout << result << endl;
    }
    assert(resultsOptimised == resultsBruteForce);
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

