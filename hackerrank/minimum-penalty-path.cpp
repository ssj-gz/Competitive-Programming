#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <utility>
#include <limits>
#include <cassert>

using namespace std;

int main()
{
    int N, M;
    cin >> N >> M;


    // Utterly bewildered, here: this is a much, much more straightforward (and efficiently implementable)
    // version of "Synchronized Shopping", which not only precedes this if you do the Graph Theory
    // problems in order, but has a lower completion rate and a lower number of points for completion.
    // I really don't get it.

    // Anyway, just as Synchronized Shopping built up a kind of "meta-graph" where the first component
    // corresponded to a node in the original graph and the second was the collection of fish obtained
    // if we reached that node,  here the second component consists of the penalty to reach this node.  
    // With that, it's a simple BFS from the node (A,0), noting all (B,x) encountered and picking the 
    // smallest x as our answer.

    struct OriginalNode
    {
        // first == other node id; second = weight.
        vector<pair<int, int>> edges;
    };

    vector<OriginalNode> originalNodes(N);

    for (int i = 0; i < M; i++)
    {
        int firstOriginalNodeId, secondOriginalNodeId, weight;
        cin >> firstOriginalNodeId >> secondOriginalNodeId >> weight;
        // Make 0-relative.
        firstOriginalNodeId--;
        secondOriginalNodeId--;

        originalNodes[firstOriginalNodeId].edges.push_back(make_pair(secondOriginalNodeId, weight));
        originalNodes[secondOriginalNodeId].edges.push_back(make_pair(firstOriginalNodeId, weight));
    }

    int A, B;
    cin >> A >> B;
    // Make 0-relative.
    A--;
    B--;

    const int maxPenalty = 1023;
    // Main "graph": each cell is a Node, and is indexed by originalNodeId x penalty to reach it.
    struct Node;
    struct Edge
    {
        Edge(Node *otherNode, int weight)
            : otherNode(otherNode), weight(weight)
        {

        }
        Node *otherNode = nullptr;
        int weight = -1;
    };
    struct Node
    {
        int originalNodeId;
        int penaltyToReach;
        vector<Edge> edges;

        bool visitedInBFS = false;
    };

    vector<vector<Node>> graph(N, vector<Node>(maxPenalty + 1));
    // Build it up via a BFS.
    Node& initialNode = graph[A][0];
    initialNode.originalNodeId = A;
    initialNode.penaltyToReach = 0;

    vector<Node*> nodesToExplore = { &initialNode };

    bool canReachB = false;
    int minPenalty = std::numeric_limits<int>::max();

    while (!nodesToExplore.empty())
    {
        vector<Node*> newNodesToExplore;
        for (auto& node : nodesToExplore)
        {
            if (node->visitedInBFS)
                continue;
            node->visitedInBFS = true;
            const OriginalNode& nodeOriginalNode = originalNodes[node->originalNodeId];
            for (auto edge : nodeOriginalNode.edges)
            {
                const int otherOriginalNodeId = edge.first;
                const int otherNodePenalty = node->penaltyToReach | edge.second;
                assert(otherNodePenalty <= maxPenalty);
                Node& otherNode = graph[otherOriginalNodeId][otherNodePenalty];

                node->edges.push_back(Edge(&otherNode, edge.second));

                if (!otherNode.visitedInBFS)
                {
                    newNodesToExplore.push_back(&otherNode);
                    otherNode.originalNodeId = otherOriginalNodeId;
                    otherNode.penaltyToReach = otherNodePenalty;

                    if (otherNode.originalNodeId == B && otherNode.penaltyToReach < minPenalty)
                    {
                        canReachB = true;
                        minPenalty = otherNode.penaltyToReach;
                    }
                }
            }
        }
        nodesToExplore = newNodesToExplore;
    }
    if (!canReachB)
    {
        cout << -1 << endl;
    }
    else
    {
        cout << minPenalty << endl;
    }

}


