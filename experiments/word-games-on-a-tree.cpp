#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <cassert>
#include <sys/time.h>

// TODO - 
//
// - Read in testcase; form graph with Nodes and Edges.
// - Generate random testcases.
// - Crudely splice in the centroid decomposition stuff from Ticket to Ride.
// - Maybe fix up the decomposition stuff so that edges end up being actually removed - need to keep a "backup" of original edges, if so.  In any case, this can be left unti later.
// - Crudely splice in SuffixTreeBuilder.
// - Modify SuffixTreeBuilder so we can find the list of words that the currently read word is a suffix of.
// - Do the main processCentroid algorithm, and hook it into the decomposition stuff.
//   - Track word cursor as we do depth-first search; find longest word; transmit back along path from centroid; track the word suffixes we've generated; etc.
// - Do the final calculation of max crossed words for each node, etc.

using namespace std;

struct Edge;
struct Node
{
    vector<Edge*> neighbours;
    int multiplier;
};
struct BestTracker
{
    static constexpr int maxToStore = 3;
    void add(int64_t value, Edge* otherEdge);
    int num = 0;
    struct Blah
    {
        int64_t value = -1;
        Edge* otherEdge = nullptr;
    };
    Blah stored[maxToStore];
};
struct Edge
{
    Node* nodeA = nullptr;
    Node* nodeB = nullptr;
    int edgeId = -1;
    BestTracker bestTracker;
    char letterFollowed;
};
struct Word
{
    string word;
    int score;
};
void BestTracker::add(int64_t value, Edge* otherEdge)
{
    //cout << "add: " << value << " otherEdge: " << otherEdge->edgeId << endl;
    assert(num <= maxToStore);
    for (int i = 0; i < num; i++)
    {
        if (stored[i].otherEdge == otherEdge)
        {
            stored[i].value = max(stored[i].value, value);
            return;
        }
    }
    vector<Blah> blee(begin(stored), begin(stored) + num);
    Blah newBlah;
    newBlah.value = value;
    newBlah.otherEdge = otherEdge;
    blee.push_back(newBlah);
    sort(blee.begin(), blee.end(), [](const auto& lhs,  const auto& rhs) { return lhs.value > rhs.value; });
    const int newNum = min(static_cast<int>(blee.size()), maxToStore);
    copy(blee.begin(), blee.begin() + newNum, stored);
    num = newNum;
};
struct PathValue
{
    PathValue() = default;
    PathValue(Edge* edgeA, Edge* edgeB, int64_t value)
        : edgeA{edgeA}, edgeB{edgeB}, value{value}
    {
    }
    Edge* edgeA = nullptr;
    Edge* edgeB = nullptr;
    int64_t value = 0;
};


ostream& operator<<(ostream& os, const PathValue& pathValue)
{
    os << "pathValue - value: " << pathValue.value << " edgeA: " << (pathValue.edgeA ? pathValue.edgeA->edgeId : -1) << " edgeB: " << (pathValue.edgeB ? pathValue.edgeB->edgeId : -1);
    return os;
}

