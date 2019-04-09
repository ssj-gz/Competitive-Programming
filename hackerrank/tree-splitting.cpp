// Simon St James (ssjgz).
#define SUBMISSION
#define VERIFY_SEGMENT_TREE
#ifdef SUBMISSION
#define NDEBUG
#undef VERIFY_SEGMENT_TREE
#endif
#include <iostream>
#include <vector>
#include <set>
#include <functional>
#include <algorithm>
#include <cassert>

using namespace std;

struct Node
{
    vector<Node*> neighbours;
    vector<Node*> children;
    Node* parent = nullptr;
    int originalNumDescendants = 0;
    int indexInChainSegmentTree = -1;
    bool removed = false;
};

struct NodeInfo
{
    NodeInfo(Node* node, int numDescendants)
        : node{node}, numDescendants{numDescendants}
    {
    }
    NodeInfo() = default;
    Node* node = nullptr;
    int numDescendants = 0;
};

template <typename ValueType, typename OperatorInfo>
class SegmentTree {
    public:

        using ApplyOperator = std::function<void(OperatorInfo operatorInfo, ValueType& value)>;
        using CombineOperators = std::function<OperatorInfo(const OperatorInfo& lhs, const OperatorInfo& rhs)>;

        SegmentTree(int maxNumber, ApplyOperator applyOperator, CombineOperators combineOperators)
            : m_maxNumber{maxNumber}, m_applyOperator{applyOperator}, m_combineOperators{combineOperators}
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
            m_numCellRows = m_cellMatrix.size();
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
            }
#ifdef VERIFY_SEGMENT_TREE
            m_dbgValues = initialValues;
#endif
        }
        void applyOperatorToAllInRange(int left, int right, OperatorInfo operatorInfo)
        {
            assert(left >= 0 && left < m_maxNumber);
            assert(right >= 0 && right < m_maxNumber);
            assert(left <= right);
            vector<Cell*> cells;
            collectMinCellsForRange(left, right, cells);
            for (auto cell : cells)
            {
                cell->addPendingOperation(operatorInfo);
                cell->servicePendingOperations();
            }
#ifdef VERIFY_SEGMENT_TREE
            for (int i = left; i <= right; i++)
            {
                m_applyOperator(operatorInfo, m_dbgValues[i]);
            }
#endif
        }
        ValueType valueAt(int pos)
        {
            assert(pos >= 0 && pos < m_maxNumber);
            vector<Cell*> cells;
            collectMinCellsForRange(pos, pos, cells);
            assert(cells.size() == 1);
            auto cell = cells.front();
            cell->servicePendingOperations();
            ValueType valueAt = cell->value; 
#ifdef VERIFY_SEGMENT_TREE
            {
                assert(m_dbgValues[pos] == valueAt);
            }
#endif
            return valueAt;
        }
    private:
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



        };

        vector<vector<Cell>> m_cellMatrix;
        int m_numCellRows = -1;

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
            if (cellRow == m_numCellRows)
                return;
            if (end < start)
                return;
            if (cellRow != 0)
            {
                // Ensure parents has service their pending operations.
                const int parentCellIndex = (start >> (m_numCellRows - cellRow ));
                const int parentCellRow = cellRow - 1;
                Cell *parentCell = &(m_cellMatrix[parentCellRow][parentCellIndex]);
                parentCell->servicePendingOperations();
            }

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

};

