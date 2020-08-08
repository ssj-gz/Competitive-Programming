#ifndef TREE_GENERATOR_H
#define TREE_GENERATOR_H

#include "random-choosable-set.h"

#include <vector>
#include <set>
#include <memory>
#include <functional>
#include <cassert>
#include <iostream>

#include "testlib.h"

template <typename NodeData>
struct TestEdge;
template <typename NodeData>
struct TestNode
{
    std::vector<TestEdge<NodeData>*> neighbours;
    int originalId = -1;
    int scrambledId = -1;
    int forceScrambledId = -1;
    int id() const
    {
        return (scrambledId == -1) ? originalId : scrambledId;
    }
    NodeData data;
};
template <typename NodeData>
struct TestEdge
{
    TestNode<NodeData>* nodeA = nullptr;
    TestNode<NodeData>* nodeB = nullptr;
    TestNode<NodeData>* otherNode(TestNode<NodeData>* node)
    {
        if (node == nodeA)
            return nodeB;
        if (node == nodeB)
            return nodeA;
        assert(false);
        return nullptr;
    }
};
template <typename NodeData>
class TreeGenerator
{
    public:
        TestNode<NodeData>* createNode()
        {
            m_nodes.emplace_back(new TestNode<NodeData>);
            auto newNode = m_nodes.back().get();
            newNode->originalId = m_nodes.size();
            return newNode;
        }
        TestNode<NodeData>* createNode(TestNode<NodeData>* attachedTo)
        {
            auto newNode = createNode();
            addEdge(newNode, attachedTo);
            return newNode;
        }
        TestNode<NodeData>* createNodeWithRandomParent()
        {
            const int parentIndex = rnd.next(static_cast<int>(m_nodes.size())); // The static_cast is necessary, as the type of m_nodes.size() varies according to platform.
            auto randomParent = m_nodes[parentIndex].get();
            return createNode(randomParent);
        }
        std::vector<TestNode<NodeData>*> createNodesWithRandomParent(int numNewNodes)
        {
            std::vector<TestNode<NodeData>*> newNodes;
            for (int i = 0; i < numNewNodes; i++)
            {
                newNodes.push_back(createNodeWithRandomParent());
            }

            return newNodes;
        }
        std::vector<TestNode<NodeData>*> createNodesWithRandomParentPreferringLeafNodes(int numNewNodes, double leafNodePreferencePercent)
        {
            std::vector<TestNode<NodeData>*> leaves; // Use std::vector instead of std::set as ordering of pointers in std::set may not be stable across runs.
            for (auto& node : m_nodes)
            {
                if (node->neighbours.size() <= 1)
                    leaves.push_back(node.get());
            }

            return createNodesWithRandomParentPreferringFromSet(leaves, numNewNodes, leafNodePreferencePercent, [](TestNode<NodeData>* newNode, TestNode<NodeData>* newNodeParent, const bool parentWasPreferred, bool& addNewNodeToSet, bool& removeParentFromSet)
                    {
                    addNewNodeToSet = true;
                    if (newNodeParent->neighbours.size() > 1)
                    removeParentFromSet = true;
                    });
        }

