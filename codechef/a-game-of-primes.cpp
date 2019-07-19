#include <iostream>
#include <vector>
#include <functional>
#include <cmath>


#include <cassert>

#include <sys/time.h>



#define VERIFY_SEGMENT_TREE

using namespace std;

template <typename ValueType, typename OperatorInfo>
class SegmentTree {
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
        void applyOperatorToAllInRange(int left, int right, bool setAllOn)
        {
            vector<Cell*> cells;
            collectMinCellsForRange(left, right, cells);
            for (auto cell : cells)
            {
                cell->addPendingOperation(setAllOn);
                cell->servicePendingOperations();
                if (cell->parent)
                    cell->parent->setNeedsUpdateFromChildren();
            }
            updateAllFromChildren();
#ifdef VERIFY_SEGMENT_TREE
            for (int i = left; i <= right; i++)
            {
                m_dbgValues[i] = setAllOn;
            }
#endif
        }
        ValueType combinedValuesInRange(int left, int right)
        {
            vector<Cell*> cells;
            collectMinCellsForRange(left, right, cells);
            ValueType combinedValuesInRange{};
            for (auto cell : cells)
            {
                cell->servicePendingOperations();
                combinedValuesInRange = m_combineValues(combinedValuesInRange, cell->value);
            }
#ifdef VERIFY_SEGMENT_TREE
            {
                ValueType dbgCombinedValuesInRange{};

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
        int m_maxNumber;
        CombineValues m_combineValues;
        ApplyOperator m_applyOperator;
        CombineOperators m_combineOperators;
        int64_t m_powerOf2BiggerThanMaxNumber;
        int m_exponentOfPowerOf2BiggerThanMaxNumber;

#ifdef VERIFY_SEGMENT_TREE
        vector<ValueType> m_dbgValues;
#endif

        struct Cell
        {
            SegmentTree* container = nullptr;
            ValueType value{};

            int64_t rangeBegin = -1;
            int64_t rangeEnd = -1;
            Cell* parent = nullptr;
            Cell* leftChild = nullptr;
            Cell* rightChild = nullptr;

            bool allOn = false;

            bool pendingIsSetAllOn = false;
            bool hasPendingOperator = false;
            bool needsUpdateFromChildren = false;

            void addPendingOperation(bool setAllOn)
            {
                hasPendingOperator = true;
                pendingIsSetAllOn = setAllOn;
            }

            void servicePendingOperations()
            {
                if (hasPendingOperator)
                {
                    if (leftChild && rightChild)
                    {
                        leftChild->addPendingOperation(pendingIsSetAllOn);
                        rightChild->addPendingOperation(pendingIsSetAllOn);
                    }

                    if (pendingIsSetAllOn)
                    {
                        value = rangeEnd - rangeBegin + 1;
                    }
                    else
                    {
                        value = 0;
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

                value = leftChild->value + rightChild->value;

                needsUpdateFromChildren = false;
            }

        };

        vector<vector<Cell>> m_cellMatrix;

        // Collect O(log2(end - start + 1)) cells in range-order that span the interval start-end (inclusive).
        void collectMinCellsForRange(const int start, const int end, vector<Cell*>& destCells)
        {
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

struct Query
{
    char queryType;
    int l = -1;
    int r = -1;
    int value = -1;
};

vector<int> solveBruteForce(const vector<Query>& queries, int64_t K, const vector<int>& primesThatDivideK)
{
    const int maxRangeEnd = 100'000;
    vector<int64_t> values(maxRangeEnd + 1, -1);
    vector<int> queryResults;
    for (const auto& query : queries)
    {
        if (query.queryType == '!')
        {
            for (int i = query.l; i <= query.r; i++)
            {
                values[i] = query.value;
            }
        }
        else if (query.queryType == '?')
        {
            int num = 0;
            for (int primeFactorOfKIndex = 0; primeFactorOfKIndex < primesThatDivideK.size(); primeFactorOfKIndex++)
            {
                bool hasFactor = false;
                for (int i = query.l; i <= query.r; i++)
                {
                    if (values[i] == -1)
                        continue;
                    if ((values[i] % primesThatDivideK[primeFactorOfKIndex]) == 0)
                    {
                        hasFactor = true;
                        break;
                    }
                }
                if (hasFactor)
                {
                    num++;
                }
            }
            queryResults.push_back(num);
        }
    }
    return queryResults;
}

vector<int> solveOptimised(const vector<Query>& queries, int64_t K, const vector<int>& primesThatDivideK)
{
    vector<int> queryResults;
    const int maxRangeEnd = 100'000;

    auto combineSetValue = [](const int64_t& newValueToSetTo, const int64_t& earlierValueToSetTo)
    {
        return newValueToSetTo;
    };

    auto combineValues = [](const int& lhsValue, const int& rhsValue)
    {
        return lhsValue + rhsValue;
    };

    auto applySetValue = [](int64_t valueToSetTo, int& value)
    {
        value = valueToSetTo;
    };

    using NumPrimesTracker = SegmentTree<int, int64_t>;
    vector<NumPrimesTracker> numWithPrimeFactorOfKTracker;
    numWithPrimeFactorOfKTracker.reserve(primesThatDivideK.size());


    for (int i = 0; i < primesThatDivideK.size(); i++)
    {
        numWithPrimeFactorOfKTracker.emplace_back(maxRangeEnd, combineValues, applySetValue, combineSetValue);
        numWithPrimeFactorOfKTracker.back().setInitialValues(vector<int>(maxRangeEnd, 0));
    }
    for (const auto& query : queries)
    {
        if (query.queryType == '!')
        {
            for (int primeFactorOfKIndex = 0; primeFactorOfKIndex < primesThatDivideK.size(); primeFactorOfKIndex++)
            {
                if ((query.value % primesThatDivideK[primeFactorOfKIndex]) == 0)
                    numWithPrimeFactorOfKTracker[primeFactorOfKIndex].applyOperatorToAllInRange(query.l, query.r, true);
                else
                    numWithPrimeFactorOfKTracker[primeFactorOfKIndex].applyOperatorToAllInRange(query.l, query.r, false);
            }
        }
        else if (query.queryType == '?')
        {
            int num = 0;
            for (int primeFactorOfKIndex = 0; primeFactorOfKIndex < primesThatDivideK.size(); primeFactorOfKIndex++)
            {
                if (numWithPrimeFactorOfKTracker[primeFactorOfKIndex].combinedValuesInRange(query.l, query.r) > 0)
                    num++;
            }
            queryResults.push_back(num);
        }
        else
        {
            assert(false);
        }
    }
    return queryResults;
}

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int K = rand() % 1'000'000'000ULL + 1;
        const int Q = rand() % 100 + 1;
        const int maxR = rand() % 1000 + 2;
        const int maxX = rand() % 1'000'000'000ULL + 1;

        cout << K << " " << Q << endl;

        for (int q = 0; q < Q; q++)
        {
            if (rand() % 2 == 0)
            {
                cout << "! ";
                int l = rand() % maxR + 1;
                int r = rand() % maxR + 1;
                if (r < l)
                    swap(l ,r);
                cout << l << " " << r << " " << (rand() % maxX + 1) << endl;
            }
            else
            {
                cout << "? ";
                int l = rand() % maxR + 1;
                int r = rand() % maxR + 1;
                if (r < l)
                    swap(l ,r);
                cout << l << " " << r << endl;
            }
        }
        return 0;
    }

    const int64_t maxXorK = 1'000'000'000ULL;
    const int sqrtMaxXorK = sqrt(maxXorK);

    vector<int> primesUpToSqrtMaxXorK;
    vector<bool> isPrime(sqrtMaxXorK + 1, true);

    for (int factor = 2; factor <= sqrtMaxXorK; factor++)
    {
        if (isPrime[factor])
        {
            primesUpToSqrtMaxXorK.push_back(factor);
            for (int multiple = factor; multiple < isPrime.size(); multiple += factor)
            {
                isPrime[multiple] = false;
            }
        }
    }


    int64_t K;
    cin >> K;

    int Q;
    cin >> Q;

    vector<int> primesThatDivideK;
    for (const auto prime : primesUpToSqrtMaxXorK)
    {
        bool addedAlready = false;
        while ((K % prime) == 0)
        {
            K /= prime;
            if (!addedAlready)
            {
                primesThatDivideK.push_back(prime);
                addedAlready = true;
            }
        }
    }

    if (K != 1)
    {
        // If value of K after being divided by all its prime divisors <= sqrt(K)
        // is not 1, then the remaining value of K is prime (otherwise: it would have
        // at least two prime factors, P and Q.   But P, Q must be > sqrt(K) else
        // they would have been divided out, and so P x Q > K, a contradiction).
        primesThatDivideK.push_back(K);
    }


    vector<Query> queries(Q);

    for (int q = 0; q < Q; q++)
    {
        cin >> queries[q].queryType;

        if (queries[q].queryType == '!')
        {
            cin >> queries[q].l;
            cin >> queries[q].r;
            queries[q].l--;
            queries[q].r--;
            cin >> queries[q].value;
        }
        else if (queries[q].queryType == '?')
        {
            cin >> queries[q].l;
            cin >> queries[q].r;
            queries[q].l--;
            queries[q].r--;
        }
        else
        {
            assert(false);
        }
    }

    const auto solutionBruteForce = solveBruteForce(queries, K, primesThatDivideK);
    cout << " solutionBruteForce: ";
    for (const auto x : solutionBruteForce)
    {
        cout << x << " ";
    }
    cout << endl;
    const auto solutionOptimised = solveOptimised(queries, K, primesThatDivideK);

    cout << " solutionOptimised:  ";
    for (const auto x : solutionOptimised)
    {
        cout << x << " ";
    }
    cout << endl;
    assert(solutionOptimised == solutionBruteForce);


}
