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
            insertValue(newValue, m_root);

        }

    private:
        TreeNode* m_root = nullptr;

        void insertValue(int newValue, TreeNode* currentNode)
        {
            if (newValue < currentNode->value)
            {
                // Values in the left subtree of node must be *strictly less* than
                // that of currentNode.
                if (currentNode->leftChild)
                {
                    insertValue(newValue, currentNode->leftChild);
                }
                else
                {
                    currentNode->leftChild = createNode(newValue);
                }
            }
        }

        TreeNode* createNode(int value)
        {
            m_nodes.push_back(make_unique<TreeNode>(value));
            return m_nodes.back().get();
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
        result = result && isBST(node->leftChild, node->value, upperValueLimit);
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
        balanceFactor -= maxLeftChildDescendantDepth;
        maxDescendantDepth = max(maxDescendantDepth, 1 + maxLeftChildDescendantDepth);
    }
    if (subtreeRoot->rightChild)
    {
        const auto& [isRightChildBalanced, maxRightChildDescendantDepth] = isSubtreeBalanced(subtreeRoot->rightChild);
        isBalanced = isBalanced && isRightChildBalanced;
        balanceFactor += maxRightChildDescendantDepth;
        maxDescendantDepth = max(maxDescendantDepth, 1 + maxRightChildDescendantDepth);
    }

    if (balanceFactor < -1 || balanceFactor > +1)
        isBalanced = false;


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
    cout << "Node " << subtreeRoot << " has value: " << subtreeRoot->value << " balanceFactor: " << subtreeRoot->balanceFactor;
    cout << " leftChild: " << subtreeRoot->leftChild << " rightChild: " << subtreeRoot->rightChild << endl;

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

    assert(isBST(tree.root()));
    assert(isBalanced(tree.root()));
    assert(checkContents(tree, expectedInOrderValues));

    cout << "Testcase passed" << endl;
}

int main()
{
    assertTestcase({1});
    assertTestcase({5, 3});

}
