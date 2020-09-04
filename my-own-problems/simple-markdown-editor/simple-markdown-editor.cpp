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

const int64_t Mod = 1'000'000'007;

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

        AVLTreeIterator findFirstNodeToRightOf(int position, AVLNode* root);

        void updateUndoStackWithNewRoot(AVLNode* newRoot)
        {
            m_rootForRevision.erase(m_rootForRevision.begin() + m_undoStackPointer + 1, m_rootForRevision.end());
            m_rootForRevision.push_back(newRoot);
            m_undoStackPointer++;
            assert(m_undoStackPointer == m_rootForRevision.size() - 1);
        }

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

int64_t solveBruteForce(const vector<Query>& queries, vector<string>& bruteForceDocs)
{
    vector<int> queryResults;
    string document;

    int64_t decryptionKey = 0; 
    int64_t powerOf2 = 2;

    vector<Query> undoStack;
    vector<string> undoStackDocuments = { "" };
    int undoStackPointer = -1;

    int queryNum = 1;
#ifdef DIAGNOSTICS
    auto repeatedString = [](const string& stringToRepeat, int numRepetitions)
    {
        string repeatedString;
        for (int i = 0; i < numRepetitions; i++)
            repeatedString += stringToRepeat;
        return repeatedString;
    };
    const string headerString = "document: ";
    const auto indentationLen = string(headerString).length();
    const auto indentationSpaces = repeatedString(" ", indentationLen);
    auto showStatus = [&](bool printDocument = true, bool printFormattedRangeDisplay = true, bool printUndoStack = true)
    {
        if (printDocument)
        {
            cout << "document: " << document << endl;
        }
        if (printUndoStack)
        {
            const int columnWidth = 72;
            const string undoStackPrefix = "undo stack:  [ ";
            const auto undoStackIndent = undoStackPrefix.size();
            int undoStackStatusPointerCol = -1;
            int undoStackLineNum = 0;
            string undoStackLine;

            undoStackLine += undoStackPrefix;

            for (int undoStackIndex = 0; undoStackIndex < undoStackDocuments.size(); undoStackIndex++)
            {
                //cout << "undoStackIndex: " << undoStackIndex << " undoStackPointer: " << undoStackPointer << endl;
                string undoStackElement = "\"" + undoStackDocuments[undoStackIndex] + "\"";
                const bool isFinalElement = (undoStackIndex == undoStackDocuments.size() - 1);
                if (!isFinalElement)
                    undoStackElement += ", ";
                else
                    undoStackElement += " ]";
                const int lineLengthAfterAddingElement = (undoStackLine + undoStackElement).size();
                const bool needToWrapBeforeThisElement = (lineLengthAfterAddingElement > columnWidth);
                const bool undoStackPointerPointsToThisElement = (undoStackIndex  == undoStackPointer + 1);

                if (!needToWrapBeforeThisElement && undoStackPointerPointsToThisElement)
                {
                    undoStackStatusPointerCol = undoStackLine.size();
                }

                auto flushLine = [&]()
                {
                    cout << undoStackLine << endl;
                    if (undoStackStatusPointerCol != -1)
                    {
                        const auto undoStackPointerText = "undo stack pointer = " + to_string(undoStackPointer + 2);
                        if (undoStackStatusPointerCol + undoStackPointerText.size() > columnWidth)
                        {
                            cout << string(undoStackStatusPointerCol - undoStackPointerText.size() - 1, ' ') + undoStackPointerText << " ↑" << endl;
                        }

                        else
                        {
                            cout << string(undoStackStatusPointerCol, ' ') << "↑ " + undoStackPointerText << endl;
                        }
                        undoStackStatusPointerCol = -1;
                    }
                };
                if (needToWrapBeforeThisElement)
                {
                    flushLine();
                    undoStackLine = string(undoStackIndent, ' ') + undoStackElement;
                    if (undoStackPointerPointsToThisElement)
                    {
                        undoStackStatusPointerCol = undoStackIndent;
                    }
                }
                else
                {
                    undoStackLine += undoStackElement;
                }
                if (isFinalElement)
                    flushLine();
            }
            //undoStackStatusString += "] ";
            //cout << undoStackStatusString << endl;
            //cout << string(undoStackStatusPointer, ' ') << "↑ undo stack pointer = " << (undoStackPointer + 2) << endl;
        }
        if (printFormattedRangeDisplay)
        {
            int formatRangeBegin = -1;

            string formattedRangeDisplayString;
            int formattedRangeDisplayStringLength = 0; // The formattedRangeDisplayStringLength string is UTF-8, so formattedRangeDisplayString.length() will give the incorrect answer.
            int numFormattedRanges = 0;
            for (int pos = 0; pos < document.size(); pos++)
            {
                if (document[pos] == '*')
                {
                    //cout << "found * at pos: " << pos << " formatRangeBegin: " << formatRangeBegin << endl;
                    if (formatRangeBegin == -1)
                    {
                        const int numSpacesToAddToDisplayString = pos - formattedRangeDisplayStringLength;
                        formattedRangeDisplayString += repeatedString(" ", numSpacesToAddToDisplayString);
                        formattedRangeDisplayStringLength += numSpacesToAddToDisplayString;
                        formatRangeBegin = pos;
                    }
                    else
                    {
                        const int numCharsToAddToDisplayString = pos + 1 - formattedRangeDisplayStringLength;
                        //cout << "adding " << (pos + 1 - static_cast<int>(formattedRangeDisplayString.size()) - 2) << " things " << " formattedRangeDisplayString size: " << formattedRangeDisplayString.size() << endl;
                        //cout << "formattedRangeDisplayString: >" << formattedRangeDisplayString << "<" << endl;
                        formattedRangeDisplayString += u8"┌" + repeatedString(u8"─", numCharsToAddToDisplayString - 2)  + u8"┐";
                        formattedRangeDisplayStringLength += numCharsToAddToDisplayString;
                        formatRangeBegin = -1;
                        numFormattedRanges++;
                    }
                    //cout << document << "<" << endl;
                    //cout << formattedRangeDisplayString << "<" << endl;
                }
            }
            formattedRangeDisplayString += repeatedString(" ", document.length() - formattedRangeDisplayStringLength);
            cout << repeatedString(" ", indentationLen) << formattedRangeDisplayString << " ← Formatted ranges" << (numFormattedRanges > 0 ? "" : " (none)") << endl;
            cout << "document: " << document << endl;
        }
    };

    cout << "**Initial status**: " << endl;
    cout << "```" << endl;
    showStatus(true, false, true);
    cout << "```" << endl;
#endif
    for (const auto& query : queries)
    {
#ifdef DIAGNOSTICS
        cout << "**Processing query " << queryNum << "**." << endl;
#endif
        switch (query.type)
        {
            case Query::InsertFormatting:
                {
                    const int insertionPos = (query.encryptedArgument ^ decryptionKey) - 1;
#ifdef DIAGNOSTICS
                    cout << "Need to insert formatting character at position $p = d \\oplus " << query.encryptedArgument << " = " << decryptionKey << "\\oplus " << query.encryptedArgument << " = " << (insertionPos + 1) << "$." << endl;
                    cout << "```" << endl;
                    showStatus(true, false, false);
                    cout << indentationSpaces << repeatedString(" ", insertionPos) << "↑" << " insert formatting character here" << endl;
                    cout << "```" << endl;
                    const bool neededToEraseUndosToRight = undoStackPointer + 1 != undoStack.size();
#endif
                    //cout << "InsertFormatting at " << insertionPos << endl;
                    document.insert(document.begin() + insertionPos, '*');
                    undoStack.erase(undoStack.begin() + (undoStackPointer + 1), undoStack.end());
                    undoStackDocuments.erase(undoStackDocuments.begin() + (undoStackPointer + 2), undoStackDocuments.end());
                    Query undoQuery = query;
#ifdef DIAGNOSTICS
                    if (neededToEraseUndosToRight)
                    {
                        cout << "The undo stack has elements to the right of the pointer, which we need to erase; new Undo Stack after erase: " << endl;
                        cout << "```" << endl;
                        showStatus(false, false, true);
                        cout << "```" << endl;
                        cout << "\nWe can now perform the insertion, giving the updated document:\n" << endl;
                        cout << "```" << endl;
                        showStatus(true, false, false);
                        cout << "```" << endl;
                    }
#endif
                    undoQuery.encryptedArgument = insertionPos; // Not strictly accurate - this is unencrypted!
                    undoStack.push_back(undoQuery);
                    undoStackDocuments.push_back(document);
                    undoStackPointer++;
                }
                break;
            case Query::InsertNonFormatting:
                {
                    const int insertionPos = (query.encryptedArgument ^ decryptionKey) - 1;
                    const int numToInsert = query.encryptedArgument2 ^ decryptionKey;
#ifdef DIAGNOSTICS
                    cout << "Need to insert " << "$c = d \\oplus " << query.encryptedArgument2 << " = " << decryptionKey << " \\oplus " << query.encryptedArgument2 << " = " << numToInsert << "$ non-formatting characters at position $p=" << decryptionKey << "\\oplus " << query.encryptedArgument2 << " = " << (insertionPos + 1) << "$." << endl;
                    cout << "```" << endl;
                    showStatus(true, false, false);
                    cout << indentationSpaces << repeatedString(" ", insertionPos) << "↑" << " insert " << numToInsert << " non-formatting characters here" << endl;
                    cout << "```" << endl;
                    const bool neededToEraseUndosToRight = undoStackPointer + 1 != undoStack.size();
#endif
                    //cout << "InsertNonFormatting " << numToInsert << " at " << insertionPos << endl;
                    const string charsToInsert(numToInsert, 'X');
                    document.insert(insertionPos, charsToInsert);
                    undoStack.erase(undoStack.begin() + (undoStackPointer + 1), undoStack.end());
                    undoStackDocuments.erase(undoStackDocuments.begin() + (undoStackPointer + 2), undoStackDocuments.end());
#ifdef DIAGNOSTICS
                    if (neededToEraseUndosToRight)
                    {
                        cout << "The undo stack has elements to the right of the pointer, which we need to erase; new Undo Stack after erase: " << endl;
                        cout << "```" << endl;
                        showStatus(false, false, true);
                        cout << "```" << endl;
                        cout << "\nWe can now perform the insertion, giving the updated document:\n" << endl;
                        cout << "```" << endl;
                        showStatus(true, false, false);
                        cout << "```" << endl;

                    }
#endif
                    Query undoQuery = query;
                    undoQuery.encryptedArgument = insertionPos; // Not strictly accurate - this is unencrypted!
                    undoQuery.encryptedArgument2 = numToInsert; // Not strictly accurate - this is unencrypted!
                    undoStack.push_back(undoQuery);
                    undoStackDocuments.push_back(document);
                    undoStackPointer++;
                }
                break;
            case Query::IsRangeFormatted:
                {
                    const int queryPosition = (query.encryptedArgument ^ decryptionKey) - 1;
                    assert(document[queryPosition] == 'X');
                    //cout << "IsRangeFormatted at " << queryPosition << endl;
                    int64_t queryAnswer = -1;
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
                    //cout << "Changed decryptionKey to " << decryptionKey << endl;
#ifdef DIAGNOSTICS
                    cout << "Need to find the size of the formatted range around the position $p = d \\oplus " << query.encryptedArgument << " = " << decryptionKey << " \\oplus " << query.encryptedArgument << " = " << (queryPosition + 1) << "$." << endl;
                    cout << "```" << endl;
                    showStatus(false, true, false);
                    cout << indentationSpaces << repeatedString(" ", queryPosition) << "↑" << " query the size of formatted range around this point" << endl;
                    cout << "```" << endl;
                    if (queryAnswer == -1)
                    {
                        queryAnswer = 3'141'592;
                        cout << "The queried position is not part of a formatted range; the answer to query #" << queryNum << " is $3141592$." << endl;
                    }
                    else
                        cout << "The queried position is part of a formatted range and the number of non-formatting characters is this range is " << queryAnswer << "; the answer to query #" << queryNum << " is $" << queryAnswer << "$." << endl;
                    cout << "\nUpdate $d$: $d = d + " << queryAnswer << "\\times  2^{" << queryNum << "}" << "  = " << decryptionKey << " + " << queryAnswer << "\\times " << ((1 << queryNum))  << " = " << decryptionKey + ((1 << queryNum)) * queryAnswer << " \\mod 10^9+7 = " << ((decryptionKey + ((1 << queryNum)) * queryAnswer) % Mod) << "$.\n\n" << endl;

#endif
                    if (queryAnswer == -1)
                        queryAnswer = 3'141'592;
                    decryptionKey = (decryptionKey + ((queryAnswer % Mod) * powerOf2) % Mod) % Mod;

                    queryResults.push_back(queryAnswer);
                }
                break;
            case Query::Undo:
                {
                    const int numToUndo = query.encryptedArgument ^ decryptionKey;
#ifdef DIAGNOSTICS
                    cout << "Need to re-wind the undo stack pointer by $u = d \\oplus " << query.encryptedArgument << " = " << decryptionKey << " \\oplus " << query.encryptedArgument << " = " << numToUndo << "$ places.\n" << endl;
#endif
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
                    const int numToRedo = (query.encryptedArgument ^ decryptionKey);
#ifdef DIAGNOSTICS
                    cout << "Need to move forward the undo stack pointer by $r = d \\oplus " << query.encryptedArgument << " = " << decryptionKey << " \\oplus " << query.encryptedArgument << " = " << numToRedo << "$ places.\n" << endl;
#endif
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

#ifdef DIAGNOSTICS
        if (queryNum != static_cast<int>(queries.size()))
        {
            cout << "**State after processing query " << queryNum << "**:" << endl;
            cout << "```" << endl;
            showStatus(true, false, true);
            cout << "```" << endl;
            cout << endl;
        }
        else
        {
            cout << "\nThat's all the queries processed, and the final value of $d$ is " << decryptionKey << "; so the answer for this test case is " << decryptionKey << "." << endl;
        }
#endif
        //cout << "document: " << document << endl;
        //cout << "Undo stack: " << endl;
        for (const auto x : undoStack)
        {
            //cout << (x.type == Query::InsertNonFormatting ? 'X' : '*') << " " << x.encryptedArgument << endl;
        }
        //cout << "undoStackPointer: " << undoStackPointer << endl;
        queryNum++;
        bruteForceDocs.push_back(document);

        powerOf2 = (2 * powerOf2) % Mod;
    }
    return decryptionKey;
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
        const AVLTreeIterator formattingCharToRightIter = findFirstNodeToRightOf(position, root());
        const int newFormattingCharSizeOfUnformattedToLeftRun = formattingCharToRightIter.currentNode()->value - (formattingCharToRightIter.currentNodePosition() - position);
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
        newRoot = adjustRunToLeftOfNodeToRightOf(treeIter, position + 1, adjustedFormattingCharToRightSizeOfUnformattedToLeftRun - formattingCharToRightIter.currentNode()->value);
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
    assert(root()); // The Sentinel node should have been added.
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
        const auto maxPosInLeftSubchild = treeIter.currentNodePosition() - subTreeRoot->value - 1;
        if (maxPosInLeftSubchild < position)
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


int64_t solveOptimised(const vector<Query>& queries, vector<string>& bruteForceDocs)
{
    vector<int> queryResults;
    AVLTree formattingCharsTree;
    // Sentinel value.
    formattingCharsTree.insertFormattingChar(0);
    formattingCharsTree.root()->isSentinelValue = true;
    //cout << "Initial formattingCharsTree: " << endl;
    //printTree(formattingCharsTree);

    int64_t decryptionKey = 0;
    int64_t powerOf2 = 2;

    int queryNum = 1;
    for (const auto& query : queries)
    {
        //cout << "Processing query " << queryNum << endl;
        switch (query.type)
        {
            case Query::InsertFormatting:
                {
                    const int insertionPos = (query.encryptedArgument ^ decryptionKey) - 1;
                    //cout << "InsertFormatting at " << insertionPos << endl;
                    formattingCharsTree.insertFormattingChar(insertionPos);
                }
                break;
            case Query::InsertNonFormatting:
                {
                    const int insertionPos = (query.encryptedArgument ^ decryptionKey) - 1;
                    const int numToInsert = (query.encryptedArgument2 ^ decryptionKey);
                    //cout << "InsertNonFormatting " << numToInsert << " at " << insertionPos << endl;
                    formattingCharsTree.insertNonFormattingChars(insertionPos, numToInsert);
                }
                break;
            case Query::IsRangeFormatted:
                {
                    const int queryPosition = (query.encryptedArgument ^ decryptionKey) - 1;
                    //cout << "IsRangeFormatted at " << queryPosition << endl;
                    int queryAnswer = formattingCharsTree.distBetweenEnclosingFormattedChars(queryPosition);
                    if (queryAnswer == -1)
                        queryAnswer = 3'141'592;
                    decryptionKey = (decryptionKey + (queryAnswer * powerOf2) % Mod) % Mod;
                    cout << "Changed decryptionKey to " << decryptionKey << endl;

                    //cout << "queryAnswer: " << queryAnswer << endl;
                    queryResults.push_back(queryAnswer);
                }
                break;
            case Query::Undo:
                {
                    const int numToUndo = (query.encryptedArgument ^ decryptionKey);
                    //cout << "Undo " << numToUndo << endl;
                    formattingCharsTree.undo(numToUndo);
                }
                break;
            case Query::Redo:
                {
                    const int numToRedo = (query.encryptedArgument ^ decryptionKey);
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
        powerOf2 = (powerOf2 * 2) % Mod;
    }

    return decryptionKey;
}

int AVLTree::distBetweenEnclosingFormattedChars(int position)
{
    const AVLTreeIterator formattingCharToRightIter = findFirstNodeToRightOf(position, root());

    if (formattingCharToRightIter.currentNode()->isSentinelValue || formattingCharToRightIter.numFormattingCharsToLeft() % 2 == 0)
        return -1;
    else
        return formattingCharToRightIter.currentNode()->value;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        //const int T = rand() % 100 + 1;
        const int T = 1;
        cout << T << endl;

        for (int t = 0; t < T; t++)
        {
            string document;
            int64_t decryptionKey = 0; 
            int64_t powerOf2 = 2;

            int queryNum = 1;

            vector<Query> undoStack;
            int undoStackPointer = -1;

            const int numQueries = 1 + rand() % 100;
            vector<Query> queries;
            for (int i = 0; i < numQueries; i++)
            {
                bool haveQuery = false;
                Query query;
                int numFormatting = count(document.begin(), document.end(), '*');
                int numNonFormatting = count(document.begin(), document.end(), 'X');
                const auto prevDecryptionKey = decryptionKey;
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
                            if (queryAnswer == -1)
                                queryAnswer = 3'141'592;
                            decryptionKey = (decryptionKey + (queryAnswer * powerOf2) % Mod) % Mod;
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
                queries.back().encryptedArgument = queries.back().encryptedArgument ^ prevDecryptionKey;
                queries.back().encryptedArgument2 = queries.back().encryptedArgument2 ^ prevDecryptionKey;
                //cerr << "document: " << document << endl;
                //cerr << "Undo stack: " << endl;
                for (const auto x : undoStack)
                {
                    //cerr << (x.type == Query::InsertNonFormatting ? 'X' : '*') << " " << x.encryptedArgument << endl;
                }
                //cerr << "undoStackPointer: " << undoStackPointer << endl;
                queryNum++;
                powerOf2 = (powerOf2 * 2) % Mod;
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
                        cout << '?' << " " << query.encryptedArgument << endl;
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
                case '?':
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
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
        cout << "solutionOptimised:  " << solutionOptimised << endl;;
        assert(solutionOptimised == solutionBruteForce);
#endif
    }

    assert(cin);
}
