// Simon St James (ssjgz) - 2020-04-01
// 
// Solution to: https://www.codechef.com/problems/MVCN2TST
//
//#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#else
#define _GLIBCXX_DEBUG       // Iterator safety; out-of-bounds access for Containers, etc.
#pragma GCC optimize "trapv" // abort() on (signed) integer overflow.
#define BRUTE_FORCE
#endif
#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

struct Node
{
    vector<Node*> children;
    Node* parent = nullptr;
    int height = -1;
    int id = -1;
    int numDescendants = -1; // Includes the node itself.
    int numCanReparentTo = -1;

    int dfsBeginVisit = -1;
    int dfsEndVisit = -1;
};

void computeDFSInfo(Node* node, int& dfsVisitNum, vector<vector<Node*>>& nodesAtHeightLookup)
{
    node->dfsBeginVisit = dfsVisitNum;
    if (!nodesAtHeightLookup.empty()) // TODO - remove this condition - it's only there as the  testgen and brute force don't use nodesAtHeightLookup.
    {
        nodesAtHeightLookup[node->height].push_back(node);
    }
    dfsVisitNum++;

    for (auto child : node->children)
    {
        computeDFSInfo(child, dfsVisitNum, nodesAtHeightLookup);
    }

    node->dfsEndVisit = dfsVisitNum;
    dfsVisitNum++;
}

void computeDFSInfo(Node* rootNode, vector<vector<Node*>>& nodesAtHeightLookup)
{
    int dfsVisitNum = 1;
    computeDFSInfo(rootNode, dfsVisitNum, nodesAtHeightLookup);
}

// Calculate the height of each node, and remove its parent from its list of "children".
void fixParentChildAndHeights(Node* node, Node* parent = nullptr, int height = 0)
{
    node->height = height;
    node->parent = parent;
    node->numDescendants = 1; // This node.

    node->children.erase(remove(node->children.begin(), node->children.end(), parent), node->children.end());

    for (auto child : node->children)
    {
        fixParentChildAndHeights(child, node, height + 1);
        node->numDescendants += child->numDescendants;
    }
}

struct AVLNode
{
    int value = -1;
    AVLNode *leftChild = nullptr;
    AVLNode *rightChild = nullptr;
    int balanceFactor = 0;
    int maxDescendantDepth = 0;
    int numDescendants = 1;

    int id = -1;
};

class AVLTree
{
    public:
        AVLTree(bool isPersistent = false, int nodeBlockSize = 1)
            : m_isPersistent{isPersistent}, m_nodeBlockSize{nodeBlockSize}
        {
            if (m_isPersistent)
                m_rootForRevision.push_back(nullptr);
        }
        AVLNode* root()
        {
            if (!m_isPersistent)
                return m_root;
            else
                return m_rootForRevision[m_revisionNumber];
        }
        void insertValue(int newValue)
        {
            if (!m_root)
                m_root = createNode(newValue);
            else
                m_root = insertValue(newValue, m_root);

            if (m_isPersistent)
            {
                m_rootForRevision.erase(m_rootForRevision.begin() + m_revisionNumber + 1, m_rootForRevision.end());
                m_rootForRevision.push_back(m_root);
                m_revisionNumber++;
                assert(m_revisionNumber == m_rootForRevision.size() - 1);
            }
        }
        void switchToRevision(int revisionNum)
        {
            assert(m_isPersistent);
            m_revisionNumber = revisionNum;
        }

    private:
        AVLNode* m_root = nullptr;

