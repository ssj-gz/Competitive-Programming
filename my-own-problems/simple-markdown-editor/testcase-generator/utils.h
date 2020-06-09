#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <deque>
#include <algorithm>

using namespace std;

struct AVLNode
{
    int64_t leftNonFormattedRunSize = -999;
    AVLNode *leftChild = nullptr;
    AVLNode *rightChild = nullptr;
    int balanceFactor = 0;
    int maxDescendantDepth = 0;
    int totalFormattedDescendants = 1;
    int64_t totalNonFormattedDescendants = 0;
    int totalFormattedDescendantsWithNonFormattedToLeft = 0;

    bool isSentinelValue = false;
    int id = -1;
};

class AVLTreeIterator;

class AVLTree
{
    public:
        AVLTree(int nodeBlockSize = 1)
            : m_nodeBlockSize{nodeBlockSize}
        {
            m_rootForRevision.push_back(nullptr);
        }
        AVLNode* root() const
        {
            return m_rootForRevision[m_undoStackPointer];
        }
        void insertFormattingChar(int64_t position);
        void insertNonFormattingChars(int64_t position, int64_t numToAdd);
        int64_t distBetweenEnclosingFormattedChars(int64_t position);
        AVLTreeIterator findFirstNodeAtOrToRightOf(int64_t position);
        AVLTreeIterator findKthFormattingCharWithNonFormattingToLeft(int64_t k);
        AVLTreeIterator findKthFormattingCharWithoutNonFormattingToLeft(int64_t k);
        AVLTreeIterator findKthFormattingChar(int64_t k);
        int numFormattingCharWithNonFormattingCharsAtOrToLeft(int64_t position);

        int undoStackPointer() const
        {
            return m_undoStackPointer - 2; // Match the definition in the Problem statement: should be -1 initially (hence - 1), but also need to subtract item resulting from adding sentinel node (the other -1).
        }
        int undoStackSize() const
        {
            return m_rootForRevision.size() - 2; // Match the definition in the Problem statement: should be 0 initially (hence -1) , but also need to subtract item resulting from adding sentinel node (the other -1).
        }

        int64_t numFormattingChars() const
        {
            return root()->totalFormattedDescendants - 1; // "-1" to discount the Sentinel node.
        }
        int64_t numNonFormattingChars() const
        {
            return root()->totalNonFormattedDescendants;
        }

        int64_t documentLength() const
        {
            return numFormattingChars() + numNonFormattingChars();
        }

        string documentString()
        {
            string document = subtreeAsDocument(root());
            document.pop_back(); // Remove the sentinel.
            return document;
        }

        AVLNode* nodeWithId(int id)
        {
            auto node = m_nodeForId[id];
            assert(node);
            return node;
        }

        void undo(int numToUndo)
        {
            m_undoStackPointer -= numToUndo;
        }
        void redo(int numToRedo)
        {
            m_undoStackPointer += numToRedo;
        }

    private:
        AVLNode* insertFormattingChar(int64_t position, int64_t sizeOfUnformattedToLeftRun, AVLTreeIterator& treeIter);

        AVLNode* rotateRight(AVLNode* subtreeRoot)
        {
            auto newSubtreeRoot = subtreeRoot->leftChild;

            // We're modifying newSubtreeRoot and subtreeRoot, so copy-on-write.
            newSubtreeRoot = createNode(*newSubtreeRoot);
            subtreeRoot = createNode(*subtreeRoot);

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
            // We're modifying newSubtreeRoot and subtreeRoot, so copy-on-write.
            newSubtreeRoot = createNode(*newSubtreeRoot);
            subtreeRoot = createNode(*subtreeRoot);
            auto previousNewSubtreeRootLeftChild = newSubtreeRoot->leftChild;
            newSubtreeRoot->leftChild = subtreeRoot;
            subtreeRoot->rightChild = previousNewSubtreeRootLeftChild;
            updateInfoFromChildren(subtreeRoot);

            updateInfoFromChildren(newSubtreeRoot);
            return newSubtreeRoot;
        }

