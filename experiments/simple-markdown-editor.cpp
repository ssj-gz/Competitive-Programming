// Simon St James (ssjgz) - 2020-03-17
// 
// Editorial Solution to: TODO - problem link here!
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

struct AVLNode
{
    int value = -1;
    AVLNode *leftChild = nullptr;
    AVLNode *rightChild = nullptr;
    int balanceFactor = 0;
    int maxDescendantDepth = 0;
    int numDescendants = 1;
    int sumOfDescendantValues = 0;

    bool isSentinelValue = false;

    int id = -1;
};

void printSubTree(AVLNode* subtreeRoot);
string subtreeAsDocument(AVLNode* subtreeRoot);

class AVLTreeIterator;

class AVLTree
{
    public:
        AVLTree(bool isPersistent = true, int nodeBlockSize = 1)
            : m_isPersistent{isPersistent}, m_nodeBlockSize{nodeBlockSize}
        {
            if (m_isPersistent)
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
            assert(m_isPersistent);
            m_undoStackPointer -= numToUndo;
        }
        void redo(int numToRedo)
        {
            assert(m_isPersistent);
            m_undoStackPointer += numToRedo;
        }

    private:
        AVLNode* insertFormattingChar(int position, int sizeOfUnformattedToLeftRun, AVLTreeIterator& treeIter);

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
            nodeToUpdate->sumOfDescendantValues = nodeToUpdate->value;

            auto leftChild = nodeToUpdate->leftChild;

            if (leftChild)
            {
                nodeToUpdate->balanceFactor -= 1 + leftChild->maxDescendantDepth;
                nodeToUpdate->maxDescendantDepth = max(nodeToUpdate->maxDescendantDepth, 1 + leftChild->maxDescendantDepth);
                nodeToUpdate->numDescendants += leftChild->numDescendants;
                nodeToUpdate->sumOfDescendantValues += leftChild->sumOfDescendantValues;
            }

            auto rightChild = nodeToUpdate->rightChild;
            if (rightChild)
            {
                nodeToUpdate->balanceFactor += 1 + rightChild->maxDescendantDepth;
                nodeToUpdate->maxDescendantDepth = max(nodeToUpdate->maxDescendantDepth, 1 + rightChild->maxDescendantDepth);
                nodeToUpdate->numDescendants += rightChild->numDescendants;
                nodeToUpdate->sumOfDescendantValues += rightChild->sumOfDescendantValues;
            }
        }

        AVLNode* adjustRunToLeftOfNodeToRightOf(AVLTreeIterator& treeIter, int position, int adjustment);

        AVLNode* createNode(int value)
        {
            auto newNode = createNode();
            newNode->value = value;
            newNode->sumOfDescendantValues = value;
            return newNode;
        }

        AVLNode* createNode(const AVLNode& nodeToCopy)
        {
            auto newNode = createNode();
            const auto idBackup = newNode->id;
            *newNode = nodeToCopy;
            newNode->id = idBackup;
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
            m_sumToLeftOffset += m_sumOfLeftSubTree + m_currentNode->value;
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
            m_numInLeftSubTree = (m_currentNode->leftChild ? m_currentNode->leftChild->numDescendants : 0);
            m_sumOfLeftSubTree = (m_currentNode->leftChild ? m_currentNode->leftChild->sumOfDescendantValues : 0);
            m_currentNodePosition = m_numToLeftOffset + m_numInLeftSubTree + m_sumToLeftOffset + m_sumOfLeftSubTree + m_currentNode->value;
        }
};

struct Query
{
    enum Type { InsertFormatting, InsertNonFormatting, IsRangeFormatted, Undo, Redo };
    Type type;
    int encryptedArgument = -1;
    int encryptedArgument2 = -1;
};

