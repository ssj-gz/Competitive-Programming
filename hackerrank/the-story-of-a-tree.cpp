// Simon St James (ssjgz) 2017-09-26 10:41
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
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
        Node* parentNode = nullptr;
        Edge* edgeFollowedFromParent = nullptr;
    };

    struct Edge
    {
        Node *node1 = nullptr;
        Node *node2 = nullptr;
        bool guessedNode1ParentofNode2 = false;
        bool guessedNode2ParentofNode1 = false;
        Node* otherNode(const Node* node)
        {
            return (node == node1 ? node2 : node1);
        }
    };
}

bool isCorrectGuess(const Edge* edge, const Node* parentNode, const Node* childNode)
{
    if (edge->guessedNode1ParentofNode2 && edge->node1 == parentNode && edge->node2 == childNode)
        return true;
    if (edge->guessedNode2ParentofNode1 && edge->node2 == parentNode && edge->node1 == childNode)
        return true;

    return false;
}

Edge* findEdgeBetweenNodes(Node* node1, Node* node2)
{
    Edge *edge = nullptr;
    if (node2->parentNode == node1)
    {
        edge = node2->edgeFollowedFromParent;
    }
    else if (node1->parentNode == node2)
    {
        edge = node1->edgeFollowedFromParent;
    }
    assert(edge && "Make sure you've called fillInParentNode first!");
    return edge;
}

void fillInParentNode(Node* node, Node* parentNode = nullptr, Edge* edgeFollowedFromParent = nullptr)
{
    node->parentNode = parentNode;
    node->edgeFollowedFromParent = edgeFollowedFromParent;

    for (const auto& edge : node->neighbours)
    {
        if (edge->node1 == parentNode || edge->node2 == parentNode)
            continue;

        Node* childNode = edge->otherNode(node);
        fillInParentNode(childNode, node, edge);
    }
}

// NB: this returns the correct answer *only* for the "root" node i.e. the node in the initial call which has no parents.
int findNumCorrectGuesses(const Node* node)
{
    const Node* parentNode = node->parentNode;
    int numCorrectGuessed = 0;
    for (const auto& edge : node->neighbours)
    {
        if (edge->node1 == parentNode || edge->node2 == parentNode)
            continue;

        const Node* childNode = edge->otherNode(node);
        if (isCorrectGuess(edge, node, childNode))
            numCorrectGuessed++;

        numCorrectGuessed += findNumCorrectGuesses(childNode);
    }
    return numCorrectGuessed;
}

void findMatchingRootNodes(const Node* node, int numCorrectGuessedRequired, int& numMatching, int rootNumCorrectGuesses, int numCorrectlyGuessedForParentNode = -1)
{
    const Node* parentNode = node->parentNode;
    const Edge* edgeFollowedFromParent = node->edgeFollowedFromParent;

    int numCorrectlyGuessed = 0;
    if (parentNode)
    {
        numCorrectlyGuessed = numCorrectlyGuessedForParentNode;
        if (isCorrectGuess(edgeFollowedFromParent, parentNode, node))
        {
            numCorrectlyGuessed--;
        }
        if (isCorrectGuess(edgeFollowedFromParent, node, parentNode))
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

        const Node* childNode = edge->otherNode(node);

        findMatchingRootNodes(childNode, numCorrectGuessedRequired, numMatching, rootNumCorrectGuesses, numCorrectlyGuessed);
    }

}

int findNumRootNodes(const Node* rootNode, int numCorrectGuessedRequired)
{
    const int numCorrectGuessesRootNode = findNumCorrectGuesses(rootNode);
    int numRootNodes = 0;
    findMatchingRootNodes(rootNode, numCorrectGuessedRequired, numRootNodes, numCorrectGuessesRootNode);
    return numRootNodes;
}