        void updateInfoFromChildren(AVLNode* nodeToUpdate)
        {
            // We assume that nodeToUpdate is already a newly-COW'd
            // copy of the original nodeToUpdate.
            nodeToUpdate->balanceFactor = 0;
            nodeToUpdate->maxDescendantDepth = 0;
            nodeToUpdate->totalFormattedDescendants = 1;
            nodeToUpdate->totalNonFormattedDescendants = nodeToUpdate->leftNonFormattedRunSize;
            nodeToUpdate->totalFormattedDescendantsWithNonFormattedToLeft = (nodeToUpdate->leftNonFormattedRunSize > 0 ? 1 : 0);

            auto leftChild = nodeToUpdate->leftChild;

            if (leftChild)
            {
                nodeToUpdate->balanceFactor -= 1 + leftChild->maxDescendantDepth;
                nodeToUpdate->maxDescendantDepth = max(nodeToUpdate->maxDescendantDepth, 1 + leftChild->maxDescendantDepth);
                nodeToUpdate->totalFormattedDescendants += leftChild->totalFormattedDescendants;
                nodeToUpdate->totalNonFormattedDescendants += leftChild->totalNonFormattedDescendants;
                nodeToUpdate->totalFormattedDescendantsWithNonFormattedToLeft += leftChild->totalFormattedDescendantsWithNonFormattedToLeft;
            }

            auto rightChild = nodeToUpdate->rightChild;
            if (rightChild)
            {
                nodeToUpdate->balanceFactor += 1 + rightChild->maxDescendantDepth;
                nodeToUpdate->maxDescendantDepth = max(nodeToUpdate->maxDescendantDepth, 1 + rightChild->maxDescendantDepth);
                nodeToUpdate->totalFormattedDescendants += rightChild->totalFormattedDescendants;
                nodeToUpdate->totalNonFormattedDescendants += rightChild->totalNonFormattedDescendants;
                nodeToUpdate->totalFormattedDescendantsWithNonFormattedToLeft += rightChild->totalFormattedDescendantsWithNonFormattedToLeft;
            }
        }

        AVLNode* adjustRunToLeftOfNodeToRightOf(AVLTreeIterator& treeIter, int64_t position, int64_t adjustment);

        AVLNode* createNode(int64_t leftNonFormattedRunSize)
        {
            auto newNode = createNode();
            newNode->leftNonFormattedRunSize = leftNonFormattedRunSize;
            newNode->totalNonFormattedDescendants = leftNonFormattedRunSize;
            newNode->totalFormattedDescendantsWithNonFormattedToLeft = (newNode->leftNonFormattedRunSize > 0 ? 1 : 0);
            return newNode;
        }

        AVLNode* createNode(const AVLNode& nodeToCopy)
        {
            auto newNode = createNode();
            *newNode = nodeToCopy;
            m_nodeForId[newNode->id] = newNode;
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
            m_nodeForId[m_nextNodeId] = newNode;
            return newNode;
        }

        void updateUndoStackWithNewRoot(AVLNode* newRoot)
        {
            m_rootForRevision.erase(m_rootForRevision.begin() + m_undoStackPointer + 1, m_rootForRevision.end());
            m_rootForRevision.push_back(newRoot);
            m_undoStackPointer++;
            assert(m_undoStackPointer == static_cast<int>(m_rootForRevision.size()) - 1);
        }

        string subtreeAsDocument(AVLNode* subtreeRoot)
        {
            if (!subtreeRoot)
                return "";
            string asDoc = subtreeAsDocument(subtreeRoot->leftChild);
            asDoc += string(subtreeRoot->leftNonFormattedRunSize, 'X');
            asDoc += "*";
            asDoc += subtreeAsDocument(subtreeRoot->rightChild);

            return asDoc;
        }


