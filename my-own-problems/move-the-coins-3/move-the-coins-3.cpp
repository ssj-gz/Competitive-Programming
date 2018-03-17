//#define VERIFY_HEIGHT_TRACKER
//#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#undef VERIFY_HEIGHT_TRACKER
#endif
#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <cassert>
#include <memory>
#include <functional>
#include <sys/time.h>


using namespace std;

constexpr auto maxHeight = 100'000;
constexpr int log2(int N, int exponent = 0, int powerOf2 = 1)
{
        return (powerOf2 >= N) ? exponent : log2(N, exponent + 1, powerOf2 * 2);
}
constexpr auto maxBinaryDigits = log2(maxHeight);

struct Node
{
    int id = -1;
    bool hasCoin = false;
    Node* parent = nullptr;
    vector<Node*> children;
    vector<Node*> neighbours;
    int numDescendents = 0;
    bool isParentEdgeHeavy = false;

    int dbgNumVisits = 0;
    int grundyNumber = 0;
};

vector<vector<Node*>> heavyChains;

int fixParentChildAndCountDescendants(Node* node, Node* parentNode)
{
    node->parent = parentNode;
    if (parentNode)
    {
        node->children.erase(find(node->children.begin(), node->children.end(), parentNode));
    }

    for (auto child : node->children)
    {
        node->numDescendents += fixParentChildAndCountDescendants(child, node);
    }

    return node->numDescendents;
}

void doHeavyLightDecomposition(Node* node, bool followedHeavyEdge)
{
    if (followedHeavyEdge)
    {
        node->isParentEdgeHeavy = true;
        heavyChains.back().push_back(node);
    }
    else
    {
        heavyChains.push_back({node});
    }
    if (!node->children.empty())
    {
        auto heaviestChildIter = max_element(node->children.begin(), node->children.end(), [](const Node* lhs, const Node* rhs)
                {
                return lhs->numDescendents < rhs->numDescendents;
                });
        iter_swap(node->children.begin(), heaviestChildIter);
        auto heavyChild = node->children.front();
        doHeavyLightDecomposition(heavyChild, true);

        for (auto lightChildIter = node->children.begin() + 1; lightChildIter != node->children.end(); lightChildIter++)
        {
            doHeavyLightDecomposition(*lightChildIter, false);
        }
    }
}