int fixParentChildAndCountDescendants(Node* node, Node* parentNode)
{
    node->parent = parentNode;
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

vector<int> findSolutionOptimised(vector<Node>& nodes, const vector<int>& queries)
{
    Node* rootNode = &(nodes.front());
    fixParentChildAndCountDescendants(rootNode, nullptr);
    doHeavyLightDecomposition(rootNode, false);

    // Set up the descendantTracker.
    using DescendantTracker = SegmentTree<NodeInfo, int>;
    auto applyRemoveDescendants = [](const int numDescendantsToRemove, NodeInfo& nodeInfo)
    {
        nodeInfo.numDescendants -= numDescendantsToRemove;
    };
    auto combineRemoveDescendants = [](const int numDescendantsToRemove1, const int numDescendantsToRemove2)
    {
        return numDescendantsToRemove1 + numDescendantsToRemove2;
    };
    DescendantTracker descendantTracker(nodes.size(),  applyRemoveDescendants, combineRemoveDescendants);

    // Put all the chains into indexInChainSegmentTree, one after the other, and 
    // load that info into descendantTracker.
    // The order of the chains doesn't matter, particular, but the order of the
    // nodes within a chain obviously does!
    vector<NodeInfo> initialChainSegmentTreeInfo;
    set<int> chainRootIndices;
    int chainSegmentTreeIndex = 0;
    for (const auto& chain : heavyChains)
    {
        chainRootIndices.insert(chainSegmentTreeIndex);

        for (auto& nodeInChain : chain)
        {
            nodeInChain->indexInChainSegmentTree = chainSegmentTreeIndex;
            initialChainSegmentTreeInfo.emplace_back(nodeInChain, nodeInChain->originalNumDescendants);
            chainSegmentTreeIndex++;
        }
    }
    descendantTracker.setInitialValues(initialChainSegmentTreeInfo);
    assert(initialChainSegmentTreeInfo.size() == nodes.size());

    auto findChainRoot = [&chainRootIndices, &initialChainSegmentTreeInfo](Node* nodeInChain)
    {
        // The indexInChainSegmentTree of the root of the chain containing nodeInChain is the largest
        // indexInChainSegmentTree which is less than or equal to nodeInChain->indexInChainSegmentTree.
        // Finding this is O(log2N).
        assert(!chainRootIndices.empty());

        auto chainRootIndexIter = chainRootIndices.lower_bound(nodeInChain->indexInChainSegmentTree);
        if (chainRootIndexIter == chainRootIndices.end())
            chainRootIndexIter = std::prev(chainRootIndices.end());
        if (*chainRootIndexIter > nodeInChain->indexInChainSegmentTree)
        {
            assert(chainRootIndexIter != chainRootIndices.begin());
            chainRootIndexIter = std::prev(chainRootIndexIter);
        }
        const auto chainRootIndex = *chainRootIndexIter;
        auto chainRoot = initialChainSegmentTreeInfo[chainRootIndex].node;
        assert(chainRoot->indexInChainSegmentTree <= nodeInChain->indexInChainSegmentTree);
        return chainRoot;
    };

    vector<int> queryResults;
    int previousAnswer = 0;
    for (int encryptedNodeIndex : queries)
    {
        // Decrypt the index (in nodes) of the node to remove using the previous query answer.
        const int nodeIndex = (encryptedNodeIndex ^ previousAnswer) - 1;
        assert(nodeIndex >= 0 && nodeIndex < nodes.size());

        Node* nodeToRemove = &(nodes[nodeIndex]);
        assert(!nodeToRemove->removed);

        // Find the root of the component containing nodeToRemove - the number of descendants for this
        // root is the number of nodes in the component containing nodeToRemove.
        // We work our way up a chain at a time - since there are at most O(log2N) such chains, 
        // this is a fast operation.
        auto rootOfChainWithNodeToRemove = findChainRoot(nodeToRemove);
        auto rootOfComponent = rootOfChainWithNodeToRemove;
        while (rootOfComponent->parent)
        {
            rootOfComponent = findChainRoot(rootOfComponent->parent);
        }

        const int thisAnswer = descendantTracker.valueAt(rootOfComponent->indexInChainSegmentTree).numDescendants;
        queryResults.push_back(thisAnswer);

        const int numDescendantsOfNodeToRemove = descendantTracker.valueAt(nodeToRemove->indexInChainSegmentTree).numDescendants;

        // Work our way up the ancestors, chain-by-chain, adjusting the numDescendants for all nodes to reflect
        // the fact that nodeToRemove (and all its descendants) have been removed.
        // There are at most O(log2N) such "ancestor chains", and we can adjust each chain (using a segment tree) in O(log2N).
        descendantTracker.applyOperatorToAllInRange(rootOfChainWithNodeToRemove->indexInChainSegmentTree, nodeToRemove->indexInChainSegmentTree, numDescendantsOfNodeToRemove);
        auto bottomOfAncestorChain = rootOfChainWithNodeToRemove->parent;
        while (bottomOfAncestorChain != nullptr)
        {
            auto rootOfAncestorChain = findChainRoot(bottomOfAncestorChain);
            descendantTracker.applyOperatorToAllInRange(rootOfAncestorChain->indexInChainSegmentTree, bottomOfAncestorChain->indexInChainSegmentTree, numDescendantsOfNodeToRemove);

            bottomOfAncestorChain = rootOfAncestorChain->parent;
        }

        // "Remove" nodeToRemove.  Note that we don't have to clear nodeToRemove's children, nor 
        // remove nodeToRemove from the list of its parents' children.
        for (auto& child : nodeToRemove->children)
        {
            child->parent = nullptr;
            // The child (if not removed) is now definitely the root of a chain, if it wasn't before.
            if (!child->removed)
                chainRootIndices.insert(child->indexInChainSegmentTree);
        }
        nodeToRemove->parent = nullptr;
        // nodeToRemove is no longer the root of a chain.
        chainRootIndices.erase(nodeToRemove->indexInChainSegmentTree);
        nodeToRemove->removed = true;

        previousAnswer = thisAnswer;
    }
    return queryResults;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);

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

    const auto optimisedResults = findSolutionOptimised(nodes, queries);
    for (int queryNum = 0; queryNum < numQueries; queryNum++)
    {
        cout << optimisedResults[queryNum] << endl;
    }
}