        std::vector<TestNode<NodeData>*> createNodesWithRandomParentPreferringFromSet(const std::vector<TestNode<NodeData>*>& preferredSet, int numNewNodes, double preferencePercent, std::function<void(TestNode<NodeData>* newNode, TestNode<NodeData>* parent, const bool parentWasPreferred, bool& addNewNodeToSet, bool& removeParentFromSet)> onCreateNode)
        {
            std::vector<TestNode<NodeData>*> newNodes;
            RandomChooseableSet<TestNode<NodeData>*> preferredSetCopy(preferredSet.begin(), preferredSet.end());
            RandomChooseableSet<TestNode<NodeData>*> nonPreferredSet;
            for (auto& node : m_nodes)
            {
                if (!preferredSetCopy.contains(node.get()))
                {
                    nonPreferredSet.insert(node.get());
                }
            }

            for (int i = 0; i < numNewNodes; )
            {
                const double random = rnd.next(100.0);
                TestNode<NodeData>* newNodeParent = nullptr;
                bool parentWasPreferred = false;
                if (random <= preferencePercent && !preferredSetCopy.empty())
                {
                    // Choose a random element from preferredSetCopy as a parent.
                    newNodeParent = preferredSetCopy.chooseRandom(); 
                    parentWasPreferred = true;
                }
                else if (!nonPreferredSet.empty())
                {
                    // Choose a random element from nonPreferredSet as a parent.
                    newNodeParent = nonPreferredSet.chooseRandom(); 
                }
                if (newNodeParent)
                {
                    auto newNode = createNode(newNodeParent);
                    newNodes.push_back(newNode);
                    bool addNewNodeToSet = false;
                    bool removeParentFromSet = false;
                    onCreateNode(newNode, newNodeParent, parentWasPreferred, addNewNodeToSet, removeParentFromSet);
                    if (addNewNodeToSet)
                        preferredSetCopy.insert(newNode);
                    else
                        nonPreferredSet.insert(newNode);
                    if (removeParentFromSet)
                    {
                        preferredSetCopy.erase(newNodeParent);
                        nonPreferredSet.insert(newNodeParent);
                    }
                    i++;
                }
            }

            return newNodes;
        }