        int m_nodeBlockSize = 1;
        deque<vector<AVLNode>> m_nodes;

        int m_undoStackPointer = 0;
        vector<AVLNode*> m_rootForRevision;

        int m_nextNodeId = 0;

        std::map<int, AVLNode*> m_nodeForId;
};

class AVLTreeIterator
{
    public:
        AVLTreeIterator(AVLNode* root)
            : m_currentNode(root)
        {
            updateCurrentNodePosition();
        };
        AVLNode* currentNode() const
        {
            return m_currentNode;
        }
        int64_t currentNodePosition() const
        {
            return m_currentNodePosition;
        }
        int64_t numFormattingCharsToLeft() const
        {
            return m_numToLeftOffset + m_numInLeftSubTree;
        }
        void followLeftChild()
        {
            m_currentNode = m_currentNode->leftChild;
            updateCurrentNodePosition();
        }
        void followRightChild()
        {
            m_numToLeftOffset += m_numInLeftSubTree + 1;
            m_sumToLeftOffset += m_sumOfLeftSubTree + m_currentNode->leftNonFormattedRunSize;
            m_currentNode = m_currentNode->rightChild;
            updateCurrentNodePosition();
        }
        bool operator==(const AVLTreeIterator& other) const
        {
            return currentNode() == other.currentNode();
        }
    private:
        AVLNode* m_currentNode = nullptr;
        int64_t m_currentNodePosition = -1;
        int64_t m_numToLeftOffset = 0;
        int64_t m_sumToLeftOffset = 0;

        int64_t m_numInLeftSubTree = 0;
        int64_t m_sumOfLeftSubTree = 0;

        void updateCurrentNodePosition()
        {
            if (!m_currentNode)
                return;
            m_numInLeftSubTree = (m_currentNode->leftChild ? m_currentNode->leftChild->totalFormattedDescendants : 0);
            m_sumOfLeftSubTree = (m_currentNode->leftChild ? m_currentNode->leftChild->totalNonFormattedDescendants : 0);
            m_currentNodePosition = m_numToLeftOffset + m_numInLeftSubTree + m_sumToLeftOffset + m_sumOfLeftSubTree + m_currentNode->leftNonFormattedRunSize;
        }
};

void AVLTree::insertFormattingChar(int64_t position)
{
    AVLNode* newRoot = nullptr;
    if (!root())
    {
        newRoot = createNode(0); // Sentinel node.
    }
    else
    {
        const AVLTreeIterator formattingCharToRightIter = findFirstNodeAtOrToRightOf(position);
        const int64_t newFormattingCharSizeOfUnformattedToLeftRun = formattingCharToRightIter.currentNode()->leftNonFormattedRunSize - (formattingCharToRightIter.currentNodePosition() - position);
        const int64_t adjustedFormattingCharToRightSizeOfUnformattedToLeftRun = formattingCharToRightIter.currentNodePosition() - position;
        assert(newFormattingCharSizeOfUnformattedToLeftRun >= 0);
        assert(adjustedFormattingCharToRightSizeOfUnformattedToLeftRun >= 0);
        {
            // Perform the actual insertion.
            AVLTreeIterator treeIter(root());
            newRoot = insertFormattingChar(position, newFormattingCharSizeOfUnformattedToLeftRun, treeIter);
        }
        // Update the "unformatted run size" of the formattingCharToRight.
        AVLTreeIterator treeIter(newRoot);
        newRoot = adjustRunToLeftOfNodeToRightOf(treeIter, position + 1, adjustedFormattingCharToRightSizeOfUnformattedToLeftRun - formattingCharToRightIter.currentNode()->leftNonFormattedRunSize);
    }

    updateUndoStackWithNewRoot(newRoot);
}

