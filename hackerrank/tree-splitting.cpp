// Simon St James (ssjgz).
#include <iostream>
#include <vector>
#include <set>
#include <set>
#include <functional>
#include <cassert>
#define VERIFY_SEGMENT_TREE

using namespace std;

struct Node
{
    vector<Node*> neighbours;
    vector<Node*> children;
    Node* parent = nullptr;
    int originalNumDescendants = 0;
    int indexInChainSegmentTree = -1;
};

struct NodeInfo
{
    Node* node = nullptr;
    int numDescendants = 0;
    bool operator==(const NodeInfo& other)
    {
        return (node == other.node) && (numDescendants == other.numDescendants);
    }
};

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



int fixParentChildAndCountDescendants(Node* node, Node* parentNode)
{
    node->originalNumDescendants = 1;
    if (parentNode)
        node->children.erase(find(node->children.begin(), node->children.end(), parentNode));

    for (auto child : node->children)
        node->originalNumDescendants += fixParentChildAndCountDescendants(child, node);

    return node->originalNumDescendants;
}


vector<vector<Node*>> heavyChains;

// Build up heavyChains; move the heavy child of each node to the front of that node's children.
void doHeavyLightDecomposition(Node* node, bool followedHeavyEdge)
{
    if (followedHeavyEdge)
    {
        // Continue this chain.
        heavyChains.back().push_back(node);
    }
    else
    {
        // Start a new chain.
        heavyChains.push_back({node});
    }
    if (!node->children.empty())
    {
        auto heaviestChildIter = max_element(node->children.begin(), node->children.end(), [](const Node* lhs, const Node* rhs)
                {
                return lhs->originalNumDescendants < rhs->originalNumDescendants;
                });
        iter_swap(node->children.begin(), heaviestChildIter);
        auto heavyChild = node->children.front();
        doHeavyLightDecomposition(heavyChild, true);

        for (auto lightChildIter = node->children.begin() + 1; lightChildIter != node->children.end(); lightChildIter++)
            doHeavyLightDecomposition(*lightChildIter, false);
    }
}


int numInComponent(Node* nodeInComponent, Node* previousNode = nullptr)
{
    int num = 1; // This node.
    for (Node* neighbour : nodeInComponent->neighbours)
    {
        if ( neighbour == previousNode)
            continue;

        num += numInComponent(neighbour, nodeInComponent);
    }
    return num;
}

vector<int> bruteForce(vector<Node>& nodes, const vector<int>& queries)
{
    vector<int> queryResults;
    int previousAnswer = 0;
    for (int encryptedNodeIndex : queries)
    {
        const int nodeIndex = (encryptedNodeIndex ^ previousAnswer) - 1;

        Node* nodeToRemove = &(nodes[nodeIndex]);
        const int thisAnswer = numInComponent(nodeToRemove);
        queryResults.push_back(thisAnswer);

        const auto neighbours = nodeToRemove->neighbours;
        nodeToRemove->neighbours.clear();

        for (const auto neighbour : neighbours)
        {
            neighbour->neighbours.erase(remove(neighbour->neighbours.begin(), neighbour->neighbours.end(), nodeToRemove), neighbour->neighbours.end());
        }

        previousAnswer = thisAnswer;
    }
    return queryResults;
}

