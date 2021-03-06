// Simon St James (ssjgz) - 2017-12-27 11:54
#define VERIFY_SEGMENT_TREE
#define SUBMISSION
#ifdef SUBMISSION
#undef VERIFY_SEGMENT_TREE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <array>
#include <functional>
#include <cassert>

using namespace std;

struct QuadrantHistogram
{
    enum Quadrant { TopRight, TopLeft, BottomLeft, BottomRight }; // In the order specified in the problem i.e. "first quadrant" is Top-Right; "second quadrant" Top-Left, etc.
    static constexpr int numQuadrants = 4;

    QuadrantHistogram(int numInFirstQuadrant, int numInSecondQuadrant, int numInThirdQuadrant, int numInFourthQuadrant)
    {   
        numInQuadrant[0] = numInFirstQuadrant;
        numInQuadrant[1] = numInSecondQuadrant;
        numInQuadrant[2] = numInThirdQuadrant;
        numInQuadrant[3] = numInFourthQuadrant;
    }       

    QuadrantHistogram() = default;

    array<int, numQuadrants> numInQuadrant = {};
    bool operator==(const QuadrantHistogram& other)
    {
        return numInQuadrant[TopLeft] == other.numInQuadrant[TopLeft] &&
            numInQuadrant[TopRight] == other.numInQuadrant[TopRight] &&
            numInQuadrant[BottomLeft] == other.numInQuadrant[BottomLeft] &&
            numInQuadrant[BottomRight] == other.numInQuadrant[BottomRight];
    }
};

enum Transform
{
    Identity,
    ReflectAlongHorizontal,
    ReflectAlongVertical,
    Rotate180,
};
constexpr int numTransforms = 4;

void applyTransform(Transform transform, QuadrantHistogram& quadrantHistogram)
{
    auto& numInQuadrant = quadrantHistogram.numInQuadrant;
    switch (transform)
    {
        case Identity:
            break;
        case ReflectAlongHorizontal:
            swap(numInQuadrant[QuadrantHistogram::TopLeft], numInQuadrant[QuadrantHistogram::BottomLeft]);
            swap(numInQuadrant[QuadrantHistogram::TopRight], numInQuadrant[QuadrantHistogram::BottomRight]);
            break;
        case ReflectAlongVertical:
            swap(numInQuadrant[QuadrantHistogram::TopLeft], numInQuadrant[QuadrantHistogram::TopRight]);
            swap(numInQuadrant[QuadrantHistogram::BottomLeft], numInQuadrant[QuadrantHistogram::BottomRight]);
            break;
        case Rotate180:
            swap(numInQuadrant[QuadrantHistogram::TopLeft], numInQuadrant[QuadrantHistogram::BottomRight]);
            swap(numInQuadrant[QuadrantHistogram::TopRight], numInQuadrant[QuadrantHistogram::BottomLeft]);
            break;
        default:
            assert(false);
    }
}