        AVLNode* insertValue(int newValue, AVLNode* currentNode)
        {
            if (m_isPersistent)
            {
                auto newCurrentNode = createNode(currentNode->value);
                *newCurrentNode = *currentNode;
                currentNode = newCurrentNode;
            }
            if (newValue < currentNode->value)
            {
                // Values in the left subtree of node must be *strictly less* than
                // that of currentNode.
                assert(newValue < currentNode->value);
                if (currentNode->leftChild)
                    currentNode->leftChild = insertValue(newValue, currentNode->leftChild);
                else
                    currentNode->leftChild = createNode(newValue);
            }
            else
            {
                // Values in the right subtree of node must be *greater than or equal to* that
                // that of currentNode.
                assert(newValue >= currentNode->value);
                if (currentNode->rightChild)
                    currentNode->rightChild = insertValue(newValue, currentNode->rightChild);
                else
                    currentNode->rightChild = createNode(newValue);
            }
            updateInfoFromChildren(currentNode);

            if (currentNode->balanceFactor < -1)
            {
                if (currentNode->leftChild->balanceFactor <= 0)
                {
                    // Simple rotation.
                    return rotateRight(currentNode);
                }
                else
                {
                    // Double-rotation.
                    currentNode->leftChild = rotateLeft(currentNode->leftChild);
                    return rotateRight(currentNode);
                }
            }
            if (currentNode->balanceFactor > +1)
            {
                if (currentNode->rightChild->balanceFactor >= 0)
                {
                    // Simple rotation.
                    return rotateLeft(currentNode);
                }
                else
                {
                    // Double-rotation.
                    currentNode->rightChild = rotateRight(currentNode->rightChild);
                    return rotateLeft(currentNode);
                }
            }

            return currentNode;
        }

        AVLNode* rotateRight(AVLNode* subtreeRoot)
        {
            auto newSubtreeRoot = subtreeRoot->leftChild;
            if (m_isPersistent)
            {
                // We're modifying newSubtreeRoot and subtreeRoot, but with Persistent
                // AVLTree's, we must copy-on-write.
                newSubtreeRoot = createNode(*newSubtreeRoot);
                subtreeRoot = createNode(*subtreeRoot);

            }
            auto previousNewSubtreeRootRightChild = newSubtreeRoot->rightChild;
            newSubtreeRoot->rightChild = subtreeRoot;
            subtreeRoot->leftChild = previousNewSubtreeRootRightChild;

            updateInfoFromChildren(subtreeRoot);

            updateInfoFromChildren(newSubtreeRoot);
            return newSubtreeRoot;
        }
        AVLNode* rotateLeft(AVLNode* subtreeRoot)
        {
            auto newSubtreeRoot = subtreeRoot->rightChild;
            if (m_isPersistent)
            {
                // We're modifying newSubtreeRoot and subtreeRoot, but with Persistent
                // AVLTree's, we must copy-on-write.
                newSubtreeRoot = createNode(*newSubtreeRoot);
                subtreeRoot = createNode(*subtreeRoot);
            }
            auto previousNewSubtreeRootLeftChild = newSubtreeRoot->leftChild;
            newSubtreeRoot->leftChild = subtreeRoot;
            subtreeRoot->rightChild = previousNewSubtreeRootLeftChild;
            updateInfoFromChildren(subtreeRoot);

            updateInfoFromChildren(newSubtreeRoot);
            return newSubtreeRoot;
        }

        void updateInfoFromChildren(AVLNode* nodeToUpdate)
        {
            // If m_isPersistent, assume that nodeToUpdate is already a newly-created
            // copy of the original nodeToUpdate.
            nodeToUpdate->balanceFactor = 0;
            nodeToUpdate->maxDescendantDepth = 0;
            nodeToUpdate->numDescendants = 1;

            auto leftChild = nodeToUpdate->leftChild;

            if (leftChild)
            {
                nodeToUpdate->balanceFactor -= 1 + leftChild->maxDescendantDepth;
                nodeToUpdate->maxDescendantDepth = max(nodeToUpdate->maxDescendantDepth, 1 + leftChild->maxDescendantDepth);
                nodeToUpdate->numDescendants += leftChild->numDescendants;
            }

            auto rightChild = nodeToUpdate->rightChild;
            if (rightChild)
            {
                nodeToUpdate->balanceFactor += 1 + rightChild->maxDescendantDepth;
                nodeToUpdate->maxDescendantDepth = max(nodeToUpdate->maxDescendantDepth, 1 + rightChild->maxDescendantDepth);
                nodeToUpdate->numDescendants += rightChild->numDescendants;
            }
        }

