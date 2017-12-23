#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
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
    static FactorialHistogram fromValue(int value)
    {
        FactorialHistogram factorialHistogram;
        if (value <= maxNonZeroFactorial)
            factorialHistogram.numWithFactorial[value] = 1;
        
        return factorialHistogram;
    }
};

template <typename ValueType>
class SegmentTree
{
    public:
        SegmentTree(int maxNumber)
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
        }
        void setInitialValues(const vector<ValueType>& initialValues)
        {
            for (int i = 0; i < initialValues.size(); i++)
            {
                auto& cell = m_cellMatrix.back()[i];
                cell.value = initialValues[i];
                if (cell.parent)
                    cell.parent->setNeedsUpdateFromChildren();
            }
            m_cellMatrix.front().front().updateFromChildren();
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
            vector<Cell*> cells;
            collectMinCellsForRange(pos, pos, cells);
            assert(cells.size() == 1);
            auto cell = cells.front();
            cell->servicePendingOperations();
            const int valueFactorialIndex = (value <= maxNonZeroFactorial ? value : -1);

            // Erase old value from histogram.
            for (int i = 1; i <= maxNonZeroFactorial; i++)
            {
                if (cell->value.numWithFactorial[i] != 0)
                {
                    assert(!foundNonZero);
                    assert(cell->value.numWithFactorial[i] == 1);
                    cell->value.numWithFactorial[i] = 0;
                    break;
                }
            }
            // Add new value, if it is valid (i.e. - non-zero).
            if (valueFactorialIndex != -1)
            {
                cell->value.numWithFactorial[valueFactorialIndex] = 1;
            }

            if (cell->parent)
                cell->parent->setNeedsUpdateFromChildren();
            m_cellMatrix.front().front().updateFromChildren();
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
                    factorialSum = (factorialSum + (cell->value.numWithFactorial[i] * factorialTable[i]) % mod) % mod;
                }
            }
            assert(factorialSum >= 0 && factorialSum < mod);
            return factorialSum;
        }
    private:
        int64_t m_powerOf2BiggerThanMaxNumber;
        int m_exponentOfPowerOf2BiggerThanMaxNumber;
        int m_maxNumber;

        struct Cell
        {
            ValueType value;

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
                    if (leftChild && rightChild)
                    {
                        leftChild->addPendingOperation(pendingOperatorInfo);
                        rightChild->addPendingOperation(pendingOperatorInfo);
                    }
                    // Add "pendingOperatorInfo" to each "value" in the array - this has the effect of shifting the histogram
                    // pendingOperatorInfo elements to the right (i.e. if there were x elements with value y,
                    // and pendingOperatorInfo was 2, then there are now x elements with value y + 2 instead).
                    for (int i = maxNonZeroFactorial; i >= pendingOperatorInfo; i--)
                    {
                        value.numWithFactorial[i] = value.numWithFactorial[i - pendingOperatorInfo];
                    }
                    // The shift to the right should, obviously, zero out the "new" elements to the left :)
                    for (int i = 0; i < min(pendingOperatorInfo, maxNonZeroFactorial + 1) ; i++)
                    {
                        value.numWithFactorial[i] = 0;
                    }

                    hasPendingOperator = false;
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

                leftChild->updateFromChildren();
                rightChild->updateFromChildren();

                for (int i = 0; i <= maxNonZeroFactorial; i++)
                {
                    value.numWithFactorial[i] = leftChild->value.numWithFactorial[i] + rightChild->value.numWithFactorial[i];
                }
                needsUpdateFromChildren = false;
            }

        };

        vector<vector<Cell>> m_cellMatrix;

        void collectMinCellsForRange(int start, int end, vector<Cell*>& destCells)
        {
            collectMinCellsForRange(start, end, 0, m_powerOf2BiggerThanMaxNumber, destCells);
        }

        void collectMinCellsForRange(int start, int end, int cellRow, int powerOf2, vector<Cell*>& destCells)
        {
            if (cellRow == m_cellMatrix.size())
                return;
            if (cellRow != 0)
            {
                // Ensure all parents have serviced their pending operations.
                const int parentCellStartIndex = start / (powerOf2 * 2);
                const int parentCellEndIndex = end / (powerOf2 * 2);
                const int parentCellRow = cellRow - 1;
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
            }
            if (end + 1 - start < powerOf2)
            {
                // Gap between start and end is too small; recurse with next row.
                collectMinCellsForRange(start, end, cellRow + 1, powerOf2 >> 1, destCells);
                return;
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
            collectMinCellsForRange(start, powerOf2AfterStart - 1, cellRow + 1, powerOf2 >> 1, destCells); // Left region.
            collectMinCellsForRange(powerOf2BeforeEnd, end, cellRow + 1, powerOf2 >> 1, destCells); // Right region.
            return;
        }
};

struct Query
{
    int type;
    int64_t n1;
    int64_t n2;
};

vector<int64_t> findResults(const vector<int64_t>& A, const vector<Query>& queries)
{
    using FactorialTracker = SegmentTree<FactorialHistogram>;
    FactorialTracker factorialTracker(A.size());

    vector<FactorialHistogram> initialValues(A.size());
    for (int i = 0; i < A.size(); i++)
    {
        initialValues[i] = FactorialHistogram::fromValue(A[i]);
    }
    factorialTracker.setInitialValues(initialValues);
    vector<int64_t> results;

    for (const auto& query : queries)
    {
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
    }
    return results;
}

int main(int argc, char** argv)
{
    factorialTable.push_back(0); // What's 0 factorial? We don't have to care, thankfully :)
    int64_t factorial = 1;
    for (int64_t i = 1; i <= maxNonZeroFactorial; i++)
    {
        factorial = (factorial * i) % mod;
        factorialTable.push_back(factorial);
    }
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
    const auto results = findResults(A, queries);
    for (const auto result : results)
    {
        cout << result << endl;
    }
}

