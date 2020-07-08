// Simon St James (ssjgz) - 2020-03-17
// 
// Editorial Solution to: https://www.codechef.com/problems/SIMKDOWN
//
#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>

#include <cassert>

using namespace std;

const int64_t Mod = 1'000'000'007;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    return toRead;
}

struct Query
{
    enum Type { InsertFormatting, InsertNonFormatting, IsRangeFormatted, Undo, Redo };
    Type type;
    int64_t encryptedArgument = -1;
    int64_t encryptedArgument2 = -1;
};

struct AVLNode
{
    int64_t leftNonFormattedRunSize = -1;
    AVLNode *leftChild = nullptr;
    AVLNode *rightChild = nullptr;
    int balanceFactor = 0;
    int maxDescendantDepth = 0;
    int totalFormattedDescendants = 1;
    int64_t totalNonFormattedDescendants = 0;

    bool isSentinelValue = false;
};

class AVLTreeIterator;

class AVLTree
{
    public:
        AVLTree()
        {
            m_rootForRevision.push_back(nullptr);
        }
        AVLNode* root()
        {
            return m_rootForRevision[m_undoStackPointer];
        }
        void insertFormattingChar(int64_t position);
        void insertNonFormattingChars(int64_t position, int64_t numToAdd);
        int64_t distBetweenEnclosingFormattedChars(int64_t position);
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

            auto leftChild = nodeToUpdate->leftChild;

            if (leftChild)
            {
                nodeToUpdate->balanceFactor -= 1 + leftChild->maxDescendantDepth;
                nodeToUpdate->maxDescendantDepth = max(nodeToUpdate->maxDescendantDepth, 1 + leftChild->maxDescendantDepth);
                nodeToUpdate->totalFormattedDescendants += leftChild->totalFormattedDescendants;
                nodeToUpdate->totalNonFormattedDescendants += leftChild->totalNonFormattedDescendants;
            }

            auto rightChild = nodeToUpdate->rightChild;
            if (rightChild)
            {
                nodeToUpdate->balanceFactor += 1 + rightChild->maxDescendantDepth;
                nodeToUpdate->maxDescendantDepth = max(nodeToUpdate->maxDescendantDepth, 1 + rightChild->maxDescendantDepth);
                nodeToUpdate->totalFormattedDescendants += rightChild->totalFormattedDescendants;
                nodeToUpdate->totalNonFormattedDescendants += rightChild->totalNonFormattedDescendants;
            }
        }

        AVLNode* adjustRunToLeftOfNodeToRightOf(AVLTreeIterator& treeIter, int64_t position, int64_t adjustment);

        AVLNode* createNode(int64_t leftNonFormattedRunSize)
        {
            auto newNode = createNode();
            newNode->leftNonFormattedRunSize = leftNonFormattedRunSize;
            newNode->totalNonFormattedDescendants = leftNonFormattedRunSize;
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
            m_nodes.push_back(AVLNode());
            auto newNode = &(m_nodes.back());
            return newNode;
        }

        AVLTreeIterator findFirstNodeToRightOf(int64_t position, AVLNode* root);

        void updateUndoStackWithNewRoot(AVLNode* newRoot)
        {
            m_rootForRevision.erase(m_rootForRevision.begin() + m_undoStackPointer + 1, m_rootForRevision.end());
            m_rootForRevision.push_back(newRoot);
            m_undoStackPointer++;
            assert(m_undoStackPointer == static_cast<int>(m_rootForRevision.size()) - 1);
        }

        deque<AVLNode> m_nodes;

        int m_undoStackPointer = 0;
        vector<AVLNode*> m_rootForRevision;
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
        const AVLTreeIterator formattingCharToRightIter = findFirstNodeToRightOf(position, root());
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

AVLTreeIterator AVLTree::findFirstNodeToRightOf(int64_t position, AVLNode* root)
{
    AVLTreeIterator treeIter(root);
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

int64_t solveOptimised(const vector<Query>& queries)
{
    int64_t decryptionKey = 0;
    int64_t powerOf2 = 2;

    vector<int64_t> queryResults;
    AVLTree formattingCharsTree;
    // Add Sentinel node.
    formattingCharsTree.insertFormattingChar(0);
    formattingCharsTree.root()->isSentinelValue = true;

    int queryNum = 1;
    for (const auto& query : queries)
    {
        switch (query.type)
        {
            case Query::InsertFormatting:
                {
                    const auto insertionPos = (query.encryptedArgument ^ decryptionKey) - 1;
                    formattingCharsTree.insertFormattingChar(insertionPos);
                }
                break;
            case Query::InsertNonFormatting:
                {
                    const auto insertionPos = (query.encryptedArgument ^ decryptionKey) - 1;
                    const auto numToInsert = (query.encryptedArgument2 ^ decryptionKey);
                    formattingCharsTree.insertNonFormattingChars(insertionPos, numToInsert);
                }
                break;
            case Query::IsRangeFormatted:
                {
                    const auto queryPosition = (query.encryptedArgument ^ decryptionKey) - 1;
                    auto queryAnswer = formattingCharsTree.distBetweenEnclosingFormattedChars(queryPosition);
                    if (queryAnswer == -1)
                        queryAnswer = 3'141'592;
                    decryptionKey = (decryptionKey + ((queryAnswer % Mod) * powerOf2) % Mod) % Mod;

                    queryResults.push_back(queryAnswer);
                }
                break;
            case Query::Undo:
                {
                    const int numToUndo = (query.encryptedArgument ^ decryptionKey);
                    formattingCharsTree.undo(numToUndo);
                }
                break;
            case Query::Redo:
                {
                    const int numToRedo = (query.encryptedArgument ^ decryptionKey);
                    formattingCharsTree.redo(numToRedo);
                }
                break;
        }

        queryNum++;
        powerOf2 = (2 * powerOf2) % Mod;

    }

    return decryptionKey;
}

int64_t AVLTree::distBetweenEnclosingFormattedChars(int64_t position)
{
    const AVLTreeIterator formattingCharToRightIter = findFirstNodeToRightOf(position, root());

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

int main()
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int numQueries = read<int>();
        vector<Query> queries(numQueries);
        for (auto& query : queries)
        {
            const auto queryType = read<char>();
            query.encryptedArgument = read<int64_t>();
            switch (queryType)
            {
                case 'F':
                    query.type = Query::InsertFormatting;
                    break;
                case 'N':
                    query.type = Query::InsertNonFormatting;
                    query.encryptedArgument2 = read<int64_t>();
                    break;
                case 'Q':
                    query.type = Query::IsRangeFormatted;
                    break;
                case 'U':
                    query.type = Query::Undo;
                    break;
                case 'R':
                    query.type = Query::Redo;
                    break;
            }
        }

        cout << solveOptimised(queries) << endl;
    }

    assert(cin);
}