        AVLNode* createNode(int value)
        {
            auto newNode = createNode();
            newNode->value = value;
            return newNode;
        }

        AVLNode* createNode(const AVLNode& nodeToCopy)
        {
            auto newNode = createNode();
            *newNode = nodeToCopy;
            return newNode;
        }

        AVLNode* createNode()
        {
            if (m_nodes.empty() || static_cast<int>(m_nodes.back().size()) == m_nodeBlockSize)
            {
                m_nodes.push_back(vector<AVLNode>());
                m_nodes.back().reserve(m_nodeBlockSize);
            }
            m_nodes.back().push_back(AVLNode());
            auto newNode = &(m_nodes.back().back());
            newNode->id = m_nextNodeId;
            m_nextNodeId++;
            return newNode;
        }

        bool m_isPersistent = false;

        int m_nodeBlockSize = 1;
        deque<vector<AVLNode>> m_nodes;

        int m_nextNodeId = 1;

        int m_revisionNumber = 0;
        vector<AVLNode*> m_rootForRevision;

};

class AVLTreeIterator
{
    public:
        AVLTreeIterator(AVLNode* root)
            : m_currentNode(root)
        {
        };
        AVLNode* currentNode() const
        {
            return m_currentNode;
        }
        int numToLeft() const
        {
            const auto numInLeftSubTree = (m_currentNode->leftChild ? m_currentNode->leftChild->numDescendants : 0);
            return m_numToLeftOffset + m_numInLeftSubTree;
        }
        void followLeftChild()
        {
            m_currentNode = m_currentNode->leftChild;
        }
        void followRightChild()
        {
            const auto numInLeftSubTree = (m_currentNode->leftChild ? m_currentNode->leftChild->numDescendants : 0);
            m_numToLeftOffset += m_numInLeftSubTree + 1;
            m_currentNode = m_currentNode->rightChild;
        }
    private:
        AVLNode* m_currentNode = nullptr;

        int m_numToLeftOffset = 0;
        int m_numInLeftSubTree = 0;
};

#define VERIFY_CHOICES_WITH_REMOVALS
void choicesWithRemovals(const vector<int>& numOfRemainingToChoose, int numToChooseFrom)
{
#ifdef VERIFY_CHOICES_WITH_REMOVALS
    cout << "choicesWithRemovals: numToChooseFrom " << numToChooseFrom << " choices:";
    for (const auto x : numOfRemainingToChoose)
    {
        cout << " " << x;
    }
    cout << endl;
    vector<bool> dbgHasBeenRemoved(numToChooseFrom, false);
#endif
    AVLTree removedIndices;

    for (const auto nthOfRemainingToChoose : numOfRemainingToChoose)
    {
        // Be optimistic and give remappedIndex the smallest possible value:
        // we'll correct our optimism as we go along :)
        int remappedIndex = nthOfRemainingToChoose;
        auto currentNode = removedIndices.root();
        int numRemovedUpToCurrentNodeIndexOffset = 0;
        while (currentNode)
        {
            const int indexOfCurrentNode = currentNode->value;
            const int numDescendantsLeftSubChild = (currentNode->leftChild ? currentNode->leftChild->numDescendants : 0);
            const int numRemovedUpToCurrentNodeIndex = numRemovedUpToCurrentNodeIndexOffset + numDescendantsLeftSubChild;
            const int numFreeUpToCurrentNodeIndex = indexOfCurrentNode - numRemovedUpToCurrentNodeIndex;
            if (numFreeUpToCurrentNodeIndex >= nthOfRemainingToChoose + 1)
            {
                // We've overshot; the required remappedIndex is to the left of indexOfCurrentNode; "recurse"
                // into left child.
                currentNode = currentNode->leftChild;
            }
            else
            {
                // Again, be optimistic about remappedIndex - we'll correct it as we go along.
                remappedIndex = max(remappedIndex, indexOfCurrentNode + (nthOfRemainingToChoose - numFreeUpToCurrentNodeIndex) + 1);
                // Required index is to the right of here; "recurse" into the right child.
                // In doing this, we're "forgetting" all the Removed indices to the left of
                // currentNode - record them in numRemovedUpToCurrentNodeIndexOffset.
                numRemovedUpToCurrentNodeIndexOffset += 1 + // currentNode is Removed ...
                                                        numDescendantsLeftSubChild; // ... as are all the indices in currentNode->leftChild.
                currentNode = currentNode->rightChild;
            }

        }
        // We've successfully found the index in the original array; now mark it as Removed.
        removedIndices.insertValue(remappedIndex);

#ifdef VERIFY_CHOICES_WITH_REMOVALS
        // Inefficient debug version.
        int dbgRemappedIndex = 0;
        while (dbgHasBeenRemoved[dbgRemappedIndex])
            dbgRemappedIndex++;
        for (int i = 0; i < nthOfRemainingToChoose; i++)
        {
            dbgRemappedIndex++;
            while (dbgHasBeenRemoved[dbgRemappedIndex])
                dbgRemappedIndex++;
        }
        cout << " remappedIndex: " << remappedIndex << " dbgRemappedIndex: " << dbgRemappedIndex << endl;
        assert(remappedIndex == dbgRemappedIndex);
        dbgHasBeenRemoved[remappedIndex] = true;
#endif


    }

    cout << "Finished choicesWithRemovals" << endl;
}

