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
            currentNode->balanceFactor = 0;
            currentNode->maxDescendantDepth = 0;
            if (currentNode->leftChild)
            {
                currentNode->maxDescendantDepth = max(currentNode->maxDescendantDepth, 1 + currentNode->leftChild->maxDescendantDepth);
                currentNode->balanceFactor -= 1 + currentNode->leftChild->maxDescendantDepth;
            }
            if (currentNode->rightChild)
            {
                currentNode->maxDescendantDepth = max(currentNode->maxDescendantDepth, 1 + currentNode->rightChild->maxDescendantDepth);
                currentNode->balanceFactor += 1 + currentNode->rightChild->maxDescendantDepth;
            }
            if (currentNode->balanceFactor < -1)
            {
                return rotateRight(currentNode);
            }
            if (currentNode->balanceFactor > +1)
            {
                cout << "balanceFactor: " << currentNode->balanceFactor << endl;
                const auto newSubtreeRoot = currentNode->rightChild;
                newSubtreeRoot->leftChild = currentNode;
                currentNode->rightChild = nullptr;
                currentNode->maxDescendantDepth = 0;
                currentNode->balanceFactor = 0;
                newSubtreeRoot->balanceFactor = -newSubtreeRoot->leftChild->maxDescendantDepth + newSubtreeRoot->rightChild->maxDescendantDepth;
                return newSubtreeRoot;
            }

            return currentNode;
        }

        TreeNode* rotateRight(TreeNode* subtreeRoot)
        {
            const auto newSubtreeRoot = subtreeRoot->leftChild;
            newSubtreeRoot->rightChild = subtreeRoot;
            subtreeRoot->leftChild = nullptr;
            subtreeRoot->maxDescendantDepth = 0;
            subtreeRoot->balanceFactor = 0;
            newSubtreeRoot->balanceFactor = -newSubtreeRoot->leftChild->maxDescendantDepth + newSubtreeRoot->rightChild->maxDescendantDepth;
            return newSubtreeRoot;
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

    assert(subtreeRoot->maxDescendantDepth == maxDescendantDepth);
    assert(subtreeRoot->balanceFactor == balanceFactor);

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

    return (actualInOrderValues == expectedInOrderValues);
}

void printSubTree(TreeNode* subtreeRoot)
{
    cout << "Node " << subtreeRoot->id << " has value: " << subtreeRoot->value << " balanceFactor: " << subtreeRoot->balanceFactor;
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
}