int main()
{
    // Fairly easy one, though again, a few schoolboy errors along the way XD
    //
    // Define numCorrectGuessesIfRoot(r) to be the number of correct guesses if
    // node r was the root of the DFS: this is very easy to calculate in O(n) using a DFS (findNumCorrectGuesses).
    // To solve the problem by brute force, we could simply find, for each node r the number of r's for which 
    // numCorrectGuessesIfRoot(r) >= k (call this numMatchingRootNodes) and output the cancelled fraction 
    // numMatchingRootNodes/n.  This, of course, would be O(n^2) in total.
    //
    // Being able to find numCorrectGuessesIfRoot(r) for all r in O(n) total would be nice :)
    //
    // It seems quite likely though that if u and v are neighbours, then numCorrectGuessesIfRoot(u)
    // would be similar to numCorrectGuessesIfRoot(v), and in fact this is exactly the case!
    //
    // Let CE = u'v' be an edge; in a DFS from a node r, CE would be traversed either in the order u'v' or in the order v'u':
    // call these two possibilities the "direction" in which CE is traversed.  Clearly, the contribution of CE
    // to numCorrectGuessesIfRoot(r) is dependent on whether the direction in which a DFS from r traverses CE is a guess or not.
    //
    // Assume that we just happen to know numCorrectGuessesIfRoot(u). If CE != e, and if P is the path from u to CE, 
    // then we have two possibilities: either P passes through v, or it does not.
    //
    // If P passes through v, then v is the second element in P, and the path P' = P with the first vertex (u) removed would 
    // be a path from v that passes through CE in the same direction as P.
    //
    // If P doesn't pass through v, then the path vP would be a path from v pass through CE in the same direction as P.
    //
    // Thus, for all edges CE != e, CE is traversed in the same direction if v were root as for if u were root,
    // so the contribution of CE to numCorrectGuessesIfRoot(u) is equal to the contribution of CE to numCorrectGuessesIfRoot(v)
    // i.e. ignoring the contribution of e, so far we have numCorrectGuessesIfRoot(u) == numCorrectGuessesIfRoot(v).
    //
    // How do we deal with e? Well, if one of the guesses was that u was v's parent, then we'd have to subtract
    // one from numCorrectGuessesIfRoot(v); this would be a correct guess if v were root, but an incorrect guess if u were root.
    // Conversely, if one of the guesses was that u were v's parent, we'd have to add one to numCorrectGuessesIfRoot(v);
    // this would be an incorrect guess if u were root, but correct if v were.
    //
    // Thus, given u and v connected by an edge e:
    //  
    //   numCorrectGuessesIfRoot(v) = numCorrectGuessesIfRoot(u) + 1 if u = parent(v) is a guess and
    //                                                           - 1 if v = parent(u) is a guess.
    //
    // Now, we need only pick some arbitrary root node r, find numCorrectGuessesIfRoot(r) (O(n)), then recursively update
    // its descendants using the formula above (a further O(n)), and we're done :)
    int Q;
    cin >> Q;
    for (int q = 0; q < Q; q++)
    {

        int n;
        cin >> n;

        vector<Node> nodes(n);
        vector<Edge> edges(n - 1);

        // Read in graph.
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
        }

        int g, k;
        cin >> g >> k;

        Node* rootNode = &(nodes.front());
        // We use fillInParentNode so that we can find, in O(1), the edge connecting any two connected
        // nodes (needed for efficient updating of guessedNode1ParentofNode2 and guessedNode2ParentofNode1 
        // from the list of guesses!).
        fillInParentNode(rootNode);

        for (int i = 0; i < g; i++)
        {
            int parentNodeIndex, childNodeIndex;
            cin >> parentNodeIndex >> childNodeIndex;
            parentNodeIndex--;
            childNodeIndex--;

            Node* parentNode = &(nodes[parentNodeIndex]);
            Node* childNode = &(nodes[childNodeIndex]);

            Edge* edge = findEdgeBetweenNodes(parentNode, childNode);

            edge->guessedNode1ParentofNode2 |= (edge->node1 == parentNode && edge->node2 == childNode);
            edge->guessedNode2ParentofNode1 |= (edge->node2 == parentNode && edge->node1 == childNode);
        }


        const auto numMatchingRootNodes = findNumRootNodes(rootNode, k);
        // Cancel fraction numMatchingRootNodes/n.
        int numerator = numMatchingRootNodes;
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
    }
}

