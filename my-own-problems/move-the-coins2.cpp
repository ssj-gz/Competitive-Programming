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
#include <sys/time.h>
#include <cassert>

constexpr auto maxN = 100'000;
constexpr int log2(int N, int exponent = 0, int powerOf2 = 1)
{
            return (powerOf2 >= N) ? exponent : log2(N, exponent + 1, powerOf2 * 2);
}
constexpr int log2MaxN = log2(maxN);


using namespace std;

template <typename ValueType, typename OperatorInfo>
class SegmentTree
{
    public:

        using CombineValues = std::function<ValueType(const ValueType& lhs, const ValueType& rhs)>;
        using ApplyOperator = std::function<void(OperatorInfo operatorInfo, ValueType& value)>;
        using CombineOperators = std::function<OperatorInfo(const OperatorInfo& lhs, const OperatorInfo& rhs)>;

        SegmentTree() = default;

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

struct Node;
struct Query
{
    Node* nodeToMove = nullptr;
    Node* newParent = nullptr;
};

struct ReorderedQuery
{
    int originalQueryIndex = -1;
    Query originalQuery;
    int originalNodesThatMakeDigitOne[log2MaxN + 1];
};


struct Node
{
    vector<Node*> children;
    int numCoins = -1;
    Node* parent = nullptr;
    int nodeId = -1;
    bool usable = true;
    int height = -1;

