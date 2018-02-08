// Simon St James (ssjgz) - 2018-02-08
// This is a Brute Force solution for testing only - not expected to pass, yet!
#define BRUTE_FORCE
#define VERIFY_SEGMENT_TREE
#define SUBMISSION
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
            : m_maxNumber{maxNumber}, m_combineValues{combineValues}, m_applyOperator{applyOperator}, m_combineOperators{combineOperators}
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
            for (int cellRow = 0; cellRow < m_cellMatrix.size() - 1; cellRow++)
            {
                int childCellIndex = 0;
                for (int cellCol = 0; cellCol < m_cellMatrix[cellRow].size(); cellCol++)
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
            for (int i = 1; i < destCells.size(); i++)
            {
                assert(destCells[i]->rangeBegin == destCells[i - 1]->rangeEnd + 1);
            }
#endif
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
    const int n = heights.size();
    //cout << "n: " << n << endl;
    vector<int64_t> minCostStartingWithStudent(n);
    vector<int64_t> d(n);
    //cout << "prices.back(): " << prices.back() << endl;
    minCostStartingWithStudent.back() = 1; // Just run instantly to finish line.
    d.back() = 1 + prices.back();
    //cout << "d[" << (n  - 1) << "] = " << d[n - 1] << endl;

    int64_t heightDifferential = 0;
    for (int i = n - 2; i >= 0; i--)
    {
        auto costIfPassedToStudent = [i, &heights, &prices, &minCostStartingWithStudent](const int nextStudent)
        {
            const int64_t costIfPassedToNextStudent = (nextStudent - i) // Cost of running to student.
                + prices[nextStudent] // Student charge
                + abs(heights[nextStudent] - heights[i]) // Height difference (time taken to exchange).
                + minCostStartingWithStudent[nextStudent]; 
            return costIfPassedToNextStudent;
        };
        //cout << "i: " << i << endl;
        int64_t minCostStartingHere = numeric_limits<int64_t>::max();
        int nextStudentChosen = -1;
        int indexOfTaller = -1;
        for (int nextStudent = i + 1; nextStudent < n; nextStudent++)
        {
            const auto costIfPassedToNextStudent = costIfPassedToStudent(nextStudent);
            if (costIfPassedToNextStudent < minCostStartingHere)
            {
                minCostStartingHere = costIfPassedToNextStudent;
                nextStudentChosen = nextStudent;
                //cout << " chosen nextStudent = " << nextStudent << " for cost " << costIfPassedToNextStudent << endl;
            }
#if 0
            if (d[nextStudent] < minD 
                    && (heights[nextStudent] <= heights[i])
               )
            {
                minDIndex = nextStudent;
                minD = d[nextStudent];
                cout << "Bleep: " << nextStudent << " minD: " << minD << endl;
                dbgMinCost = costIfPassedToNextStudent;
            }
            if (nextStudent == n - 1 && (heights[nextStudent] <= heights[i]))
            {
                if (costIfPassedToNextStudent < dbgMinCost)
                {
                    dbgMinCost = costIfPassedToNextStudent;
                    minDIndex = nextStudent;
                }
            }
#endif
            //cout << " nextStudent: " << nextStudent << " minCostStartingHere becomes " << minCostStartingHere << endl;
            if (heights[nextStudent] > heights[i])
            {
                //cout << "  Forced exchange!" << endl;
                //forcedExchange = true;
                indexOfTaller = nextStudent;
                //cout << " chosen (forced) nextStudent = " << nextStudent << " for cost " << costIfPassedToNextStudent << endl;
#if 0
                if (costIfPassedToNextStudent < dbgMinCost)
                {
                    minDIndex = nextStudent;
                    dbgMinCost = min(dbgMinCost, costIfPassedToNextStudent);
                }
#endif
                break;
            }
        }
        const bool forcedExchange = (indexOfTaller != -1);
        int minDIndex = -1;
        int64_t minD = numeric_limits<int64_t>::max();
        int64_t dbgMinCost = numeric_limits<int64_t>::max();
        const int lastStudentIndex =  (indexOfTaller == -1 ? n - 1 : indexOfTaller - 1);
        for (int nextStudent = i + 1; nextStudent <= lastStudentIndex; nextStudent++)
        {
            assert(heights[nextStudent] <= heights[i]);
            if (d[nextStudent] <= minD)
            {
                minD = d[nextStudent];
                minDIndex = nextStudent;
                dbgMinCost = min(dbgMinCost, costIfPassedToStudent(nextStudent));
                //cout << "Bleep: " << nextStudent << " minD: " << minD << endl;
            }
        }
        if (indexOfTaller != -1)
        {
            dbgMinCost = min(dbgMinCost, costIfPassedToStudent(indexOfTaller));
        }
        if (!forcedExchange)
        {
            // Run to finish line.
            minCostStartingHere = min(minCostStartingHere, static_cast<int64_t>(n - i));
            dbgMinCost = min(dbgMinCost, static_cast<int64_t>(n - i));
            //cout << " Run straight to finish line" << endl;
        }
        minCostStartingWithStudent[i] = minCostStartingHere;
        heightDifferential += heights[i + 1] - heights[i];
        d[i] = minCostStartingWithStudent[i] + 
            (prices[i]) 
            - (n - i) + 
            heightDifferential;
        //cout << " minCostStartingHere: " << minCostStartingHere << " d[" << i<< "] = " << d[i] << endl;
        for (int j = i + 1; j < n; j++)
        {
#if 0
            if (d[j] < minD)
            {
                minDIndex = j;
                minD = d[j];
            }
            if (heights[j] > heights[i])
                break;
#endif
        }
        //cout << "forcedExchange: " << forcedExchange << " nextStudentChosen: " << nextStudentChosen << " minDIndex: " << minDIndex << " minCostStartingHere: " << minCostStartingHere << " dbgMinCost: " << dbgMinCost << endl;
        assert(dbgMinCost == minCostStartingHere);



    }

    for (int i = 0; i < n; i++)
    {
        //cout << "minCostStartingWithStudent[" << i << "] = " << minCostStartingWithStudent[i] << endl;
    }

    for (int i = 0; i < n; i++)
    {
        //cout << "minCostStartingWithStudent[" << i << "] = " << minCostStartingWithStudent[i] << endl;
        //cout << "d[" << i << "] = " << d[i] << endl;
    }

    return minCostStartingWithStudent;
}