vector<int> findSolutionOptimised(vector<Node>& nodes, const vector<int>& queries)
{
    Node* rootNode = &(nodes.front());
    fixParentChildAndCountDescendants(rootNode, nullptr);

    doHeavyLightDecomposition(rootNode, false);

    using DescendantTracker = SegmentTree<NodeInfo, int>;
    auto combineNodeInfos = [](const NodeInfo& lhs, const NodeInfo& rhs) 
    {
        // Dummy - remove TODO
        NodeInfo newNodeInfo;
        newNodeInfo.node = rhs.node;
        newNodeInfo.numDescendants = rhs.numDescendants;

        return newNodeInfo;
    };
    auto applyRemoveDescendants = [](const int numDescendantsToRemove, NodeInfo& nodeInfo)
    {
        nodeInfo.numDescendants -= numDescendantsToRemove;
    };
    auto combineRemoveDescendants = [](const int numDescendantsToRemove1, const int numDescendantsToRemove2)
    {
        return numDescendantsToRemove1 + numDescendantsToRemove2;
    };
    DescendantTracker descendantTracker(nodes.size(),  combineNodeInfos, applyRemoveDescendants, combineRemoveDescendants);

    vector<NodeInfo> initialNodeInfo;
    set<int> chainRootIndices;
    int chainSegmentTreeIndex = 0;
    for (const auto& chain : heavyChains)
    {
        chainRootIndices.insert(chainSegmentTreeIndex);

        for (auto& nodeInChain : chain)
        {
            nodeInChain->indexInChainSegmentTree = chainSegmentTreeIndex;
            NodeInfo nodeInfo;
            nodeInfo.node = nodeInChain;
            nodeInfo.numDescendants = nodeInChain->originalNumDescendants;
            initialNodeInfo.push_back(nodeInfo);
        }

        chainSegmentTreeIndex++;
    }
    descendantTracker.setInitialValues(initialNodeInfo);
    assert(initialNodeInfo.size() == nodes.size());

    // TODO - remove
    {
        for (int i = 0; i < nodes.size(); i++)
        {
            cout << "i: " << i << " descendantTracker.combinedValuesInRange(i, i).numDescendants : " << descendantTracker.combinedValuesInRange(i, i).numDescendants  << " initialNodeInfo[i].numDescendants: " << initialNodeInfo[i].numDescendants << std::endl;
            assert(descendantTracker.combinedValuesInRange(i, i).numDescendants == initialNodeInfo[i].numDescendants);
        }


        descendantTracker.applyOperatorToAllInRange(1, 2, 10);
        for (int i = 0; i < nodes.size(); i++)
        {
            (void)descendantTracker.combinedValuesInRange(i, i);
        }

        descendantTracker.applyOperatorToAllInRange(0, 3, 20);
        for (int i = 0; i < nodes.size(); i++)
        {
            (void)descendantTracker.combinedValuesInRange(i, i);
        }

        descendantTracker.applyOperatorToAllInRange(2, 3, 15);
        for (int i = 0; i < nodes.size(); i++)
        {
            (void)descendantTracker.combinedValuesInRange(i, i);
            cout << "i: " << i << " descendantTracker.combinedValuesInRange(i, i).numDescendants : " << descendantTracker.combinedValuesInRange(i, i).numDescendants  << " initialNodeInfo[i].numDescendants: " << initialNodeInfo[i].numDescendants << std::endl;
        }
    }


    vector<int> queryResults;
    int previousAnswer = 0;
    for (int encryptedNodeIndex : queries)
    {
        const int nodeIndex = (encryptedNodeIndex ^ previousAnswer) - 1;

        Node* nodeToRemove = &(nodes[nodeIndex]);
        const int thisAnswer = -1; // TODO
        queryResults.push_back(thisAnswer);

        previousAnswer = thisAnswer;
    }
    return queryResults;

}

int main()
{
    auto readInt = []() { int x; cin >> x; return x; };

    const int n = readInt();

    vector<Node> nodes(n);

    for (int edge = 0; edge < n - 1; edge++)
    {
        const int u = readInt() - 1;
        const int v = readInt() - 1;

        nodes[u].neighbours.push_back(&(nodes[v]));
        nodes[v].neighbours.push_back(&(nodes[u]));

        nodes[u].children.push_back(&(nodes[v]));
        nodes[v].children.push_back(&(nodes[u]));
    }

    const int numQueries = readInt();

    vector<int> queries;
    for (int queryNum = 0; queryNum < numQueries; queryNum++)
    {
        queries.push_back(readInt());
    }

    const auto bruteForceResults = bruteForce(nodes, queries);
    for (const auto result : bruteForceResults)
    {
        std::cout << result << std::endl;
    }

    const auto optimisedResults = findSolutionOptimised(nodes, queries);

    assert(bruteForceResults == optimisedResults);

}