    int grundyNumber = -1;
    vector<ReorderedQuery> queriesForNode;
};


void grundyNumberForTreeBruteForce(Node* node, const int depth, int& grundyNumber)
{
    for (int i = 0; i < node->numCoins; i++)
    {
        grundyNumber ^= depth;
    }
    for (Node* child : node->children)
    {
        grundyNumberForTreeBruteForce(child, depth + 1, grundyNumber);
    }
}

int grundyNumberForTreeBruteForce(Node* node)
{
    int grundyNumber = 0;
    grundyNumberForTreeBruteForce(node, 0, grundyNumber);
    cout << "grundyNumberForTreeBruteForce node: " << node->nodeId << " = " << grundyNumber << endl;
    return grundyNumber;
}

void markDescendentsAsUsable(Node* node,  bool usable)
{
    node->usable = usable;

    for (Node* child : node->children)
    {
        markDescendentsAsUsable(child, usable);
    }
}

void reparentNode(Node* nodeToMove, Node* newParent)
{
    auto oldParent = nodeToMove->parent;
    if (oldParent)
    {
        assert(find(oldParent->children.begin(), oldParent->children.end(), nodeToMove) != oldParent->children.end());
        oldParent->children.erase(remove(oldParent->children.begin(), oldParent->children.end(), nodeToMove), oldParent->children.end());
        assert(find(oldParent->children.begin(), oldParent->children.end(), nodeToMove) == oldParent->children.end());
    }
    nodeToMove->parent = newParent;
    if (newParent)
    {
        assert(find(newParent->children.begin(), newParent->children.end(), nodeToMove) == newParent->children.end());
        newParent->children.push_back(nodeToMove);
    }
}

void fixParentChildAndHeights(Node* node, Node* parent = nullptr, int height = 0)
{
    node->height = height;
    node->parent = parent;

    node->children.erase(remove(node->children.begin(), node->children.end(), parent), node->children.end());

    for (auto child : node->children)
    {
        fixParentChildAndHeights(child, node, height + 1);
    }
}

int findGrundyNumberForNodes(Node* node, const int depth)
{
    int grundyNumber = 0;
    if ((node->numCoins % 2) == 1)
        grundyNumber ^= depth;

    for (auto child : node->children)
    {
        grundyNumber ^= findGrundyNumberForNodes(child, depth + 1);
    }

    node->grundyNumber = grundyNumber;


    return grundyNumber;
}
int findGrundyNumberForNodes(Node* node)
{
    return findGrundyNumberForNodes(node, 0);
}

vector<int> grundyNumbersForQueriesBruteForce(vector<Node>& nodes, const vector<Query>& queries)
{
    auto rootNode = &(nodes.front());
    vector<int> grundyNumbersForQueries;
    for (const auto& query : queries)
    {
        auto originalParent = query.nodeToMove->parent;
        reparentNode(query.nodeToMove, query.newParent);
        grundyNumbersForQueries.push_back(grundyNumberForTreeBruteForce(rootNode));
        reparentNode(query.nodeToMove, originalParent);
    }

    return grundyNumbersForQueries;
}

vector<int> numNodesWithHeight;
vector<int> queryResults;
int originalTreeGrundyNumber;

using NumberTracker = SegmentTree<int, int>;

NumberTracker numNodesWithHeightModuloPowerOf2[log2MaxN + 1];




void solve(Node* node)
{
    for (auto& reorderedQuery : node->queriesForNode)
    {
        auto nodeToMove = reorderedQuery.originalQuery.nodeToMove;
        auto newParent = reorderedQuery.originalQuery.newParent;
        const int heightChange = newParent->height - nodeToMove->parent->height;
        for (int binaryDigitNum = 0; binaryDigitNum <= log2MaxN; binaryDigitNum++)
        {
            for (int height = 0; height < numNodesWithHeight.size(); height++)
            {
                if (((height + heightChange) & (1 << binaryDigitNum)) != 0)
                {
                    reorderedQuery.originalNodesThatMakeDigitOne[binaryDigitNum] += numNodesWithHeight[height];
                }
            }

        } 
    }
    const auto originalNumNodesWithHeight = numNodesWithHeight;
    if ((node->numCoins) % 2 == 1)
        numNodesWithHeight[node->height]++;
    for (int binaryDigitNum = 0; binaryDigitNum <= log2MaxN; binaryDigitNum++)
    {
        const int heightModuloPowerOf2 = (1 << (binaryDigitNum + 1));
        numNodesWithHeightModuloPowerOf2[binaryDigitNum].applyOperatorToAllInRange(heightModuloPowerOf2, heightModuloPowerOf2, 1);
    }
    for (auto child : node->children)
    {
        solve(child);
    }
    vector<int> numDescendendantNodesWithHeight(numNodesWithHeight.size());
    for (int height = 0; height < numDescendendantNodesWithHeight.size(); height++)
    {
        numDescendendantNodesWithHeight[height] = numNodesWithHeight[height] - originalNumNodesWithHeight[height];
    }
    for (auto& reorderedQuery : node->queriesForNode)
    {
        cout << "reorderedQuery" << endl;
        auto nodeToMove = reorderedQuery.originalQuery.nodeToMove;
        auto newParent = reorderedQuery.originalQuery.newParent;
        const int heightChange = newParent->height - nodeToMove->parent->height;

        const int grundyNumberMinusSubtree = originalTreeGrundyNumber ^ nodeToMove->grundyNumber;
        //int newGrundyNumber = grundyNumberMinusSubtree;
        //cout << "originalTreeGrundyNumber: " << originalTreeGrundyNumber << endl;

        int relocatedSubtreeGrundyDigits[log2MaxN + 1] = {};

        int relocatedSubtreeGrundyNumber = 0;
        for (int binaryDigitNum = 0; binaryDigitNum <= log2MaxN; binaryDigitNum++)
        {
            cout << "binaryDigitNum: " << binaryDigitNum << endl;
            relocatedSubtreeGrundyDigits[binaryDigitNum] -= reorderedQuery.originalNodesThatMakeDigitOne[binaryDigitNum];
            for (int height = 0; height < numNodesWithHeight.size(); height++)
            {
                cout << " height: " << height << endl;
                if (((height + heightChange) & (1 << binaryDigitNum)) != 0)
                {
                    //cout << "gloop!" << endl;
                    cout << " gleep!" << endl;
                    relocatedSubtreeGrundyDigits[binaryDigitNum] += numNodesWithHeight[height];
                }
            }
            assert(relocatedSubtreeGrundyDigits[binaryDigitNum] >= 0);
            relocatedSubtreeGrundyDigits[binaryDigitNum] %= 2;
            relocatedSubtreeGrundyNumber = relocatedSubtreeGrundyNumber + (1 << binaryDigitNum) * relocatedSubtreeGrundyDigits[binaryDigitNum];
            cout << " relocatedSubtreeGrundyDigits[" << binaryDigitNum << "] = " << relocatedSubtreeGrundyDigits[binaryDigitNum] << endl;
        } 
        int newGrundyNumber = grundyNumberMinusSubtree;
        newGrundyNumber ^= relocatedSubtreeGrundyNumber;
        queryResults[reorderedQuery.originalQueryIndex] = newGrundyNumber;
        cout << " relocatedSubtreeGrundyNumber: " << relocatedSubtreeGrundyNumber << endl;
        //cout << " relocatedSubtreeGrundyNumber: " << blee << endl;
        {
            int verifyRelocatedSubtreeGrundyNumber = 0;
            for (int height = 0; height < numDescendendantNodesWithHeight.size(); height++)
            {
                if ((numDescendendantNodesWithHeight[height] % 2) == 1)
                {
                    verifyRelocatedSubtreeGrundyNumber ^= (height + heightChange);
                    cout << " height: " << height << " contributes " << (height + heightChange) << " to relocatedSubtreeGrundyNumber!" << endl;
                }
            }
            assert(relocatedSubtreeGrundyNumber == verifyRelocatedSubtreeGrundyNumber);
        }
    }

}

vector<int> grundyNumbersForQueries(vector<Node>& nodes, const vector<Query>& queries)
{
    for (int binaryDigitNum = 0; binaryDigitNum <= log2MaxN; binaryDigitNum++)
    {
        auto add = [](const auto& x, int& destValue)
        {
            destValue += x;
        };
        auto combineValues = [](const int lhs, const int rhs)
        {
            return lhs + rhs;
        };
        auto combineAdditions = [](const int x1, int x2)
        {
            return x1 + x2;
        };
        numNodesWithHeightModuloPowerOf2[binaryDigitNum] = NumberTracker((1 << (binaryDigitNum + 1)) + 1, combineValues, add, combineAdditions);
    }
    numNodesWithHeight.resize(nodes.size() + 1);
    auto rootNode = &(nodes.front());
    originalTreeGrundyNumber = findGrundyNumberForNodes(rootNode);
    assert(rootNode->grundyNumber == grundyNumberForTreeBruteForce(rootNode));
    // TODO - optimise this - don't use Brute Force XD
    vector<int> grundyNumbersForQueries;
    for (int queryIndex = 0; queryIndex < queries.size(); queryIndex++)
    {
        const auto query = queries[queryIndex];
        auto originalParent = query.nodeToMove->parent;
        {
            reparentNode(query.nodeToMove, nullptr);
            assert(grundyNumberForTreeBruteForce(rootNode) == (originalTreeGrundyNumber ^ query.nodeToMove->grundyNumber));
        }
        reparentNode(query.nodeToMove, query.newParent);
        grundyNumbersForQueries.push_back(grundyNumberForTreeBruteForce(rootNode));
        reparentNode(query.nodeToMove, originalParent);

        query.nodeToMove->queriesForNode.push_back({queryIndex, query});
    }
    queryResults.resize(queries.size());
    solve(rootNode);
    for (int queryIndex = 0; queryIndex < queries.size(); queryIndex++)
    {
        cout << " queryIndex: " << queryIndex << " queryResults: " << queryResults[queryIndex] << " grundyNumbersForQueries: " << grundyNumbersForQueries[queryIndex] << endl;
    }
    assert(queryResults == grundyNumbersForQueries);

    return grundyNumbersForQueries;
}

int main(int argc, char** argv)
{
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int maxNumNodes = 20;
        const int maxNumQueries = 20;

        int numNodes = rand() % maxNumNodes + 1;
        int numQueries = rand() % maxNumQueries + 1;

        int numMetaAttempts = 0;

        while (true)
        {
            //cout << "Random tree with nodes: " << numNodes << endl;
            vector<Node> nodes(numNodes);
            vector<pair<int, int>> edges;
            for (int i = 0; i < numNodes; i++)
            {
                if (i > 0)
                {
                    const int parentNodeIndex = rand() % i;
                    nodes[parentNodeIndex].children.push_back(&nodes[i]);
                    nodes[i].parent = &(nodes[parentNodeIndex]);
                    edges.push_back({i, parentNodeIndex});
                }
                nodes[i].numCoins = rand() % 20;
                nodes[i].nodeId = i;
            }

            int numQueriesToGenerate = numQueries;
            int numAttempts = 0;
            vector<pair<int, int>> potentialQueries;
            for (int nodeIndexToMove = 0; nodeIndexToMove < numNodes; nodeIndexToMove++)
            {
                auto& nodeToMove = nodes[nodeIndexToMove];
                auto oldParent = nodeToMove.parent;
                markDescendentsAsUsable(&nodeToMove, false);
                for (int newParentNodeIndex = 0; newParentNodeIndex < numNodes; newParentNodeIndex++)
                {
                    if (nodes[newParentNodeIndex].usable)
                    {
                        potentialQueries.push_back({nodeIndexToMove, newParentNodeIndex});
                    }
                }
                markDescendentsAsUsable(&nodeToMove, true);
            }
            const bool successful = (potentialQueries.size() >= numQueries);
            if (successful)
            {
                random_shuffle(potentialQueries.begin(), potentialQueries.end());
                potentialQueries.erase(potentialQueries.begin() + numQueries, potentialQueries.end());
                assert(potentialQueries.size() == numQueries);

                cout << numNodes << endl;
                for (const auto& node : nodes)
                {
                    cout << node.numCoins << " ";
                }
                cout << endl;
                assert(edges.size() == numNodes - 1);
                for (auto& edge : edges)
                {
                    if (rand() % 2 == 0)
                        swap(edge.first, edge.second);
                }
                random_shuffle(edges.begin(), edges.end());
                for (const auto& edge : edges)
                {
                    cout << (edge.first + 1) << " " << (edge.second + 1) << endl;
                }
                cout << numQueries << endl;
                for (const auto& query : potentialQueries)
                {
                    cout << (query.first + 1) << " " << (query.second + 1) << endl;
                }
                break;
            }

            numMetaAttempts++;
            //cout << "numMetaAttempts: " << numMetaAttempts << endl;
            if (numMetaAttempts > 1000)
            {
                //cout << "Whoops" << endl;
                numNodes = rand() % maxNumNodes + 1;
                numQueries = rand() % maxNumQueries + 1;
                numMetaAttempts = 0;
            }
        }
        return 0;
    }