vector<int64_t> minCost(const vector<int64_t>& heights, const vector<int64_t>& prices)
{
    const int n = heights.size();
    //cout << "n: " << n << endl;
    vector<int64_t> minCostStartingWithStudent(n);
    vector<int64_t> d(n);
    //cout << "prices.back(): " << prices.back() << endl;
    minCostStartingWithStudent.back() = 1; // Just run instantly to finish line.
    d.back() = 1 + prices.back();
    //cout << "d[" << (n  - 1) << "] = " << d[n - 1] << endl;

    auto combineValues = [](const auto& x, const auto& y)
    {
        return min(x, y);
    };
    auto applyOperator = [](auto blah, const auto& blee)
    {
        assert(false);
    };
    auto combineOperators = [](const auto& blah, const auto& blee)
    {
        assert(false);
        return 0;
    };
    using MinTree = SegmentTree<int64_t, int>;
    MinTree minTree(n + 1, combineValues, applyOperator, combineOperators);
    //cout << "fleep: " << d[n - 1] << endl;
    minTree.setValue(n - 1, d[n - 1]);
#if 0

    minTree.setValue(0, 5);
    minTree.setValue(1, 5);
    minTree.setValue(2, -1);
    minTree.setValue(3, -3);
    minTree.setValue(4, -4);

    cout << "min 0-1 " << minTree.combinedValuesInRange(0, 1, std::numeric_limits<int64_t>::max()) << endl;
    cout << "min 0-2 " << minTree.combinedValuesInRange(0, 2, std::numeric_limits<int64_t>::max()) << endl;
    cout << "min 0-3 " << minTree.combinedValuesInRange(0, 3, std::numeric_limits<int64_t>::max()) << endl;
    cout << "min 1-3 " << minTree.combinedValuesInRange(1, 3, std::numeric_limits<int64_t>::max()) << endl;
    cout << "min 2-3 " << minTree.combinedValuesInRange(2, 3, std::numeric_limits<int64_t>::max()) << endl;
    cout << "min 2-2 " << minTree.combinedValuesInRange(2, 2, std::numeric_limits<int64_t>::max()) << endl;
    cout << "min 3-4 " << minTree.combinedValuesInRange(3, 4, std::numeric_limits<int64_t>::max()) << endl;
#endif
    map<int64_t, int> indexOfNextStudentWithHeight;
    vector<int> indexOfNextTallerStudent(n, -1);
    
    for (int i = n - 1; i >= 0; i--)
    {
        auto tallerIndexIter = indexOfNextStudentWithHeight.upper_bound(heights[i]);
        if (tallerIndexIter != indexOfNextStudentWithHeight.end())
        {
            indexOfNextTallerStudent[i] = tallerIndexIter->second;
        }

        indexOfNextStudentWithHeight[heights[i]] = i;
    }

    int64_t heightDifferential = 0;
    for (int i = n - 2; i >= 0; i--)
    {
        auto costIfPassedToStudent = [i, &heights, &prices, &minCostStartingWithStudent](const int nextStudent)
        {
            const int64_t costIfPassedToNextStudent = (nextStudent - i) // Cost of running to student.
                + prices[nextStudent] // Student charge
                + abs(heights[nextStudent] - heights[i]) // Height difference (time taken to exchange).
                + minCostStartingWithStudent[nextStudent]; 
            return costIfPassedToNextStudent;
        };
        //cout << "i: " << i << endl;
        int tallerStudentIndex = -1;
        for (int j = i + 1; j < n; j++)
        {
            if (heights[j] > heights[i])
            {
                tallerStudentIndex = j;
                break;
            }
        }
        assert(tallerStudentIndex == indexOfNextTallerStudent[i]);

        //cout << "tallerStudentIndex: " << tallerStudentIndex << endl;
        int64_t minD = numeric_limits<int64_t>::max();
        int64_t minCostStartingHere = numeric_limits<int64_t>::max();
        const auto rangeMin = i + 1;
        const auto rangeMax = tallerStudentIndex == -1 ? n - 1 : tallerStudentIndex - 1;
        //cout << "i: " << i << " rangeMin: " << rangeMin << " rangeMax: " << rangeMax << endl;
        if (rangeMax >= rangeMin)
        {
            minD = minTree.combinedValuesInRange(rangeMin, rangeMax, std::numeric_limits<int64_t>::max());
            int bestStudentIndex = -1;
            for (int j = rangeMin; j <= rangeMax; j++)
            {
                if (d[j] == minD)
                {
                    bestStudentIndex = j;
                }
            }
            assert(bestStudentIndex != -1);
            minCostStartingHere = min(minCostStartingHere, costIfPassedToStudent(bestStudentIndex));
        }


        const bool forcedExchange = (tallerStudentIndex != -1);
        if (!forcedExchange)
        {
            // Run to finish line.
            minCostStartingHere = min(minCostStartingHere, static_cast<int64_t>(n - i));
        }
        else
        {
            minCostStartingHere = min(minCostStartingHere, costIfPassedToStudent(tallerStudentIndex));
        }
        minCostStartingWithStudent[i] = minCostStartingHere;

        heightDifferential += heights[i + 1] - heights[i];
        d[i] = minCostStartingWithStudent[i] + 
            (prices[i]) 
            - (n - i) + 
            heightDifferential;
        minTree.setValue(i, d[i]);

    }

    return minCostStartingWithStudent;
}



int main(int argc, char** argv)
{
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int N = rand() % 20 + 1;
        const int maxHeight = 5;
        const int maxPrice = 5;
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

#if 0
    cout << "heights: " << endl;
    for (const auto x : heights)
        cout << x << " ";
    cout << endl;
    cout << "prices: " << endl;
    for (const auto x : prices)
        cout << x << " ";
    cout << endl;
#endif
    const auto result = minCost(heights, prices);
    cout << result[0] << endl;

#ifdef BRUTE_FORCE
    const auto resultBruteForce = minCostBruteForce(heights, prices);
    //cout << "resultBruteForce: " << resultBruteForce << endl;
    cout << "resultBruteForce: " << resultBruteForce[0] << " result: " << result[0] << endl;
    assert(resultBruteForce == result);
#endif

}