class IndexRemapper
{
    public:
        int remapNthRemainingToIndexAndRemove(int nthOfRemainingToChoose)
        {
            // Be optimistic and give remappedIndex the smallest possible value:
            // we'll correct our optimism as we go along :)
            int remappedIndex = nthOfRemainingToChoose;
            auto currentNode = removedIndices.root();
            int numRemovedUpToCurrentNodeIndexOffset = 0;
            AVLTreeIterator treeIter(removedIndices.root());
            while (treeIter.currentNode())
            {
                const int indexOfCurrentNode = treeIter.currentNode()->value;
                const int numRemovedUpToCurrentNodeIndex = treeIter.numToLeft();
                const int numFreeUpToCurrentNodeIndex = indexOfCurrentNode - numRemovedUpToCurrentNodeIndex;
                if (numFreeUpToCurrentNodeIndex >= nthOfRemainingToChoose + 1)
                {
                    // We've overshot; the required remappedIndex is to the left of indexOfCurrentNode; "recurse"
                    // into left child.
                    treeIter.followLeftChild();
                }
                else
                {
                    // Again, be optimistic about remappedIndex - we'll correct it as we go along.
                    remappedIndex = max(remappedIndex, indexOfCurrentNode + (nthOfRemainingToChoose - numFreeUpToCurrentNodeIndex) + 1);
                    // Required index is to the right of here; "recurse" into the right child.
                    treeIter.followRightChild();
                }

            }
            // We've successfully found the index in the original array; now mark it as Removed.
            removedIndices.insertValue(remappedIndex);
            return remappedIndex;
        }
    private:
        AVLTree removedIndices;
};



vector<pair<Node*, Node*>> computeOrderedValidReparentings(vector<Node>& nodes)
{
    vector<pair<Node*, Node*>> validReparentings;

    for (auto& nodeToReparent : nodes)
    {
        if (nodeToReparent.parent == nullptr)
            continue;

        for (auto& newParent : nodes)
        {
            assert(newParent.dfsBeginVisit != -1 && newParent.dfsEndVisit != -1);
            const bool newParentIsDescendant = (newParent.dfsBeginVisit >= nodeToReparent.dfsBeginVisit && newParent.dfsEndVisit <= nodeToReparent.dfsEndVisit);
            if (!newParentIsDescendant)
                validReparentings.push_back({&nodeToReparent, &newParent});
        }
    }

    sort(validReparentings.begin(), validReparentings.end(),
            [](const auto& lhs, const auto& rhs)
            {
            if (lhs.first->id != rhs.first->id)
                return lhs.first->id < rhs.first->id;
            if (lhs.second->height != rhs.second->height)
                return lhs.second->height < rhs.second->height;
            return lhs.second->id < rhs.second->id;
            });


    return validReparentings;
}

