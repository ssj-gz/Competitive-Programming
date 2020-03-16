#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include <memory>
#include <limits>
#include <cassert>

using namespace std;

struct AVLNode
{
    int value = -1;
    AVLNode *leftChild = nullptr;
    AVLNode *rightChild = nullptr;
    int balanceFactor = 0;
    int maxDescendantDepth = 0;
    int numDescendants = 1;
    int sumOfDescendantValues = 0;

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

// Debugging/ diagnostics.
bool isBST(AVLNode* node, int lowerValueLimit, int upperValueLimit)
{
    if (!node)
        return true;
    bool result = true;
    if (node->leftChild)
    {
        // Assume everything to the left is *strictly less than* value.
        result = result && isBST(node->leftChild, lowerValueLimit, node->value - 1);
    }
    if (node->rightChild)
    {
        // Assume everything to the left is *greater than or equal to* value.
        result = result && isBST(node->rightChild, node->value, upperValueLimit);
    }
    return result;
}

bool isBST(AVLNode* node)
{
    return isBST(node, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
}

std::pair<bool, int> isSubtreeBalanced(AVLNode* subtreeRoot)
{
    if (!subtreeRoot)
    {
        return {true,0};

    }
    bool isBalanced = true;
    int maxDescendantDepth = 0;
    int balanceFactor = 0;
    if (subtreeRoot->leftChild)
    {
        const auto& [isLeftChildBalanced, maxLeftChildDescendantDepth] = isSubtreeBalanced(subtreeRoot->leftChild);
        isBalanced = isBalanced && isLeftChildBalanced;
        balanceFactor -= maxLeftChildDescendantDepth + 1;
        maxDescendantDepth = max(maxDescendantDepth, 1 + maxLeftChildDescendantDepth);
    }
    if (subtreeRoot->rightChild)
    {
        const auto& [isRightChildBalanced, maxRightChildDescendantDepth] = isSubtreeBalanced(subtreeRoot->rightChild);
        isBalanced = isBalanced && isRightChildBalanced;
        balanceFactor += maxRightChildDescendantDepth + 1;
        maxDescendantDepth = max(maxDescendantDepth, 1 + maxRightChildDescendantDepth);
    }

    if (balanceFactor < -1 || balanceFactor > +1)
        isBalanced = false;

    if (subtreeRoot->maxDescendantDepth != maxDescendantDepth)
    {
        cout << "Error: node " << subtreeRoot->id << " maxDescendantDepth should be " << maxDescendantDepth << " but is " << subtreeRoot->maxDescendantDepth << endl;
        assert(subtreeRoot->maxDescendantDepth == maxDescendantDepth);
    }
    if (subtreeRoot->balanceFactor != balanceFactor)
    {
        cout << "Error: node " << subtreeRoot->id << " balanceFactor should be " << balanceFactor << " but is " << subtreeRoot->balanceFactor << endl;
        assert(subtreeRoot->balanceFactor == balanceFactor);
    }

    return {isBalanced, maxDescendantDepth};
}

std::pair<bool, int> isDescendantCountCorrect(AVLNode* subtreeRoot)
{
    if (!subtreeRoot)
        return {true, 0};
    bool isCorrect = true;
    int numDescendants = 1;

    if (subtreeRoot->leftChild)
    {
        const auto& [isLeftDescendantCountCorrect, leftChildNumDescendants] = isDescendantCountCorrect(subtreeRoot->leftChild);
        isCorrect = isCorrect && isLeftDescendantCountCorrect;
        numDescendants += leftChildNumDescendants;
    }
    if (subtreeRoot->rightChild)
    {
        const auto& [isRightDescendantCountCorrect, rightChildNumDescendants] = isDescendantCountCorrect(subtreeRoot->rightChild);
        isCorrect = isCorrect && isRightDescendantCountCorrect;
        numDescendants += rightChildNumDescendants;
    }

    isCorrect = isCorrect && (subtreeRoot->numDescendants == numDescendants);

    if (subtreeRoot->numDescendants != numDescendants)
    {
        cout << "Error: node " << subtreeRoot->id << " numDescendants should be " << numDescendants << " but is " << subtreeRoot->numDescendants << endl;
        assert(subtreeRoot->numDescendants == numDescendants);
    }

    return {isCorrect, numDescendants};
}

bool isDescendantCountCorrect(AVLTree& tree)
{
    return isDescendantCountCorrect(tree.root()).first;
}

bool isBalanced(AVLNode* node)
{
    return isSubtreeBalanced(node).first;
}

void collectInOrderValues(AVLNode* subtreeRoot, vector<int>& destValues)
{
    if (!subtreeRoot)
        return;
    if (subtreeRoot->leftChild)
        collectInOrderValues(subtreeRoot->leftChild, destValues);

    destValues.push_back(subtreeRoot->value);

    if (subtreeRoot->rightChild)
        collectInOrderValues(subtreeRoot->rightChild, destValues);
}

bool checkContents(AVLTree& tree, const vector<int>& expectedInOrderValues)
{
    vector<int> actualInOrderValues;
    collectInOrderValues(tree.root(), actualInOrderValues);

    if (actualInOrderValues != expectedInOrderValues)
    {
        cout << "Expected values:";
        for(const auto x : expectedInOrderValues)
            cout << " " << x;
        cout << endl;
        cout << "Actual values:  ";
        for(const auto x : actualInOrderValues)
            cout << " " << x;
        cout << endl;
        return false;
    }

    return true;
}

void printSubTree(AVLNode* subtreeRoot)
{
    if (subtreeRoot == nullptr)
        return;
    cout << "Node " << subtreeRoot->id << " has value: " << subtreeRoot->value << " balanceFactor: " << subtreeRoot->balanceFactor << " maxDescendantDepth: " << subtreeRoot->maxDescendantDepth << " numDescendants: " << subtreeRoot->numDescendants;
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

void assertTestcase(const vector<int>& valuesToInsert)
{
    cout << "Testcase:";
    for (const auto x : valuesToInsert)
        cout << " " << x;
    cout << endl;

    auto expectedInOrderValues = valuesToInsert;
    sort(expectedInOrderValues.begin(), expectedInOrderValues.end());

    AVLTree tree;
    for (const auto value : valuesToInsert)
    {
        tree.insertValue(value);
    }
    printTree(tree);

    assert(isBST(tree.root()));
    assert(isBalanced(tree.root()));
    assert(isDescendantCountCorrect(tree));
    assert(checkContents(tree, expectedInOrderValues));

    cout << "Testcase passed" << endl;
}

void assertPersistentTestcase(const vector<int>& valuesToInsert)
{
#if 0
    cout << "Testcase:";
    for (const auto x : valuesToInsert)
        cout << " " << x;
    cout << endl;
#endif

    AVLTree tree(true);
    vector<int> expectedInOrderValues;
    int numAdded = 0;
    for (const auto value : valuesToInsert)
    {
        tree.insertValue(value);
        expectedInOrderValues.push_back(value);
        sort(expectedInOrderValues.begin(), expectedInOrderValues.end());

        //printTree(tree);

        assert(isBST(tree.root()));
        assert(isBalanced(tree.root()));
        assert(isDescendantCountCorrect(tree));
        assert(checkContents(tree, expectedInOrderValues));
        numAdded++;
        //cout << "Added " << numAdded << " of " << valuesToInsert.size() << endl;
    }

    expectedInOrderValues.clear();
    for (int revisionNum = 0; revisionNum < valuesToInsert.size() + 1; revisionNum++)
    {

        //printTree(tree);
        tree.switchToRevision(revisionNum);

        assert(isBST(tree.root()));
        assert(isBalanced(tree.root()));
        assert(isDescendantCountCorrect(tree));
        assert(checkContents(tree, expectedInOrderValues));

        if (revisionNum < valuesToInsert.size())
        {
            expectedInOrderValues.push_back(valuesToInsert[revisionNum]);
            sort(expectedInOrderValues.begin(), expectedInOrderValues.end());
        }

        //cout << "Verified " << (revisionNum + 1) << " of " << (valuesToInsert.size() + 1) << endl;
    }


    cout << "Testcase passed" << endl;
}

AVLNode* findKth(AVLNode* subtreeRoot, int k)
{
    assert(k >= 0);
    int numDescendantsLeftChild = 0;
    auto leftChild = subtreeRoot->leftChild;
    if (leftChild)
    {
        numDescendantsLeftChild = leftChild->numDescendants;
        if (numDescendantsLeftChild == k)
            return subtreeRoot;

        if (numDescendantsLeftChild > k)
        {
            // The answer is somewhere in the left subtree.
            return findKth(leftChild, k);
        }
    }
    if (k == 0)
        return subtreeRoot;
    // Recurse into right child.
    assert(subtreeRoot->rightChild);
    // We're "cutting out" subtreeRoot and all its left-descendents, so reduce k
    // by this number.
    return findKth(subtreeRoot->rightChild, k - numDescendantsLeftChild - 1);
}

AVLNode* findKthIterative(AVLTree& tree, int k)
{
    assert(k >= 0);
    auto currentNode = tree.root();
    while (true)
    {
        int numDescendantsLeftChild = 0;
        auto leftChild = currentNode->leftChild;
        if (leftChild)
        {
            numDescendantsLeftChild = leftChild->numDescendants;
            if (numDescendantsLeftChild == k)
                return currentNode;

            if (numDescendantsLeftChild > k)
            {
                // The answer is somewhere in the left subtree.
                currentNode = leftChild;
                continue;
            }
        }
        if (k == 0)
            return currentNode;
        // Recurse into right child.
        assert(currentNode->rightChild);
        currentNode = currentNode->rightChild;
        // We're "cutting out" currentNode and all its left-descendents, so reduce k
        // by this number.
        k = k - numDescendantsLeftChild - 1;
    }
}

AVLNode* findKth(AVLTree& tree, int k) // k is 0-relative.
{
    return findKth(tree.root(), k);
}

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

AVLNode* findKthFromPair(int k, AVLTree& tree1, AVLTree& tree2)
{
    auto currentNode1 = tree1.root();
    int numToLeftOffset1 = 0;
    while (currentNode1)
    {
        const auto currentValue1 = currentNode1->value;
        //cout << "currentValue1: " << currentValue1 << endl;
        int numToLeftOffset2 = 0;
        int numToLeft2 = 0;
        int lastValue2 = -1;
        auto currentNode2 = tree2.root();
        while (currentNode2)
        {
            lastValue2 = currentNode2->value;
            const int numInLeftSubchild2 = (currentNode2->leftChild ? currentNode2->leftChild->numDescendants : 0);
            if (currentNode2->value < currentValue1)
            {
                numToLeft2 = numToLeftOffset2 + numInLeftSubchild2 + 1;
            }
            if (currentNode2->value > currentValue1)
            {
                currentNode2 = currentNode2->leftChild;
            }
            else
            {
                currentNode2 = currentNode2->rightChild;
                numToLeftOffset2 += 1 + numInLeftSubchild2;
            }
        }
        //cout << "lastValue2: " << lastValue2 << endl;
        //cout << "numToLeft2: " << numToLeft2 << endl;

        const auto numInLeftSubchild1 = (currentNode1->leftChild ? currentNode1->leftChild->numDescendants : 0);
        const auto numToLeft1 = numToLeftOffset1 + numInLeftSubchild1;
        const auto numToLeftInBoth = numToLeft1 + numToLeft2;
        //cout << "numToLeft1: " << numToLeft1 << " numToLeftInBoth: " << numToLeftInBoth << endl;
        if (numToLeftInBoth == k)
        {
            //cout << "Found: " << currentNode1->value << endl;
            return currentNode1;
        }

        if (numToLeftInBoth > k)
        {
            currentNode1 = currentNode1->leftChild;
        }
        else
        {
            currentNode1 = currentNode1->rightChild;
            numToLeftOffset1 += 1 + numInLeftSubchild1;
        }
    }

    return nullptr;
}

void checkBlah(const vector<int>& testcaseOriginal)
{
    const int n = testcaseOriginal.size();
    for (int t = 0; t < 100; t++)
    {
        int removeBeginL = rand() % n;
        int removeBeginR = rand() % n;
        if (removeBeginL > removeBeginR)
            swap(removeBeginL, removeBeginR);

        const auto numLeft = n - (removeBeginR - removeBeginL + 1);
        if (numLeft <= 0)
            return;

        const int K = rand() % numLeft;

        cout << "checkBlah testcase:" ;
        for (const auto x : testcaseOriginal)
        {
            cout << " " << x;
        }
        cout << endl;
        cout << "n: " << n << " removeBeginL: " <<removeBeginL << " removeBeginR: " << removeBeginR << " K: " << K << endl;

        AVLTree sortedPrefixes(true);
        for (const auto value : testcaseOriginal)
        {
            sortedPrefixes.insertValue(value);
        }
        AVLTree sortedSuffixes(true);
        for (auto valueReverseIter = testcaseOriginal.rbegin(); valueReverseIter != testcaseOriginal.rend(); valueReverseIter++)
        {
            sortedSuffixes.insertValue(*valueReverseIter);
        }

        auto testcase = testcaseOriginal;
        testcase.erase(testcase.begin() + removeBeginL, testcase.begin() + removeBeginR + 1);

        sort(testcase.begin(), testcase.end());

        cout << "After removal and sorting:";
        for (const auto value : testcase)
        {
            cout << " " << value;
        }
        cout << endl;

        const auto dbgKthValue = testcase[K];

        sortedPrefixes.switchToRevision(removeBeginL);
        cout << "sortedPrefix:";
        vector<int> bloop;
        collectInOrderValues(sortedPrefixes.root(), bloop);
        for (const auto x : bloop)
        {
            cout << " " << x;
        }
        cout << endl;
        printTree(sortedPrefixes);
        sortedSuffixes.switchToRevision(n - 1 - removeBeginR);
        cout << "sortedSuffix: " << endl;
        bloop.clear();
        collectInOrderValues(sortedSuffixes.root(), bloop);
        for (const auto x : bloop)
        {
            cout << " " << x;
        }
        cout << endl;
        printTree(sortedSuffixes);

        auto blah = findKthFromPair(K, sortedPrefixes, sortedSuffixes);
        if (!blah)
        {
            blah = findKthFromPair(K, sortedSuffixes, sortedPrefixes);
        }
        assert(blah);
        cout << "dbgKthValue: " << dbgKthValue << " actual: " << blah->value << endl;
        assert(blah->value == dbgKthValue);
    }
}

int timefindKthFromPair(const vector<int>& testcaseOriginal)
{
    const int n = testcaseOriginal.size();
    AVLTree sortedPrefixes(true, testcaseOriginal.size());
    for (const auto value : testcaseOriginal)
    {
        sortedPrefixes.insertValue(value);
    }
    AVLTree sortedSuffixes(true, testcaseOriginal.size());
    for (auto valueReverseIter = testcaseOriginal.rbegin(); valueReverseIter != testcaseOriginal.rend(); valueReverseIter++)
    {
        sortedSuffixes.insertValue(*valueReverseIter);
    }
    int blee = 0;
    for (int t = 0; t < 100'000; t++)
    {
        int removeBeginL = rand() % n;
        int removeBeginR = rand() % n;
        if (removeBeginL > removeBeginR)
            swap(removeBeginL, removeBeginR);

        const auto numLeft = n - (removeBeginR - removeBeginL + 1);
        if (numLeft <= 0)
            continue;

        const int K = rand() % numLeft;

        sortedPrefixes.switchToRevision(removeBeginL);
        sortedSuffixes.switchToRevision(n - 1 - removeBeginR);

        auto blah = findKthFromPair(K, sortedPrefixes, sortedSuffixes);
        if (!blah)
        {
            blah = findKthFromPair(K, sortedSuffixes, sortedPrefixes);
        }
        blee += blah->value;
    }
    return blee;
}

int main()
{
    assertTestcase({1});
    assertTestcase({5, 3});
    assertTestcase({5, 7});

    assertTestcase({5, 3, 1});
    assertTestcase({1, 3, 5});

    assertTestcase({5, 3, 6, 2, 1});
    assertTestcase({5, 4, 6, 7, 8});

    assertTestcase({5, 4, 6, 2, 3});
    assertTestcase({5, 4, 6, 8, 7});

    assertTestcase({1,2, 6, 4, 5, 3});

    {
        vector<int> testcase;
        testcase.push_back(5);
        choicesWithRemovals(testcase, 10);

    }
    {
        choicesWithRemovals({5, 3}, 10);
    }
    {
        choicesWithRemovals({3, 5}, 10);
    }
    {
        choicesWithRemovals({9, 8, 7}, 10);
    }
    {
        choicesWithRemovals({1, 2, 3}, 10);
    }
    {
        choicesWithRemovals({1, 2, 3, 5, 0, 4, 3, 1, 1, 0}, 10);
    }
    {
        // Quick "findKthFromPair" performance test.
        vector<int> allNums;
        const int N = 200'000;
        for (int i = 0; i < N; i++)
        {
            allNums.push_back(i);
        }
        // Quick "findKthFromPair" test.
        vector<int> blee = allNums;
        random_shuffle(blee.begin(), blee.end());
        blee.erase(blee.begin() + N, blee.end());

        cout << "timefindKthFromPair" << endl;
        cout << timefindKthFromPair(blee) << endl;
        return 0;
    }
    {
        vector<int> allNums;
        for (int i = 0; i < 100'000; i++)
        {
            allNums.push_back(i);
        }
        for (int t = 0; t < 1000; t++)
        {
            // Quick "findKthFromPair" test.
            const int N = rand() % 1000 + 1;
            vector<int> blee = allNums;
            random_shuffle(blee.begin(), blee.end());
            blee.erase(blee.begin() + N, blee.end());

            checkBlah(blee);
        }


    }
    {
        // Quick Persistence performance test.
        const int N = 500'000;
        vector<int> testcase;
        const int maxValue = 1'000'000;
        for (int i = 0; i < N; i++)
            testcase.push_back(rand() % maxValue);
        AVLTree tree(true, 10'000);
        for (const auto x : testcase)
        {
            tree.insertValue(x);
        }
        auto sortedTestcase = testcase;
        sort(sortedTestcase.begin(), sortedTestcase.end());
        for (int k = 0; k < sortedTestcase.size(); k++)
        {
            //assert(findKth(tree, k)->value == sortedTestcase[k]);
            assert(findKthIterative(tree, k)->value == sortedTestcase[k]);
            //cout << "k: " << k << " findKth: " << findKth(tree, k)->value << " actual kth: " << sortedTestcase[k] << endl;
        }
        //return 0;
    }

    {
        assertPersistentTestcase({1,2,3});
        assertPersistentTestcase({1,5,3});
        assertPersistentTestcase({1,5,3,2,4});
        for (int t = 0; t < 10'000; t++)
        {
            const int N = rand() % 1000 + 1;
            const int maxValue = rand() % 1000 + 1;
            vector<int> testcase;
            for (int i = 0; i < N; i++)
            {
                testcase.push_back(rand()% maxValue + 1);
            }
            cout << "Performing Persistence test " << (t + 1) << " out of " << 10'000 << endl;
            assertPersistentTestcase(testcase);
        }
    }

    {
#if 1
        for (int t = 0; t < 10000; t++)
        {
            const int numToChooseFrom = 1 + rand() % 20;
            vector<int> numOfRemainingToChoose;
            for (int i = 0; i < numToChooseFrom; i++)
            {
                cout << "Blah: " << numToChooseFrom - (numToChooseFrom - i) << endl;
                numOfRemainingToChoose.push_back(rand() % (numToChooseFrom - i));
            }
            choicesWithRemovals(numOfRemainingToChoose, numToChooseFrom);
        }
#else
        const int numToChooseFrom = 200'000;
        vector<int> numOfRemainingToChoose;
        for (int i = 0; i < numToChooseFrom; i++)
        {
            numOfRemainingToChoose.push_back(rand() % (numToChooseFrom - i));
        }
        choicesWithRemovals(numOfRemainingToChoose, numToChooseFrom);
        return 0;
#endif
    }

    {
        // Quick performance test.
        const int N = 200'000;
        vector<int> testcase;
        const int maxValue = 1'000'000;
        for (int i = 0; i < N; i++)
            testcase.push_back(rand() % maxValue);
        AVLTree tree;
        for (const auto x : testcase)
        {
            tree.insertValue(x);
        }
        auto sortedTestcase = testcase;
        sort(sortedTestcase.begin(), sortedTestcase.end());
        for (int k = 0; k < sortedTestcase.size(); k++)
        {
            //assert(findKth(tree, k)->value == sortedTestcase[k]);
            assert(findKthIterative(tree, k)->value == sortedTestcase[k]);
            //cout << "k: " << k << " findKth: " << findKth(tree, k)->value << " actual kth: " << sortedTestcase[k] << endl;
        }
        //return 0;
    }
    {
        const vector<int> testcase = { 2, 3, 5};
        const int K = 2;
        auto sortedTestcase = testcase;
        sort(sortedTestcase.begin(), sortedTestcase.end());
        const int debugKthSmallestValue = sortedTestcase[K];

        AVLTree tree;
        cout << "testcase:";
        for (const auto x : testcase)
        {
            cout << " " << x;
            tree.insertValue(x);
        }
        cout << endl;
        cout << "K: " << K << endl;
        printTree(tree);

        const auto kthSmallestValue = findKth(tree, K)->value;
        assert(kthSmallestValue == debugKthSmallestValue);
        //return 0;
    }


    while (true)
    {
        const int N = rand() % 1000 + 1;
        vector<int> testcase;
        const int maxValue = 1 + rand() % 1000;
        for (int i = 0; i < N; i++)
            testcase.push_back(rand() % maxValue);
        random_shuffle(testcase.begin(), testcase.end());
        const int K = rand() % testcase.size();
        auto sortedTestcase = testcase;
        sort(sortedTestcase.begin(), sortedTestcase.end());
        const int debugKthSmallestValue = sortedTestcase[K];
        assertTestcase(testcase);

        AVLTree tree;
        cout << "testcase:";
        for (const auto x : testcase)
        {
            cout << " " << x;
            tree.insertValue(x);
        }
        cout << endl;
        printTree(tree);
        cout << "K: " << K << endl;


        const auto kthSmallestValue = findKth(tree, K)->value;
        cout << "expected kthSmallestValue: " << debugKthSmallestValue << " actual kthSmallestValue: " << kthSmallestValue << endl;
        assert(kthSmallestValue == debugKthSmallestValue);
    }
}