    // Draft solution:
    //  - Store all queries that re-parent Node n in node n, so we know which newParent n will be re-parented to on each query.
    //  - For node n, calculate grundy sum Gn of subtree rooted at n, though where heights of each descendant is measured
    //    relative to original root, not to n (easy).
    //  - The xor sum for whole tree after removing n is then Groot (lol) ^ Gn.
    //  - For each q at n, what is grundy sum of new tree where n is re-parented to newParent?
    //  - For each power of 2 up to n, maintain a segment tree, numModuloPowerOf2 of length (you guessed it!) powerOf2.
    //  - Each query will essentially add (or subtract) some number m from the height of each descendent of n.
    //  - If we can find out, for each binary digit/ power of 2, how many descendents of n have a height which, when
    //    m is added to/ subtracted from it, will leave that binary digit 1 (or 0), we're in business.
    //  - Whether a height will have digit B switched on after adding m depends, in a fairly simple way, of height % powerOf2 and m; see
    //    list of binary numbers below:
    //
    //     0000
    //     0001
    //     0010
    //     0011
    //     0100
    //     0101
    //     0110
    //     0111
    //     1000
    //     1001
    //     1010
    //     1011
    //     1100
    //     1101
    //     1110
    //     1111
    //
    //  - The number of descendents with bit B set when we add m to their heights will presumably a range (possibly) split
    //    of moduli modulo the power of 2; for example, the bit corresponding to power of 2 == 4 is on for height h
    //    after adding 3 for h = 1, 2, 3, 4 mod 8 and 0 otherwise i.e. for 5, 6, 7, 0.
    //  - When we visit a node with height h:
    //     - For each power of 2, for each m from each query, count the number of heights we've seen so far in the given 
    //       "h modulo power of 2 + m will give binary digit 1" range. (A)
    //     - Add the height h for this node modulo power of 2 to the segment tree corresponding to each power of 2.
    //     - Explore descendants.
    //     - Recalculate A for each query m, and subtract original from it - we now know how many descendants of h will have a given bit B set to on
    //       for m.
    //     - We can then use this to calculate Grundy number for re-parented tree.  I hope :)

