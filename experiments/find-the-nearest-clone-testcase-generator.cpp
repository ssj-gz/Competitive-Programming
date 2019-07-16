#include <iostream>
#include <vector>
#include <set>
#include <functional>
#include <memory>

#include <cassert>
#include <sys/time.h>

using namespace std;

struct NodeData
{
    int colour = -1;
};


struct TestEdge;
struct TestNode
{
    vector<TestEdge*> neighbours;
    int originalId = -1;
    int scrambledId = -1;
    int id() const
    {
        return (scrambledId == -1) ? originalId : scrambledId;
    }
    NodeData data;

    // Filled in my doGenericInfoDFS.
    TestNode* parent = nullptr;
    int visitNum = -1;
    int endVisitNum = -1;
    int height = -1;
};
struct TestEdge
{
    TestNode* nodeA = nullptr;
    TestNode* nodeB = nullptr;
    TestNode* otherNode(TestNode* node)
    {
        if (node == nodeA)
            return nodeB;
        if (node == nodeB)
            return nodeA;
        assert(false);
        return nullptr;
    }
};
bool operator<(const unique_ptr<TestEdge>& lhs, const unique_ptr<TestEdge>& rhs)
{
    TestNode *lhsNode1 = lhs->nodeA;
    TestNode *lhsNode2 = lhs->nodeA;
    if (lhsNode2 < lhsNode1)
        swap(lhsNode1, lhsNode2);

    TestNode *rhsNode1 = rhs->nodeA;
    TestNode *rhsNode2 = rhs->nodeA;
    if (rhsNode2 < rhsNode1)
        swap(rhsNode1, rhsNode2);

    if (lhsNode1 != rhsNode1)
        return lhsNode1 < rhsNode1;

    return lhsNode2 < rhsNode2;
}
class TreeGenerator
{
    public:
        TestNode* createNode()
        {
            m_nodes.emplace_back(new TestNode);
            auto newNode = m_nodes.back().get();
            newNode->originalId = m_nodes.size();
            return newNode;
        }
        TestNode* createNode(TestNode* attachedTo)
        {
            auto newNode = createNode();
            addEdge(newNode, attachedTo);
            return newNode;
        }
        TestNode* createNodeWithRandomParent()
        {
            const int parentIndex = rand() % m_nodes.size();
            auto randomParent = m_nodes[parentIndex].get();
            return createNode(randomParent);
        }
        vector<TestNode*> createNodesWithRandomParent(int numNewNodes)
        {
            vector<TestNode*> newNodes;
            for (int i = 0; i < numNewNodes; i++)
            {
                newNodes.push_back(createNodeWithRandomParent());
            }

            return newNodes;
        }
        vector<TestNode*> createNodesWithRandomParentPreferringLeafNodes(int numNewNodes, double leafNodePreferencePercent)
        {
            set<TestNode*> leaves;
            for (auto& node : m_nodes)
            {
                if (node->neighbours.size() <= 1)
                    leaves.insert(node.get());
            }

            return createNodesWithRandomParentPreferringFromSet(leaves, numNewNodes, leafNodePreferencePercent, [](TestNode* newNode, TestNode* newNodeParent, const bool parentWasPreferred, bool& addNewNodeToSet, bool& removeParentFromSet)
                    {
                    addNewNodeToSet = true;
                    if (newNodeParent->neighbours.size() > 1)
                    removeParentFromSet = true;
                    });
        }

