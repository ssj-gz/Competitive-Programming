// Simon St James (ssjgz) 2019-04-09
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
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

/**
 * Allows fast updates/ queries for the numDescendants of ranges of NodeInfo's.
 * Uses a Segment Tree (adapted from Quadrant Queries) to allow us to subtract
 * a number of descendants from a range of NodeInfo's in O(log2N), and 
 * retrieve the number of descendants for a given NodeInfo in O(log2N).
 */
class DescendantTracker {
    public:

        DescendantTracker(int numNodeInfos)
            : m_numNodeInfos{numNodeInfos}
        {
            int exponentOfPowerOf2 = 0;
            int64_t powerOf2 = 1;
            while (numNodeInfos > powerOf2)
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
        }
        void setInitialValues(const vector<NodeInfo>& initialValues)
        {
            for (int i = 0; i < initialValues.size(); i++)
            {
                auto& cell = m_cellMatrix.back()[i];
                cell.nodeInfo = initialValues[i];
            }
        }
        void subtractNumDescendantsFromAllInRange(int left, int right, int numDescendantsToSubtract)
        {
            assert(left >= 0 && left < m_numNodeInfos);
            assert(right >= 0 && right < m_numNodeInfos);
            assert(left <= right);
            vector<Cell*> cells;
            collectMinCellsForRange(left, right, cells);
            for (auto cell : cells)
            {
                cell->addPendingDescendantSubtraction(numDescendantsToSubtract);
                cell->servicePendingDescendantSubtractions();
            }
        }
        NodeInfo nodeInfoAt(int pos)
        {
            assert(pos >= 0 && pos < m_numNodeInfos);
            vector<Cell*> cells;
            collectMinCellsForRange(pos, pos, cells);
            assert(cells.size() == 1);
            auto cell = cells.front();
            cell->servicePendingDescendantSubtractions();
            NodeInfo nodeInfoAt = cell->nodeInfo; 
            return nodeInfoAt;
        }
    private:
        int m_numNodeInfos;
        int64_t m_powerOf2BiggerThanMaxNumber;
        int m_exponentOfPowerOf2BiggerThanMaxNumber;

        struct Cell
        {
            DescendantTracker* container = nullptr;
            NodeInfo nodeInfo;

            int64_t rangeBegin = -1;
            int64_t rangeEnd = -1;
            Cell* parent = nullptr;
            Cell* leftChild = nullptr;
            Cell* rightChild = nullptr;

            int pendingNumDescendantsToSubtract;
            bool hasPendingDescendantSubtraction = false;

            void addPendingDescendantSubtraction(int numDescendantsToSubtract)
            {
                if (!hasPendingDescendantSubtraction)
                {
                    hasPendingDescendantSubtraction = true;
                    pendingNumDescendantsToSubtract = numDescendantsToSubtract;
                }
                else
                {
                    pendingNumDescendantsToSubtract += numDescendantsToSubtract;
                }
            }

            void servicePendingDescendantSubtractions()
            {
                if (hasPendingDescendantSubtraction)
                {
                    if (leftChild && rightChild)
                    {
                        leftChild->addPendingDescendantSubtraction(pendingNumDescendantsToSubtract);
                        rightChild->addPendingDescendantSubtraction(pendingNumDescendantsToSubtract);
                    }

                    nodeInfo.numDescendants -= pendingNumDescendantsToSubtract;

                    hasPendingDescendantSubtraction = false;
                }
            }
        };

        vector<vector<Cell>> m_cellMatrix;
        int m_numCellRows = -1;

        // Collect O(log2(end - start + 1)) cells in range-order that span the interval start-end (inclusive).
        void collectMinCellsForRange(const int start, const int end, vector<Cell*>& destCells)
        {
            collectMinCellsForRange(start, end, 0, m_powerOf2BiggerThanMaxNumber, destCells);
        }

