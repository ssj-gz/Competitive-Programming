// Simon St James (ssjgz) - 2018-02-08 14:36
#define BRUTE_FORCE
#define VERIFY_SEGMENT_TREE
//#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#undef BRUTE_FORCE
#undef VERIFY_SEGMENT_TREE
#endif
#include <iostream>
#include <vector>
#include <limits>
#include <functional>
#include <map>
#include <algorithm>
#include <sys/time.h>
#include <cassert>

using namespace std;

template <typename ValueType, typename OperatorInfo>
class SegmentTree
{
    public:

        using CombineValues = std::function<ValueType(const ValueType& lhs, const ValueType& rhs)>;
        using ApplyOperator = std::function<void(OperatorInfo operatorInfo, ValueType& value)>;
        using CombineOperators = std::function<OperatorInfo(const OperatorInfo& lhs, const OperatorInfo& rhs)>;

        SegmentTree(int maxNumber, CombineValues combineValues, ApplyOperator applyOperator, CombineOperators combineOperators)
            : m_combineValues{combineValues}, m_applyOperator{applyOperator}, m_combineOperators{combineOperators}, m_maxNumber{maxNumber}
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
                    auto& cell = m_cellMatrix.back()[cellCol];
                    cell.container = this;
                    cell.rangeBegin = rangeBegin;
                    cell.rangeEnd = rangeBegin + powerOf2 - 1;
                    assert(cell.rangeEnd >= cell.rangeBegin);
                    rangeBegin += powerOf2;
                }
                numCellsInThisRow *= 2;
                powerOf2 /= 2;
            }
            // Cell matrix parent/child.
            for (std::size_t cellRow = 0; cellRow < m_cellMatrix.size() - 1; cellRow++)
            {
                int childCellIndex = 0;
                for (std::size_t cellCol = 0; cellCol < m_cellMatrix[cellRow].size(); cellCol++)
                {
                    auto& cell = m_cellMatrix[cellRow][cellCol];
                    cell.leftChild = &(m_cellMatrix[cellRow + 1][childCellIndex]);
                    cell.leftChild->parent = &cell;
                    childCellIndex++;
                    cell.rightChild = &(m_cellMatrix[cellRow + 1][childCellIndex]);
                    cell.rightChild->parent = &cell;
                    childCellIndex++;
                }
            }
#ifdef VERIFY_SEGMENT_TREE
            m_dbgValues.resize(maxNumber);
#endif
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
            updateAllFromChildren();
#ifdef VERIFY_SEGMENT_TREE
            m_dbgValues = initialValues;
#endif
        }
        void applyOperatorToAllInRange(int left, int right, OperatorInfo operatorInfo)
        {
            vector<Cell*> cells;
            collectMinCellsForRange(left, right, cells);
            for (auto cell : cells)
            {
                cell->addPendingOperation(operatorInfo);
                cell->servicePendingOperations();
                if (cell->parent)
                    cell->parent->setNeedsUpdateFromChildren();
            }
            updateAllFromChildren();
#ifdef VERIFY_SEGMENT_TREE
            for (int i = left; i <= right; i++)
            {
                m_applyOperator(operatorInfo, m_dbgValues[i]);
            }
#endif
        }
        void setValue(int pos, const ValueType& newValue)
        {
            vector<Cell*> cells;
            collectMinCellsForRange(pos, pos, cells);
            assert(cells.size() == 1);
            auto cell = cells.front();
            cell->servicePendingOperations();
            cell->value = newValue;

            if (cell->parent)
                cell->parent->setNeedsUpdateFromChildren();
            updateAllFromChildren();
#ifdef VERIFY_SEGMENT_TREE
            m_dbgValues[pos] = newValue;
#endif
        }
        ValueType combinedValuesInRange(int left, int right, const ValueType& initialValue = ValueType())
        {
            vector<Cell*> cells;
            collectMinCellsForRange(left, right, cells);
            ValueType combinedValuesInRange{initialValue};
            for (auto cell : cells)
            {
                cell->servicePendingOperations();
                combinedValuesInRange = m_combineValues(combinedValuesInRange, cell->value);
            }
#ifdef VERIFY_SEGMENT_TREE
            {
                ValueType dbgCombinedValuesInRange(initialValue);

                for (int i = left; i <= right; i++)
                {
                    dbgCombinedValuesInRange = m_combineValues(dbgCombinedValuesInRange, m_dbgValues[i]);
                }

                assert(dbgCombinedValuesInRange == combinedValuesInRange);
            }
#endif
            return combinedValuesInRange;
        }
    private:
        CombineValues m_combineValues;
        ApplyOperator m_applyOperator;
        CombineOperators m_combineOperators;
        int64_t m_powerOf2BiggerThanMaxNumber;
        int m_exponentOfPowerOf2BiggerThanMaxNumber;
        int m_maxNumber;