        int numNodes() const
        {
            return m_nodes.size();
        };
        TestNode<NodeData>* firstNode() const
        {
            assert(!m_nodes.empty());
            return m_nodes.front().get();
        }
        TestEdge<NodeData>* addEdge(TestNode<NodeData>* nodeA, TestNode<NodeData>* nodeB)
        {
            m_edges.emplace_back(new TestEdge<NodeData>);
            auto newEdge = m_edges.back().get();
            newEdge->nodeA = nodeA;
            newEdge->nodeB = nodeB;
            nodeA->neighbours.push_back(newEdge);
            nodeB->neighbours.push_back(newEdge);

            return newEdge;
        }
        std::vector<TestNode<NodeData>*> addNodeChain(TestNode<NodeData>* chainStart, int numInChain)
        {
            std::vector<TestNode<NodeData>*> nodeChain;

            auto lastNodeInChain = chainStart;
            for (int i = 0; i < numInChain; i++)
            {
                lastNodeInChain = createNode(lastNodeInChain);
                nodeChain.push_back(lastNodeInChain);
            }

            return nodeChain;
        }
        void scrambleNodeOrder()
        {
            shuffle(m_nodes.begin(), m_nodes.end());
        }
        void scrambleEdgeOrder()
        {
            shuffle(m_edges.begin(), m_edges.end());
            for (auto& edge : m_edges)
            {
                if (rnd.next(2) == 1)
                    std::swap(edge->nodeA, edge->nodeB);
            }
        }
        // Scrambles the order of the nodes, then re-ids them, in new order, with the first
        // node in the new order having scrambledId 1, then next 2, etc.
        void scrambleNodeIdsAndReorder(TestNode<NodeData>* forceAsRootNode)
        {
            scrambleNodeOrder();
            for (int i = 0; i < numNodes(); i++)
            {
                m_nodes[i]->scrambledId = (i + 1);
            }
            if (forceAsRootNode)
            {
                auto forcedRootNodeIter = find_if(m_nodes.begin(), m_nodes.end(), [forceAsRootNode](const auto& nodePtr) { return nodePtr.get() == forceAsRootNode; });
                assert(forcedRootNodeIter != m_nodes.end());
                if (forcedRootNodeIter != m_nodes.begin())
                {
                    std::swap((*m_nodes.begin())->scrambledId, forceAsRootNode->scrambledId);
                    iter_swap(m_nodes.begin(), forcedRootNodeIter);
                } 
                assert(forceAsRootNode->scrambledId == 1);
            }
        }
        // Scrambles the order of the nodes, then re-ids them, in new order, with the first
        // node in the new order having scrambledId 1, then next 2, etc.
        // This respects the "forceScrambledId" of the Node, if any.
        void scrambleNodeIdsAndReorderNG()
        {
            const int numNodes = this->numNodes();
            std::vector<std::unique_ptr<TestNode<NodeData>>> newNodeOrder(numNodes);
            auto oldNodeOrder = std::move(m_nodes);

            for (auto& nodeUniquePtr : oldNodeOrder)
            {
                if (nodeUniquePtr->forceScrambledId != -1)
                {
                    assert(newNodeOrder[nodeUniquePtr->forceScrambledId - 1] == nullptr);
                    newNodeOrder[nodeUniquePtr->forceScrambledId - 1] = std::move(nodeUniquePtr);
                }
            }
            // Remove the "empty" nodes (the ones that had a forceScrambledId).
            std::cout << "# oldNodeOrder before erase: " << oldNodeOrder.size() << std::endl;
            oldNodeOrder.erase(std::remove_if(oldNodeOrder.begin(), oldNodeOrder.end(), [](const auto& nodeUniquePtr) { return nodeUniquePtr == nullptr; }), oldNodeOrder.end());
            std::cout << "# oldNodeOrder after erase: " << oldNodeOrder.size() << std::endl;
            ::shuffle(oldNodeOrder.begin(), oldNodeOrder.end());
            auto firstEmptyNewSlotIter = newNodeOrder.begin();
            for (auto& nodeToMoveUniquePtr : oldNodeOrder)
            {
                while (*firstEmptyNewSlotIter != nullptr)
                {
                    firstEmptyNewSlotIter++;
                    assert(firstEmptyNewSlotIter != newNodeOrder.end());
                }
                *firstEmptyNewSlotIter = std::move(nodeToMoveUniquePtr);
                (*firstEmptyNewSlotIter)->scrambledId = (firstEmptyNewSlotIter - newNodeOrder.begin()) + 1;
            }

            m_nodes = std::move(newNodeOrder);
            assert(static_cast<int>(m_nodes.size()) == numNodes);

            for (int i = 0; i < numNodes; i++)
            {
                assert(m_nodes[i]);
                assert(m_nodes[i]->id() == i + 1);
                if (m_nodes[i]->forceScrambledId != -1)
                    assert(m_nodes[i]->id() == m_nodes[i]->forceScrambledId);

            }

        }
        std::vector<TestNode<NodeData>*> nodes() const
        {
            std::vector<TestNode<NodeData>*> nodes;
            for (auto& node : m_nodes)
            {
                nodes.push_back(node.get());
            }
            return nodes;
        }
        std::vector<TestEdge<NodeData>*> edges() const
        {
            std::vector<TestEdge<NodeData>*> edges;
            for (auto& edge : m_edges)
            {
                edges.push_back(edge.get());
            }
            return edges;
        }
        /**
         * Remove all edges.  For each pair of nodes that were originally connected by an edge,
         * insert a chain of nodes of length \a pathLength between them.
         */
        void turnEdgesIntoPaths(int pathLength)
        {
            std::vector<TestEdge<NodeData>> originalEdges;
            for (auto& edge : edges())
            {
                originalEdges.push_back(*edge);
            }
            m_edges.clear();
            for (auto& node : m_nodes)
            {
                node->neighbours.clear();
            }

            for (const auto& originalEdge : originalEdges)
            {
                addEdge(originalEdge.nodeB, addNodeChain(originalEdge.nodeA, pathLength).back());
            }
        }
    private:
        std::vector<std::unique_ptr<TestNode<NodeData>>> m_nodes;
        std::vector<std::unique_ptr<TestEdge<NodeData>>> m_edges;

};

#endif