    int numNodes;
    cin >> numNodes;

    vector<Node> nodes(numNodes);
    for (int i = 0; i < numNodes; i++)
    {
        nodes[i].nodeId = i;
        cin >> nodes[i].numCoins;
    }
    for (int i = 0; i < numNodes - 1; i++)
    {
        int a, b;
        cin >> a >> b;
        a--;
        b--;

        nodes[a].children.push_back(&nodes[b]);
        nodes[b].children.push_back(&nodes[a]);
    }

    int numQueries;
    cin >> numQueries;

    vector<Query> queries(numQueries);

    for (int i = 0; i < numQueries; i++)
    {
        int u, v;
        cin >> u >> v;
        u--;
        v--;


        queries[i].nodeToMove = &(nodes[u]);
        queries[i].newParent = &(nodes[v]);
    }

    auto rootNode = &(nodes.front());
    fixParentChildAndHeights(rootNode);

    const auto result = grundyNumbersForQueries(nodes, queries);

#ifdef BRUTE_FORCE
    const auto resultBruteForce = grundyNumbersForQueriesBruteForce(nodes, queries);
    cout << "resultBruteForce: " << endl;
    for (const auto queryResult : resultBruteForce)
    {
        cout << queryResult << " ";
    }
    cout << endl;
    cout << "result: " << endl;
    for (const auto queryResult : result)
    {
        cout << queryResult << " ";
    }
    cout << endl;
    assert(result == resultBruteForce);
#endif


#if 0
    auto add = [](const auto& x, int& destValue)
    {
        destValue += x;
    };
    auto combineValues = [](const int lhs, const int rhs)
    {
        return lhs + rhs;
    };
    auto combineAdditions = [](const int x1, int x2)
    {
        return x1 + x2;
    };
    SegmentTree<int, int> numInRangeTracker(maxN + 1, combineValues, add, combineAdditions);

