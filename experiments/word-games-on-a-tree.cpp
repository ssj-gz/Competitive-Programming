#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <cassert>
#include <sys/time.h>

// TODO - 
//
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
    int index = -1;
    int multiplier;
    void addElbow(Edge* edge1, Edge* edge2, int score);
    bool doNotExplore = false;

    int crossedWordsBestScore = 0;
    int singleWordBestScore = 0;
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
constexpr int BestTracker::maxToStore;
struct Edge
{
    Node* nodeA = nullptr;
    Node* nodeB = nullptr;
    int edgeId = -1;
    BestTracker bestTrackerNodeA;
    BestTracker bestTrackerNodeB;
    char letterFollowed;
    //bool remove = false;
    Node* otherNode(Node* node)
    {
        if (node == nodeA)
            return nodeB;
        return nodeA;
    }

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

void Node::addElbow(Edge* edge1, Edge* edge2, int score)
{
    assert(edge1 != nullptr || edge2 != nullptr);
    assert(edge1 == nullptr || (edge1->nodeA == this || edge1->nodeB == this));
    assert(edge2 == nullptr || (edge2->nodeA == this || edge2->nodeB == this));
    if (edge1 != nullptr)
    {
        auto& bestTracker = (edge1->nodeA == this ? edge1->bestTrackerNodeA : edge1->bestTrackerNodeB);
        bestTracker.add(score, edge2);
    }
    if (edge2 != nullptr)
    {
        auto& bestTracker = (edge2->nodeA == this ? edge2->bestTrackerNodeA : edge2->bestTrackerNodeB);
        bestTracker.add(score, edge1);
    }
}

struct WordPath
{
    Word word;
    set<Node*> nodesInPath;
};

bool operator<(const WordPath& lhs, const WordPath& rhs)
{
    if (lhs.word.word != rhs.word.word)
       return lhs.word.word < rhs.word.word; 
    if (lhs.word.score != rhs.word.score)
       return lhs.word.score < rhs.word.score; 
    return lhs.nodesInPath < rhs.nodesInPath;
}


ostream& operator<<(ostream& os, const PathValue& pathValue)
{
    os << "pathValue - value: " << pathValue.value << " edgeA: " << (pathValue.edgeA ? pathValue.edgeA->edgeId : -1) << " edgeB: " << (pathValue.edgeB ? pathValue.edgeB->edgeId : -1);
    return os;
}

int countDescendants(Node* node, Node* parentNode)
{
    //if (node->doNotExplore)
        //return 0;
    int numDescendants = 1; // Current node.

#if 0
    node->neighbours.erase(remove_if(node->neighbours.begin(), node->neighbours.end(), 
                [](Edge* edge)
                {
                    return edge->remove;
                }), node->neighbours.end());
#endif

    for (const auto& edge : node->neighbours)
    {
        auto child = edge->otherNode(node);
        if (child == parentNode)
            continue;
        //if (child->doNotExplore)
            //continue;

        numDescendants += countDescendants(child, node);
    }

    return numDescendants;
}

vector<Node*> getDescendants(Node* node, Node* parentNode)
{
    vector<Node*> descendants;
    //if (node->doNotExplore)
        //return descendants;

    descendants.push_back(node);

    for (const auto& edge : node->neighbours)
    {
        auto child = edge->otherNode(node);
        if (child == parentNode)
            continue;
        //if (child->doNotExplore)
            //continue;

        const auto& childDescendants = getDescendants(child, node);
        descendants.insert(descendants.end(), childDescendants.begin(), childDescendants.end());
    }

    return descendants;
}

int findCentroidAux(Node* currentNode, Node* parentNode, const int totalNodes, Node** centroid)
{
    //if (currentNode->doNotExplore)
        //return 0;
    int numDescendents = 1;

    bool childHasTooManyDescendants = false;


    for (const auto& edge : currentNode->neighbours)
    {
        auto child = edge->otherNode(currentNode);
        if (child == parentNode)
            continue;
        //if (child->doNotExplore)
            //continue;

        const auto numChildDescendants = findCentroidAux(child, currentNode, totalNodes, centroid);
        if (numChildDescendants > totalNodes / 2)
        {
            // Not the centroid, but can't break here - must continue processing children.
            childHasTooManyDescendants = true;
        }

        numDescendents += numChildDescendants;
    }

    if (!childHasTooManyDescendants)
    {
        // No child has more than totalNodes/2 descendants, but what about the remainder of the graph?
        const auto nonChildDescendants = totalNodes - numDescendents;
        if (nonChildDescendants <= totalNodes / 2)
        {
            assert(centroid);
            *centroid = currentNode;
        }
    }

    return numDescendents;
}

Node* findCentroid(Node* startNode)
{
    const auto totalNumNodes = countDescendants(startNode, nullptr);
    Node* centroid = nullptr;
    findCentroidAux(startNode, nullptr, totalNumNodes, &centroid);
    if (!centroid)
    {
        cout << "totalNumNodes: " << totalNumNodes << endl;
    }
    assert(centroid);
    return centroid;
}


#if 0
Node* findCentroidBruteForce(Node* startNode, int numNodes)
{
    int numInGraph = 1;
    //vector<Node*> allNodes = getDescendants(startNode, nullptr);
    //const auto numNodes = countDescendants(startNode, nullptr);

    if (numNodes == 1)
        return startNode;

    for (auto& node : allNodes)
    {
        bool isMedian = true;
        for (auto& neighbour : node->neighbours)
        {
            if (getDescendants(neighbour, node).size() > numNodes / 2)
            {
                isMedian = false;
                break;
            }
        }
        if (isMedian)
            return node;

    }

    assert(false);
    return nullptr;
}
#endif

int numNodesTotal = 0;
void decompose(Node* startNode, vector<vector<int>>& blee, int indentLevel = 0)
{
    //if (startNode->doNotExplore)
        //return;
    auto countPair = [&blee](Node* node1, Node* node2)
    {
        assert(node1 != node2);
        if (node1->index > node2->index)
            swap(node1, node2);
        blee[node1->index][node2->index]++;
        blee[node2->index][node1->index]++;
    };
    const string indent(indentLevel, ' ');
    //cout << indent << "Decomposing graph containing " << startNode->index << endl;
    const auto numNodes = countDescendants(startNode, nullptr);
    assert(numNodes > 0);
    Node* centroid = findCentroid(startNode);
    //cout << indent << " centroid: " << centroid->index << " num nodes: " << numNodes << endl;
    //cout << " indentLevel: " << indentLevel << " numNodes: " << numNodes << endl;

    numNodesTotal += numNodes;
    //cout << "numNodesTotal: " << numNodesTotal << endl;

    //if (centroid->doNotExplore)
        //return;
    //centroid->doNotExplore = true;
    for (auto& edge : centroid->neighbours)
    {
        //edge->remove = true;
        auto neighbour = edge->otherNode(centroid);
        auto thisEdge = find(neighbour->neighbours.begin(), neighbour->neighbours.end(), edge);
        assert(thisEdge != neighbour->neighbours.end());
        //cout << "Fleep: " << (thisEdge - neighbour->neighbours.begin()) << endl;
        neighbour->neighbours.erase(thisEdge);
    }
    //centroid->neighbours.clear();

    vector<Node*> descendantsSoFar;
    descendantsSoFar.push_back(centroid);
    for (auto& edge : centroid->neighbours)
    {
        auto neighbour = edge->otherNode(centroid);
        //neighbour->neighbours.erase(find(neighbour->neighbours.begin(), neighbour->neighbours.end(), edge));
        auto newDescendants = getDescendants(neighbour, centroid);
        assert(newDescendants.size() <= numNodes / 2);
#if 1
        for (const auto& descendant : newDescendants)
        {
            for (const auto& oldDescendant : descendantsSoFar)
            {
                countPair(descendant, oldDescendant);
            }
        }
        descendantsSoFar.insert(descendantsSoFar.end(), newDescendants.begin(), newDescendants.end());
#endif
        decompose(neighbour, blee, indentLevel + 1);
    }

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

void findWordPathsFrom(Node* currentNode, vector<Edge*>& edgesFollowedSoFar, vector<Node*>& nodesFollowedSoFar, string& wordFollowedSoFar, const vector<Word>& words, set<WordPath>& wordPaths)
{
    for (const auto& word : words)
    {
        if (wordFollowedSoFar == word.word)
        {
            cout << "Found word: " << word.word << endl;
            set<Node*> nodesInPath = {currentNode};
            for (auto node : nodesFollowedSoFar)
            {
                nodesInPath.insert(node);
            }
            WordPath wordPath;
            wordPath.nodesInPath = nodesInPath;
            wordPath.word = word;
            wordPaths.insert(wordPath);
            assert(wordPath.nodesInPath.size() == word.word.size() + 1);
#if 0
            nodesFollowedSoFar.push_back(currentNode);
            Edge* previousEdge = nullptr;
            assert(nodesFollowedSoFar.size() == edgesFollowedSoFar.size() + 1);
            for (int i = 0; i < word.word.size(); i++)
            {
                auto thisEdge = (i == edgesFollowedSoFar.size() ? nullptr : edgesFollowedSoFar[i]);
                nodesFollowedSoFar[i]->addElbow(previousEdge, thisEdge, word.score);

                previousEdge = thisEdge;
            }
            nodesFollowedSoFar.pop_back();
#endif

        }
    }
    for (auto edge : currentNode->neighbours)
    {
        if (!edgesFollowedSoFar.empty() && edgesFollowedSoFar.back() == edge)
            continue;

        auto childNode = edge->otherNode(currentNode);
        edgesFollowedSoFar.push_back(edge);
        nodesFollowedSoFar.push_back(currentNode);
        wordFollowedSoFar.push_back(edge->letterFollowed);

        findWordPathsFrom(childNode, edgesFollowedSoFar, nodesFollowedSoFar, wordFollowedSoFar, words, wordPaths);

        wordFollowedSoFar.pop_back();
        nodesFollowedSoFar.pop_back();
        edgesFollowedSoFar.pop_back();

    }
}

set<WordPath> findWordPathsFrom(Node* currentNode, const vector<Word>& words, set<WordPath>& wordPaths)
{
    vector<Edge*> edgesFollowedSoFar; 
    vector<Node*> nodesFollowedSoFar; 
    string wordFollowedSoFar;
    findWordPathsFrom(currentNode, edgesFollowedSoFar, nodesFollowedSoFar, wordFollowedSoFar, words, wordPaths);
    return wordPaths;
}

vector<int> findNodeScoresBruteForce(vector<Node>& nodes, const vector<Word>& words)
{
    vector<int> nodeScores;
    set<WordPath> wordPaths;
    for (auto& node : nodes)
    {
        findWordPathsFrom(&node, words, wordPaths);
    }

    for (auto& node : nodes)
    {
        node.crossedWordsBestScore = 0;
        node.singleWordBestScore = 0;
    }

    for (const auto& p1 : wordPaths)
    {
        for (const auto& p2 : wordPaths)
        {
            vector<Node*> pathIntersection;
            set_intersection(p1.nodesInPath.begin(), p1.nodesInPath.end(),
                             p2.nodesInPath.begin(), p2.nodesInPath.end(),
                             back_inserter(pathIntersection));
            if (pathIntersection.size() == 1)
            {
                auto crossNode = pathIntersection.front();
                cout << "paths with words " << p1.word.word << " & " << p2.word.word << " intersect at node: " << crossNode->index << endl;
                crossNode->crossedWordsBestScore = max(crossNode->crossedWordsBestScore, p1.word.score * p2.word.score);

            }

        }
    }
    for (const auto& path : wordPaths)
    {
        for (auto node : path.nodesInPath)
        {
            node->singleWordBestScore = max(node->singleWordBestScore, path.word.score);
        }
    }

    for (auto& node : nodes)
    {
        int nodeScore = 0;
        if (node.crossedWordsBestScore > 0)
        {
            nodeScore = node.crossedWordsBestScore;
        }
        else
        {
            nodeScore = node.singleWordBestScore;
        }
        nodeScore *= node.multiplier;

        nodeScores.push_back(nodeScore);
        cout << "node: " << nodeScores.size() << " has score: " << nodeScores.back() << endl;
    }

    return nodeScores;
}

int main(int argc, char* argv[])
{
    struct timeval time;
    gettimeofday(&time,NULL);
    srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

    auto readInt = [](){ int x; cin >> x; return x; };

    if (argc == 2)
    {
        const int maxNumNodes = 50;
        const int maxNumWords = 50;
        const int maxNumLetters = 26;
        const int maxWordScore = 500;
        const int maxMaxWordLength = 100;
        const int maxNodeMultiplier = 500;
        const int numForcedWords = 2;
        const string forcedWord("haggis");
        const string forcedWord2("ragamuffin");
        const int numNodes = max(static_cast<int>(forcedWord.size() + forcedWord2.size()), (rand() % maxNumNodes) + 1);
        const int numLettersToUse = (rand() % maxNumLetters) + 1;
        const int numWords = max(numForcedWords, (rand() % maxNumWords) + 1);
        const int maxWordLength = (rand() % maxMaxWordLength) + 1;



        TreeGenerator treeGenerator;
        auto addWord = [&treeGenerator](const string& word, TestNode* startNode)
        {
            auto nodeChain = treeGenerator.addNodeChain(startNode, word.size());
            nodeChain.insert(nodeChain.begin(), startNode);
            const auto edgeChain = treeGenerator.edgeChainForNodeChain(nodeChain);
            int wordCharIndex = 0;
            for (auto edge : edgeChain)
            {
                edge->data.letterFollowed = word[wordCharIndex];
                wordCharIndex++;
            }
            cerr << "added word: " << word << endl;
        };
        auto rootNode = treeGenerator.createNode();
        addWord(forcedWord, rootNode);
        assert(treeGenerator.nodes().size() >= 4);
        auto crossNode = treeGenerator.nodes()[3];
        addWord(forcedWord2.substr(3), crossNode);
        addWord(string(forcedWord2.rbegin(), forcedWord2.rend()).substr(forcedWord2.size() - 3), crossNode);

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
        words[0].word = forcedWord;
        words[1].word = forcedWord2;

        {
            treeGenerator.scrambleNodeIdsAndReorder(nullptr);
            treeGenerator.scrambleEdgeOrder();
            random_shuffle(words.begin(), words.end());
        }
        cout << treeGenerator.numNodes() << endl;

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
                pathValue.edgeA->bestTrackerNodeA.add(pathValue.value, pathValue.edgeB); 
                pathValue.edgeB->bestTrackerNodeA.add(pathValue.value, pathValue.edgeA); 
            }

            auto shareAnEdge = [](const PathValue& pathValue1, const PathValue& pathValue2)
            {
                return !( pathValue1.edgeA != pathValue2.edgeA && pathValue1.edgeA != pathValue2.edgeB &&
                        pathValue1.edgeB != pathValue2.edgeA && pathValue1.edgeB != pathValue2.edgeB);
            };

            PathValue dbgBest1;
            PathValue dbgBest2;
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
                            dbgBest1 = pathValue1;
                            dbgBest2 = pathValue2;
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
            PathValue best1;
            PathValue best2;
            for (auto& edge : edges)
            {
                //int64_t maxFromThis = -1;
                for (auto blahIter = begin(edge.bestTrackerNodeA.stored); blahIter != begin(edge.bestTrackerNodeA.stored) + edge.bestTrackerNodeA.num; blahIter++)
                {
                    PathValue blee(&edge, blahIter->otherEdge, blahIter->value);
                    //cout << " Blee: " << blee << endl;
                    for (const auto& otherPathValue : pathValuesByVal)
                    {
                        //cout << " otherPathValue: " << otherPathValue << endl;
                        if (!shareAnEdge(blee, otherPathValue))
                        {
                            //maxFromThis = max(maxFromThis, );
                            const int product = blee.value * otherPathValue.value;
                            if (product > maxPathValueProduct)
                            {
                                maxPathValueProduct = product;
                                best1 = blee;
                                best2 = otherPathValue;
                                break;
                            }
                        }
                    }
                }
                for (auto blahIter = begin(edge.bestTrackerNodeA.stored); blahIter != begin(edge.bestTrackerNodeA.stored) + edge.bestTrackerNodeA.num; blahIter++)
                {
                    PathValue blee(&edge, blahIter->otherEdge, blahIter->value);
                    pathValuesByVal.insert(blee);
                }
            }
            cout << "maxPathValueProduct: " << maxPathValueProduct << " dbgMaxPathValueProduct: " << dbgMaxPathValueProduct << " " << (maxPathValueProduct == dbgMaxPathValueProduct ? "MATCH" : "NOMATCH") <<  " numTests: " << numTests << endl;
            if (maxPathValueProduct != dbgMaxPathValueProduct)
            {
                cout << "Mismatch found!" << endl;
                cout << "best1: " << best1 << " best2: " << best2 << " dbgBest1: " << dbgBest1 << " dbgBest2: " << dbgBest2 << endl;
                cout << "numNeighbours: " << numNeighbours << " numPathValues: " << numPathValues << " path values: " << endl;
                for (const auto& pathValue : pathValues)
                {
                    cout << " " << pathValue << endl;
                }
            }
            assert(maxPathValueProduct == dbgMaxPathValueProduct);
            numTests++;
        }
    }