vector<int> solveBruteForce(const vector<Query>& queries, vector<string>& bruteForceDocs)
{
    vector<int> queryResults;
    string document;
    int decryptionKey = 0; 

    vector<Query> undoStack;
    int undoStackPointer = -1;

    int queryNum = 1;
    for (const auto& query : queries)
    {
        //cout << "Processing query " << queryNum << endl;
        switch (query.type)
        {
            case Query::InsertFormatting:
                {
                    const int insertionPos = query.encryptedArgument - 1;
                    //cout << "InsertFormatting at " << insertionPos << endl;
                    document.insert(document.begin() + insertionPos, '*');
                    undoStackPointer++;
                    undoStack.erase(undoStack.begin() + undoStackPointer, undoStack.end());
                    Query undoQuery = query;
                    undoQuery.encryptedArgument = insertionPos; // Not strictly accurate - this is unencrypted!
                    undoStack.push_back(undoQuery);
                }
                break;
            case Query::InsertNonFormatting:
                {
                    const int insertionPos = query.encryptedArgument - 1;
                    const int numToInsert = query.encryptedArgument2;
                    //cout << "InsertNonFormatting " << numToInsert << " at " << insertionPos << endl;
                    const string charsToInsert(numToInsert, 'X');
                    document.insert(insertionPos, charsToInsert);
                    undoStackPointer++;
                    undoStack.erase(undoStack.begin() + undoStackPointer, undoStack.end());
                    Query undoQuery = query;
                    undoQuery.encryptedArgument = insertionPos; // Not strictly accurate - this is unencrypted!
                    undoQuery.encryptedArgument2 = numToInsert; // Not strictly accurate - this is unencrypted!
                    undoStack.push_back(undoQuery);
                }
                break;
            case Query::IsRangeFormatted:
                {
                    const int queryPosition = query.encryptedArgument - 1;
                    assert(document[queryPosition] == 'X');
                    //cout << "IsRangeFormatted at " << queryPosition << endl;
                    int queryAnswer = -1;
                    {
                        int openingFormatPos = -1;
                        for (int pos = 0; pos < document.size(); pos++)
                        {
                            if (document[pos] == '*')
                            {
                                if (openingFormatPos == -1)
                                {
                                    // Open formatting.
                                    openingFormatPos = pos;
                                }
                                else
                                {
                                    // Close formatting.
                                    if (openingFormatPos < queryPosition && queryPosition < pos)
                                    {
                                        queryAnswer = pos - openingFormatPos - 1;
                                    }
                                    openingFormatPos = -1;
                                }
                            }
                        }
                    }
                    //cout << "queryAnswer: " << queryAnswer << endl;
                    queryResults.push_back(queryAnswer);
                }
                break;
            case Query::Undo:
                {
                    const int numToUndo = query.encryptedArgument;
                    //cout << "Undo " << numToUndo << endl;
                    for (int i = 0; i < numToUndo; i++)
                    {
                        const auto& queryToUndo = undoStack[undoStackPointer];
                        const auto removalPosition = queryToUndo.encryptedArgument;
                        const auto numToRemove = (queryToUndo.type == Query::InsertNonFormatting ? queryToUndo.encryptedArgument2 : 1);
                        document.erase(document.begin() + removalPosition, document.begin() + removalPosition + numToRemove);
                        undoStackPointer--;
                    }
                }
                break;
            case Query::Redo:
                {
                    const int numToRedo = query.encryptedArgument;
                    //cout << "Redo " << numToRedo << endl;
                    for (int i = 0; i < numToRedo; i++)
                    {
                        undoStackPointer++;
                        const auto& queryToUndo = undoStack[undoStackPointer];
                        const auto insertPosition = queryToUndo.encryptedArgument;
                        const auto charToInsert = queryToUndo.type == Query::InsertNonFormatting ? 'X' : '*';
                        const auto numToInsert = queryToUndo.type == Query::InsertNonFormatting ? queryToUndo.encryptedArgument2 : 1;
                        document.insert(insertPosition, string(numToInsert, charToInsert));
                    }

                }
                break;
        }
        //cout << "document: " << document << endl;
        //cout << "Undo stack: " << endl;
        for (const auto x : undoStack)
        {
            //cout << (x.type == Query::InsertNonFormatting ? 'X' : '*') << " " << x.encryptedArgument << endl;
        }
        //cout << "undoStackPointer: " << undoStackPointer << endl;
        queryNum++;
        bruteForceDocs.push_back(document);
    }
    return queryResults;
}

void printSubTree(AVLNode* subtreeRoot)
{
    if (subtreeRoot == nullptr)
        return;
    cout << "Node " << subtreeRoot->id << " has value: " << subtreeRoot->value << " balanceFactor: " << subtreeRoot->balanceFactor << " maxDescendantDepth: " << subtreeRoot->maxDescendantDepth << " numDescendants: " << subtreeRoot->numDescendants << " sumOfDescendantValues: " << subtreeRoot->sumOfDescendantValues << " isSentinelValue: " << subtreeRoot->isSentinelValue;
    cout << " leftChild: " << (subtreeRoot->leftChild ? subtreeRoot->leftChild->id : -1) << " rightChild: " << (subtreeRoot->rightChild ? subtreeRoot->rightChild->id : -1) << endl;

    if (subtreeRoot->leftChild)
        printSubTree(subtreeRoot->leftChild);
    if (subtreeRoot->rightChild)
        printSubTree(subtreeRoot->rightChild);
}
string subtreeAsDocument(AVLNode* subtreeRoot)
{
    if (!subtreeRoot)
        return "";
    string asDoc = subtreeAsDocument(subtreeRoot->leftChild);
    asDoc += string(subtreeRoot->value, 'X');
    asDoc += "*";
    asDoc += subtreeAsDocument(subtreeRoot->rightChild);

    return asDoc;
}

