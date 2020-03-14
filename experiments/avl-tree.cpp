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
    AVLNode(int value)
        : value{value}
    {

    }
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
        AVLNode* root()
        {
            return m_root;
        }
        void insertValue(int newValue)
        {
            if (!m_root)
            {
                m_root = createNode(newValue);
                return;
            }
            m_root = insertValue(newValue, m_root);
        }

    private:
        AVLNode* m_root = nullptr;

        AVLNode* insertValue(int newValue, AVLNode* currentNode)
        {
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
            const auto newSubtreeRoot = subtreeRoot->leftChild;
            auto previousNewSubtreeRootRightChild = newSubtreeRoot->rightChild;
            newSubtreeRoot->rightChild = subtreeRoot;
            subtreeRoot->leftChild = previousNewSubtreeRootRightChild;
            updateInfoFromChildren(subtreeRoot);

            updateInfoFromChildren(newSubtreeRoot);
            return newSubtreeRoot;
        }
        AVLNode* rotateLeft(AVLNode* subtreeRoot)
        {
            const auto newSubtreeRoot = subtreeRoot->rightChild;
            auto previousNewSubtreeRootLeftChild = newSubtreeRoot->leftChild;
            newSubtreeRoot->leftChild = subtreeRoot;
            subtreeRoot->rightChild = previousNewSubtreeRootLeftChild;
            updateInfoFromChildren(subtreeRoot);

            updateInfoFromChildren(newSubtreeRoot);
            return newSubtreeRoot;
        }

        void updateInfoFromChildren(AVLNode* nodeToUpdate)
        {
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
            if (m_nodes.empty() || m_nodes.back().size() == nodeBlockSize - 1)
            {
                m_nodes.push_back(vector<AVLNode>());
                m_nodes.back().reserve(nodeBlockSize);
            }
            m_nodes.back().push_back(AVLNode(value));
            auto newNode = &(m_nodes.back().back());
            newNode->id = m_nextNodeId;
            m_nextNodeId++;
            return newNode;
        }

        static constexpr int nodeBlockSize = 10000;
        deque<vector<AVLNode>> m_nodes;
        int m_nextNodeId = 1;
};

// Debugging/ diagnostics.
bool isBST(AVLNode* node, int lowerValueLimit, int upperValueLimit)
{
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

    for (const auto nthRemaining : numOfRemainingToChoose)
    {
        // Be optimistic and give remappedIndex the smallest possible value:
        // we'll correct our optimism as we go along :)
        int remappedIndex = nthRemaining;
        auto currentNode = removedIndices.root();
        int numRemovedUpToCurrentNodeIndexOffset = 0;
        while (currentNode)
        {
            const int indexOfCurrentNode = currentNode->value;
            const int numDescendantsLeftSubChild = (currentNode->leftChild ? currentNode->leftChild->numDescendants : 0);
            const int numRemovedUpToCurrentNodeIndex = numRemovedUpToCurrentNodeIndexOffset + numDescendantsLeftSubChild;
            const int numFreeUpToCurrentNodeIndex = indexOfCurrentNode - numRemovedUpToCurrentNodeIndex;
            if (numFreeUpToCurrentNodeIndex >= nthRemaining + 1)
            {
                // We've overshot; the required remappedIndex is to the left of indexOfCurrentNode; "recurse"
                // into left child.
                currentNode = currentNode->leftChild;
            }
            else
            {
                // Again, be optimistic about remappedIndex - we'll correct it as we go along.
                remappedIndex = max(remappedIndex, indexOfCurrentNode + (nthRemaining - numFreeUpToCurrentNodeIndex) + 1);
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
        for (int i = 0; i < nthRemaining; i++)
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