vector<pair<Node*, Node*>> solveBruteForce(vector<Node>& nodes, const vector<int64_t>& queries)
{
    vector<pair<Node*, Node*>> result;
    auto rootNode = &(nodes.front());
    vector<vector<Node*>> nodesAtHeightLookupDummy;
    computeDFSInfo(rootNode, nodesAtHeightLookupDummy);

    auto validReparentings = computeOrderedValidReparentings(nodes);

    for (const auto query : queries)
    {
        const auto index = query - 1; // Make 0-relative.
        assert(0 <= index && index < validReparentings.size());
        auto queryResultIter = validReparentings.begin() + index;
        result.push_back(*queryResultIter);

        validReparentings.erase(queryResultIter);
    }

    return result;
}

#if 1
vector<pair<Node*, Node*>> solveOptimised(vector<Node>& nodes, const vector<int64_t>& queries)
{
    int maxNodeHeight = -1;
    for (const auto& node : nodes)
    {
        maxNodeHeight = max(maxNodeHeight, node.height);
    }
    assert(maxNodeHeight != -1);

    vector<pair<Node*, Node*>> result;
    auto rootNode = &(nodes.front());
    vector<vector<Node*>> nodesAtHeightLookup(maxNodeHeight + 1);
    computeDFSInfo(rootNode, nodesAtHeightLookup);
    const int numNodes = nodes.size();
    for (auto& node : nodes)
    {
        // We can reparent to any node that is not a descendant.
        node.numCanReparentTo = numNodes - node.numDescendants;
    }
    cout << "solveOptimised - numNodes: " << numNodes << endl;
    for (const auto& node : nodes)
    {
        cout << "node: " << node.id << " numCanReparentTo: " << node.numCanReparentTo << endl;
    }
    
    const auto validReparentings = computeOrderedValidReparentings(nodes);
    vector<bool> reparentingRemoved(validReparentings.size(), false);

    for (const auto query : queries)
    {
        const auto dbgIndexOriginal = query - 1; // Make 0-relative.
        cout << " query: dbgIndexOriginal: " << dbgIndexOriginal << endl;
        cout << "validReparentings: " << endl;
        for (int i = 0; i < validReparentings.size(); i++)
        {
            cout << "i: " << i << " nodeToReparent: " << validReparentings[i].first->id << " newParent: " << validReparentings[i].second->id << " height: " << validReparentings[i].second->height << " removed? " << (reparentingRemoved[i] ? "true" : "false") << endl;
        }

        int indexInOriginalList = 0;
        Node* dbgNodeToReparent = nullptr;
        Node* dbgNewParent = nullptr;
        {
            int adjustedIndex = 0;
            while (true)
            {
                if (!reparentingRemoved[indexInOriginalList])
                {
                    if (adjustedIndex == dbgIndexOriginal)
                    {
                        dbgNodeToReparent = validReparentings[indexInOriginalList].first;
                        dbgNewParent = validReparentings[indexInOriginalList].second;
                        break;
                    }
                    adjustedIndex++;
                }
                indexInOriginalList++;
            }
        }
        assert(dbgNodeToReparent != nullptr && dbgNewParent != nullptr);
        cout << " indexInOriginalList: " << indexInOriginalList << endl;
        //assert(0 <= dbgIndex && dbgIndex < validReparentings.size());
        //auto queryResultIter = validReparentings.begin() + dbgIndex;
        //const auto dbgNodeToReparent = queryResultIter->first;
        //const auto dbgNewParent = queryResultIter->second;
        cout << "  dbgNodeToReparent: " << dbgNodeToReparent->id << " dbgNewParent: " << dbgNewParent-> id << endl;
        result.push_back({dbgNodeToReparent, dbgNewParent});

        int sumOfNumCanReparentTo = 0;
        Node* nodeToReparent = nullptr;
        const int index = indexInOriginalList;
        for (auto& node : nodes)
        {
            cout << " node: " << node.id << " numCanReparentTo: " << node.numCanReparentTo << " sumOfNumCanReparentTo: " << sumOfNumCanReparentTo << " sumOfNumCanReparentTo: " << endl;
            if (sumOfNumCanReparentTo + node.numCanReparentTo > index)
            {
                nodeToReparent = &node;
                break;
            }
            sumOfNumCanReparentTo += node.numCanReparentTo;

        }
        assert(nodeToReparent);
        cout << "nodeToReparent: " << nodeToReparent->id << endl;
        assert(nodeToReparent == dbgNodeToReparent);
        reparentingRemoved[indexInOriginalList] = true;
    }

    return result;
}
#endif


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        // TODO - generate randomised test.
        //const int T = rand() % 100 + 1;
        const int T = 1;
        cout << T << endl;

        for (int t = 0; t < T; t++)
        {
            const auto numNodes = 1 + rand() % 10;
            vector<Node> nodes(numNodes);
            for (int i = 0; i < numNodes; i++)
                nodes[i].id = (i + 1);

            vector<pair<Node*, Node*>> edges;

            for (int childNodeIndex = 1; childNodeIndex < numNodes; childNodeIndex++)
            {
                const int parentIndex = rand() % childNodeIndex;
                edges.push_back({&(nodes[parentIndex]), &(nodes[childNodeIndex])});
            }
            for (auto& edge : edges)
            {
                edge.first->children.push_back(edge.second);
                edge.second->children.push_back(edge.first);
            }

            auto rootNode = &(nodes.front());
            fixParentChildAndHeights(rootNode);
            vector<vector<Node*>> nodesAtHeightLookupDummy;
            computeDFSInfo(rootNode, nodesAtHeightLookupDummy);
            const auto validReparentings = computeOrderedValidReparentings(nodes);

            const auto numQueries = validReparentings.empty() ? 0 : 1 + rand() % validReparentings.size();

            vector<int64_t> queries;
            for (int queryNum = 0; queryNum < numQueries; queryNum++)
            {
                queries.push_back(1 + rand() % (validReparentings.size() - queryNum));
            }

            cout << numNodes << endl;
            for (const auto edge : edges)
            {
                cout << edge.first->id << " " << edge.second->id << endl;
            }
            cout << numQueries << endl;
            for (const auto query : queries)
            {
                cout << query << endl;
            }
        }

        return 0;
    }
    
    // TODO - read in testcase.
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const auto numNodes = read<int>();

        vector<Node> nodes(numNodes);
        for (int i = 0; i < numNodes; i++)
        {
            nodes[i].id = i + 1;
        }
        for (auto i = 0; i < numNodes - 1; i++)
        {
            // Make a and b 0-relative.
            const auto a = read<int>() - 1;
            const auto b = read<int>() - 1;

            nodes[a].children.push_back(&nodes[b]);
            nodes[b].children.push_back(&nodes[a]);
        }

        auto rootNode = &(nodes.front());
        fixParentChildAndHeights(rootNode);

        // TODO - eventually, we will need to make this online, using a decryption-key
        // approach (similar to "Simple Markdown Editor").
        const auto numQueries = read<int>();
        vector<int64_t> queries(numQueries);
        for (auto& query : queries)
        {
            query = read<int64_t>();
        }


#ifdef BRUTE_FORCE
#if 1
        const auto solutionBruteForce = solveBruteForce(nodes, queries);
        cout << "solutionBruteForce: " << endl;
        for (const auto result : solutionBruteForce)
        {
            cout << "solutionBruteForce: " << result.first->id << " " << result.second->id << endl;
        }
#endif
#if 1
        const auto solutionOptimised = solveOptimised(nodes, queries);
        cout << "solutionOptimised: " << endl;
        for (const auto result : solutionOptimised)
        {
            cout << "solutionOptimised: " << result.first->id << " " << result.second->id << endl;
        }

        assert(solutionOptimised == solutionBruteForce);
#endif
#else
        const auto solutionOptimised = solveOptimised();
        cout << solutionOptimised << endl;
#endif
    }

    assert(cin);
}