    if (true)
    {
        const int maxNodes = 10000;
        const int numNodes = (rand() % (maxNodes - 1)) + 2;
        //const int numNodes = 20000;
        vector<Node> nodes(numNodes);
        vector<Edge> edges(numNodes - 1);

        int nodeIndex = 0;
        for (auto& node : nodes)
        {
            node.index = nodeIndex;
            if (nodeIndex != 0)
            {
                const int neighbourNodeIndex = rand() % nodeIndex;
                auto neighbourNode = &(nodes[neighbourNodeIndex]);
                auto newNode = &(nodes[nodeIndex]);

                Edge* edge = &(edges[nodeIndex - 1]);
                edge->nodeA = newNode;
                edge->nodeB = neighbourNode;

                newNode->neighbours.push_back(edge);
                neighbourNode->neighbours.push_back(edge);

            }

            nodeIndex++;

        }

#if 0
        cout << "Tree: " << endl;
        for (const auto& node : nodes)
        {
            cout << "Node id: " << node.index << " neighbour ids: ";
            for (const auto& neighbour : node.neighbours)
            {
                cout << neighbour->index << " ";
            }
            cout << endl;
        }
#endif

        vector<vector<int>> blee(numNodes, vector<int>(numNodes, 0));

        decompose(&(nodes.front()), blee);

#if 0
        for (int i = 0; i < numNodes; i++)
        {
            for (int j = 0; j < numNodes; j++)
            {
                if (i == j)
                    continue;
                //cout << "i: " << i << " j: " << j << " blee: " << blee[i][j] << endl;
                assert(blee[i][j] == 1);
            }
        }
#endif
        cout << "numNodes: " << numNodes << " numNodesTotal: " << numNodesTotal << endl;
        return 0;
    }

    const int numNodes = readInt();
    vector<Node> nodes(numNodes);
    cout << "num nodes: " << numNodes << endl;

    int nodeIndex = 1;
    for (auto& node : nodes)
    {
        node.multiplier = readInt();
        node.index = nodeIndex;
        nodeIndex++;
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

    const auto bruteForceNodeScores = findNodeScoresBruteForce(nodes, words);
}