class HeightTracker
{
    public:
        HeightTracker()
        {
            m_makesDigitOneBegin.resize(maxBinaryDigits + 1);
            m_makesDigitOneEnd.resize(maxBinaryDigits + 1);

            int powerOf2 = 2;
            for (int binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
            {
                m_heightsModPowerOf2.push_back(vector<VersionedValue>(powerOf2));
                powerOf2 <<= 1;
            }
            clear();
        }
        void insertHeight(const int newHeight)
        {
            //cout << "insertHeight: " << newHeight << " m_cumulativeHeightAdjustment: " << m_cumulativeHeightAdjustment << endl;
            if (newHeight < m_smallestHeight)
                m_smallestHeight = newHeight;
            int powerOf2 = 2;
            int newHeightAdjusted = newHeight - m_cumulativeHeightAdjustment;
            if (newHeightAdjusted < 0)
            {
                newHeightAdjusted += (1 << (maxBinaryDigits + 1));
            }
            assert(newHeightAdjusted >= 0);
            for (int binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
            {
                const int heightModPowerOf2 = newHeightAdjusted % powerOf2;
                numHeightsModPowerOf2(binaryDigitNum, heightModPowerOf2)++;
                if (m_makesDigitOneBegin[binaryDigitNum] <= m_makesDigitOneEnd[binaryDigitNum])
                {
                    if (heightModPowerOf2 >= m_makesDigitOneBegin[binaryDigitNum] && heightModPowerOf2 <= m_makesDigitOneEnd[binaryDigitNum])
                    {
                        m_grundyNumber ^= (powerOf2 >> 1);
                    }
                }
                else
                {
                    const int makeDigitZeroBegin = m_makesDigitOneEnd[binaryDigitNum] + 1;
                    const int makeDigitZeroEnd = m_makesDigitOneBegin[binaryDigitNum] - 1;
                    assert(makeDigitZeroBegin <= makeDigitZeroEnd);
                    assert(0 <= makeDigitZeroEnd < powerOf2);
                    assert(0 <= makeDigitZeroBegin < powerOf2);
                    if (!(heightModPowerOf2 >= makeDigitZeroBegin && heightModPowerOf2 <= makeDigitZeroEnd))
                    {
                        m_grundyNumber ^= (powerOf2 >> 1);
                    }
                }

                powerOf2 <<= 1;
            }
#ifdef VERIFY_HEIGHT_TRACKER
            m_dbgHeights.push_back(newHeight);
#endif
        };
        bool canDecreaseHeights() const
        {
            // This is just  for testing - won't be needed in real-life.
            // TODO - remove this!
            if (m_smallestHeight == 0)
                return false;
            return true;
        }
        bool canIncreaseHeights() const
        {
            // This is just  for testing - won't be needed in real-life.
            // TODO - remove this!
            return m_cumulativeHeightAdjustment <= maxHeight;
        }
        void adjustAllHeights(int heightDiff)
        {
            if (heightDiff == 0)
                return;
            assert(heightDiff == 1 || heightDiff == -1);
            m_cumulativeHeightAdjustment += heightDiff;
            m_smallestHeight += heightDiff;
            if (heightDiff == 1)
            {
                int powerOf2 = 2;
                for (int binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
                {
                    // Scroll the begin/ end of the "makes digit one" zone to the left, updating m_grundyNumber
                    // on-the-fly.
                    int changeToNumberOfHeightsThatMakeDigitsOne = 0;
                    changeToNumberOfHeightsThatMakeDigitsOne -= numHeightsModPowerOf2(binaryDigitNum, m_makesDigitOneEnd[binaryDigitNum]);
                    m_makesDigitOneEnd[binaryDigitNum] = (powerOf2 + m_makesDigitOneEnd[binaryDigitNum] - 1) % powerOf2;

                    m_makesDigitOneBegin[binaryDigitNum] = (powerOf2 + m_makesDigitOneBegin[binaryDigitNum] - 1) % powerOf2;
                    changeToNumberOfHeightsThatMakeDigitsOne += numHeightsModPowerOf2(binaryDigitNum, m_makesDigitOneBegin[binaryDigitNum]);

                    if (abs(changeToNumberOfHeightsThatMakeDigitsOne) % 2 == 1)
                        m_grundyNumber ^= (powerOf2 >> 1);

                    powerOf2 <<= 1;
                } 
            }
            else
            {
                int powerOf2 = 2;
                for (int binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
                {
                    // As above, but scroll the "makes digit one" zone to the right.
                    int changeToNumberOfHeightsThatMakeDigitsOne = 0;
                    changeToNumberOfHeightsThatMakeDigitsOne -= numHeightsModPowerOf2(binaryDigitNum, m_makesDigitOneBegin[binaryDigitNum]);
                    m_makesDigitOneBegin[binaryDigitNum] = (m_makesDigitOneBegin[binaryDigitNum] + 1) % powerOf2;

                    m_makesDigitOneEnd[binaryDigitNum] = (m_makesDigitOneEnd[binaryDigitNum] + 1) % powerOf2;
                    changeToNumberOfHeightsThatMakeDigitsOne += numHeightsModPowerOf2(binaryDigitNum, m_makesDigitOneEnd[binaryDigitNum]);

                    if (abs(changeToNumberOfHeightsThatMakeDigitsOne) % 2 == 1)
                        m_grundyNumber ^= (powerOf2 >> 1);

                    powerOf2 <<= 1;
                } 
            }

#ifdef VERIFY_HEIGHT_TRACKER
            for (auto& height : m_dbgHeights)
            {
                height += heightDiff;
                assert(height >= 0);
            }
#endif
        }
        int& numHeightsModPowerOf2(int binaryDigitNum, int modPowerOf2)
        {
            auto& numHeights = m_heightsModPowerOf2[binaryDigitNum][modPowerOf2];
            if (numHeights.versionNumber != m_versionNumber)
            {
                numHeights.value = 0;
                numHeights.versionNumber = m_versionNumber;
            }
            return numHeights.value;
        }
        int grundyNumber() const
        {
#ifdef VERIFY_HEIGHT_TRACKER
            int dbgGrundyNumber = 0;
            for (const auto height : m_dbgHeights)
            {
                dbgGrundyNumber ^= height;
            }
            //cout << "dbgGrundyNumber: " << dbgGrundyNumber << " m_grundyNumber: " << m_grundyNumber << endl;
            assert(dbgGrundyNumber == m_grundyNumber);
#endif
            return m_grundyNumber;
        }
        void clear()
        {
            //cout << "Clear!" << endl;
            m_versionNumber++;
            int powerOf2 = 2;
            for (int binaryDigitNum = 0; binaryDigitNum <= maxBinaryDigits; binaryDigitNum++)
            {
                //m_heightsModPowerOf2.push_back(vector<int>(powerOf2, 0));
                m_makesDigitOneBegin[binaryDigitNum] = powerOf2 >> 1;
                m_makesDigitOneEnd[binaryDigitNum] = powerOf2 - 1;

                powerOf2 <<= 1;
            }
            m_grundyNumber = 0;
            m_cumulativeHeightAdjustment = 0;
#ifdef VERIFY_HEIGHT_TRACKER
            m_dbgHeights.clear();
#endif
        }
#ifdef VERIFY_HEIGHT_TRACKER
        void printHeights() const
        {
            cout << "Heights in height tracker: ";
            for (const auto height : m_dbgHeights)
            {
                cout << height << " ";
            }
            cout << endl;
        }
        vector<int> m_dbgHeights;
#endif
        struct VersionedValue
        {
            int value = 0;
            int versionNumber = -1;
        };
        vector<vector<VersionedValue>> m_heightsModPowerOf2;
        vector<int> m_makesDigitOneBegin;
        vector<int> m_makesDigitOneEnd;

        int m_cumulativeHeightAdjustment = 0;
        int m_grundyNumber = 0;

        int m_versionNumber = 0;
        int m_smallestHeight = 0;
};
enum HeightTrackerAdjustment {DoNotAdjust, AdjustUpWithDepth};
template <typename NodeProcessor>
void doDfs(Node* node, int depth, HeightTracker& heightTracker, HeightTrackerAdjustment heightTrackerAdjustment, NodeProcessor& nodeProcessor)
{
    if (heightTrackerAdjustment == AdjustUpWithDepth)
        heightTracker.adjustAllHeights(1);

    nodeProcessor(node, depth);

    for (auto child : node->children)
    {
        doDfs(child, depth + 1, heightTracker, heightTrackerAdjustment, nodeProcessor);
    }

    if (heightTrackerAdjustment == AdjustUpWithDepth)
        heightTracker.adjustAllHeights(-1);
}

template <typename NodeProcessor>
void doLightFirstDFS(Node* node, HeightTracker& heightTracker, HeightTrackerAdjustment heightTrackerAdjustment, NodeProcessor nodeProcessor)
{
    nodeProcessor(node, 0);
    if (node->children.size() > 1)
    {
        for (auto lightChildIter = node->children.begin() + 1; lightChildIter != node->children.end(); lightChildIter++)
        {
            doDfs(*lightChildIter, 1, heightTracker, heightTrackerAdjustment, nodeProcessor);
        }
    }
}

int grundyNumberBruteForce(Node* node, Node* parent = nullptr, int depth = 0)
{
    static int maxDepth = 0;
    if (depth > maxDepth)
    {
        maxDepth = depth;
        cout << " new max depth: " << maxDepth << endl; 
    }
    int grundyNumber = (node->hasCoin ? depth : 0);
    for (const auto child : node->neighbours)
    {
        if (child == parent)
            continue;

        grundyNumber ^= grundyNumberBruteForce(child, node, depth + 1);
    }
    return grundyNumber;
}


vector<int> computeGrundyNumberForAllNodes(vector<Node>& nodes)
{
    vector<int> grundyNumbers;
    HeightTracker heightTracker;
    auto collect = [&heightTracker](Node* node, int depth)
                        {
                            if (node->hasCoin)
                                heightTracker.insertHeight(depth);
                        };
    auto broadcast = [&heightTracker](Node* node, int depth)
                        {
                            node->grundyNumber ^= heightTracker.grundyNumber();
                        };
    for (auto& chain : heavyChains)
    {
        for (int i = 0; i < 2; i++)
        {
            heightTracker.clear();
            // Crawl along chain, collecting from one node and broadcasting to the next.
            for (auto node : chain)
            {
                heightTracker.adjustAllHeights(1);
                doLightFirstDFS(node, heightTracker, AdjustUpWithDepth, broadcast);
                doLightFirstDFS(node, heightTracker, DoNotAdjust, collect);
            }
            // Now do it backwards.
            reverse(chain.begin(), chain.end());
        }
    }
    for (auto& node : nodes)
    {
        if (node.children.empty())
            continue;
        // Collect and update node from all its light-first descendents.
        heightTracker.clear();
        doLightFirstDFS(&node, heightTracker, DoNotAdjust, collect);
        node.grundyNumber ^= heightTracker.grundyNumber();
        // Broadcast this nodes' coin info to descendents.
        if (node.hasCoin)
        {
            heightTracker.clear();
            heightTracker.insertHeight(0);
            doLightFirstDFS(&node, heightTracker, AdjustUpWithDepth, broadcast);
        }
        // Broadcast light-first descendent info to other light-first descendents.
        vector<Node*> lightChildren = vector<Node*>(node.children.begin() + 1, node.children.end());
        heightTracker.clear();
        for (auto child : lightChildren)
        {
            doDfs(child, 1, heightTracker, AdjustUpWithDepth, broadcast);
            doDfs(child, 1, heightTracker, DoNotAdjust, collect);
        }
        reverse(lightChildren.begin(), lightChildren.end());
        // ... and again, using reversed order of children.
        heightTracker.clear();
        for (auto child : lightChildren)
        {
            doDfs(child, 1, heightTracker, AdjustUpWithDepth, broadcast);
            doDfs(child, 1, heightTracker, DoNotAdjust, collect);
        }
    }

    return grundyNumbers;
}

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
};
struct TestEdge
{
    TestNode* nodeA = nullptr;
    TestNode* nodeB = nullptr;
};
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

            return createNodesWithRandomParentPreferringFromSet(leaves, numNewNodes, leafNodePreferencePercent, [](TestNode* newNode, TestNode* newNodeParent, bool& addNewNodeToSet, bool& removeParentFromSet)
                    {
                        addNewNodeToSet = true;
                        removeParentFromSet = true;
                    });
        }

        vector<TestNode*> createNodesWithRandomParentPreferringFromSet(const set<TestNode*>& preferredSet, int numNewNodes, double preferencePercent, std::function<void(TestNode* newNode, TestNode* parent, bool& addNewNodeToSet, bool& removeParentFromSet)> onCreateNode)
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
                auto nodeIter = nodeSet.begin();
                for (int i = 0; i < randomIndex; i++)
                {
                    nodeIter++;
                }
                return *nodeIter;
            };