AVLNode* AVLTree::insertFormattingChar(int64_t position, int64_t sizeOfUnformattedToLeftRun, AVLTreeIterator& treeIter)
{
    // We'll be altering this node whatever happens, so do a COW.
    auto currentNode = createNode(*treeIter.currentNode());
    if (position <= treeIter.currentNodePosition())
    {
        if (currentNode->leftChild)
        {
            treeIter.followLeftChild();
            currentNode->leftChild = insertFormattingChar(position, sizeOfUnformattedToLeftRun, treeIter);
        }
        else
        {
            currentNode->leftChild = createNode(sizeOfUnformattedToLeftRun);
        }
    }
    else
    {
        if (currentNode->rightChild)
        {
            treeIter.followRightChild();
            currentNode->rightChild = insertFormattingChar(position, sizeOfUnformattedToLeftRun, treeIter);
        }
        else
        {
            currentNode->rightChild = createNode(sizeOfUnformattedToLeftRun);
        }
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

void AVLTree::insertNonFormattingChars(int64_t position, int64_t numToAdd)
{
    AVLTreeIterator treeIter(root());
    auto newRoot = adjustRunToLeftOfNodeToRightOf(treeIter, position, numToAdd);
    updateUndoStackWithNewRoot(newRoot);
}

AVLNode* AVLTree::adjustRunToLeftOfNodeToRightOf(AVLTreeIterator& treeIter, int64_t position, int64_t adjustment)
{
    auto subTreeRoot = treeIter.currentNode();
    if (!subTreeRoot)
        return subTreeRoot;
    // Whatever happens, we'll be altering properties of this node, so do a COW.
    subTreeRoot = createNode(*subTreeRoot);
    if (position <= treeIter.currentNodePosition())
    {
        const auto maxPosInLeftSubchild = treeIter.currentNodePosition() - subTreeRoot->leftNonFormattedRunSize - 1;
        if (maxPosInLeftSubchild < position)
        {
            // This is the node to adjust.
            subTreeRoot->leftNonFormattedRunSize += adjustment;
        }
        else
        {
            treeIter.followLeftChild();
            subTreeRoot->leftChild = adjustRunToLeftOfNodeToRightOf(treeIter, position, adjustment);
        }
    }
    else
    {
        treeIter.followRightChild();
        subTreeRoot->rightChild = adjustRunToLeftOfNodeToRightOf(treeIter, position, adjustment);
    }
    updateInfoFromChildren(subTreeRoot);
    return subTreeRoot;
}

AVLTreeIterator AVLTree::findFirstNodeAtOrToRightOf(int64_t position)
{
    AVLTreeIterator treeIter(root());
    AVLTreeIterator result(nullptr);
    while (treeIter.currentNode())
    {
        if (treeIter.currentNodePosition() >= position)
        {
            result = treeIter;
            treeIter.followLeftChild();
        }
        else
        {
            treeIter.followRightChild();
        }
    }
    assert(result.currentNode());
    return result;
}

AVLTreeIterator AVLTree::findKthFormattingChar(int64_t k)
{
    AVLTreeIterator treeIter(root());
    AVLTreeIterator result(nullptr);
    while (treeIter.currentNode())
    {
        if (treeIter.numFormattingCharsToLeft() == k)
        {
            return treeIter;
        }
        else if (treeIter.numFormattingCharsToLeft() < k)
        {
            treeIter.followRightChild();
        }
        else
        {
            result = treeIter;
            treeIter.followLeftChild();
        }
    }
    assert(result.currentNode());
    return result;
}

AVLTreeIterator AVLTree::findKthFormattingCharWithNonFormattingToLeft(int64_t k)
{
    AVLTreeIterator treeIter(root());
    AVLTreeIterator result(nullptr);
    int numOffset = 0;
    while (treeIter.currentNode())
    {
        const int numInLeftSubTree = treeIter.currentNode()->leftChild ? treeIter.currentNode()->leftChild->totalFormattedDescendantsWithNonFormattedToLeft  : 0;
        const int numAtNode = (treeIter.currentNode()->leftNonFormattedRunSize > 0 ? 1 : 0);
        const int totalNumBefore = numInLeftSubTree + numOffset;
        if (totalNumBefore == k && (treeIter.currentNode()->leftNonFormattedRunSize))
        {
            return treeIter;
        }
        else if (totalNumBefore + numAtNode < k + 1)
        {
            treeIter.followRightChild();
            numOffset += numInLeftSubTree + numAtNode;
        }
        else
        {
            result = treeIter;
            treeIter.followLeftChild();
        }
    }
    assert(result.currentNode());
    return result;
}

AVLTreeIterator AVLTree::findKthFormattingCharWithoutNonFormattingToLeft(int64_t k)
{
    AVLTreeIterator treeIter(root());
    AVLTreeIterator result(nullptr);
    int numOffset = 0;
    while (treeIter.currentNode())
    {
        const int numInLeftSubTree = treeIter.currentNode()->leftChild ? treeIter.currentNode()->leftChild->totalFormattedDescendants - treeIter.currentNode()->leftChild->totalFormattedDescendantsWithNonFormattedToLeft  : 0;
        const int numAtNode = (treeIter.currentNode()->leftNonFormattedRunSize > 0 ? 0 : 1);
        const int totalNumBefore = numInLeftSubTree + numOffset;
        if (totalNumBefore == k && (treeIter.currentNode()->leftNonFormattedRunSize == 0))
        {
            return treeIter;
        }
        else if (totalNumBefore + numAtNode < k + 1)
        {
            treeIter.followRightChild();
            numOffset += numInLeftSubTree + numAtNode;
        }
        else
        {
            result = treeIter;
            treeIter.followLeftChild();
        }
    }
    assert(result.currentNode());
    return result;
}

int AVLTree::numFormattingCharWithNonFormattingCharsAtOrToLeft(int64_t position)
{
    AVLTreeIterator treeIter(root());
    int result = 0;
    int numOffset = 0;
    //cout << "numFormattingCharWithNonFormattingCharsToLeft - position: " << position << endl;
    while (treeIter.currentNode())
    {
        const int numInLeftSubTree = treeIter.currentNode()->leftChild ? treeIter.currentNode()->leftChild->totalFormattedDescendantsWithNonFormattedToLeft  : 0;
        const int numAtNode = (treeIter.currentNode()->leftNonFormattedRunSize > 0 ? 1 : 0);
        const int totalNumBefore = numInLeftSubTree + numOffset;
        //cout << "currentNodePosition: " << treeIter.currentNodePosition() << " numOffset: " << numOffset << " totalNumBefore: " << totalNumBefore << " numAtNode: " << numAtNode << endl;
        if (treeIter.currentNodePosition() == position)
        {
            //cout << " returning " << totalNumBefore << endl;
            return totalNumBefore + numAtNode;
        }
        else if (treeIter.currentNodePosition() < position)
        {
            treeIter.followRightChild();
            result = totalNumBefore + numAtNode;
            //cout << " Following right child; updating result to " << result << endl;
            numOffset += numInLeftSubTree + numAtNode;
        }
        else
        {
            //cout << " Following left child" << endl;
            treeIter.followLeftChild();
        }
    }
    //cout << " returning last updated result: " << result << endl;
    return result;
}

int64_t AVLTree::distBetweenEnclosingFormattedChars(int64_t position)
{
    const AVLTreeIterator formattingCharToRightIter = findFirstNodeAtOrToRightOf(position);

    if (formattingCharToRightIter.currentNode()->isSentinelValue)
    {
        // There are no formatting chars to the right - only the "fake" Sentinel value.
        return -1;
    }
    if (formattingCharToRightIter.numFormattingCharsToLeft() % 2 == 0)
    {
        // The formatting char to the right is the *beginning* of a formatted range, not
        // the end.
        return -1;
    }
    return formattingCharToRightIter.currentNode()->leftNonFormattedRunSize;
}

#endif