        vector<TestNode*> createNodesWithRandomParentPreferringFromSet(const set<TestNode*>& preferredSet, int numNewNodes, double preferencePercent, std::function<void(TestNode* newNode, TestNode* parent, const bool parentWasPreferred, bool& addNewNodeToSet, bool& removeParentFromSet)> onCreateNode)
        {
            vector<TestNode*> newNodes;
            set<TestNode*> preferredSetCopy(preferredSet);
            set<TestNode*> nonPreferredSet;
            for (auto& node : m_nodes)
            {
                if (preferredSetCopy.find(node.get()) == preferredSetCopy.end())
                {
                    nonPreferredSet.insert(node.get());
                }
            }

            auto chooseRandomNodeFromSet = [](set<TestNode*>& nodeSet)
            {
                const int randomIndex = rand() % nodeSet.size();
                //cerr << "nodeSet.size(): " << nodeSet.size() << " randomIndex: " << randomIndex << endl;
                auto nodeIter = nodeSet.begin();
                for (int i = 0; i < randomIndex; i++)
                {
                    nodeIter++;
                }
                return *nodeIter;
            };

            for (int i = 0; i < numNewNodes; )
            {
                //cerr << "createNodesWithRandomParentPreferringFromSet: " << (i + 1) << " / " << numNewNodes << endl;
                const double random = static_cast<double>(rand()) / RAND_MAX * 100;
                TestNode* newNodeParent = nullptr;
                bool parentWasPreferred = false;
                //cerr << "random: " << random << " preferencePercent: " << preferencePercent << endl;
                if (random <= preferencePercent && !preferredSetCopy.empty())
                {
                    // Choose a random element from preferredSetCopy as a parent.
                    //cerr << " choosing preferred" << endl;
                    newNodeParent = chooseRandomNodeFromSet(preferredSetCopy); 
                    parentWasPreferred = true;
                    //cerr << "Chose leaf: " << newNodeParent->neighbours.size() << endl;
                    //assert(newNodeParent->neighbours.size() <= 1);
                }
                else if (!nonPreferredSet.empty())
                {
                    // Choose a random element from nonPreferredSet as a parent.
                    //cerr << " choosing non-leaf" << endl;
                    newNodeParent = chooseRandomNodeFromSet(nonPreferredSet); 
                    //assert(newNodeParent->neighbours.size() > 1);
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
        int numEdges() const
        {
            return m_edges.size();
        };
        TestNode* firstNode() const
        {
            assert(!m_nodes.empty());
            return m_nodes.front().get();
        }
        TestEdge* addEdge(TestNode* nodeA, TestNode* nodeB)
        {
            m_edges.emplace_back(new TestEdge);
            auto newEdge = m_edges.back().get();
            newEdge->nodeA = nodeA;
            newEdge->nodeB = nodeB;
            nodeA->neighbours.push_back(newEdge);
            nodeB->neighbours.push_back(newEdge);

            return newEdge;
        }
        vector<TestNode*> addNodeChain(TestNode* chainStart, int numInChain)
        {
            vector<TestNode*> nodeChain;

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
            random_shuffle(m_nodes.begin(), m_nodes.end());
        }
        void scrambleEdgeOrder()
        {
            random_shuffle(m_edges.begin(), m_edges.end());
            for (auto& edge : m_edges)
            {
                if (rand() % 2 == 1)
                    swap(edge->nodeA, edge->nodeB);
            }
        }
        // Scrambles the order of the nodes, then re-ids them, in new order, with the first
        // node in the new order having scrambledId 1, then next 2, etc.
        void scrambleNodeIdsAndReorder(TestNode* forceAsRootNode)
        {
            scrambleNodeOrder();
            for (int i = 0; i < m_nodes.size(); i++)
            {
                m_nodes[i]->scrambledId = (i + 1);
            }
            if (forceAsRootNode)
            {
                auto forcedRootNodeIter = find_if(m_nodes.begin(), m_nodes.end(), [forceAsRootNode](const auto& nodePtr) { return nodePtr.get() == forceAsRootNode; });
                assert(forcedRootNodeIter != m_nodes.end());
                if (forcedRootNodeIter != m_nodes.begin())
                {
                    swap((*m_nodes.begin())->scrambledId, forceAsRootNode->scrambledId);
                    iter_swap(m_nodes.begin(), forcedRootNodeIter);
                } 
                assert(forceAsRootNode->scrambledId == 1);
            }
        }
        vector<TestNode*> nodes() const
        {
            vector<TestNode*> nodes;
            for (auto& node : m_nodes)
            {
                nodes.push_back(node.get());
            }
            return nodes;
        }
        vector<TestEdge*> edges() const
        {
            vector<TestEdge*> edges;
            for (auto& edge : m_edges)
            {
                edges.push_back(edge.get());
            }
            return edges;
        }
        void removeDuplicateEdges()
        {
            auto compareEdges = [](const TestEdge* lhs, const TestEdge* rhs)
            {
                assert(lhs);
                assert(rhs);
                TestNode *lhsNode1 = lhs->nodeA;
                TestNode *lhsNode2 = lhs->nodeB;
                if (lhsNode2 < lhsNode1)
                    swap(lhsNode1, lhsNode2);

                TestNode *rhsNode1 = rhs->nodeA;
                TestNode *rhsNode2 = rhs->nodeB;
                if (rhsNode2 < rhsNode1)
                    swap(rhsNode1, rhsNode2);

                if (lhsNode1 != rhsNode1)
                    return lhsNode1 < rhsNode1;

                return lhsNode2 < rhsNode2;
            };
            set<TestEdge*, decltype(compareEdges)> edges(compareEdges);

            vector<unique_ptr<TestEdge>> uniqueEdges;
            for (auto& edge : m_edges)
            {
                assert(edge.get());
                if (edges.find(edge.get()) == edges.end())
                {
                    edges.insert(edge.get());
                    uniqueEdges.push_back(std::move(edge));
                }
            }

            swap(uniqueEdges, m_edges);

        }
        void printEdges() const
        {
            for (const auto& edge : m_edges)
            {
                cout << edge->nodeA->id() << " " << edge->nodeB->id() << endl;
            }
        }
    private:
        vector<unique_ptr<TestNode>> m_nodes;
        vector<unique_ptr<TestEdge>> m_edges;
};


int main()
{
    struct timeval time;
    gettimeofday(&time,NULL);
    srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

    const int maxArmLength = 25;
    const int minArmLength = 15;
    // Not sure why, but maxNodesForArms seems to be arbitrary-ish 
    // (but maybe around 50'000 gives good results?), but upToNumNodesForCentre
    // should not be very big - 900'000 seems to work very badly.
    const int maxNodesForArms = 40'000;

    const int desiredColour = 0;

    const int maxColours = rand() % 1'000'000;

    int numWastedArmSpace = 0;

    TreeGenerator treeGenerator;

    vector<vector<TestNode*>> arms;
    vector<TestNode*> armRoots;

    while (treeGenerator.numNodes() <= maxNodesForArms)
    {
        TestNode* armRoot = treeGenerator.createNode();
        const int numInArm = rand() % (maxArmLength - minArmLength) + minArmLength + 1;
        assert(numInArm >= minArmLength);

        vector<TestNode*> arm = treeGenerator.addNodeChain(armRoot, numInArm);

        arms.push_back(arm);
        armRoots.push_back(armRoot);
    }
    const int numArms = arms.size();

    const int minDistanceFromArmRoot = 14;
    const int restOfArmsMinDistanceFromArmRoot = minDistanceFromArmRoot + 1;

    assert(numArms > 2);

    // These two nodes will be the closest two nodes of the desiredColour.
    arms[0][minDistanceFromArmRoot]->data.colour = desiredColour;
    arms[1][minDistanceFromArmRoot - 3]->data.colour = desiredColour;

    for (int i = 2; i < numArms; i++)
    {
        const int distanceFromArmRoot = rand() % (arms[i].size() - restOfArmsMinDistanceFromArmRoot) + restOfArmsMinDistanceFromArmRoot;
        assert(distanceFromArmRoot > minDistanceFromArmRoot && distanceFromArmRoot < arms[i].size());
        numWastedArmSpace += arms[i].size() - distanceFromArmRoot - 1;
        arms[i][distanceFromArmRoot]->data.colour = desiredColour;
    }

    // Connect the arm roots in a ring (the "central ring").
    TestNode* previousArmRoot = armRoots.back();
    for (auto armRoot : armRoots)
    {
        treeGenerator.addEdge(armRoot, previousArmRoot);
        previousArmRoot = armRoot;
    }

    auto createAdditionalConnections = [&treeGenerator](const vector<TestNode*>& amongstNodes, const int stopAtTotalEdges)
    {
        // May create duplicate edges; these will be removed later on.
        while (treeGenerator.numEdges() <= stopAtTotalEdges)
        {
            const int randomNode1 = rand() % amongstNodes.size();
            const int randomNode2 = rand() % amongstNodes.size();
            if (randomNode1 == randomNode2)
                continue;
            treeGenerator.addEdge(amongstNodes[randomNode1], amongstNodes[randomNode2]);
        }
    };

    // Add some more nodes that will be joined to the central ring.
    const int upToNumNodesForCentre = min(treeGenerator.numNodes() + 10'000, 90'000);
    const int upToNumEdgesForCentre = 90'000;
    vector<TestNode*> centralNodes = armRoots;
    while (treeGenerator.numNodes() < upToNumNodesForCentre)
    {
        bool connectToArmRoot = rand() % 2;
        TestNode* connectTo = nullptr;
        if (connectToArmRoot)
        {
            connectTo = armRoots[rand() % armRoots.size()];
        }
        else
        {
            connectTo = centralNodes[rand() % centralNodes.size()];
        }
        centralNodes.push_back(treeGenerator.createNode(connectTo));
    }
    createAdditionalConnections(centralNodes, upToNumEdgesForCentre);

    // Make "fans" originating in Arm Nodes to use up the remaining nodes/ edges.
    const int maxTotalNodes = 100'000;
    const int maxTotalEdges = 100'000;
    vector<TestNode*> armNodes;
    for (const auto& arm : arms)
    {
        for (auto armNode : arm)
        {
            armNodes.push_back(armNode);
        }
    }
    while (treeGenerator.numNodes() < maxTotalNodes && treeGenerator.numEdges() < maxTotalEdges)
    {
        const int maxInFan = min(maxTotalNodes - treeGenerator.numNodes(), maxTotalEdges - treeGenerator.numEdges());
        const int numInFan = min(rand() % 100 + 1, maxInFan);
        TestNode* armNode = armNodes[rand() % armNodes.size()];
        for (int i = 0; i < numInFan; i++)
        {
            treeGenerator.createNode(armNode);
        }
    }

    // Randomised colours for remaining nodes.  Don't add any more of desiredColour, though!
    set<int> distinctColoursSet;
    while (distinctColoursSet.size() < maxColours)
    {
        distinctColoursSet.insert(rand() % 100'000'000);
    }
    vector<int> distinctColoursArray(distinctColoursSet.begin(), distinctColoursSet.end());
    random_shuffle(distinctColoursArray.begin(), distinctColoursArray.end());
    for (const auto node : treeGenerator.nodes())
    {
        if (node->data.colour == -1)
        {
            node->data.colour = 1 + rand() % maxColours;
            assert(node->data.colour != desiredColour);
        }
        node->data.colour = distinctColoursArray[node->data.colour];
    }

    // Scramble the tree and print out the testcase.
    treeGenerator.scrambleNodeIdsAndReorder(nullptr);
    treeGenerator.scrambleEdgeOrder();

    cerr << " original numEdges: " << treeGenerator.numEdges() << endl;
    treeGenerator.removeDuplicateEdges();
    cerr << " after numEdges: " << treeGenerator.numEdges() << endl;

    cout << treeGenerator.numNodes() << " " << treeGenerator.numEdges() << endl;
    treeGenerator.printEdges();

    // Output nodes colours, and desiredColour.
    for (const auto node : treeGenerator.nodes())
    {
        cout << node->data.colour << " ";
    }
    cout << endl;
    cout << distinctColoursArray[desiredColour] << endl;

    cerr << "Flibble: " << arms[0][minDistanceFromArmRoot]->scrambledId << ", " << arms[1][minDistanceFromArmRoot - 1]->scrambledId << endl;
    cerr << "# arms: " << arms.size() << endl;
    cerr << "numWastedArmSpace: " << numWastedArmSpace << endl;
}
