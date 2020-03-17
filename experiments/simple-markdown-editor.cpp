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
        void insertFormattingChar(int position)
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
                    auto currentNode = root();
                    int numToLeftOffset = 0;
                    int sumToLeftOffset = 0;
                    while (currentNode)
                    {
                        int numInLeftSubTree = (currentNode->leftChild ? currentNode->leftChild->numDescendants : 0);
                        int sumOfLeftSubTree = (currentNode->leftChild ? currentNode->leftChild->sumOfDescendantValues : 0);
                        const int currentNodePosition = numToLeftOffset + numInLeftSubTree + sumToLeftOffset + sumOfLeftSubTree + currentNode->value;
                        if (currentNodePosition >= position)
                        {
                            formattingCharToRight = currentNode;
                            formattingCharToRightPos = currentNodePosition;
                            currentNode = currentNode->leftChild;
                        }
                        else
                        {
                            numToLeftOffset += 1 + numInLeftSubTree;
                            sumToLeftOffset += currentNode->value + sumOfLeftSubTree;
                            currentNode = currentNode->rightChild;
                        }
                    }
                }
                assert(formattingCharToRight);
                cout << "formattingCharToRight: " << formattingCharToRight->id << " value: " << formattingCharToRight->value << endl;
                const int newFormattingCharSizeOfUnformattedToLeftRun = formattingCharToRight->value - (formattingCharToRightPos - position);
                const int adjustedFormattingCharToRightSizeOfUnformattedToLeftRun = formattingCharToRightPos - position;
                cout << " newFormattingCharSizeOfUnformattedToLeftRun: " << newFormattingCharSizeOfUnformattedToLeftRun << endl; 
                cout << " adjustedFormattingCharToRightSizeOfUnformattedToLeftRun: " << adjustedFormattingCharToRightSizeOfUnformattedToLeftRun << endl; 
                cout << " formattingCharToRight->value: " << formattingCharToRight->value << endl;
                assert(newFormattingCharSizeOfUnformattedToLeftRun >= 0);
                assert(adjustedFormattingCharToRightSizeOfUnformattedToLeftRun >= 0);
                // Perform the actual insertion.
                newRoot = insertFormattingChar(position, newFormattingCharSizeOfUnformattedToLeftRun, root(), 0, 0);
                cout << " Inserted " << newFormattingCharSizeOfUnformattedToLeftRun << endl;
                cout << "Current formattingCharsTree: " << endl;
                printSubTree(newRoot);
                // Update the "unformatted run size" of the formattingCharToRight.
                newRoot = adjustRunToLeftOfNodeToRightOf(newRoot, position + 1, adjustedFormattingCharToRightSizeOfUnformattedToLeftRun - formattingCharToRight->value, 0, 0);
            }

            if (m_isPersistent)
            {
                m_rootForRevision.erase(m_rootForRevision.begin() + m_undoStackPointer + 1, m_rootForRevision.end());
                m_rootForRevision.push_back(newRoot);
                m_undoStackPointer++;
                assert(m_undoStackPointer == m_rootForRevision.size() - 1);
            }
        }
        void insertNonFormattingChars(int position, int numToAdd)
        {
            assert(root()); // The Sentinel node should have been added.
            auto oldRoot = root();
            auto newRoot = adjustRunToLeftOfNodeToRightOf(root(), position, numToAdd, 0, 0);

            cout << "newRoot: " << newRoot->id << " oldRoot: " << oldRoot->id << endl;

            if (m_isPersistent)
            {
                m_rootForRevision.erase(m_rootForRevision.begin() + m_undoStackPointer + 1, m_rootForRevision.end());
                m_rootForRevision.push_back(newRoot);
                m_undoStackPointer++;
                assert(m_undoStackPointer == m_rootForRevision.size() - 1);
            }
        }
        int distBetweenEnclosingFormattedChars(int position)
        {
            // Find node representing the formatting character immediately to the 
            // right of "position".
            // It's guaranteed that there will be one, due to the Sentinel node.
            AVLNode* formattingCharToRight = nullptr;
            int formattingCharToRightPos = -1;
            int formattingCharToRightPosNumFormattingToLeft = -1;
            {
                auto currentNode = root();
                int numToLeftOffset = 0;
                int sumToLeftOffset = 0;
                while (currentNode)
                {
                    int numInLeftSubTree = (currentNode->leftChild ? currentNode->leftChild->numDescendants : 0);
                    int sumOfLeftSubTree = (currentNode->leftChild ? currentNode->leftChild->sumOfDescendantValues : 0);
                    const int currentNodePosition = numToLeftOffset + numInLeftSubTree + sumToLeftOffset + sumOfLeftSubTree + currentNode->value;
                    if (currentNodePosition >= position)
                    {
                        formattingCharToRight = currentNode;
                        formattingCharToRightPos = currentNodePosition;
                        formattingCharToRightPosNumFormattingToLeft = numToLeftOffset + numInLeftSubTree;
                        currentNode = currentNode->leftChild;
                    }
                    else
                    {
                        numToLeftOffset += 1 + numInLeftSubTree;
                        sumToLeftOffset += currentNode->value + sumOfLeftSubTree;
                        currentNode = currentNode->rightChild;
                    }
                }
            }
            assert(formattingCharToRight);
            cout << " distBetweenEnclosingFormattedChars formattingCharToRight: " << formattingCharToRight->id << " isSentinelValue: " << formattingCharToRight->isSentinelValue << endl;
            if (formattingCharToRight->isSentinelValue || formattingCharToRightPosNumFormattingToLeft % 2 == 0)
                return -1;
            else
                return formattingCharToRight->value + 1;
        }
        int undo(int numToUndo)
        {
            assert(m_isPersistent);
            m_undoStackPointer -= numToUndo;
        }
        int redo(int numToRedo)
        {
            assert(m_isPersistent);
            m_undoStackPointer += numToRedo;
        }

    private:
        AVLNode* insertFormattingChar(int position, int sizeOfUnformattedToLeftRun,  AVLNode* currentNode, int numToLeftOffset, int sumToLeftOffset)
        {
            auto originalNode = currentNode;
            if (m_isPersistent)
            {
                auto newCurrentNode = createNode(currentNode->value);
                *newCurrentNode = *currentNode;
                currentNode = newCurrentNode;
            }
            int numInLeftSubTree = (currentNode->leftChild ? currentNode->leftChild->numDescendants : 0);
            int sumOfLeftSubTree = (currentNode->leftChild ? currentNode->leftChild->sumOfDescendantValues : 0);
            const int currentNodePosition = numToLeftOffset + numInLeftSubTree + sumToLeftOffset + sumOfLeftSubTree + currentNode->value;
            cout << " About to do actual insert; currentNode: " << currentNode->id << " currentNodePosition: " << currentNodePosition << endl;
            if (position <= currentNodePosition)
            {
                // Positions in the left subtree of node must be *strictly less* than
                // that of currentNode.
                if (currentNode->leftChild)
                    currentNode->leftChild = insertFormattingChar(position, sizeOfUnformattedToLeftRun, currentNode->leftChild, numToLeftOffset, sumToLeftOffset);
                else
                {
                    cout << "Inserted at leftChild" << endl;
                    currentNode->leftChild = createNode(sizeOfUnformattedToLeftRun);
                }
            }
            else
            {
                // Positions in the right subtree of node must be *greater than or equal to* that
                // that of currentNode.
                if (currentNode->rightChild)
                    currentNode->rightChild = insertFormattingChar(position, sizeOfUnformattedToLeftRun, currentNode->rightChild, numToLeftOffset + 1 + numInLeftSubTree,
                                                                                                                                  sumToLeftOffset + currentNode->value + sumOfLeftSubTree);
                else
                {
                    cout << "Inserted at rightChild" << endl;
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

        AVLNode* adjustRunToLeftOfNodeToRightOf(AVLNode* subTreeRoot, int position, int adjustment, int numToLeftOffset, int sumToLeftOffset)
        {
            if (!subTreeRoot)
                return subTreeRoot;
            int numInLeftSubTree = (subTreeRoot->leftChild ? subTreeRoot->leftChild->numDescendants : 0);
            int sumOfLeftSubTree = (subTreeRoot->leftChild ? subTreeRoot->leftChild->sumOfDescendantValues : 0);
            auto originalSubTreeRoot = subTreeRoot;
            const int currentNodePosition = numToLeftOffset + numInLeftSubTree + sumToLeftOffset + sumOfLeftSubTree + subTreeRoot->value;
            cout << " adjustRunToLeftOfNodeToRightOf originalSubTreeRoot: " << originalSubTreeRoot->id << " value: " << originalSubTreeRoot->value << " currentNodePosition: " << currentNodePosition << endl;
            if (position <= currentNodePosition)
            {
                // TODO - work out the rightmost formatted char position in the leftChild: this should be O(1), I think.
                // Then we can go back to *always* COWing at the top of the function, rather than scattering COWS all over the place!
                if (!subTreeRoot->leftChild)
                {
                    // This is the node to adjust.  Do copy-on-write.
                    cout << "Adjusted old value of node " << originalSubTreeRoot->id  << " from " << originalSubTreeRoot->value << " to " << originalSubTreeRoot->value + adjustment << endl;
                    subTreeRoot = createNode(*subTreeRoot);
                    subTreeRoot->value += adjustment;
                }
                else
                {
                    auto oldLeftChild = subTreeRoot->leftChild;
                    adjustRunToLeftOfNodeToRightOf(subTreeRoot->leftChild, position, adjustment, numToLeftOffset, sumToLeftOffset);
                    subTreeRoot = createNode(*subTreeRoot);
                    subTreeRoot->leftChild = adjustRunToLeftOfNodeToRightOf(subTreeRoot->leftChild, position, adjustment, numToLeftOffset, sumToLeftOffset);
                    const bool nodeToChangeWasInLeftChild = (subTreeRoot->leftChild != oldLeftChild);
                    if (!nodeToChangeWasInLeftChild)
                    {
                        subTreeRoot->value += adjustment;
                    }
                }
            }
            else
            {
                auto oldRightChild = subTreeRoot->rightChild;
                auto newRightChild = adjustRunToLeftOfNodeToRightOf(subTreeRoot->rightChild, position, adjustment, numToLeftOffset + numInLeftSubTree + 1 + sumOfLeftSubTree + subTreeRoot->value, sumToLeftOffset);
                if (newRightChild != oldRightChild)
                {
                    subTreeRoot = createNode(*subTreeRoot);
                    subTreeRoot->rightChild = newRightChild;
                }
            }
            return subTreeRoot;
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

struct Query
{
    enum Type { InsertFormatting, InsertNonFormatting, IsRangeFormatted, Undo, Redo };
    Type type;
    int encryptedArgument = -1;
    int encryptedArgument2 = -1;
};

int64_t solveBruteForce(const vector<Query>& queries)
{
    string document;
    int decryptionKey = 0; 

    vector<std::pair<Query::Type, int>> undoStack;
    int undoStackPointer = -1;

    int queryNum = 1;
    for (const auto& query : queries)
    {
        cout << "Processing query " << queryNum << endl;
        switch (query.type)
        {
            case Query::InsertFormatting:
                {
                    const int insertionPos = query.encryptedArgument - 1;
                    cout << "InsertFormatting at " << insertionPos << endl;
                    document.insert(document.begin() + insertionPos, '*');
                    undoStackPointer++;
                    undoStack.erase(undoStack.begin() + undoStackPointer, undoStack.end());
                    undoStack.push_back({query.type, insertionPos});
                }
                break;
            case Query::InsertNonFormatting:
                {
                    const int insertionPos = query.encryptedArgument - 1;
                    const int numToInsert = query.encryptedArgument2;
                    cout << "InsertNonFormatting " << numToInsert << " at " << insertionPos << endl;
                    const string charsToInsert(numToInsert, 'X');
                    document.insert(insertionPos, charsToInsert);
                    undoStackPointer++;
                    undoStack.erase(undoStack.begin() + undoStackPointer, undoStack.end());
                    undoStack.push_back({query.type, insertionPos});
                }
                break;
            case Query::IsRangeFormatted:
                {
                    const int queryPosition = query.encryptedArgument - 1;
                    cout << "IsRangeFormatted at " << queryPosition << endl;
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
                    cout << "queryAnswer: " << queryAnswer << endl;
                }
                break;
            case Query::Undo:
                {
                    const int numToUndo = query.encryptedArgument;
                    cout << "Undo " << numToUndo << endl;
                    for (int i = 0; i < numToUndo; i++)
                    {
                        const auto removalPosition = undoStack[undoStackPointer].second;
                        document.erase(document.begin() + removalPosition);
                        undoStackPointer--;
                    }
                }
                break;
            case Query::Redo:
                {
                    const int numToRedo = query.encryptedArgument;
                    cout << "Redo " << numToRedo << endl;
                    for (int i = 0; i < numToRedo; i++)
                    {
                        undoStackPointer++;
                        const auto insertPosition = undoStack[undoStackPointer].second;
                        const auto charToInsert = undoStack[undoStackPointer].first == Query::InsertNonFormatting ? 'X' : '*';
                        document.insert(document.begin() + insertPosition, charToInsert);
                    }

                }
                break;
        }
        cout << "document: " << document << endl;
        cout << "Undo stack: " << endl;
        for (const auto x : undoStack)
        {
            cout << (undoStack[undoStackPointer].first == Query::InsertNonFormatting ? 'X' : '*') << " " << x.second << endl;
        }
        cout << "undoStackPointer: " << undoStackPointer << endl;
        queryNum++;
    }
    return 0;
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

void printTree(AVLTree& tree)
{
    printSubTree(tree.root());
}


int64_t solveOptimised(const vector<Query>& queries)
{
    AVLTree formattingCharsTree;
    // Sentinel value.
    formattingCharsTree.insertFormattingChar(0);
    formattingCharsTree.root()->isSentinelValue = true;
    cout << "Initial formattingCharsTree: " << endl;
    printTree(formattingCharsTree);

    int queryNum = 1;
    for (const auto& query : queries)
    {
        cout << "Processing query " << queryNum << endl;
        switch (query.type)
        {
            case Query::InsertFormatting:
                {
                    const int insertionPos = query.encryptedArgument - 1;
                    cout << "InsertFormatting at " << insertionPos << endl;
                    formattingCharsTree.insertFormattingChar(insertionPos);
                }
                break;
            case Query::InsertNonFormatting:
                {
                    const int insertionPos = query.encryptedArgument - 1;
                    const int numToInsert = query.encryptedArgument2;
                    cout << "InsertNonFormatting " << numToInsert << " at " << insertionPos << endl;
                    formattingCharsTree.insertNonFormattingChars(insertionPos, numToInsert);
                }
                break;
            case Query::IsRangeFormatted:
                {
                    const int queryPosition = query.encryptedArgument - 1;
                    cout << "IsRangeFormatted at " << queryPosition << endl;
                    const int queryAnswer = formattingCharsTree.distBetweenEnclosingFormattedChars(queryPosition);
                    cout << "queryAnswer: " << queryAnswer << endl;
                }
                break;
            case Query::Undo:
                {
                    const int numToUndo = query.encryptedArgument;
                    cout << "Undo " << numToUndo << endl;
                    assert(false && "Not yet implemented");
                }
                break;
            case Query::Redo:
                {
                    const int numToRedo = query.encryptedArgument;
                    cout << "Redo " << numToRedo << endl;
                    assert(false && "Not yet implemented");

                }
                break;
        }

        cout << "Current formattingCharsTree: " << endl;
        printTree(formattingCharsTree);
        queryNum++;
    }

    return 0;
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
        //const int T = rand() % 100 + 1;
        const int T = 1;
        cout << T << endl;

        for (int t = 0; t < T; t++)
        {
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
        const auto solutionBruteForce = solveBruteForce(queries);
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
        const auto solutionOptimised = solveOptimised(queries);
        cout << "solutionOptimised: " << solutionOptimised << endl;
        assert(solutionOptimised == solutionBruteForce);
#endif
    }

    assert(cin);
}
