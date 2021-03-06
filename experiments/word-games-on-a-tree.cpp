#define BRUTE_FORCE
//#define SUBMISSION
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <functional>
#include <cassert>
#include <sys/time.h>

using namespace std;

const auto maxK = 50;
constexpr auto maxToStore = 3;

struct Edge;
struct Node
{
    vector<Edge*> neighbours;
    int index = -1;
    int64_t multiplier;
    void addElbow(Edge* edge1, Edge* edge2, int64_t score);

    int64_t crossedWordsBestScore = 0;
    int64_t singleWordBestScore = 0;
    int64_t score = 0;

    int64_t crossedWordsBestScoreBruteForce = 0;
    int64_t singleWordBestScoreBruteForce = 0;

    vector<Edge*> originalNeighbours;
};
struct BestTracker
{
    void add(int64_t value, Edge* otherEdge);
    int num = 0;
    //int64_t lowestValue = numeric_limits<int64_t>::min();
    int64_t lowestValue = 0;
    struct Blah
    {
        int64_t value = -1;
        Edge* otherEdge = nullptr;
    };
    Blah stored[maxToStore + 1]; // "+ 1" as we temporarily add one more than maxToStore.
};
struct Edge
{
    Node* nodeA = nullptr;
    Node* nodeB = nullptr;
    int edgeId = -1;
    BestTracker& bestTrackerForNode(Node* node)
    {
        if (node == nodeA)
            return bestTrackerNodeA;
        assert(node == nodeB);
        return bestTrackerNodeB;
    }
    char letterFollowed;
    Node* otherNode(Node* node)
    {
        if (node == nodeA)
            return nodeB;
        return nodeA;
    }
    private:
        BestTracker bestTrackerNodeA;
        BestTracker bestTrackerNodeB;

};
struct Word
{
    string word;
    int64_t score;
};
void BestTracker::add(int64_t value, Edge* otherEdge)
{
    assert(num <= maxToStore);
    if (value < lowestValue && num == 3)
        return;
    for (auto i = 0; i < num; i++)
    {
        if (stored[i].otherEdge == otherEdge)
        {
            stored[i].value = max(stored[i].value, value);
            return;
        }
    }
    stored[num].value = value;
    stored[num].otherEdge = otherEdge;
    num++;
    sort(stored, stored + num, [](const auto& lhs,  const auto& rhs) { return lhs.value > rhs.value; });
    num = min(num, maxToStore);
    assert(num > 0);
    lowestValue = numeric_limits<int64_t>::max();
    for (int i = 0; i < num; i++)
    {
        if (stored[i].value < lowestValue)
            lowestValue = stored[i].value;
    }
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

int64_t bestCrosswordScore(Node* node)
{
    //cout << "bestCrosswordScore: " << node->index << endl; 
    auto comparePathValues = [](const PathValue& lhs, const PathValue& rhs) 
    {
        if (lhs.value != rhs.value)
            return lhs.value > rhs.value;
        if (lhs.edgeA != rhs.edgeA)
            return lhs.edgeA < rhs.edgeA;
        return lhs.edgeB < rhs.edgeB;
    };
    auto shareAnEdge = [](const PathValue& pathValue1, const PathValue& pathValue2)
    {
        if (pathValue1.edgeA != nullptr && (pathValue2.edgeA == pathValue1.edgeA || pathValue2.edgeB == pathValue1.edgeA))
                return true;
        if (pathValue1.edgeB != nullptr && (pathValue2.edgeA == pathValue1.edgeB || pathValue2.edgeB == pathValue1.edgeB))
                return true;
        if (pathValue2.edgeA != nullptr && (pathValue1.edgeA == pathValue2.edgeA || pathValue1.edgeB == pathValue2.edgeA))
                return true;
        if (pathValue2.edgeB != nullptr && (pathValue1.edgeA == pathValue2.edgeB || pathValue1.edgeB == pathValue2.edgeB))
                return true;
        return false;
    };
    set<PathValue, decltype(comparePathValues)> pathValuesByVal(comparePathValues);

    int64_t maxPathValueProduct = 0;
    for (auto& edge : node->neighbours)
    {
        BestTracker& bestTrackerForEdgeAndNode = edge->bestTrackerForNode(node);
        for (auto blahIter = begin(bestTrackerForEdgeAndNode.stored); blahIter != begin(bestTrackerForEdgeAndNode.stored) + bestTrackerForEdgeAndNode.num; blahIter++)
        {
            PathValue pathValue(edge, blahIter->otherEdge, blahIter->value);
            for (const auto& otherPathValue : pathValuesByVal)
            {
                if (!shareAnEdge(pathValue, otherPathValue))
                {
                    const auto product = pathValue.value * otherPathValue.value;
                    if (product > maxPathValueProduct)
                    {
                        maxPathValueProduct = product;
                        break;
                    }
                }
            }
        }
        for (auto blahIter = begin(bestTrackerForEdgeAndNode.stored); blahIter != begin(bestTrackerForEdgeAndNode.stored) + bestTrackerForEdgeAndNode.num; blahIter++)
        {
            PathValue pathValue(edge, blahIter->otherEdge, blahIter->value);
            pathValuesByVal.insert(pathValue);
        }
    }
    return maxPathValueProduct;
}

void Node::addElbow(Edge* edge1, Edge* edge2, int64_t score)
{
    singleWordBestScore = max(singleWordBestScore, score);
    //cout << "Add elbow to node: " << index << " edge1: " << (edge1 != nullptr ? edge1->edgeId : -1) << " edge2: " << (edge2 != nullptr ? edge2->edgeId : -1) << " score: " << score << endl;

    assert(edge1 != nullptr || edge2 != nullptr);
    assert(edge1 == nullptr || (edge1->nodeA == this || edge1->nodeB == this));
    assert(edge2 == nullptr || (edge2->nodeA == this || edge2->nodeB == this));
    if (edge1 != nullptr)
        edge1->bestTrackerForNode(this).add(score, edge2);
    if (edge2 != nullptr)
        edge2->bestTrackerForNode(this).add(score, edge1);
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

int countDescendants(Node* node, Node* parentNode)
{
    int numDescendants = 1; // Current node.

    for (const auto& edge : node->neighbours)
    {
        auto child = edge->otherNode(node);
        if (child == parentNode)
            continue;

        numDescendants += countDescendants(child, node);
    }

    return numDescendants;
}

int findCentroidAux(Node* currentNode, Node* parentNode, const int totalNodes, Node** centroid)
{
    int numDescendents = 1;

    bool childHasTooManyDescendants = false;

    for (const auto& edge : currentNode->neighbours)
    {
        auto child = edge->otherNode(currentNode);
        if (child == parentNode)
            continue;

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


void decompose(Node* startNode, std::function<void(Node*)> processCentroid )
{
    const auto numNodes = countDescendants(startNode, nullptr);
    assert(numNodes > 0);
    Node* centroid = findCentroid(startNode);

    assert(centroid);
    processCentroid(centroid);

    // Decompose further.
    for (auto& edge : centroid->neighbours)
    {
        auto neighbour = edge->otherNode(centroid);
        auto thisEdge = find(neighbour->neighbours.begin(), neighbour->neighbours.end(), edge);
        assert(thisEdge != neighbour->neighbours.end());
        neighbour->neighbours.erase(thisEdge);

        decompose(neighbour, processCentroid);
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

vector<int64_t> findNodeScoresBruteForce(vector<Node>& nodes, const vector<Word>& words)
{
    vector<int64_t> nodeScores;
    set<WordPath> wordPaths;
    for (auto& node : nodes)
    {
        findWordPathsFrom(&node, words, wordPaths);
    }

    for (auto& node : nodes)
    {
        node.crossedWordsBestScoreBruteForce = 0;
        node.singleWordBestScoreBruteForce = 0;
    }
    for (const auto& p1 : wordPaths)
    {
        cout << "path word: " << p1.word.word << " nodes: " << endl;
        for (auto node : p1.nodesInPath)
        {
            cout << node->index << " ";
        }
        cout << endl;
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
                crossNode->crossedWordsBestScoreBruteForce = max(crossNode->crossedWordsBestScoreBruteForce, p1.word.score * p2.word.score);

            }

        }
    }
    for (const auto& path : wordPaths)
    {
        for (auto node : path.nodesInPath)
        {
            node->singleWordBestScoreBruteForce = max(node->singleWordBestScoreBruteForce, path.word.score);
        }
    }

    for (auto& node : nodes)
    {
        int64_t nodeScore = 0;
        if (node.crossedWordsBestScoreBruteForce > 0)
        {
            nodeScore = node.crossedWordsBestScoreBruteForce;
        }
        else
        {
            nodeScore = node.singleWordBestScoreBruteForce;
        }
        nodeScore *= node.multiplier;

        nodeScores.push_back(nodeScore);
        cout << "node: " << nodeScores.size() << " has score: " << nodeScores.back() << endl;
    }

    return nodeScores;
}

// Ukkonen's Algorithm.
struct StateData
{
    int wordLength = -1;
    vector<int> wordIndicesIsFinalStateFor;
};

/**
 * Simple implementation of Ukkonen's algorithm:
 *  https://en.wikipedia.org/wiki/Ukkonen's_algorithm
 * for online construction of suffix trees.
 * @author Simon St James, Jan 2017.
 */
class SuffixTreeBuilder
{
    private:
        struct State;
        struct Substring
        {
            Substring()
                : Substring(-1, -1)
            {
            }
            Substring(int startIndex, int endIndex)
                : startIndex(startIndex), endIndex(endIndex)
            {
            }
            int length() const
            {
                assert(endIndex != openTransitionEnd);
                const auto length = endIndex - startIndex + 1;
                assert(length >= 0);
                return length;
            }
            int startIndex = -1;
            int endIndex = -1;
        };
        struct Transition
        {
            Transition(State *nextState, const Substring& substringFollowed, const string& currentString)
                : nextState(nextState), substringFollowed(substringFollowed)
            {
                if (substringFollowed.startIndex >= 1)
                    firstLetter = currentString[substringFollowed.startIndex - 1];
            }
            int substringLength() const
            {
                return substringFollowed.length();
            }

            State *nextState = nullptr;
            Substring substringFollowed;
            char firstLetter;
        };
        struct State
        {
            vector<Transition> transitions;
            State* suffixLink = nullptr;
            State* parent = nullptr;
            int index = -1;
            bool isFinal = false;

            StateData data;
        };
    public:
        SuffixTreeBuilder(const vector<Word>& words)
        {
            m_states.reserve(1'000'000);

            m_root = createNewState();
            m_root->data.wordLength = 0;
            m_auxiliaryState = createNewState();

            for (int i = 0; i < alphabetSize; i++)
            {
                m_auxiliaryState->transitions.push_back(Transition(m_root, Substring(-(i + 1), -(i + 1)), m_currentString));
            }
            m_root->suffixLink = m_auxiliaryState;

            m_s = m_root;
            m_k = 1;

            cout << "wordSeparatorCharEnd: " << wordSeparatorCharEnd << endl;


            assert(words.size() <= maxK);
            string combinedWords;
            for (int i = 0; i < words.size(); i++)
            {
                combinedWords += words[i].word + static_cast<char>(wordSeparatorCharBegin + i);
            }
            cout << "combinedWords: " << combinedWords << endl;
            appendString(combinedWords);
            vector<int> indexOfNextSeparatorChar(combinedWords.size());
            int lastSeenSeparatorIndex = -1;
            for (int i = combinedWords.size() - 1; i >= 0; i--)
            {
                //cout << " i: " << i << " char: " << combinedWords[i] << " bloo: " << (combinedWords[i] >= wordSeparatorCharBegin) << " blee: " << (static_cast<int>(combinedWords[i]) <= static_cast<int>(wordSeparatorCharEnd)) << " glarp: " << static_cast<int>(wordSeparatorCharEnd) << endl;

                if (combinedWords[i] >= wordSeparatorCharBegin && combinedWords[i] <= wordSeparatorCharEnd)
                {
                    lastSeenSeparatorIndex = i;
                    cout << "Seen separator :" << lastSeenSeparatorIndex << "(" << combinedWords[i] << ")" << endl;
                }
                indexOfNextSeparatorChar[i] = lastSeenSeparatorIndex;
            }
            stripWordSeparatorsAndStoreWordEndStates(m_root, indexOfNextSeparatorChar);

            for (auto state : m_states)
            {
                //cout << " state: " << &state << " # is final for: " << state.data.wordIndicesIsFinalStateFor.size() << endl;
            }
        }
        void stripWordSeparatorsAndStoreWordEndStates(State* state, const vector<int>& indexOfNextSeparatorChar)
        {
            auto transitionIter = state->transitions.begin();
            while (transitionIter != state->transitions.end())
            {
                auto& substringFollowed = transitionIter->substringFollowed;
                assert(substringFollowed.startIndex >= 1);
                substringFollowed.startIndex--;
                if (substringFollowed.endIndex == openTransitionEnd)
                    substringFollowed.endIndex = m_currentString.size() - 1;
                else
                    substringFollowed.endIndex--;

                const int nextSeparatorCharIndex = indexOfNextSeparatorChar[substringFollowed.startIndex];

                //cout << " startIndex: " << substringFollowed.startIndex << " endIndex: " << substringFollowed.endIndex << " nextSeparatorCharIndex: " << nextSeparatorCharIndex << " char: " << (nextSeparatorCharIndex != -1 ? m_currentString[nextSeparatorCharIndex] : '-') << endl;

                if (nextSeparatorCharIndex == substringFollowed.startIndex)
                {
                    transitionIter = state->transitions.erase(transitionIter);
                    if (state != m_root)
                    {
                        state->data.wordIndicesIsFinalStateFor.push_back(m_currentString[nextSeparatorCharIndex] - wordSeparatorCharBegin);
                        //cout << "state " << state << " is final (erased transition) for: " << state->data.wordIndicesIsFinalStateFor.back() << endl;
                    }
                    continue;
                }
                if (nextSeparatorCharIndex >= substringFollowed.startIndex && nextSeparatorCharIndex <= substringFollowed.endIndex)
                {
                    substringFollowed.endIndex = nextSeparatorCharIndex - 1;
                    transitionIter->nextState->transitions.clear();
                    //cout << "Cleared transitions for " << transitionIter->nextState << endl;
                    //if (state != m_root)
                    {
                        transitionIter->nextState->data.wordIndicesIsFinalStateFor.push_back(m_currentString[nextSeparatorCharIndex] - wordSeparatorCharBegin);
                        //cout << "state " << transitionIter->nextState << " is final (next state) for: " << transitionIter->nextState->data.wordIndicesIsFinalStateFor.back() << endl;
                    }
                }

                stripWordSeparatorsAndStoreWordEndStates(transitionIter->nextState, indexOfNextSeparatorChar);

                transitionIter++;
            }

        }
        SuffixTreeBuilder(const SuffixTreeBuilder& other) = delete;
        void appendLetter(char letter)
        {
            m_currentString += letter;
            const auto updateResult = update(m_s, m_k, m_currentString.size());
            m_s = updateResult.first;
            m_k = updateResult.second;
            const auto canonizeResult = canonize(m_s, m_k, m_currentString.size());
            m_s = canonizeResult.first;
            m_k = canonizeResult.second;
        }
        void appendString(const string& stringToAppend)
        {
            for (auto letter : stringToAppend)
            {
                appendLetter(letter);
            }
        }
        int numStates() const
        {
            return m_states.size();
        }
        /**
         * Class used to navigate the suffix tree.  Can be invalidated by making changes to the tree!
         */
        class Cursor
        {
            public:
                Cursor() = default;
                Cursor(const Cursor& other) = default;
                bool operator==(const Cursor& other) const
                {
                    if (m_state != other.m_state)
                        return false;
                    if (m_transition != other.m_transition)
                        return false;
                    if (m_posInTransition != other.m_posInTransition)
                        return false;
                    return true;
                }
                bool operator!=(const Cursor& other) const
                {
                    return !(*this == other);
                }
                bool operator<(const Cursor& other) const
                {
                    if (m_state < other.m_state)
                        return true;
                    if (m_state > other.m_state)
                        return false;
                    if (m_transition < other.m_transition)
                        return true;
                    if (m_transition > other.m_transition)
                        return false;
                    if (m_posInTransition < other.m_posInTransition)
                        return true;
                    if (m_posInTransition > other.m_posInTransition)
                        return false;
                    return false;
                }
                bool isValid() const
                {
                    return m_isValid;
                }
                bool isOnExplicitState() const
                {
                    return (m_transition == nullptr);
                }
                bool isOnFinalState() const
                {
                    assert(isOnExplicitState());
                    return m_state->isFinal;
                }
                StateData& stateData()
                {
                    //cout << "Asking for state data for state: " << m_state << endl;
                    return m_state->data;
                }
                int stateId() const
                {
                    const int stateId = m_state->index;
                    return stateId;
                }
                int posInTransition() const
                {
                    assert(!isOnExplicitState());
                    return m_posInTransition;
                }
                vector<char> nextLetters() const
                { char nextLetters[27];
                    const int numNextLetters = this->nextLetters(nextLetters);
                    return vector<char>(nextLetters, nextLetters + numNextLetters);
                }
                int nextLetters(char* dest) const
                {
                    int numNextLetters = 0;
                    if (m_transition == nullptr)
                    {
                        for (const auto& transition : m_state->transitions)
                        {
                            const char letter = (*m_string)[transition.substringFollowed.startIndex];
                            *dest = letter;
                            dest++;
                            numNextLetters++;
                        }
                    }
                    else
                    {
                        *dest = (*m_string)[m_transition->substringFollowed.startIndex + m_posInTransition];
                        dest++;
                        numNextLetters++;
                    }
                    return numNextLetters;
                }

                bool canFollowLetter(char letter) const
                {
                    if (isOnExplicitState())
                    {
                        for (const auto& transition : m_state->transitions)
                        {
                            if (transition.firstLetter == letter)
                                return true;
                        }
                        return false;
                    }
                    else
                    {
                        char nextLetter;
                        nextLetters(&nextLetter);
                        return nextLetter == letter;

                    }
                }

                void followLetter(char letter)
                {
                    const string& theString = *m_string;
                    if (m_transition == nullptr)
                    {
                        for (auto& transition : m_state->transitions)
                        {
                            assert(transition.substringFollowed.startIndex >= 0);
                            {
                                assert(theString[transition.substringFollowed.startIndex] == transition.firstLetter);
                                if (transition.firstLetter == letter)
                                {
                                    m_transition = &transition;
                                    break;
                                }
                            }
                        }
                        assert(m_transition);
                        //cout << "followLetter: substringLength: " << m_transition->substringLength(m_string->size()) << endl;
                        if (m_transition->substringLength() == 1)
                        {
                            followToTransitionEnd();
                        }
                        else
                        {
                            m_posInTransition = 1; // We've just followed the 0th.
                        }
                    }
                    else
                    {
                        assert(m_posInTransition != -1);
                        const int transitionStringLength = m_transition->substringLength();
                        assert(m_posInTransition <= transitionStringLength);
                        m_posInTransition++;
                        if (m_posInTransition == transitionStringLength)
                        {
                            m_state = m_transition->nextState;
                            movedToExplicitState();
                        }

                    }
                }
                char moveUp()
                {
                    if (m_transition)
                    {
                        assert(m_posInTransition > 0);
                        const char charFollowed = (*m_string)[m_transition->substringFollowed.startIndex - 1 + m_posInTransition - 1];
                        if (m_posInTransition != 1)
                        {
                            m_posInTransition--;
                        }
                        else
                        {
                            movedToExplicitState();
                        }
                        return charFollowed;
                    }
                    else
                    {
                        Transition* transitionFromParent = findTransitionFromParent();
                        m_state = m_state->parent;
                        m_transition = transitionFromParent;
                        m_posInTransition = transitionFromParent->substringLength() - 1;
                        const char charFollowed = (*m_string)[m_transition->substringFollowed.startIndex - 1 + m_posInTransition];
                        if (m_posInTransition == 0)
                        {
                            movedToExplicitState();
                        }
                        return charFollowed;
                    }
                }

                void followNextLetter()
                {
                    followNextLetters(1);
                }
                void followNextLetters(int numLetters)
                {
                    assert(m_transition);
                    assert(remainderOfCurrentTransition().length() >= numLetters);
                    m_posInTransition += numLetters;
                    const int transitionStringLength = m_transition->substringLength();
                    if (m_posInTransition == transitionStringLength)
                    {
                        m_state = m_transition->nextState;
                        movedToExplicitState();
                    }
                }
                class Substring
                {
                    public:
                        Substring()
                        {
                        }
                        Substring(const Substring& other) = default;
                        Substring(int startIndex, int endIndex)
                            : m_startIndex{startIndex}, m_endIndex{endIndex}
                        {
                        }
                        int startIndex() const {
                            return m_startIndex;
                        }
                        int endIndex() const
                        {
                            return m_endIndex;
                        }
                        int length() const
                        {
                            return m_endIndex - m_startIndex + 1;
                        }
                    private:
                        int m_startIndex = -1;
                        int m_endIndex = -1;
                };
                Substring remainderOfCurrentTransition()
                {
                    assert(!isOnExplicitState());
                    auto startIndex = m_transition->substringFollowed.startIndex - 1 + m_posInTransition;
                    auto endIndex = (m_transition->substringFollowed.endIndex == openTransitionEnd ? static_cast<int>(m_string->size() - 1) : m_transition->substringFollowed.endIndex - 1);

                    return {startIndex, endIndex};
                }
                void followToTransitionEnd()
                {
                    assert(m_transition);
                    m_state = m_transition->nextState;
                    movedToExplicitState();
                } 
                State* state()
                {
                    return m_state;
                }
            private:
                Cursor(State* state, const string& str, State* root)
                    : m_state{state}, 
                    m_string{&str},
                    m_root{root},
                    m_isValid{true}
                {
                }

                void movedToExplicitState()
                {
                    assert(m_state);
                    m_transition = nullptr;
                    m_posInTransition = -1;
                }

                void enterTransitionAndSkipLetter(Transition& transition)
                {
                    m_transition = &transition;
                    if (m_transition->substringLength() == 1)
                    {
                        followToTransitionEnd();
                    }
                    else
                    {
                        m_posInTransition = 1; // We've just followed the 0th.
                    }
                }

                Transition* findTransitionFromParent(State* state = nullptr)
                {
                    if (!state)
                        state = m_state;
                    Transition* transitionFromParent = nullptr;
                    for (Transition& transition : state->parent->transitions)
                    {
                        if (transition.nextState == state)
                        {
                            transitionFromParent = &transition;
                            break;
                        }
                    }
                    assert(transitionFromParent);
                    return transitionFromParent;
                }

                friend class SuffixTreeBuilder;
                State *m_state = nullptr;
                Transition *m_transition = nullptr;
                int m_posInTransition = -1;
                const string* m_string = nullptr;
                State *m_root = nullptr;
                bool m_isValid = false;
        };
        Cursor rootCursor() const
        {
            return Cursor(m_root, m_currentString, m_root);
        }
        static Cursor invalidCursor()
        {
            return Cursor();
        }
    private:
        static const int alphabetSize = 26 + maxK + 1; // Include the magic "separator" characters for putting up to maxK words in suffix tree.
        const char wordSeparatorCharBegin = 'a' - 1 - maxK;
        const char wordSeparatorCharEnd = wordSeparatorCharBegin + maxK;
        static const int openTransitionEnd = numeric_limits<int>::max();

        string m_currentString;

        vector<State> m_states;
        State *m_root = nullptr;
        State *m_auxiliaryState = nullptr;

        // "Persistent" versions of s & k from Algorithm 2 in Ukkonen's paper!
        State *m_s; 
        int m_k;


        std::pair<State*, int> update(State* s, int k, int i)
        {
            State* oldr = m_root;
            const auto testAndSplitResult = testAndSplit(s, k, i - 1, t(i));
            auto isEndPoint = testAndSplitResult.first;
            auto r = testAndSplitResult.second;
            //cout << "r: " << r << endl;
            while (!isEndPoint)
            {
                auto rPrime = createNewState(r);
                r->transitions.push_back(Transition(rPrime, Substring(i, openTransitionEnd), m_currentString));
                if (oldr != m_root)
                {
                    oldr->suffixLink = r;
                }
                oldr = r;
                const auto canonizeResult = canonize(s->suffixLink, k, i - 1);
                s = canonizeResult.first;
                k = canonizeResult.second;

                const auto testAndSplitResult = testAndSplit(s, k, i - 1, t(i));
                isEndPoint = testAndSplitResult.first;
                r = testAndSplitResult.second;
            }

            if (oldr != m_root)
            {
                oldr->suffixLink = s;
            }
            return {s, k};


        }
        pair<bool, State*> testAndSplit(State* s, int k, int p, int letterIndex)
        {
            assert(s);
            if (k <= p)
            {
                const auto tkTransitionIter = findTransitionIter(s, t(k));
                auto sPrime = tkTransitionIter->nextState;
                auto kPrime = tkTransitionIter->substringFollowed.startIndex;
                auto pPrime = tkTransitionIter->substringFollowed.endIndex;
                if (letterIndex == t(kPrime + p - k + 1))
                    return {true, s};
                else
                {
                    s->transitions.erase(tkTransitionIter);
                    auto r = createNewState(s);
                    r->data.wordLength = s->data.wordLength + p - k + 1;
                    s->transitions.push_back(Transition(r, Substring(kPrime, kPrime + p - k), m_currentString));
                    r->transitions.push_back(Transition(sPrime, Substring(kPrime + p - k + 1, pPrime), m_currentString));
                    sPrime->parent = r;
                    return {false, r};
                }
            }
            else
            {
                auto tTransitionIter = findTransitionIter(s, letterIndex, false);
                if (tTransitionIter == s->transitions.end())
                    return {false, s};
                else
                    return {true, s};
            }
        }
        std::pair<State*, int> canonize(State* s, int k, int p)
        {
            assert(s);
            if (p < k)
                return {s, k};
            else
            {
                auto tkTransitionIter = findTransitionIter(s, t(k));
                auto sPrime = tkTransitionIter->nextState;
                auto kPrime = tkTransitionIter->substringFollowed.startIndex;
                auto pPrime = tkTransitionIter->substringFollowed.endIndex;
                while (pPrime - kPrime <= p - k)
                {
                    k = k + pPrime - kPrime + 1;
                    s = sPrime;
                    if (k <= p)
                    {
                        tkTransitionIter = findTransitionIter(s, t(k));
                        sPrime = tkTransitionIter->nextState;
                        kPrime = tkTransitionIter->substringFollowed.startIndex;
                        pPrime = tkTransitionIter->substringFollowed.endIndex;
                    }
                }
            }
            return {s, k};
        }
        State *createNewState(State* parent = nullptr)
        {
            m_states.push_back(State());
            State *newState = &(m_states.back());
            newState->parent = parent;
            newState->index = m_states.size() - 1;
            return newState;
        }
        decltype(State::transitions.begin()) findTransitionIter(State* state, int letterIndex, bool assertFound = true)
        {
            for (auto transitionIter = state->transitions.begin(); transitionIter != state->transitions.end(); transitionIter++)
            {
                if (transitionIter->substringFollowed.startIndex >= 0 && t(transitionIter->substringFollowed.startIndex) == letterIndex)
                    return transitionIter;

                if (transitionIter->substringFollowed.startIndex == -letterIndex)
                    return transitionIter;
            }
            if (assertFound)
                assert(false);
            return state->transitions.end();
        };
        int t(int i)
        {
            // Ukkonen's algorithm uses 1-indexed strings throughout and alphabet throughout; adjust for this.
            return m_currentString[i - 1] - wordSeparatorCharBegin + 1;
        }

};

using Cursor = SuffixTreeBuilder::Cursor;

Node* currentCentroid = nullptr;
Edge* firstEdgeFromCentroid = nullptr;
vector<Word> words;

class SuffixTracker
{
    public:
    SuffixTracker(const vector<Word>& words)
    {
        m_wasSuffixForWordBeginningAtFound.resize(words.size());
        for (int wordIndex = 0; wordIndex < words.size(); wordIndex++)
        {
            m_wasSuffixForWordBeginningAtFound[wordIndex].clear();
            m_wasSuffixForWordBeginningAtFound[wordIndex].resize(words[wordIndex].word.size() 
                    // "+ 1" allows us to deal with "empty" suffix, which is always (trivially) found.
                    + 1);
        }
    }
    bool wasSuffixForWordBeginningAtFound(int wordIndex, int suffixBeginPos)
    {
        if (suffixBeginPos == words[wordIndex].word.length()) // Yes - the empty suffix!
            return true;
        return !value(wordIndex, suffixBeginPos).empty();
    }
    const vector<Edge*>& firstEdgesFromCentroidForFoundSuffix(int wordIndex, int suffixBeginPos)
    {
        assert(wasSuffixForWordBeginningAtFound(wordIndex, suffixBeginPos));
        return value(wordIndex, suffixBeginPos);

    }
    void setSuffixForWordBeginningAtFound(int wordIndex, int suffixBeginPos, Edge* firstEdgeFromCentroid)
    {
        auto& firstEdgesFromCentroid = value(wordIndex, suffixBeginPos);
        if (firstEdgesFromCentroid.size() >= maxToStore)
            return;
        firstEdgesFromCentroid.push_back(firstEdgeFromCentroid);
    }
    void clear()
    {
        m_versionNumber++;
    }
    private:
        struct VersionedValue
        {
            int versionNumber = 0;
            vector<Edge*> firstEdgesFromCentroid;
        };
        vector<vector<VersionedValue>> m_wasSuffixForWordBeginningAtFound;
        int m_versionNumber = 0;
        vector<Edge*>& value(int wordIndex, int suffixBeginPos)
        {
            auto& versionedValue = m_wasSuffixForWordBeginningAtFound[wordIndex][suffixBeginPos];
            if (versionedValue.versionNumber != m_versionNumber)
            {
                versionedValue.firstEdgesFromCentroid.clear();
                versionedValue.versionNumber = m_versionNumber;
            }
            return versionedValue.firstEdgesFromCentroid;
        }
};

void findAndLogSuffixes(Node* node, Node* parent, int depth, string& wordFollowed, Cursor cursor, SuffixTreeBuilder& wordSuffixes, const vector<Word>& words, SuffixTracker& suffixTracker)
{
    if (cursor.isOnExplicitState())
    {
        for (auto wordIndex : cursor.stateData().wordIndicesIsFinalStateFor)
        {
            //cout << "Found suffix of word: " << words[wordIndex].word << " length: " << depth << " wordFollowed: " << wordFollowed << endl;
            const auto suffixBeginPos = words[wordIndex].word.size() - depth;
            suffixTracker.setSuffixForWordBeginningAtFound(wordIndex, suffixBeginPos, firstEdgeFromCentroid);
        }

    }
    for (auto childEdge : node->neighbours)
    {
        auto child = childEdge->otherNode(node);
        if (child == parent)
            continue;

        const auto childEdgeLetter = childEdge->letterFollowed;
        if (cursor.canFollowLetter(childEdgeLetter))
        {
            cursor.followLetter(childEdgeLetter);
            wordFollowed += childEdgeLetter;
            findAndLogSuffixes(child, node, depth + 1, wordFollowed, cursor, wordSuffixes, words, suffixTracker);
            wordFollowed.pop_back();

            cursor.moveUp();
        }
    }

}

int64_t findPrefixes(Node* node, Node* parent, Edge* parentEdge, int depth, string& wordFollowed, Cursor cursor, SuffixTreeBuilder& reversedWordPrefixes, SuffixTracker& suffixTracker)
{
    int64_t maxScore = 0;
    if (cursor.isOnExplicitState())
    {
        for (auto wordIndex : cursor.stateData().wordIndicesIsFinalStateFor)
        {
            if (suffixTracker.wasSuffixForWordBeginningAtFound(wordIndex, depth))
            {
                const int64_t completeWordScore = words[wordIndex].score;
                maxScore = max(maxScore, completeWordScore);
                if (wordFollowed.size() == words[wordIndex].word.length())
                    currentCentroid->addElbow(firstEdgeFromCentroid, nullptr, completeWordScore);


                node->addElbow(parentEdge, nullptr, completeWordScore);

                for (const auto& firstEdgeFromCentroidForMatchingSuffix : suffixTracker.firstEdgesFromCentroidForFoundSuffix(wordIndex, depth))
                    currentCentroid->addElbow(firstEdgeFromCentroid, firstEdgeFromCentroidForMatchingSuffix, completeWordScore);
            }
        }

    }
    for (auto childEdge : node->neighbours)
    {
        auto child = childEdge->otherNode(node);
        if (child == parent)
            continue;

        const auto childEdgeLetter = childEdge->letterFollowed;
        if (cursor.canFollowLetter(childEdgeLetter))
        {
            cursor.followLetter(childEdgeLetter);
            wordFollowed += childEdgeLetter;
            const int64_t childScore = findPrefixes(child, node, childEdge, depth + 1, wordFollowed, cursor, reversedWordPrefixes, suffixTracker);
            wordFollowed.pop_back();

            node->addElbow(childEdge, parentEdge, childScore);

            cursor.moveUp();

            maxScore = max(maxScore, childScore);
        }
    }

    return maxScore;
}


void findWordsCenteredAroundCentroid(Node* centroid, SuffixTreeBuilder& wordSuffixes, SuffixTreeBuilder& reversedWordPrefixes, SuffixTracker& suffixTracker)
{
    suffixTracker.clear();
    string wordFollowed;

    currentCentroid = centroid;

    for (auto childEdge : centroid->neighbours)
    {
        firstEdgeFromCentroid = childEdge;
        auto child = childEdge->otherNode(centroid);
        auto letterToFollow = childEdge->letterFollowed;
        Cursor reversedWordPrefixCursor = reversedWordPrefixes.rootCursor();
        if (reversedWordPrefixCursor.canFollowLetter(letterToFollow))
        {
            wordFollowed += letterToFollow;
            reversedWordPrefixCursor.followLetter(letterToFollow); 
            const int64_t childScore = findPrefixes(child, centroid, childEdge, 1, wordFollowed, reversedWordPrefixCursor, reversedWordPrefixes, suffixTracker);
            centroid->singleWordBestScore = max(centroid->singleWordBestScore, childScore);
            reversedWordPrefixCursor.moveUp();
            wordFollowed.clear();
        }

        Cursor suffixCursor = wordSuffixes.rootCursor();
        if (suffixCursor.canFollowLetter(letterToFollow))
        {
            wordFollowed += letterToFollow;
            suffixCursor.followLetter(letterToFollow);
            findAndLogSuffixes(child, centroid, 1, wordFollowed, suffixCursor, wordSuffixes, words, suffixTracker);
            suffixCursor.moveUp();
            wordFollowed.clear();
        }
    }
}

void processCentroid(Node* centroid, SuffixTreeBuilder& wordSuffixes, SuffixTreeBuilder& reversedWordPrefixes, SuffixTracker& suffixTracker)
{
    findWordsCenteredAroundCentroid(centroid, wordSuffixes, reversedWordPrefixes, suffixTracker);
    findWordsCenteredAroundCentroid(centroid, reversedWordPrefixes, wordSuffixes, suffixTracker);

    reverse(centroid->neighbours.begin(), centroid->neighbours.end());

    findWordsCenteredAroundCentroid(centroid, wordSuffixes, reversedWordPrefixes, suffixTracker);
    findWordsCenteredAroundCentroid(centroid, reversedWordPrefixes, wordSuffixes, suffixTracker);
}

vector<int64_t> findNodeScores(vector<Node>& nodes)
{
    vector<int64_t> nodeScores;
    SuffixTreeBuilder wordSuffixes(words);
    vector<Word> reversedWords(words);
    for (auto& word : reversedWords)
    {
        reverse(word.word.begin(), word.word.end());
    }
    SuffixTreeBuilder reversedWordPrefixes(reversedWords);

    SuffixTracker suffixTracker(words);

    auto someNode = &(nodes.front());
    decompose(someNode, [&wordSuffixes, &reversedWordPrefixes, &suffixTracker](Node* node) 
            {
                processCentroid(node, wordSuffixes, reversedWordPrefixes, suffixTracker);
            });

    // Decomposition removed the edges - restore them.
    for (auto& node : nodes)
        node.neighbours = node.originalNeighbours;

    for (auto& node : nodes)
    {
        node.crossedWordsBestScore = bestCrosswordScore(&node);
        //cout << "node: " << node.index << " singleWordBestScore: " << node.singleWordBestScore << " crossedWordsBestScore: " << node.crossedWordsBestScore << endl;
        if (node.crossedWordsBestScore != 0)
            node.score = node.crossedWordsBestScore;
        else
            node.score = node.singleWordBestScore;

        node.score *= node.multiplier;

        nodeScores.push_back(node.score);
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
        const int maxNumNodes = 100;
        const int maxNumWords = 50;
        const int maxNumLetters = 26;
        const int maxWordScore = 500;
        const int maxMaxWordLength = 100;
        const int maxNodeMultiplier = 500;
        const int numForcedWords = 2;
        const string forcedWord("haggis");
        const string forcedWord2("ragamuffin");
        //const int numNodes = max(static_cast<int>(forcedWord.size() + forcedWord2.size()), (rand() % maxNumNodes) + 1);
        const int numLettersToUse = (rand() % maxNumLetters) + 1;
        //const int numWords = max(numForcedWords, (rand() % maxNumWords) + 1);
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
#if 0
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
#else
        const int numNodes = 100'000;
        treeGenerator.addNodeChain(rootNode, 300);
        treeGenerator.addNodeChain(rootNode, 300);
        treeGenerator.addNodeChain(rootNode, 300);
        treeGenerator.createNodesWithRandomParentPreferringLeafNodes(numNodes - treeGenerator.numNodes(), 50.0);
        cerr << "Blee: " << treeGenerator.numNodes() << endl;
        for (auto& edge : treeGenerator.edges())
        {
            edge->data.letterFollowed = 'a';
        }
        for (auto& node : treeGenerator.nodes())
        {
            node->data.multiplier = (rand() % maxNodeMultiplier) + 1;
        }
        const int numWords = 50;
        vector<TestWord> words(numWords);
        for (int i = 0; i < numWords; i++)
        {
            words[i].score = i + 1;
            words[i].word = string(3 * i + 1, 'a');
        }
#endif


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
    auto edgeId = 1;
    for (auto& edge : edges)
    {
        const auto node1Index = readInt() - 1;
        const auto node2Index = readInt() - 1;
        //cout << "node1Index: " << node1Index << " node2Index: " << node2Index << endl;
        char letter;
        cin >> letter;
        //cout << "letter followed: " << letter << endl;

        auto node1 = &(nodes[node1Index]);
        auto node2 = &(nodes[node2Index]);

        edge.nodeA = node1;
        edge.nodeB = node2;
        edge.letterFollowed = letter;
        edge.edgeId = edgeId;

        node1->neighbours.push_back(&edge);
        node2->neighbours.push_back(&edge);

        edgeId++;
    }

    const auto numWords = readInt();
    words.resize(numWords);
    for (auto& word : words)
    {
        cin >> word.word;
        cin >> word.score;
        assert(word.score != 0);
    }
    for (auto& node : nodes)
    {
        node.originalNeighbours = node.neighbours;
    }

    auto nodeScores = findNodeScores(nodes);
#ifdef BRUTE_FORCE
    const auto nodeScoresOriginal = nodeScores;
#endif
    sort(nodeScores.begin(), nodeScores.end());
    bool currentPlayerIsAlice = true;
    int64_t aliceScore = 0;
    int64_t bobScore = 0;
    while (!nodeScores.empty())
    {
        (currentPlayerIsAlice ? aliceScore : bobScore) += nodeScores.back();

        currentPlayerIsAlice = !currentPlayerIsAlice;
        nodeScores.pop_back();
    }

    cout << aliceScore << endl;
    cout << bobScore << endl;

#ifdef BRUTE_FORCE

    const auto bruteForceNodeScores = findNodeScoresBruteForce(nodes, words);
    assert(bruteForceNodeScores == nodeScoresOriginal);
    for (const auto& node : nodes)
    {
        cout << "wee node: " << node.index << " singleWordBestScoreBruteForce: " << node.singleWordBestScoreBruteForce << " singleWordBestScore: " << node.singleWordBestScore << endl;
        cout << "wee node: " << node.index << " crossedWordsBestScoreBruteForce: " << node.crossedWordsBestScoreBruteForce << " crossedWordsBestScore: " << node.crossedWordsBestScore << endl;
        assert(node.singleWordBestScoreBruteForce == node.singleWordBestScore);
        assert(node.crossedWordsBestScoreBruteForce == node.crossedWordsBestScore);
    }
    cout << "aliceScore: " << aliceScore << " bobScore: " << bobScore << endl;
#endif
}