        void collectMinCellsForRange(int start, int end, int cellRow, int powerOf2, vector<Cell*>& destCells)
        {
            assert(start <= end);
            if (cellRow == m_numCellRows)
                return;
            if (cellRow != 0)
            {
                // Ensure parent has serviced its pending descendant subtractions.
                const int parentCellIndex = (start >> (m_numCellRows - cellRow ));
                const int parentCellRow = cellRow - 1;
                Cell *parentCell = &(m_cellMatrix[parentCellRow][parentCellIndex]);
                parentCell->servicePendingDescendantSubtractions();
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
                if (start <= end)
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
                if (start <= end)
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
            assert(start <= powerOf2AfterStart - 1);
            collectMinCellsForRange(start, powerOf2AfterStart - 1, cellRow + 1, powerOf2 >> 1, destCells); // Left region.
            if (middleCellToAdd)
                destCells.push_back(middleCellToAdd);
            assert(powerOf2BeforeEnd <= end);
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
    auto rootNode = &(nodes.front());
    fixParentChildAndCountDescendants(rootNode, nullptr);
    doHeavyLightDecomposition(rootNode, false);

    DescendantTracker descendantTracker(nodes.size());

    // Put all the chains into indexInChainSegmentTree, one after the other, and 
    // load that info into descendantTracker.
    // The order of the chains doesn't matter, particular, but the order of the
    // nodes within a chain obviously does!
    vector<NodeInfo> initialChainSegmentTreeInfo;
    set<int> chainRootIndices;
    auto chainSegmentTreeIndex = 0;
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
    auto previousAnswer = 0;
    for (auto encryptedNodeIndex : queries)
    {
        // Decrypt the index (in nodes) of the node to remove using the previous query answer.
        const auto nodeToRemoveIndex = (encryptedNodeIndex ^ previousAnswer) - 1;
        assert(nodeToRemoveIndex >= 0 && nodeToRemoveIndex < nodes.size());

        Node* nodeToRemove = &(nodes[nodeToRemoveIndex]);
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

        const auto thisAnswer = descendantTracker.nodeInfoAt(rootOfComponent->indexInChainSegmentTree).numDescendants;
        queryResults.push_back(thisAnswer);

        const auto numDescendantsOfNodeToRemove = descendantTracker.nodeInfoAt(nodeToRemove->indexInChainSegmentTree).numDescendants;

        // Work our way up the ancestors, chain-by-chain, adjusting the numDescendants for all nodes to reflect
        // the fact that nodeToRemove (and all its descendants) have been removed.
        // There are at most O(log2N) such "ancestor chains", and we can adjust each chain (using a segment tree) in O(log2N).
        descendantTracker.subtractNumDescendantsFromAllInRange(rootOfChainWithNodeToRemove->indexInChainSegmentTree, nodeToRemove->indexInChainSegmentTree, numDescendantsOfNodeToRemove);
        auto bottomOfAncestorChain = rootOfChainWithNodeToRemove->parent;
        while (bottomOfAncestorChain != nullptr)
        {
            auto rootOfAncestorChain = findChainRoot(bottomOfAncestorChain);
            descendantTracker.subtractNumDescendantsFromAllInRange(rootOfAncestorChain->indexInChainSegmentTree, bottomOfAncestorChain->indexInChainSegmentTree, numDescendantsOfNodeToRemove);

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
    // Took me ages, in that I first started thinking about it probably a couple of
    // years ago, and could never do it.
    //
    // Recently re-tried, though, now I have more knowledge under my belt, and it
    // was quite easy now :)
    //
    // So: let's assume that we can somehow, for all nodes, efficiently keep track
    // of the number of *descendants* each node has (assuming that we pick some
    // arbitrary node as the "root" node) as we remove nodes from the tree.
    //
    // Answering a query, then, would be as easy as finding the "root" of the component
    // containing the nodeToRemove (i.e. that node found my working our way up from the
    // parent of nodeToRemove, to the parent of the parent of nodeToRemove, etc)
    // and finding the number of descendants of that node.
    //
    // To remove nodeToRemove, we need only find all ancestor nodes of nodeToRemove, and subtract
    // nodeToRemove.numDescendants from the number of descendants of each ancestor node of nodeToRemove.
    //
    // Since the number of ancestors can be O(N), naive attempts to perform either of these operations
    // (finding the root of the component containing nodeToRemove; subtracting the number of descendants
    // from all ancestors when we remove nodeToRemove) are O(N), giving a O(N^2) algorithm
    // in practice, which is no good.
    //
    // Can we do better?
    //
    // Yes - if we perform a heavy-light decomposition of the graph, then the number of *chains* 
    // that nodeToRemove will have in its list of ancestor nodes will be just O(log2N). By
    // laying all these chains end-to-end into one big array (initialChainSegmentTreeInfo), we can
    // easily figure out how to find the root of chain containing nodeToRemove (see findChainRoot())
    // and so work our way up the list of ancestors in O(log2N * log2N) (there are O(log2N) ancestor 
    // chains, and finding the root of a chain is O(log2N) - chainRootIndices is the key to this) until 
    // we find the required root of the component containing nodeToRemove.
    //
    // Likewise, if we loaded initialChainSegmentTreeInfo into a SegmentTree, and each element of 
    // initialChainSegmentTreeInfo consisted of a node and the number of descendants in that node,
    // then subtracting the number of descendants from all ancestors of nodeToRemove is similarly
    // efficient - again, O(log2N * log2N) (O(log2N) ancestor chains, and subtracting the number of 
    // descendants from all of the elements in a chain - or from the root of the chain down to nodeToRemove - 
    // in the Segment Tree is O(log2N)).
    //
    // And that's essentially it: the loading of chains into initialChainSegmentTreeInfo is done in
    // and arbitrary order - only the order of the nodes within each chain (root of chain; child of
    // root of chain; child of child of root of chain; etc!) matters.  When removing a node, we need
    // only adjust the numDescendants of its ancestors (by traversing the chains as far as we can go),
    // and mark the children of nodeToRemove as new roots of chains (by adding their indexInChainSegmentTree
    // to chainRootIndices) - in particular, removing nodeToRemove from its parents list of children, or clearing
    // nodeToRemove's own list of children is not necessary, so we skip it for efficiency.
    //
    // The total runtime is num queries (O(N)) times O(log2N * log2N) == O(N * log2N * log2N).
    //
    // The main challenge here was getting the constant factor down small enough - my SegmentTree is bit shit :)

    ios::sync_with_stdio(false);

    auto readInt = []() { int x; cin >> x; return x; };

    const auto n = readInt();

    vector<Node> nodes(n);

    for (auto edge = 0; edge < n - 1; edge++)
    {
        const auto u = readInt() - 1;
        const auto v = readInt() - 1;

        nodes[u].children.push_back(&(nodes[v]));
        nodes[v].children.push_back(&(nodes[u]));
    }

    const auto numQueries = readInt();

    vector<int> queries;
    for (auto queryNum = 0; queryNum < numQueries; queryNum++)
    {
        queries.push_back(readInt());
    }

    const auto optimisedResults = findSolutionOptimised(nodes, queries);
    for (auto queryNum = 0; queryNum < numQueries; queryNum++)
    {
        cout << optimisedResults[queryNum] << endl;
    }
}