void printTree(AVLTree& tree)
{
    printSubTree(tree.root());
}

void AVLTree::insertFormattingChar(int position)
{
    AVLNode* newRoot = nullptr;
    if (!root())
    {
        newRoot = createNode(0);
    }
    else
    {
        // Find node representing the formatting character immediately to the 
        // right of "position".
        // It's guaranteed that there will be one, due to the Sentinel node.
        AVLNode* formattingCharToRight = nullptr;
        int formattingCharToRightPos = -1;
        {
            AVLTreeIterator treeIter(root());
            while (treeIter.currentNode())
            {
                if (treeIter.currentNodePosition() >= position)
                {
                    formattingCharToRight = treeIter.currentNode();
                    formattingCharToRightPos = treeIter.currentNodePosition();
                    treeIter.followLeftChild();
                }
                else
                {
                    treeIter.followRightChild();
                }
            }
        }
        assert(formattingCharToRight);
        //cout << "formattingCharToRight: " << formattingCharToRight->id << " value: " << formattingCharToRight->value << endl;
        const int newFormattingCharSizeOfUnformattedToLeftRun = formattingCharToRight->value - (formattingCharToRightPos - position);
        const int adjustedFormattingCharToRightSizeOfUnformattedToLeftRun = formattingCharToRightPos - position;
        //cout << " newFormattingCharSizeOfUnformattedToLeftRun: " << newFormattingCharSizeOfUnformattedToLeftRun << endl; 
        //cout << " adjustedFormattingCharToRightSizeOfUnformattedToLeftRun: " << adjustedFormattingCharToRightSizeOfUnformattedToLeftRun << endl; 
        //cout << " formattingCharToRight->value: " << formattingCharToRight->value << endl;
        assert(newFormattingCharSizeOfUnformattedToLeftRun >= 0);
        assert(adjustedFormattingCharToRightSizeOfUnformattedToLeftRun >= 0);
        {
            // Perform the actual insertion.
            AVLTreeIterator treeIter(root());
            newRoot = insertFormattingChar(position, newFormattingCharSizeOfUnformattedToLeftRun, treeIter);
        }
        //cout << " Inserted " << newFormattingCharSizeOfUnformattedToLeftRun << endl;
        //cout << "Current formattingCharsTree: " << endl;
        //printSubTree(newRoot);
        // Update the "unformatted run size" of the formattingCharToRight.
        AVLTreeIterator treeIter(newRoot);
        newRoot = adjustRunToLeftOfNodeToRightOf(treeIter, position + 1, adjustedFormattingCharToRightSizeOfUnformattedToLeftRun - formattingCharToRight->value);
    }

    if (m_isPersistent)
    {
        m_rootForRevision.erase(m_rootForRevision.begin() + m_undoStackPointer + 1, m_rootForRevision.end());
        m_rootForRevision.push_back(newRoot);
        m_undoStackPointer++;
        assert(m_undoStackPointer == m_rootForRevision.size() - 1);
    }
}

