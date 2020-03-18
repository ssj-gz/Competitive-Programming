// Simon St James (ssjgz) - 2020-03-17
// 
// Editorial Solution to: TODO - problem link here!
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
    assert(cin);
    return toRead;
}

struct Query
{
    enum Type { InsertFormatting, InsertNonFormatting, IsRangeFormatted, Undo, Redo };
    Type type;
    int encryptedArgument = -1;
    int encryptedArgument2 = -1;
};

struct AVLNode
{
    int leftNonFormattedRunSize = -1;
    AVLNode *leftChild = nullptr;
    AVLNode *rightChild = nullptr;
    int balanceFactor = 0;
    int maxDescendantDepth = 0;
    int totalFormattedDescendants = 1;
    int totalNonFormattedDescendants = 0;

    bool isSentinelValue = false;
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
        AVLNode* root()
        {
            return m_rootForRevision[m_undoStackPointer];
        }
        void insertFormattingChar(int position);
        void insertNonFormattingChars(int position, int numToAdd);
        int distBetweenEnclosingFormattedChars(int position);
        void undo(int numToUndo)
        {
            m_undoStackPointer -= numToUndo;
        }
        void redo(int numToRedo)
        {
            m_undoStackPointer += numToRedo;
        }

    private:
        AVLNode* insertFormattingChar(int position, int sizeOfUnformattedToLeftRun, AVLTreeIterator& treeIter);

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

        AVLNode* adjustRunToLeftOfNodeToRightOf(AVLTreeIterator& treeIter, int position, int adjustment);

        AVLNode* createNode(int leftNonFormattedRunSize)
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
            if (m_nodes.empty() || static_cast<int>(m_nodes.back().size()) == m_nodeBlockSize)
            {
                m_nodes.push_back(vector<AVLNode>());
                m_nodes.back().reserve(m_nodeBlockSize);
            }
            m_nodes.back().push_back(AVLNode());
            auto newNode = &(m_nodes.back().back());
            return newNode;
        }

        AVLTreeIterator findFirstNodeToRightOf(int position, AVLNode* root);

        void updateUndoStackWithNewRoot(AVLNode* newRoot)
        {
            m_rootForRevision.erase(m_rootForRevision.begin() + m_undoStackPointer + 1, m_rootForRevision.end());
            m_rootForRevision.push_back(newRoot);
            m_undoStackPointer++;
            assert(m_undoStackPointer == static_cast<int>(m_rootForRevision.size()) - 1);
        }

        int m_nodeBlockSize = 1;
        deque<vector<AVLNode>> m_nodes;

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
        int currentNodePosition() const
        {
            return m_currentNodePosition;
        }
        int numFormattingCharsToLeft() const
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
        int m_currentNodePosition = -1;
        int m_numToLeftOffset = 0;
        int m_sumToLeftOffset = 0;

        int m_numInLeftSubTree = 0;
        int m_sumOfLeftSubTree = 0;

        void updateCurrentNodePosition()
        {
            if (!m_currentNode)
                return;
            m_numInLeftSubTree = (m_currentNode->leftChild ? m_currentNode->leftChild->totalFormattedDescendants : 0);
            m_sumOfLeftSubTree = (m_currentNode->leftChild ? m_currentNode->leftChild->totalNonFormattedDescendants : 0);
            m_currentNodePosition = m_numToLeftOffset + m_numInLeftSubTree + m_sumToLeftOffset + m_sumOfLeftSubTree + m_currentNode->leftNonFormattedRunSize;
        }
};

void AVLTree::insertFormattingChar(int position)
{
    AVLNode* newRoot = nullptr;
    if (!root())
    {
        newRoot = createNode(0); // Sentinel node.
    }
    else
    {
        const AVLTreeIterator formattingCharToRightIter = findFirstNodeToRightOf(position, root());
        const int newFormattingCharSizeOfUnformattedToLeftRun = formattingCharToRightIter.currentNode()->leftNonFormattedRunSize - (formattingCharToRightIter.currentNodePosition() - position);
        const int adjustedFormattingCharToRightSizeOfUnformattedToLeftRun = formattingCharToRightIter.currentNodePosition() - position;
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

AVLNode* AVLTree::insertFormattingChar(int position, int sizeOfUnformattedToLeftRun, AVLTreeIterator& treeIter)
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

void AVLTree::insertNonFormattingChars(int position, int numToAdd)
{
    AVLTreeIterator treeIter(root());
    auto newRoot = adjustRunToLeftOfNodeToRightOf(treeIter, position, numToAdd);
    updateUndoStackWithNewRoot(newRoot);
}

AVLNode* AVLTree::adjustRunToLeftOfNodeToRightOf(AVLTreeIterator& treeIter, int position, int adjustment)
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

AVLTreeIterator AVLTree::findFirstNodeToRightOf(int position, AVLNode* root)
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

    vector<int> queryResults;
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
                    const int insertionPos = (query.encryptedArgument ^ decryptionKey) - 1;
                    formattingCharsTree.insertFormattingChar(insertionPos);
                }
                break;
            case Query::InsertNonFormatting:
                {
                    const int insertionPos = (query.encryptedArgument ^ decryptionKey) - 1;
                    const int numToInsert = (query.encryptedArgument2 ^ decryptionKey);
                    formattingCharsTree.insertNonFormattingChars(insertionPos, numToInsert);
                }
                break;
            case Query::IsRangeFormatted:
                {
                    const int queryPosition = (query.encryptedArgument ^ decryptionKey) - 1;
                    int queryAnswer = formattingCharsTree.distBetweenEnclosingFormattedChars(queryPosition);
                    if (queryAnswer == -1)
                        queryAnswer = 3'141'592;
                    decryptionKey = (decryptionKey + (queryAnswer * powerOf2) % Mod) % Mod;

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

int AVLTree::distBetweenEnclosingFormattedChars(int position)
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
            query.encryptedArgument = read<int>();
            switch (queryType)
            {
                case 'F':
                    query.type = Query::InsertFormatting;
                    break;
                case 'N':
                    query.type = Query::InsertNonFormatting;
                    query.encryptedArgument2 = read<int>();
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
