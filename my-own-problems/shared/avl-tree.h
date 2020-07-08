#include <vector>
#include <deque>

namespace Internal
{
    struct AVLNode
    {
        int64_t value = -1;
        AVLNode *leftChild = nullptr;
        AVLNode *rightChild = nullptr;
        int balanceFactor = 0;
        int maxDescendantDepth = 0;
        int numDescendants = 1;
        int64_t sumOfDescendantValues = 0;

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
            void setMaintainSumOfDescendentValues(const bool maintainSumOfDescendentValues)
            {
                m_maintainSumOfDescendentValues = maintainSumOfDescendentValues;
            }
            AVLNode* root()
            {
                if (!m_isPersistent)
                    return m_root;
                else
                    return m_rootForRevision[m_revisionNumber];
            }
            void insertValue(int64_t newValue)
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
                    assert(m_revisionNumber == static_cast<int>(m_rootForRevision.size()) - 1);
                }
            }
            void switchToRevision(int revisionNum)
            {
                assert(m_isPersistent);
                m_revisionNumber = revisionNum;
            }

        private:
            AVLNode* m_root = nullptr;

            AVLNode* insertValue(int64_t newValue, AVLNode* currentNode)
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
                if (m_maintainSumOfDescendentValues)
                    nodeToUpdate->sumOfDescendantValues = nodeToUpdate->value;

                auto leftChild = nodeToUpdate->leftChild;

                if (leftChild)
                {
                    nodeToUpdate->balanceFactor -= 1 + leftChild->maxDescendantDepth;
                    nodeToUpdate->maxDescendantDepth = std::max(nodeToUpdate->maxDescendantDepth, 1 + leftChild->maxDescendantDepth);
                    nodeToUpdate->numDescendants += leftChild->numDescendants;
                    if (m_maintainSumOfDescendentValues)
                        nodeToUpdate->sumOfDescendantValues += leftChild->sumOfDescendantValues;
                }

                auto rightChild = nodeToUpdate->rightChild;
                if (rightChild)
                {
                    nodeToUpdate->balanceFactor += 1 + rightChild->maxDescendantDepth;
                    nodeToUpdate->maxDescendantDepth = std::max(nodeToUpdate->maxDescendantDepth, 1 + rightChild->maxDescendantDepth);
                    nodeToUpdate->numDescendants += rightChild->numDescendants;
                    if (m_maintainSumOfDescendentValues)
                        nodeToUpdate->sumOfDescendantValues += rightChild->sumOfDescendantValues;
                }
            }

            AVLNode* createNode(int64_t value)
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
                    m_nodes.push_back(std::vector<AVLNode>());
                    m_nodes.back().reserve(m_nodeBlockSize);
                }
                m_nodes.back().push_back(AVLNode());
                auto newNode = &(m_nodes.back().back());
                newNode->id = m_nextNodeId;
                m_nextNodeId++;
                return newNode;
            }

            bool m_isPersistent = false;
            bool m_maintainSumOfDescendentValues = true;

            int m_nodeBlockSize = 1;
            std::deque<std::vector<AVLNode>> m_nodes;

            int m_nextNodeId = 1;

            int m_revisionNumber = 0;
            std::vector<AVLNode*> m_rootForRevision;

    };
}