AVLNode* AVLTree::insertFormattingChar(int position, int sizeOfUnformattedToLeftRun, AVLTreeIterator& treeIter)
{
    auto currentNode = treeIter.currentNode();
    auto originalNode = currentNode;
    if (m_isPersistent)
    {
        auto newCurrentNode = createNode(currentNode->value);
        *newCurrentNode = *currentNode;
        currentNode = newCurrentNode;
    }
    if (position <= treeIter.currentNodePosition())
    {
        // Positions in the left subtree of node must be *strictly less* than
        // that of currentNode.
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
        // Positions in the right subtree of node must be *greater than or equal to* that
        // that of currentNode.
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
    assert(root()); // The Sentinel node should have been added.
    auto oldRoot = root();
    AVLTreeIterator treeIter(root());
    auto newRoot = adjustRunToLeftOfNodeToRightOf(treeIter, position, numToAdd);

    //cout << "newRoot: " << newRoot->id << " oldRoot: " << oldRoot->id << endl;

    if (m_isPersistent)
    {
        m_rootForRevision.erase(m_rootForRevision.begin() + m_undoStackPointer + 1, m_rootForRevision.end());
        m_rootForRevision.push_back(newRoot);
        m_undoStackPointer++;
        assert(m_undoStackPointer == m_rootForRevision.size() - 1);
    }
}

AVLNode* AVLTree::adjustRunToLeftOfNodeToRightOf(AVLTreeIterator& treeIter, int position, int adjustment)
{
    auto subTreeRoot = treeIter.currentNode();
    if (!subTreeRoot)
        return subTreeRoot;
    subTreeRoot = createNode(*subTreeRoot);
    if (position <= treeIter.currentNodePosition())
    {
        bool adjustInLeftSubChild = true;
        if (position == treeIter.currentNodePosition())
            adjustInLeftSubChild = false;
        if (!subTreeRoot->leftChild)
            adjustInLeftSubChild = false;
        else
        {
            const auto maxPosInLeftSubchild = treeIter.currentNodePosition() - subTreeRoot->value - 1;
            if (maxPosInLeftSubchild < position)
                adjustInLeftSubChild = false;
        }
        if (!adjustInLeftSubChild)
        {
            // This is the node to adjust.
            subTreeRoot->value += adjustment;
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

vector<int> solveOptimised(const vector<Query>& queries, vector<string>& bruteForceDocs)
{
    vector<int> queryResults;
    AVLTree formattingCharsTree;
    // Sentinel value.
    formattingCharsTree.insertFormattingChar(0);
    formattingCharsTree.root()->isSentinelValue = true;
    //cout << "Initial formattingCharsTree: " << endl;
    //printTree(formattingCharsTree);

    int queryNum = 1;
    for (const auto& query : queries)
    {
        //cout << "Processing query " << queryNum << endl;
        switch (query.type)
        {
            case Query::InsertFormatting:
                {
                    const int insertionPos = query.encryptedArgument - 1;
                    //cout << "InsertFormatting at " << insertionPos << endl;
                    formattingCharsTree.insertFormattingChar(insertionPos);
                }
                break;
            case Query::InsertNonFormatting:
                {
                    const int insertionPos = query.encryptedArgument - 1;
                    const int numToInsert = query.encryptedArgument2;
                    //cout << "InsertNonFormatting " << numToInsert << " at " << insertionPos << endl;
                    formattingCharsTree.insertNonFormattingChars(insertionPos, numToInsert);
                }
                break;
            case Query::IsRangeFormatted:
                {
                    const int queryPosition = query.encryptedArgument - 1;
                    //cout << "IsRangeFormatted at " << queryPosition << endl;
                    const int queryAnswer = formattingCharsTree.distBetweenEnclosingFormattedChars(queryPosition);
                    //cout << "queryAnswer: " << queryAnswer << endl;
                    queryResults.push_back(queryAnswer);
                }
                break;
            case Query::Undo:
                {
                    const int numToUndo = query.encryptedArgument;
                    //cout << "Undo " << numToUndo << endl;
                    formattingCharsTree.undo(numToUndo);
                }
                break;
            case Query::Redo:
                {
                    const int numToRedo = query.encryptedArgument;
                    //cout << "Redo " << numToRedo << endl;
                    formattingCharsTree.redo(numToRedo);

                }
                break;
        }

        //cout << "Current formattingCharsTree: " << endl;
        //printTree(formattingCharsTree);
        //cout << "as doc:" << endl;
        //const auto actual = subtreeAsDocument(formattingCharsTree.root());
        //cout << actual << endl;
        //const auto expected = bruteForceDocs[queryNum - 1] + "*" ;
        //cout << "expected doc: " << endl << expected << endl;
        //assert(actual == expected);
        queryNum++;
    }

    return queryResults;
}

int AVLTree::distBetweenEnclosingFormattedChars(int position)
{
    // Find node representing the formatting character immediately to the 
    // right of "position".
    // It's guaranteed that there will be one, due to the Sentinel node.
    AVLNode* formattingCharToRight = nullptr;
    int formattingCharToRightPos = -1;
    int formattingCharToRightPosNumFormattingToLeft = -1;
    {
        AVLTreeIterator treeIter(root());
        while (treeIter.currentNode())
        {
            if (treeIter.currentNodePosition() >= position)
            {
                formattingCharToRight = treeIter.currentNode();
                formattingCharToRightPos = treeIter.currentNodePosition();
                formattingCharToRightPosNumFormattingToLeft = treeIter.numFormattingCharsToLeft();
                treeIter.followLeftChild();
            }
            else
            {
                treeIter.followRightChild();
            }
        }
    }
    assert(formattingCharToRight);
    //cout << " distBetweenEnclosingFormattedChars formattingCharToRight: " << formattingCharToRight->id << " isSentinelValue: " << formattingCharToRight->isSentinelValue << endl;
    if (formattingCharToRight->isSentinelValue || formattingCharToRightPosNumFormattingToLeft % 2 == 0)
        return -1;
    else
        return formattingCharToRight->value;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        // TODO - generate randomised test.
        const int T = rand() % 100 + 1;
        //const int T = 1;
        cout << T << endl;

        for (int t = 0; t < T; t++)
        {
            string document;
            int decryptionKey = 0; 

            vector<Query> undoStack;
            int undoStackPointer = -1;

            const int numQueries = 1 + rand() % 250'000;
            vector<Query> queries;
            for (int i = 0; i < numQueries; i++)
            {
                bool haveQuery = false;
                Query query;
                int numFormatting = count(document.begin(), document.end(), '*');
                int numNonFormatting = count(document.begin(), document.end(), 'X');
                while (!haveQuery)
                {
                    const int queryType = rand() % 5;
                    query.type = static_cast<Query::Type>(queryType);
                    if (queryType == Query::Undo)
                    {
                        if (rand() % 4 >= 1)
                            continue; // Undos should be fairly rare.
                        if (undoStackPointer == -1)
                            continue;
                        else
                        {
                            const int numToUndo = 1 + rand() % (undoStackPointer + 1);
                            query.encryptedArgument = numToUndo;
                            haveQuery = true;
                        }
                    }
                    if (queryType == Query::Redo)
                    {
                        if (rand() % 4 >= 1)
                            continue; // Redos should be fairly rare.
                        if (undoStackPointer + 1 == undoStack.size())
                            continue;
                        else
                        {
                            const int numToRedo = 1 + rand() % (undoStack.size() - 1 - undoStackPointer);
                            query.encryptedArgument = numToRedo;
                            haveQuery = true;
                        }
                    }
                    if (queryType == Query::InsertFormatting)
                    {
                        const int pos = rand() % (document.size() + 1);
                        query.encryptedArgument = pos + 1;
                        haveQuery = true;
                    }
                    if (queryType == Query::InsertNonFormatting)
                    {
                        const int pos = rand() % (document.size() + 1);
                        query.encryptedArgument = pos + 1;
                        const int num = 1 + rand() % 10;
                        query.encryptedArgument2 = num;
                        haveQuery = true;
                    }
                    if (queryType == Query::IsRangeFormatted)
                    {
                        if (numNonFormatting == 0)
                            continue;
                        else
                        {
                            int nonFormattedToPick = rand() % numNonFormatting;
                            int numNonFormattedSoFar = 0;
                            int position = -1;
                            for (int i = 0; i < document.size(); i++)
                            {
                                if (document[i] == 'X')
                                {
                                    if (numNonFormattedSoFar == nonFormattedToPick)
                                    {
                                        position = i;
                                        break;
                                    }
                                    numNonFormattedSoFar++;
                                }
                            }
                            assert(position != -1);
                            query.encryptedArgument = position + 1;
                            haveQuery = true;
                        }

                    }

                }
                queries.push_back(query);
                switch (query.type)
                {
                    case Query::InsertFormatting:
                        {
                            const int insertionPos = query.encryptedArgument - 1;
                            //cerr << "InsertFormatting at " << insertionPos << endl;
                            document.insert(document.begin() + insertionPos, '*');
                            undoStackPointer++;
                            undoStack.erase(undoStack.begin() + undoStackPointer, undoStack.end());
                            Query undoQuery = query;
                            undoQuery.encryptedArgument = insertionPos; // Not strictly accurate - this is unencrypted!
                            undoStack.push_back(undoQuery);
                        }
                        break;
                    case Query::InsertNonFormatting:
                        {
                            const int insertionPos = query.encryptedArgument - 1;
                            const int numToInsert = query.encryptedArgument2;
                            //cerr << "InsertNonFormatting " << numToInsert << " at " << insertionPos << endl;
                            const string charsToInsert(numToInsert, 'X');
                            document.insert(insertionPos, charsToInsert);
                            undoStackPointer++;
                            undoStack.erase(undoStack.begin() + undoStackPointer, undoStack.end());
                            Query undoQuery = query;
                            undoQuery.encryptedArgument = insertionPos; // Not strictly accurate - this is unencrypted!
                            undoQuery.encryptedArgument2 = numToInsert; // Not strictly accurate - this is unencrypted!
                            undoStack.push_back(undoQuery);
                        }
                        break;
                    case Query::IsRangeFormatted:
                        {
                            const int queryPosition = query.encryptedArgument - 1;
                            assert(document[queryPosition] == 'X');
                            //cerr << "IsRangeFormatted at " << queryPosition << endl;
                            int queryAnswer = -1;
                            {
                                int openingFormatPos = -1;
                                for (int pos = 0; pos < document.size(); pos++)
                                {
                                    if (document[pos] == '*')
                                    {
                                        if (openingFormatPos == -1)
                                        {
                                            // Open formatting.
                                            openingFormatPos = pos;
                                        }
                                        else
                                        {
                                            // Close formatting.
                                            if (openingFormatPos < queryPosition && queryPosition < pos)
                                            {
                                                queryAnswer = pos - openingFormatPos - 1;
                                            }
                                            openingFormatPos = -1;
                                        }
                                    }
                                }
                            }
                            //cerr << "queryAnswer: " << queryAnswer << endl;
                        }
                        break;
                    case Query::Undo:
                        {
                            const int numToUndo = query.encryptedArgument;
                            //cerr << "Undo " << numToUndo << endl;
                            for (int i = 0; i < numToUndo; i++)
                            {
                                const auto& queryToUndo = undoStack[undoStackPointer];
                                const auto removalPosition = queryToUndo.encryptedArgument;
                                const auto numToRemove = (queryToUndo.type == Query::InsertNonFormatting ? queryToUndo.encryptedArgument2 : 1);
                                document.erase(document.begin() + removalPosition, document.begin() + removalPosition + numToRemove);
                                undoStackPointer--;
                            }
                        }
                        break;
                    case Query::Redo:
                        {
                            const int numToRedo = query.encryptedArgument;
                            //cerr << "Redo " << numToRedo << endl;
                            for (int i = 0; i < numToRedo; i++)
                            {
                                undoStackPointer++;
                                const auto& queryToUndo = undoStack[undoStackPointer];
                                const auto insertPosition = queryToUndo.encryptedArgument;
                                const auto charToInsert = queryToUndo.type == Query::InsertNonFormatting ? 'X' : '*';
                                const auto numToInsert = queryToUndo.type == Query::InsertNonFormatting ? queryToUndo.encryptedArgument2 : 1;
                                document.insert(insertPosition, string(numToInsert, charToInsert));
                            }

                        }
                        break;
                }
                //cerr << "document: " << document << endl;
                //cerr << "Undo stack: " << endl;
                for (const auto x : undoStack)
                {
                    //cerr << (x.type == Query::InsertNonFormatting ? 'X' : '*') << " " << x.encryptedArgument << endl;
                }
                //cerr << "undoStackPointer: " << undoStackPointer << endl;
            }
            cout << queries.size() << endl;
            for (const auto& query : queries)
            {
                switch (query.type)
                {
                    case Query::InsertFormatting:
                        cout << 'F' << " " << query.encryptedArgument << endl;
                        break;
                    case Query::InsertNonFormatting:
                        cout << 'N' << " " << query.encryptedArgument << " " << query.encryptedArgument2 << endl;
                        break;
                    case Query::IsRangeFormatted:
                        cout << 'Q' << " " << query.encryptedArgument << endl;
                        break;
                    case Query::Undo:
                        cout << 'U' << " " << query.encryptedArgument << endl;
                        break;
                    case Query::Redo:
                        cout << 'R' << " " << query.encryptedArgument << endl;
                        break;
                }
            }
        }

        return 0;
    }
    
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

#ifdef BRUTE_FORCE
        vector<string> bruteForceDocs;
        const auto solutionBruteForce = solveBruteForce(queries, bruteForceDocs);
        const auto solutionOptimised = solveOptimised(queries, bruteForceDocs);
        cout << "solutionBruteForce:";
        for (const auto x : solutionBruteForce)
            cout << " " << x;
        cout << endl;
        cout << "solutionOptimised: ";
        for (const auto x : solutionOptimised)
            cout << " " << x;
        cout << endl;
        assert(solutionOptimised == solutionBruteForce);
#endif
    }

    assert(cin);
}
