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
                return m_rootForRevision[m_undoStackPointer];
        }
        void insertFormattingChar(int position)
        {
            if (!m_root)
                m_root = createNode(position);
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
                        const int currentNodePosition = numToLeftOffset + numInLeftSubTree + sumToLeftOffset + sumOfLeftSubTree;
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
                const int newFormattingCharSizeOfUnformattedToLeftRun = formattingCharToRight->value - (formattingCharToRightPos - position);
                const int adjustedFormattingCharToRightSizeOfUnformattedToLeftRun = formattingCharToRightPos - position;
                // Perform the actual insertion.
                m_root = insertFormattingChar(position, newFormattingCharSizeOfUnformattedToLeftRun, m_root, 0, 0);
                // Update the "unformatted run size" of the formattingCharToRight.
                m_root = adjustRunToLeftOfNodeToRightOf(m_root, position + 1, adjustedFormattingCharToRightSizeOfUnformattedToLeftRun - formattingCharToRight->value, 0, 0);
            }

            if (m_isPersistent)
            {
                m_rootForRevision.erase(m_rootForRevision.begin() + m_undoStackPointer + 1, m_rootForRevision.end());
                m_rootForRevision.push_back(m_root);
                m_undoStackPointer++;
                assert(m_undoStackPointer == m_rootForRevision.size() - 1);
            }
        }
        void insertNonFormattingChars(int position, int numToAdd)
        {
            assert(m_root); // The Sentinel node should have been added.
            m_root = adjustRunToLeftOfNodeToRightOf(m_root, position, numToAdd, 0, 0);

            if (m_isPersistent)
            {
                m_rootForRevision.erase(m_rootForRevision.begin() + m_undoStackPointer + 1, m_rootForRevision.end());
                m_rootForRevision.push_back(m_root);
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
                    const int currentNodePosition = numToLeftOffset + numInLeftSubTree + sumToLeftOffset + sumOfLeftSubTree;
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
        AVLNode* m_root = nullptr;

        AVLNode* insertFormattingChar(int position, int sizeOfUnformattedToLeftRun,  AVLNode* currentNode, int numToLeftOffset, int sumToLeftOffset)
        {
            if (m_isPersistent)
            {
                auto newCurrentNode = createNode(currentNode->value);
                *newCurrentNode = *currentNode;
                currentNode = newCurrentNode;
            }
            int numInLeftSubTree = (currentNode->leftChild ? currentNode->leftChild->numDescendants : 0);
            int sumOfLeftSubTree = (currentNode->leftChild ? currentNode->leftChild->sumOfDescendantValues : 0);
            const int currentNodePosition = numToLeftOffset + numInLeftSubTree + sumToLeftOffset + sumOfLeftSubTree;
            if (position < currentNodePosition)
            {
                // Values in the left subtree of node must be *strictly less* than
                // that of currentNode.
                assert(position < currentNode->value);
                if (currentNode->leftChild)
                    currentNode->leftChild = insertFormattingChar(position, sizeOfUnformattedToLeftRun, currentNode->leftChild, numToLeftOffset, sumToLeftOffset);
                else
                    currentNode->leftChild = createNode(position);
            }
            else
            {
                // Values in the right subtree of node must be *greater than or equal to* that
                // that of currentNode.
                assert(position >= currentNode->value);
                if (currentNode->rightChild)
                    currentNode->rightChild = insertFormattingChar(position, sizeOfUnformattedToLeftRun, currentNode->rightChild, numToLeftOffset + 1 + numInLeftSubTree,
                                                                                                                                  sumToLeftOffset + currentNode->value + sumOfLeftSubTree);
                else
                    currentNode->rightChild = createNode(position);
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
            int numInLeftSubTree = (subTreeRoot->leftChild ? subTreeRoot->leftChild->numDescendants : 0);
            int sumOfLeftSubTree = (subTreeRoot->leftChild ? subTreeRoot->leftChild->sumOfDescendantValues : 0);
            const int currentNodePosition = numToLeftOffset + numInLeftSubTree + sumToLeftOffset + sumOfLeftSubTree;
            // We'll be changing *some* aspect of subTreeRoot whatever happens, so do a copy-on-write.
            subTreeRoot = createNode(*subTreeRoot);
            if (currentNodePosition >= position)
            {
                if (!subTreeRoot->leftChild)
                {
                    // This is the node to adjust.  Do copy-on-write.
                    subTreeRoot->value += adjustment;
                }
                else
                {
                    subTreeRoot->leftChild = adjustRunToLeftOfNodeToRightOf(subTreeRoot->leftChild, position, adjustment, numToLeftOffset, sumToLeftOffset);
                }
            }
            else
            {
                subTreeRoot->rightChild = adjustRunToLeftOfNodeToRightOf(subTreeRoot->rightChild, position, adjustment, numToLeftOffset + numInLeftSubTree + 1 + sumOfLeftSubTree + subTreeRoot->value, sumToLeftOffset);
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

    for (const auto& query : queries)
    {
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
    }
    return 0;
}

int64_t solveOptimised(const vector<Query>& queries)
{
    AVLTree formattingCharsTree;
    // Sentinel value.
    formattingCharsTree.insertFormattingChar(0);
    formattingCharsTree.root()->isSentinelValue = true;

    for (const auto& query : queries)
    {
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
