#include <iostream>
#include <limits>

using namespace std;

struct TreeNode
{
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
        }

    private:
        TreeNode* m_root = nullptr;
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

int main()
{
}