ostream& operator<<(ostream& os, const QuadrantHistogram& quadrantHistogram)
{
    auto& numInQuadrant = quadrantHistogram.numInQuadrant;
    os << '\n' << numInQuadrant[QuadrantHistogram::TopLeft] << " " << numInQuadrant[QuadrantHistogram::TopRight] << '\n' << numInQuadrant[QuadrantHistogram::BottomLeft] << " " << numInQuadrant[QuadrantHistogram::BottomRight] << '\n';
    return os;
}

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
        ValueType combinedValuesInRange(int left, int right)
        {
            vector<Cell*> cells;
            collectMinCellsForRange(left, right, cells);
            ValueType combinedValuesInRange;
            for (auto cell : cells)
            {
                cell->servicePendingOperations();
                combinedValuesInRange = m_combineValues(combinedValuesInRange, cell->value);
            }
#ifdef VERIFY_SEGMENT_TREE
            {
                ValueType dbgCombinedValuesInRange;

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

            OperatorInfo pendingOperatorInfo;
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

int main(int argc, char** argv)
{
    // Very easy one: we don't care too much about the "full" values of each x and y; we only care which quadrant this point is in.
    // Imagine we maintained, for each point, a QuadrantHistogram: this is simply the number of points in each quadrant for this point
    // i.e. all but one quadrants in the histogram would have 0 points in them; the other would have 1, representing the quadrant that the point
    // is in.  Applying a transform to this histogram permutes the values in an obvious way (see applyTransform()).
    //
    // It might seem odd to represent each point by a histogram of points, but this makes sense once we transplant the problem - as we must with most
    // range-query problems - into a SegmentTree problem; then, each QuadrantHistogram represents the number of points in each quadrant for a *range* of points.
    // With our lazy-evaluation SegmentTree, we need to be able to combine Transforms together into a new transform: while the original transforms
    // consist only of ReflectAlongVertical and ReflectAlongHorizontal, we see that combining these can create new transforms: for example, ReflectAlongHorizontal followed
    // by ReflectAlongVertical actually creates a *rotation* by 180 degrees (Rotate180); similarly, two consecutive flips along the same axis is the same as doing no
    // flips at all i.e. the Identity transform.  These combinations are stored in combinedTransformsTable.  
    // And that's about it - given all this, the conversion to a SegmentTree problem is hopefully obvious!

    // Build transform table - if we apply transform T1 followed by transform T2 to some QuadrantHistogram q, then the result will be the same as if we had just
    // applied combinedTransforms[T2][T1] to q instead.
    Transform combinedTransformsTable[numTransforms][numTransforms];
    for (int firstTransform = 0; firstTransform < numTransforms; firstTransform++)
    {
        for (int secondTransform = 0; secondTransform < numTransforms; secondTransform++)
        {
            QuadrantHistogram quadrantHistogramWithTwoTransforms(1, 2, 3 ,4);

            applyTransform(static_cast<Transform>(firstTransform), quadrantHistogramWithTwoTransforms);
            applyTransform(static_cast<Transform>(secondTransform), quadrantHistogramWithTwoTransforms);

            bool found = false;
            for (int i = 0; i < numTransforms; i++)
            {
                QuadrantHistogram quadrantHistogramSingleTransform(1, 2, 3 ,4);
                const auto singleTransform = static_cast<Transform>(i);

                applyTransform(singleTransform, quadrantHistogramSingleTransform);
                if (quadrantHistogramSingleTransform == quadrantHistogramWithTwoTransforms)
                {
                    found = true;
                    combinedTransformsTable[secondTransform][firstTransform] = singleTransform;
                    break;
                }
            }
            assert(found);
        }
    }

    auto combineQuadrantHistograms = [](const QuadrantHistogram& lhs, const QuadrantHistogram& rhs)
    {
        QuadrantHistogram combined;
        for (int i = 0; i < QuadrantHistogram::numQuadrants; i++)
        {
            combined.numInQuadrant[i] = lhs.numInQuadrant[i] + rhs.numInQuadrant[i];

        }
        return combined;
    };

    auto combineTransforms = [&combinedTransformsTable](const Transform& secondTransform, const Transform& firstTransform)
    {
        return combinedTransformsTable[secondTransform][firstTransform];
    };


    int n;
    cin >> n;

    using QuadrantTracker = SegmentTree<QuadrantHistogram, Transform>;
    QuadrantTracker quadrantTracker(n,  combineQuadrantHistograms, applyTransform, combineTransforms);

    // Compute the initial values for the points: each "point" is represented by a histogram where all quadrants but the one
    // the point belongs to are empty, and the remaining quadrant has a single point (i.e. this point!).
    vector<QuadrantHistogram> initialValues(n);
    for (int i = 0; i < n; i++)
    {
        int x, y;
        cin >> x >> y;

        x = (x < 0 ? -1 : 1);
        y = (y < 0 ? -1 : 1);

        if (x == -1 && y == -1)
            initialValues[i].numInQuadrant[QuadrantHistogram::BottomLeft] = 1;
        else if (x == -1 && y == 1)
            initialValues[i].numInQuadrant[QuadrantHistogram::TopLeft] = 1;
        else if (x == 1 && y == -1)
            initialValues[i].numInQuadrant[QuadrantHistogram::BottomRight] = 1;
        else if (x == 1 && y == 1)
            initialValues[i].numInQuadrant[QuadrantHistogram::TopRight] = 1;
    }

    quadrantTracker.setInitialValues(initialValues);

    int Q;
    cin >> Q;

    for (int q = 0; q < Q; q++)
    {
        char operationType;
        cin >> operationType;

        int start, end;
        cin >> start >> end;
        // Make 0-relative.
        start--;
        end--;

        switch (operationType)
        {
            case 'X':
                quadrantTracker.applyOperatorToAllInRange(start, end, ReflectAlongHorizontal); 
                break;
            case 'Y':
                quadrantTracker.applyOperatorToAllInRange(start, end, ReflectAlongVertical); 
                break;
            case 'C':
                {
                    const auto combinedQuadrantHistogramsInRange = quadrantTracker.combinedValuesInRange(start, end);
                    const auto& numInQuadrant = combinedQuadrantHistogramsInRange.numInQuadrant;
                    cout << numInQuadrant[0] << " " <<
                        numInQuadrant[1] << " " <<
                        numInQuadrant[2] << " " <<
                        numInQuadrant[3] << endl;
                    break;
                }
            default:
                assert(false);
        }
    }
}