#include <set> 
#include <memory> 
#include <functional> 
struct TestNode;
struct NodeData
{
    int multiplier = -1;
};
struct EdgeData
{
    char letterFollowed = '\0';
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
struct TestWord
{
    string word;
    int score = -1;
};
struct TestEdge
{
    TestNode* nodeA = nullptr;
    TestNode* nodeB = nullptr;
    EdgeData data;
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
                cerr << "createNodesWithRandomParentPreferringFromSet: " << (i + 1) << " / " << numNewNodes << endl;
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
                    cerr << "Chose leaf: " << newNodeParent->neighbours.size() << endl;
                    assert(newNodeParent->neighbours.size() <= 1);
                }
                else if (!nonPreferredSet.empty())
                {
                    // Choose a random element from nonPreferredSet as a parent.
                    cerr << " choosing non-leaf" << endl;
                    newNodeParent = chooseRandomNodeFromSet(nonPreferredSet); 
                    assert(newNodeParent->neighbours.size() > 1);
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
        vector<TestEdge*> edgeChainForNodeChain(const vector<TestNode*>& nodeChain)
        {
            vector<TestEdge*> edgeChain;
            auto nodeIter = nodeChain.begin();
            auto currentNode = *nodeIter;
            while (nodeIter + 1 != nodeChain.end())
            {
                nodeIter++;
                bool foundEdge = false;
                for (auto edge : currentNode->neighbours)
                {
                    if (edge->otherNode(currentNode) == *nodeIter)
                    {
                        foundEdge = true;
                        edgeChain.push_back(edge);
                        break;
                    }
                }

                currentNode = *nodeIter;
            }

            return edgeChain;
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

void doDFS(TestNode* node, TestNode* parent, int depth, std::function<void(TestNode* node, int depth)> onVisitNode, std::function<void(TestNode*, int depth)> onEndVisitNode = std::function<void(TestNode*, int depth)>())
{
    onVisitNode(node, depth);
    for (auto edge : node->neighbours)
    {
        auto child = edge->otherNode(node);
        if (child == parent)
            continue;

        doDFS(child, node, depth + 1, onVisitNode, onEndVisitNode);
    }
    if (onEndVisitNode)
        onEndVisitNode(node, depth);
}

void doGenericInfoDFS(TestNode* rootNode)
{
    int visitNum = 0;
    vector<TestNode*> ancestors = {nullptr};
    doDFS(rootNode, nullptr, 0, [&ancestors, &visitNum](TestNode* node, int depth) 
            { 
                node->parent = ancestors.back(); 
                node->height = depth; 
                node->visitNum = visitNum;
                visitNum++;
                ancestors.push_back(node);
            },
            [&ancestors, &visitNum](TestNode* node, int depth)
            { 
                ancestors.pop_back();
                node->endVisitNum = visitNum;
                visitNum++;
            });

}


int main(int argc, char* argv[])
{
    struct timeval time;
    gettimeofday(&time,NULL);
    srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

    auto readInt = [](){ int x; cin >> x; return x; };

    if (argc == 2)
    {
        const int maxNumNodes = 20;
        const int maxNumWords = 50;
        const int maxNumLetters = 26;
        const int maxWordScore = 500;
        const int maxMaxWordLength = 100;
        const int maxNodeMultiplier = 500;
        const int numNodes = (rand() % maxNumNodes) + 1;
        const int numLettersToUse = (rand() % maxNumLetters) + 1;
        const int numWords = (rand() % maxNumWords) + 1;
        const int maxWordLength = (rand() % maxMaxWordLength) + 1;

        cout << numNodes << endl;

        TreeGenerator treeGenerator;
        auto rootNode = treeGenerator.createNode();
        const string forcedWord = "haggis";
        auto nodeChain = treeGenerator.addNodeChain(rootNode, forcedWord.size());
        nodeChain.insert(nodeChain.begin(), rootNode);
        const auto edgeChain = treeGenerator.edgeChainForNodeChain(nodeChain);

        int forcedWordCharIndex = 0;
        for (auto edge : edgeChain)
        {
            edge->data.letterFollowed = forcedWord[forcedWordCharIndex];
            forcedWordCharIndex++;
        }
        treeGenerator.createNodesWithRandomParent(numNodes - treeGenerator.numNodes());

        for (auto node : treeGenerator.nodes())
        {
            node->data.multiplier = (rand() % maxNodeMultiplier) + 1;
        }

        for (auto edge : treeGenerator.edges())
        {
            if (edge->data.letterFollowed == '\0')
                edge->data.letterFollowed = (rand() % numLettersToUse) + 'a';
        }

        vector<TestWord> words(numWords);
        for (auto& testWord : words)
        {
            testWord.score = (rand() % maxWordScore) + 1;
            const int wordLength = (rand() % maxWordLength) + 1;
            for (int j = 0; j < wordLength; j++)
            {
                testWord.word.push_back('a' + (rand() % numLettersToUse));
            }
        }
        words.front().word = forcedWord;

        {
            treeGenerator.scrambleNodeIdsAndReorder(nullptr);
            treeGenerator.scrambleEdgeOrder();
            random_shuffle(words.begin(), words.end());
        }

        for (auto node : treeGenerator.nodes())
        {
            cout << node->data.multiplier << endl;
        }

        for (auto edge : treeGenerator.edges())
        {
            cout << edge->nodeA->id()  << " " << edge->nodeB->id() << " " << edge->data.letterFollowed << endl;
        }

        cout << numWords << endl;
        for (const auto& word : words)
        {
            cout << word.word << " " << word.score << endl;
        }
        return 0;
    }

    if (false)
    {

        const int maxNumNeighbours = 20;
        const int maxMaxValue = 100;
        const int maxPathValues = 20;

        int numTests = 0;

        while (true)
        {
            Node rootNode;
            const int numNeighbours = (rand() % (maxNumNeighbours - 1)) + 2;
            vector<Edge> edges(numNeighbours);
            int edgeId = 0;
            for (auto& edge : edges)
            {
                edge.nodeA = &rootNode;
                edge.edgeId = edgeId;
                edgeId++;
            }

            const int numPathValues = (rand() % maxPathValues) + 1;
            const int64_t maxValue = (rand() % maxMaxValue) + 1;
            vector<PathValue> pathValues;
            for (int i = 0; i < numPathValues; i++)
            {
                while (true)
                {
                    auto edge1 = &(edges[rand() % numNeighbours]);
                    auto edge2 = &(edges[rand() % numNeighbours]);
                    if (edge1 == edge2)
                        continue;


                    pathValues.push_back({edge1, edge2, (rand() % maxValue) + 1});
                    break;
                }
            }

            for (const auto& pathValue : pathValues)
            {
                //cout << pathValue << endl;
            }
            for (const auto& pathValue : pathValues)
            {
                pathValue.edgeA->bestTracker.add(pathValue.value, pathValue.edgeB); 
                pathValue.edgeB->bestTracker.add(pathValue.value, pathValue.edgeA); 
            }

            auto shareAnEdge = [](const PathValue& pathValue1, const PathValue& pathValue2)
            {
                return !( pathValue1.edgeA != pathValue2.edgeA && pathValue1.edgeA != pathValue2.edgeB &&
                        pathValue1.edgeB != pathValue2.edgeA && pathValue1.edgeB != pathValue2.edgeB);
            };

            int64_t dbgMaxPathValueProduct = -1;
            for (const auto& pathValue1 : pathValues)
            {
                for (const auto& pathValue2 : pathValues)
                {
                    if (!shareAnEdge(pathValue1, pathValue2))
                    {
                        const auto pathValueProduct = pathValue1.value * pathValue2.value;
                        if (pathValueProduct > dbgMaxPathValueProduct)
                        {
                            dbgMaxPathValueProduct = pathValueProduct;
                            //cout << "New best: " << pathValueProduct << " from " << pathValue1 << " and " << pathValue2 << endl;
                        }
                    }
                }
            }

            //cout << "dbgMaxPathValueProduct: " << dbgMaxPathValueProduct << endl;

            auto comparePathValues = [](const PathValue& lhs, const PathValue& rhs) 
            {
                if (lhs.value != rhs.value)
                    return lhs.value > rhs.value;
                if (lhs.edgeA != rhs.edgeA)
                    return lhs.edgeA < rhs.edgeA;
                return lhs.edgeB < rhs.edgeB;
            };
            set<PathValue, decltype(comparePathValues)> pathValuesByVal(comparePathValues);

            int64_t maxPathValueProduct = -1;
            for (auto& edge : edges)
            {
                int64_t maxFromThis = -1;
                for (auto blahIter = begin(edge.bestTracker.stored); blahIter != begin(edge.bestTracker.stored) + edge.bestTracker.num; blahIter++)
                {
                    PathValue blee(&edge, blahIter->otherEdge, blahIter->value);
                    //cout << " Blee: " << blee << endl;
                    for (const auto& otherPathValue : pathValuesByVal)
                    {
                        //cout << " otherPathValue: " << otherPathValue << endl;
                        if (!shareAnEdge(blee, otherPathValue))
                        {
                            maxFromThis = max(maxFromThis, blee.value * otherPathValue.value);
                            break;
                        }
                    }
                    maxPathValueProduct = max(maxPathValueProduct, maxFromThis);
                }
                for (auto blahIter = begin(edge.bestTracker.stored); blahIter != begin(edge.bestTracker.stored) + edge.bestTracker.num; blahIter++)
                {
                    PathValue blee(&edge, blahIter->otherEdge, blahIter->value);
                    pathValuesByVal.insert(blee);
                }
            }
            cout << "maxPathValueProduct: " << maxPathValueProduct << " dbgMaxPathValueProduct: " << dbgMaxPathValueProduct << " " << (maxPathValueProduct == dbgMaxPathValueProduct ? "MATCH" : "NOMATCH") <<  " numTests: " << numTests << endl;
            assert(maxPathValueProduct == dbgMaxPathValueProduct);
            numTests++;
        }
    }

    const int numNodes = readInt();
    vector<Node> nodes(numNodes);
    cout << "num nodes: " << numNodes << endl;

    for (auto& node : nodes)
    {
        node.multiplier = readInt();
    }


    vector<Edge> edges(numNodes - 1);
    for (auto& edge : edges)
    {
        const int node1Index = readInt() - 1;
        const int node2Index = readInt() - 1;
        cout << "node1Index: " << node1Index << " node2Index: " << node2Index << endl;
        char letter;
        cin >> letter;
        cout << "letter followed: " << letter << endl;

        auto node1 = &(nodes[node1Index]);
        auto node2 = &(nodes[node2Index]);

        edge.nodeA = node1;
        edge.nodeB = node2;
        edge.letterFollowed = letter;

        node1->neighbours.push_back(&edge);
        node2->neighbours.push_back(&edge);
    }

    const int numWords = readInt();
    vector<Word> words(numWords);
    for (auto& word : words)
    {
        cin >> word.word;
        cin >> word.score;
        cout << "word: " << word.word << " score: " << word.score << endl;
    }
}
