#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include <limits>
#include <cassert>

using namespace std;

struct TreeNode
{
    TreeNode(int value)
        : value{value}
    {

    }
    int value = -1;
    TreeNode *leftChild = nullptr;
    TreeNode *rightChild = nullptr;
    int balanceFactor = 0;
    int maxDescendantDepth = 0;

    int id = -1;
};

class AVLTree
{
    public:
        TreeNode* root()
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
        TreeNode* m_root = nullptr;

        TreeNode* insertValue(int newValue, TreeNode* currentNode)
        {
            if (newValue < currentNode->value)
            {
                // Values in the left subtree of node must be *strictly less* than
                // that of currentNode.
                if (currentNode->leftChild)
                {
                    currentNode->leftChild = insertValue(newValue, currentNode->leftChild);
                    currentNode->maxDescendantDepth = max(currentNode->maxDescendantDepth, 1 + currentNode->leftChild->maxDescendantDepth);
                }
                else
                {
                    currentNode->leftChild = createNode(newValue);
                    currentNode->maxDescendantDepth++;
                    currentNode->balanceFactor--;
                }
            }
            else
            {
                // Values in the right subtree of node must be *greater than or equal to* that
                // that of currentNode.
                assert(newValue >= currentNode->value);
                if (currentNode->rightChild)
                {
                    currentNode->rightChild = insertValue(newValue, currentNode->rightChild);
                    currentNode->maxDescendantDepth = max(currentNode->maxDescendantDepth, 1 + currentNode->rightChild->maxDescendantDepth);
                }
                else
                {
                    currentNode->rightChild = createNode(newValue);
                    currentNode->maxDescendantDepth++;
                    currentNode->balanceFactor++;
                }
            }
            updateInfoFromChildren(currentNode);

            if (currentNode->balanceFactor < -1)
            {
                if (currentNode->leftChild->balanceFactor <= 0)
                    return rotateRight(currentNode);
                else
                {
                    currentNode->leftChild = rotateLeft(currentNode->leftChild);
                    return rotateRight(currentNode);
                }
            }
            if (currentNode->balanceFactor > +1)
            {
                if (currentNode->rightChild->balanceFactor >= 0)
                    return rotateLeft(currentNode);
                else
                {
                    currentNode->rightChild = rotateRight(currentNode->rightChild);
                    return rotateLeft(currentNode);
                }
            }

            return currentNode;
        }

        TreeNode* rotateRight(TreeNode* subtreeRoot)
        {
            const auto newSubtreeRoot = subtreeRoot->leftChild;
            auto previousNewSubtreeRootRightChild = newSubtreeRoot->rightChild;
            newSubtreeRoot->rightChild = subtreeRoot;
            subtreeRoot->leftChild = previousNewSubtreeRootRightChild;
            updateInfoFromChildren(subtreeRoot);

            updateInfoFromChildren(newSubtreeRoot);
            return newSubtreeRoot;
        }
        TreeNode* rotateLeft(TreeNode* subtreeRoot)
        {
            const auto newSubtreeRoot = subtreeRoot->rightChild;
            auto previousNewSubtreeRootLeftChild = newSubtreeRoot->leftChild;
            newSubtreeRoot->leftChild = subtreeRoot;
            subtreeRoot->rightChild = previousNewSubtreeRootLeftChild;
            updateInfoFromChildren(subtreeRoot);

            updateInfoFromChildren(newSubtreeRoot);

            return newSubtreeRoot;

        }

        void updateInfoFromChildren(TreeNode* nodeToUpdate)
        {
            nodeToUpdate->balanceFactor = 0;
            nodeToUpdate->maxDescendantDepth = 0;
            if (nodeToUpdate->leftChild)
            {
                nodeToUpdate->balanceFactor -= 1 + nodeToUpdate->leftChild->maxDescendantDepth;
                nodeToUpdate->maxDescendantDepth = max(nodeToUpdate->maxDescendantDepth, 1 + nodeToUpdate->leftChild->maxDescendantDepth);
            }
            if (nodeToUpdate->rightChild)
            {
                nodeToUpdate->balanceFactor += 1 + nodeToUpdate->rightChild->maxDescendantDepth;
                nodeToUpdate->maxDescendantDepth = max(nodeToUpdate->maxDescendantDepth, 1 + nodeToUpdate->rightChild->maxDescendantDepth);
            }
        }

        TreeNode* createNode(int value)
        {
            m_nodes.push_back(make_unique<TreeNode>(value));
            auto newNode = m_nodes.back().get();
            newNode->id = m_nodes.size();
            return newNode;
        }
        vector<unique_ptr<TreeNode>> m_nodes;
};

// Debugging/ diagnostics.
bool isBST(TreeNode* node, int lowerValueLimit, int upperValueLimit)
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

bool isBST(TreeNode* node)
{
    return isBST(node, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
}

std::pair<bool, int> isSubtreeBalanced(TreeNode* subtreeRoot)
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

bool isBalanced(TreeNode* node)
{
    return isSubtreeBalanced(node).first;
}

void collectInOrderValues(TreeNode* subtreeRoot, vector<int>& destValues)
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

void printSubTree(TreeNode* subtreeRoot)
{
    cout << "Node " << subtreeRoot->id << " has value: " << subtreeRoot->value << " balanceFactor: " << subtreeRoot->balanceFactor << " maxDescendantDepth: " << subtreeRoot->maxDescendantDepth;
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
    assert(checkContents(tree, expectedInOrderValues));

    cout << "Testcase passed" << endl;
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
        // Quick performance test.
        const int N = rand() % 1'000'000 + 1;
        vector<int> testcase;
        const int maxValue = 1000;
        for (int i = 0; i < N; i++)
            testcase.push_back(rand() % maxValue);
        AVLTree tree;
        for (const auto x : testcase)
        {
            tree.insertValue(x);
        }
        return 0;
    }

    while (true)
    {
        const int N = rand() % 1000 + 1;
        vector<int> testcase;
        const int maxValue = 1 + rand() % 1000;
        for (int i = 0; i < N; i++)
            testcase.push_back(rand() % maxValue);
        random_shuffle(testcase.begin(), testcase.end());
        assertTestcase(testcase);
    }
}