    numInRangeTracker.applyOperatorToAllInRange(1, 1, 1);
    numInRangeTracker.applyOperatorToAllInRange(1, 2, 1);
    numInRangeTracker.applyOperatorToAllInRange(3, 3, 1);
    cout << numInRangeTracker.combinedValuesInRange(1, 1) << endl; cout << numInRangeTracker.combinedValuesInRange(1, 2) << endl; cout << numInRangeTracker.combinedValuesInRange(1, 3) << endl; 
#endif
#if 0
    while (true)
    {
        const int numNodes = rand() % 100'000 + 1;
        cout << "Random tree with nodes: " << numNodes << endl;
        vector<Node> nodes(numNodes);
        nodes.front().originalHeight = 0;
        for (int i = 0; i < numNodes; i++)
        {
            if (i > 0)
            {
                const int parentNodeIndex = rand() % i;
                nodes[parentNodeIndex].children.push_back(&nodes[i]);
                nodes[i].parent = &(nodes[parentNodeIndex]);
                nodes[i].originalHeight = nodes[i].parent->originalHeight + 1;
            }
            nodes[i].numCoins = rand() % 20;
            nodes[i].nodeId = i;
        }

        for (Node& node : nodes)
        {
            if (!node.parent)
                continue;

            auto oldParent = node.parent;
            markDescendentsAsUsable(&node, false);

            for (Node& newParent : nodes)
            {
                if (node.parent == &newParent)
                    continue;
                if (!newParent.usable)
                    continue;

                reparentNode(&node, &newParent);
                if (grundyNumberForTree(&(nodes.front())) == 0)
                {
                    cout << " woo!" << node.nodeId << "," << newParent.nodeId << endl;
                }
                reparentNode(&node, oldParent);

            }
            markDescendentsAsUsable(&node, true);
        }
    }
#endif
}
