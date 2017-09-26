// Simon St James (ssjgz) 2017-09-26 10:41
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <map>
#include <vector>
#include <cmath>
#include <cassert>

using namespace std;

namespace 
{
    struct Edge;
    struct Node
    {
        vector<Edge*> neighbours;
    };

    struct Edge
    {
        Node *node1 = nullptr;
        Node *node2 = nullptr;
        bool guessedNode1ParentofNode2 = false;
        bool guessedNode2ParentofNode1 = false;
    };
}

int findNumCorrectGuessed(const Node* node, const Node* parentNode = nullptr, const Edge* edgeTravelledFromParent = nullptr)
{
    //cout << "findNumCorrectGuessed: " << node << endl;
    int numCorrectGuessed = 0;
    for (const auto& edge : node->neighbours)
    {
        if (edge->node1 == parentNode || edge->node2 == parentNode)
            continue;

        //cout << " edge: " << edge << " guessedNode1ParentofNode2: " << edge->guessedNode1ParentofNode2 << " guessedNode2ParentofNode1: " << edge->guessedNode2ParentofNode1 << endl;

        const Node* childNode = (edge->node1 == node ? edge->node2 : edge->node1);
        if (edge->guessedNode1ParentofNode2 && edge->node1 == node && edge->node2 == childNode)
        {
            //cout << " found correct guess" << endl;
            numCorrectGuessed++;
        }
        if (edge->guessedNode2ParentofNode1 && edge->node2 == node && edge->node1 == childNode)
        {
            //cout << " found correct guess" << endl;
            numCorrectGuessed++;
        }

        //cout << " recursing into: " << childNode << endl;
        numCorrectGuessed += findNumCorrectGuessed(childNode, node, edge);

    }
    return numCorrectGuessed;
}

int findNumRootNodesBruteForce(const vector<Node>& nodes, int numCorrectGuessedRequired)
{
    int numRootNodes = 0;
    for (const auto& node : nodes)
    {
        if (findNumCorrectGuessed(&node, nullptr, nullptr) >= numCorrectGuessedRequired)
        {
            numRootNodes++;
        }
    }
    return numRootNodes;
}

void findMatchingRootNodes(const Node* node, int numCorrectGuessedRequired, int& numMatching, int rootNumCorrectGuesses, const Node* parentNode = nullptr, const Edge* edgeTravelledFromParent = nullptr, int numCorrectlyGuessedForParentNode = -1)
{
    int numCorrectlyGuessed = 0;
    if (parentNode)
    {
        numCorrectlyGuessed = numCorrectlyGuessedForParentNode;
        if ((edgeTravelledFromParent->guessedNode1ParentofNode2 && edgeTravelledFromParent->node1 == parentNode && edgeTravelledFromParent->node2 == node) ||
            (edgeTravelledFromParent->guessedNode2ParentofNode1 && edgeTravelledFromParent->node2 == parentNode && edgeTravelledFromParent->node1 == node)   )
        {
            numCorrectlyGuessed--;
        }
        if ((edgeTravelledFromParent->guessedNode1ParentofNode2 && edgeTravelledFromParent->node2 == parentNode && edgeTravelledFromParent->node1 == node) ||
            (edgeTravelledFromParent->guessedNode2ParentofNode1 && edgeTravelledFromParent->node1 == parentNode && edgeTravelledFromParent->node2 == node)   )
        {
            numCorrectlyGuessed++;
        }
    }
    else
    {
        // I'm root.
        numCorrectlyGuessed = rootNumCorrectGuesses;
    }
    if (numCorrectlyGuessed >= numCorrectGuessedRequired)
    {
        numMatching++;
    }
    for (const auto& edge : node->neighbours)
    {
        if (edge->node1 == parentNode || edge->node2 == parentNode)
            continue;

        const Node* childNode = (edge->node1 == node ? edge->node2 : edge->node1);

        findMatchingRootNodes(childNode, numCorrectGuessedRequired, numMatching, rootNumCorrectGuesses, node, edge, numCorrectlyGuessed);
    }

}

int findNumRootNodes(const vector<Node>& nodes, int numCorrectGuessedRequired)
{
    const Node* rootNode = &(nodes.front());
    const int numCorrectGuessesRootNode = findNumCorrectGuessed(rootNode);
    int numRootNodes = 0;
    findMatchingRootNodes(rootNode, numCorrectGuessedRequired, numRootNodes, numCorrectGuessesRootNode);
    return numRootNodes;
}

int main()
{
    int Q;
    cin >> Q;
    for (int q = 0; q < Q; q++)
    {

        int n;
        cin >> n;

        vector<Node> nodes(n);
        vector<Edge> edges(n - 1);
        map<pair<int, int>, Edge*> nodePairToEdge;

        for (int i = 0; i < n - 1; i++)
        {
            int u, v;
            cin >> u >> v;
            u--;
            v--;

            Edge& edge = edges[i];
            edge.node1 = &(nodes[u]);
            edge.node2 = &(nodes[v]);
            nodes[u].neighbours.push_back(&edge);
            nodes[v].neighbours.push_back(&edge);

            nodePairToEdge[make_pair(u, v)] = &edge;
            nodePairToEdge[make_pair(v, u)] = &edge;
        }


        int g, k;
        cin >> g >> k;

        for (int i = 0; i < g; i++)
        {
            int parentNodeIndex, childNodeIndex;
            cin >> parentNodeIndex >> childNodeIndex;
            parentNodeIndex--;
            childNodeIndex--;

            Node* parentNode = &(nodes[parentNodeIndex]);
            Node* childNode = &(nodes[childNodeIndex]);

            Edge* edge = nodePairToEdge[make_pair(parentNodeIndex, childNodeIndex)];
            assert(edge);
            edge->guessedNode1ParentofNode2 |= (edge->node1 == parentNode && edge->node2 == childNode);
            edge->guessedNode2ParentofNode1 |= (edge->node2 == parentNode && edge->node1 == childNode);
        }

        const auto numMatchingRootNodesOptimised = findNumRootNodes(nodes, k);
        int numerator = numMatchingRootNodesOptimised;
        int denominator = n;
        for (int i = 2; i <= sqrt(n); i++)
        {
            while ((numerator % i) == 0 && (denominator % i) == 0)
            {
                numerator /= i;
                denominator /= i;
            }
        }
        cout << numerator << "/" << denominator << endl;
//#define BRUTE_FORCE
#ifdef BRUTE_FORCE
        const auto numMatchingRootNodesBruteForce = findNumRootNodesBruteForce(nodes, k);
        cout << "numMatchingRootNodesBruteForce: " << numMatchingRootNodesBruteForce << " numMatchingRootNodesOptimised: " << numMatchingRootNodesOptimised << endl;
        assert(numMatchingRootNodesBruteForce == numMatchingRootNodesOptimised);
#endif
    }
}