#ifdef VERIFY_SEGMENT_TREE
        vector<ValueType> m_dbgValues;
#endif

        struct Cell
        {
            SegmentTree* container = nullptr;
            ValueType value;

            int64_t rangeBegin = -1;
            int64_t rangeEnd = -1;
            Cell* parent = nullptr;
            Cell* leftChild = nullptr;
            Cell* rightChild = nullptr;

            OperatorInfo pendingOperatorInfo = 0;
            bool hasPendingOperator = false;
            bool needsUpdateFromChildren = false;

            void addPendingOperation(OperatorInfo operatorInfo)
            {
                if (!hasPendingOperator)
                {
                    hasPendingOperator = true;
                    pendingOperatorInfo = operatorInfo;
                }
                else
                {
                    pendingOperatorInfo = container->m_combineOperators(operatorInfo, pendingOperatorInfo);
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

                    container->m_applyOperator(pendingOperatorInfo, value);

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

                value = container->m_combineValues(leftChild->value, rightChild->value);

                needsUpdateFromChildren = false;
            }

        };

        vector<vector<Cell>> m_cellMatrix;

        // Collect O(log2(end - start + 1)) cells in range-order that span the interval start-end (inclusive).
        void collectMinCellsForRange(const int start, const int end, vector<Cell*>& destCells)
        {
            if (end < start)
                return;
            collectMinCellsForRange(start, end, 0, m_powerOf2BiggerThanMaxNumber, destCells);
#ifdef VERIFY_SEGMENT_TREE
            assert(destCells.front()->rangeBegin == start);
            assert(destCells.back()->rangeEnd == end);
            for (std::size_t i = 1; i < destCells.size(); i++)
            {
                assert(destCells[i]->rangeBegin == destCells[i - 1]->rangeEnd + 1);
            }
#endif
        }

        void collectMinCellsForRange(int start, int end, std::size_t cellRow, int powerOf2, vector<Cell*>& destCells)
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
                Cell* cellToAdd = nullptr;
                if (start <= end - powerOf2 + 1)
                {
                    cellToAdd = &(m_cellMatrix[cellRow][end/powerOf2]);
                    end -= powerOf2;
                }
                collectMinCellsForRange(start, end, cellRow + 1, powerOf2 >> 1, destCells);
                if (cellToAdd)
                    destCells.push_back(cellToAdd);
                return;
            }
            // Neither start nor end is a multiple of powerOf2 ... sum up the complete cells in between (if any) ...
            const int powerOf2AfterStart = (start / powerOf2 + 1) * powerOf2;
            const int powerOf2BeforeEnd =  (end / powerOf2) * powerOf2;
            Cell* middleCellToAdd = nullptr;
            // ... make a note to add the complete cell in between (if there is one) ...
            if (powerOf2AfterStart < powerOf2BeforeEnd)
            {
                middleCellToAdd = &(m_cellMatrix[cellRow][powerOf2AfterStart / powerOf2]);
            }
            // ... and then split into two, and recurse: for each of two split regions, at least one of the start or end will be a multiple of powerOf2, so they
            // will not split further.
            collectMinCellsForRange(start, powerOf2AfterStart - 1, cellRow + 1, powerOf2 >> 1, destCells); // Left region.
            if (middleCellToAdd)
                destCells.push_back(middleCellToAdd);
            collectMinCellsForRange(powerOf2BeforeEnd, end, cellRow + 1, powerOf2 >> 1, destCells); // Right region.
            return;
        }

        void updateAllFromChildren()
        {
            m_cellMatrix.front().front().updateFromChildren();
        }
};