            for (int i = 0; i < numNewNodes; i++)
            {
                const double random = static_cast<double>(rand()) / RAND_MAX;
                TestNode* newNodeParent = nullptr;
                if (random <= preferencePercent)
                {
                    // Choose a random element from preferredSetCopy as a parent.
                    newNodeParent = chooseRandomNodeFromSet(preferredSetCopy); 
                }
                else
                {
                    // Choose a random element from nonPreferredSet as a parent.
                    newNodeParent = chooseRandomNodeFromSet(nonPreferredSet); 
                }
                auto newNode = createNode(newNodeParent);
                newNodes.push_back(newNode);
                bool addNewNodeToSet = false;
                bool removeParentFromSet = false;
                onCreateNode(newNode, newNodeParent, addNewNodeToSet, removeParentFromSet);
                if (addNewNodeToSet)
                    preferredSetCopy.insert(newNode);
                else
                    nonPreferredSet.insert(newNode);
                if (removeParentFromSet)
                    nonPreferredSet.erase(newNodeParent);
            }

            return newNodes;
        }

        int numNodes() const
        {
            return m_nodes.size();
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
        void scrambleNodeIdsAndReorder()
        {
            scrambleNodeOrder();
            for (int i = 0; i < m_nodes.size(); i++)
            {
                m_nodes[i]->scrambledId = (i + 1);
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

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
#if 1
    struct timeval time;
    gettimeofday(&time,NULL);
    srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
#endif
    if (argc == 2)
    {
        //const int numNodes = rand() % 10 + 1;
        const int numNodes = 100'000;
        const int numEdges = numNodes - 1;
        const int leafNodePercentage = 98;
        set<int> leafNodes;
        leafNodes.insert(1);
        set<int> nonLeafNodes;
        cout << numNodes << endl;
        for (int i = 0; i < numEdges; i++)
        {
            int parentNodeIndex = -1;
            if (nonLeafNodes.empty())
            {
                parentNodeIndex = (rand() % (i + 1)) + 1;
            }
            else
            {
                if ((rand() % 100) + 1 <= leafNodePercentage)
                {
                    //cerr << "Chose leaf node" << endl;
                    const int leafNodeIndex = rand() % leafNodes.size();
                    auto leafNodeIter = leafNodes.begin();
                    for (int i = 0; i < leafNodeIndex; i++)
                    {
                        leafNodeIter++;
                    }
                    parentNodeIndex = *leafNodeIter;
                }
                else
                {
                    //cerr << "Chose non-leaf node" << endl;
                    const int nonLeafNodeIndex = rand() % nonLeafNodes.size();
                    auto nonLeafNodeIter = nonLeafNodes.begin();
                    for (int i = 0; i < nonLeafNodeIndex; i++)
                    {
                        nonLeafNodeIter++;
                    }
                    parentNodeIndex = *nonLeafNodeIter;
                }
            }
            leafNodes.erase(parentNodeIndex);
            nonLeafNodes.insert(parentNodeIndex);
            leafNodes.insert(i + 2);
            cout << (i + 2) << " " << parentNodeIndex << endl;
        }
        for (int i = 0; i < numNodes; i++)
        {
            cout << rand() % 20 << endl;
        }
        return 0;
    }

    int numNodes;
    cin >> numNodes;

    cout << "numNodes: " << numNodes << endl;

    vector<Node> nodes(numNodes);
    for (int nodeIndex = 0; nodeIndex < numNodes; nodeIndex++)
    {
        nodes[nodeIndex].id = nodeIndex + 1;
    }

    for (int edgeNum = 0; edgeNum < numNodes - 1; edgeNum++)
    {
        int node1;
        cin >> node1;
        int node2;
        cin >> node2;
        //cout << "node1 : " << node1 << " node2: " << node2 << endl;
        // Make 0-relative.
        node1--;
        node2--;
        assert(cin);

        //cout << "edge: " << nodes[node1].id << " - " << nodes[node2].id << endl;

        nodes[node1].children.push_back(&(nodes[node2]));
        nodes[node2].children.push_back(&(nodes[node1]));

        nodes[node1].neighbours.push_back(&(nodes[node2]));
        nodes[node2].neighbours.push_back(&(nodes[node1]));
    }

    for (int i = 0; i < numNodes; i++)
    {
        int numCoins;
        cin >> numCoins;
        //cout << "numCoins: " << numCoins << endl;

        nodes[i].hasCoin = ((numCoins % 2) == 1);
        //nodes[i].hasCoin = rand() % 2;
    }
    assert(cin);

    auto rootNode = &(nodes.front());
    fixParentChildAndCountDescendants(rootNode, nullptr);
    doHeavyLightDecomposition(rootNode, false);

    computeGrundyNumberForAllNodes(nodes);
    int numBlah = 0;
    for (auto& node : nodes)
    {
        const auto resultBruteForce = grundyNumberBruteForce(&node);
        cout << "Node: " << node.id << " real grundy number: " <<  resultBruteForce << " optimised grundy number: " << node.grundyNumber << " " << (resultBruteForce == node.grundyNumber ? "MATCH" : "MISMATCH") << endl; 
        //cout << "Node: " << node.id << " grundy:" << resultBruteForce << endl;
        assert(resultBruteForce == node.grundyNumber);
        if (node.grundyNumber == 0)
        {
            numBlah++;
        }
    }

    cout << "numBlah: " << numBlah << " numNodes: " << numNodes << endl;
}