vector<int64_t> minCostBruteForce(const vector<int64_t>& heights, const vector<int64_t>& prices)
{
    // An O(N^2) algorithm provided as a reference implementation.
    const int n = heights.size();
    vector<int64_t> minCostStartingWithStudent(n);
    minCostStartingWithStudent.back() = 1; // Just run instantly to finish line.

    for (int studentIndex = n - 2; studentIndex >= 0; studentIndex--)
    {
        auto costIfPassedToStudent = [studentIndex, &heights, &prices, &minCostStartingWithStudent](const int nextStudentIndex)
        {
            const int64_t costIfPassedToNextStudent = (nextStudentIndex - studentIndex) // Cost of running to student.
                + prices[nextStudentIndex] // Student charge
                + abs(heights[nextStudentIndex] - heights[studentIndex]) // Height difference (time taken to exchange).
                + minCostStartingWithStudent[nextStudentIndex]; 
            return costIfPassedToNextStudent;
        };
        int64_t minCostStartingHere = numeric_limits<int64_t>::max();
        bool forcedExchange = false;
        for (int nextStudentIndex = studentIndex + 1; nextStudentIndex < n; nextStudentIndex++)
        {
            const auto costIfPassedToNextStudent = costIfPassedToStudent(nextStudentIndex);
            if (costIfPassedToNextStudent < minCostStartingHere)
            {
                minCostStartingHere = costIfPassedToNextStudent;
            }
            if (heights[nextStudentIndex] > heights[studentIndex])
            {
                forcedExchange = true;
                break;
            }
        }
        if (!forcedExchange)
        {
            // Run to finish line.
            minCostStartingHere = min(minCostStartingHere, static_cast<int64_t>(n - studentIndex));
        }
        minCostStartingWithStudent[studentIndex] = minCostStartingHere;
    }

    return minCostStartingWithStudent;
}

vector<int64_t> minCost(const vector<int64_t>& heights, const vector<int64_t>& prices)
{
    const int n = heights.size();
    vector<int64_t> minCostStartingWithStudent(n);

    minCostStartingWithStudent.back() = 1; // Just run instantly to finish line.

    // The d array is a rather abstract thing intended to make choosing the 
    // best student to hand to easier to find: it can be shown that
    //
    //   d[i] < d[j] if and only if cost of handing to student i < cost of handing to student j
    //
    // (and the same for ==, >, etc).
    vector<int64_t> d(n);
    d.back() = minCostStartingWithStudent.back() + prices.back();
    map<int64_t, int> nextIndexOfDWithValue;
    nextIndexOfDWithValue[d[n - 1]] = n - 1;

    auto minimumOfValues = [](const auto& x, const auto& y)
    {
        return min(x, y);
    };
    auto minDApplyOperatorUnused = [](auto, const auto&)
    {
        assert(false);
    };
    auto minDCombineOperatorsUnused = [](const auto&, const auto&)
    {
        assert(false);
        return 0;
    };
    using MinTree = SegmentTree<int64_t, int>;
    // Allow us to find the minimum value of d for a given range of students.
    MinTree minDTree(n + 1, minimumOfValues, minDApplyOperatorUnused, minDCombineOperatorsUnused);
    minDTree.setValue(n - 1, d[n - 1]);

    vector<int> indexOfNextTallerStudent(n, -1);
    {
        // Build up indexOfNextTallerStudent in O(nlogn).
        // We do this by compressing heights so that they are in the range 0 ... n (rather than 0 - 10^9)
        // so that the given heights H1 and H2 with corresponding compressed height indicies hi1 and hi2,
        // H1 < H2 if and only if hi1 < hi2 (and same for =, >, etc).
        map<int64_t, int> heightToCompressedIndex;
        vector<int64_t> heightsSorted(heights);
        sort(heightsSorted.begin(), heightsSorted.end());
        int compressedHeightIndex = 0;
        for (const auto height : heightsSorted)
        {
            auto iter = heightToCompressedIndex.find(height);
            if (iter == heightToCompressedIndex.end())
            {
                heightToCompressedIndex[height] = compressedHeightIndex;
                compressedHeightIndex++;
            }
        }

        map<int64_t, int> nextIndexOfStudentWithCompressedHeightIndex;
        for (int i = n - 1; i >= 0; i--)
        {
            const auto thisCompressedHeightIndex = heightToCompressedIndex[heights[i]];
            if (!nextIndexOfStudentWithCompressedHeightIndex.empty())
            {
                auto iter = nextIndexOfStudentWithCompressedHeightIndex.begin();
                while (iter != nextIndexOfStudentWithCompressedHeightIndex.end() && iter->first <= thisCompressedHeightIndex)
                {
                    iter = nextIndexOfStudentWithCompressedHeightIndex.erase(iter);
                }

                if (iter != nextIndexOfStudentWithCompressedHeightIndex.end())
                {
                    indexOfNextTallerStudent[i] = iter->second;
                }
            }
            nextIndexOfStudentWithCompressedHeightIndex[thisCompressedHeightIndex] = i;
        }
    }
    
    int64_t heightDifferential = 0;
    for (int studentIndex = n - 2; studentIndex >= 0; studentIndex--)
    {
        auto costIfPassedToStudent = [studentIndex, &heights, &prices, &minCostStartingWithStudent](const int nextStudentIndex)
        {
            const int64_t costIfPassedToNextStudent = (nextStudentIndex - studentIndex) // Cost of running to student.
                + prices[nextStudentIndex] // Student charge
                + abs(heights[nextStudentIndex] - heights[studentIndex]) // Height difference (time taken to exchange).
                + minCostStartingWithStudent[nextStudentIndex]; 
            return costIfPassedToNextStudent;
        };
        const int tallerStudentIndex = indexOfNextTallerStudent[studentIndex];

        int64_t minCostStartingHere = numeric_limits<int64_t>::max();
        const auto rangeMin = studentIndex + 1;
        const auto rangeMax = tallerStudentIndex == -1 ? n - 1 : tallerStudentIndex - 1;
        if (rangeMax >= rangeMin)
        {
            const auto minD = minDTree.combinedValuesInRange(rangeMin, rangeMax, numeric_limits<int64_t>::max());
            const int bestUnforcedPassStudent = nextIndexOfDWithValue[minD];
            assert(bestUnforcedPassStudent >= studentIndex + 1);
            minCostStartingHere = min(minCostStartingHere, costIfPassedToStudent(bestUnforcedPassStudent));
        }

        const bool forcedExchange = (tallerStudentIndex != -1);
        if (!forcedExchange)
        {
            // Run to finish line.
            minCostStartingHere = min(minCostStartingHere, static_cast<int64_t>(n - studentIndex));
        }
        else
        {
            minCostStartingHere = min(minCostStartingHere, costIfPassedToStudent(tallerStudentIndex));
        }
        minCostStartingWithStudent[studentIndex] = minCostStartingHere;

        // Update d.
        heightDifferential += heights[studentIndex + 1] - heights[studentIndex];
        d[studentIndex] = minCostStartingWithStudent[studentIndex] + 
            (prices[studentIndex])      // Equalise price comparison.
            - (n - 1 - studentIndex) +  // Equalise "cost to run to".
            heightDifferential; // Equalise height-difference (time to exchange) cost comparison.
        minDTree.setValue(studentIndex, d[studentIndex]);
        nextIndexOfDWithValue[d[studentIndex]] = studentIndex;
    }

    return minCostStartingWithStudent;
}



int main(int argc, char** argv)
{
    if (argc == 2)
    {
        // Generate randomised testcase.
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int maxMaxHeight = 10000;
        const int maxMaxPrice = 10000;

        const int N = rand() % 2000 + 1;
        const int maxHeight = rand() % maxMaxHeight + 1;
        const int maxPrice = rand() % maxMaxPrice + 1;
        const int minPrice = -maxPrice;

        cout << N << endl;
        for (int i = 0; i < N; i++)
        {
            cout << rand() % maxHeight << " "; 
        }
        cout << endl;
        for (int i = 0; i < N - 1; i++)
        {
            cout << (rand() % (maxPrice - minPrice + 1) + minPrice) << " ";
        }
        return 0;

    }

    int N;
    cin >> N;

    vector<int64_t> heights(N);

    // Treat Madison's height as heights[0].
    for (int i = 0; i < N; i++)
    {
        cin >> heights[i];
    }

    vector<int64_t> prices(N);
    prices[0] = 0; // No charge for Madison.

    for (int i = 1; i <= N - 1; i++)
    {
        cin >> prices[i];
    }
    const auto result = minCost(heights, prices);
    cout << result[0] << endl;

#ifdef BRUTE_FORCE
    const auto resultBruteForce = minCostBruteForce(heights, prices);
    cout << "resultBruteForce: " << resultBruteForce[0] << " result: " << result[0] << endl;
    assert(resultBruteForce == result);
#endif

}
