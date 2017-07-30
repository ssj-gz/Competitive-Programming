#include <cmath>
#include <cstdio>
#include <vector>
#include <stack>
#include <iostream>
#include <algorithm>
#include <cassert>
using namespace std;

class Node;
class Edge
{
    public:
        Edge(Node* otherNode, long weight)
            : otherNode(otherNode), weight(weight)
        {

        }
        Node *otherNode;
        long weight;
};

class Node
{
    public:
        Node(long id)
            : id(id)
        {

        }
        long id;
        vector<Edge> edges;
};

pair<long, long> sumOfPathsFrom(Node* startNode, Node *parentNode)
{
    struct State
    {
        State(Node *currNode, Node *parentNode, long currentPathLength)
            : currNode(currNode), parentNode(parentNode), currentPathLength(currentPathLength)
        {
        }
        Node *currNode;
        Node *parentNode;
        long currentPathLength;
    };

    vector<State> toExplore = { State(startNode, parentNode, 0)};
    long pathSum = 0;
    while (!toExplore.empty())
    {
        vector<State> toExploreNext;
        for (const State& state : toExplore)
        {
            pathSum += state.currentPathLength;
            for (auto edge : state.currNode->edges)
            {
                if (edge.otherNode == state.parentNode) 
                    continue;
                toExploreNext.push_back(State(edge.otherNode, state.currNode, state.currentPathLength + edge.weight));
            }
        }
        toExplore = toExploreNext;
    }
    return make_pair(pathSum, -1);
}



int main() {
    // Read in graph.
    long N;
    cin >> N;
    vector<Node*> allNodes(N);
    for (int i = 0; i < N; i++)
    {
        allNodes[i] = new Node(i + 1);
    }
    for (int i = 0; i < N - 1; i++)
    {
        long node1Id, node2Id, weight;
        cin >> node1Id >> node2Id >> weight;
        Node *node1 = allNodes[node1Id - 1];
        Node *node2 = allNodes[node2Id - 1];
        node1->edges.push_back(Edge (node2, weight));
        node2->edges.push_back(Edge (node1, weight));
    }
    // Ok - I'm going to have to come back and revisit this one, as I mainly achieved it 
    // through empirical observation, *some* analysis, and then a lump of guesswork :/
    // So: imagine we have chosen the maximum possible values for row 1; then 
    // column 1 will have the *negation* of these maximum values.
    // For each of the rows 2 ... n-1, we have to fill in cells  that make that row sum equal to 
    // 0 - tricky, as now that we've put a big, fat, negative number in the first column of each such row.
    // Note that we can pick any value (including the maximum) for each cell in the nth column without repercussions as, though
    // this would then add a big negative number to the nth row, the nth row has no restrictions
    // on its sum.  Quite often, this will suffice to "cancel" out the negative number in the first column,
    // giving us 0 in each of row 2 ... n-1.  Sometimes, it's not enough and we need to start introducing 
    // positive numbers in other cells, which of course puts a negative number in another row, making 
    // the task of zero-ing out that row more difficult :/
    // From empirical observation and guesswork, it seems that we can zero-out each row if and only if
    // sum i = 2...n-1 (A1,i) <= sum i = 2...n-1 (dist(n,i)) i.e. after choosing our first row (hence forcing
    // the contents of the 1st column), the sum of the leftmost cells from row 2 to n-1 is less than
    // the sum of the rightmost cells from row 2 to n-1.  If we choose the cells in the first row "badly", we
    // can alter the choice so that the required leftmost sum is equal to the rightmost sum.
    // I can't yet come up with a good rationale behind why this should work, though :/
    // We can always pick the rightmost cell of the first column to be the maximum with no repercussions,
    // so the max value for the first row ends up being bounded by sum i=2...n-1(dist(n, i)) + dist(1,n) = 
    // sum i=1...n-1(dist(n,i)).
    // Calculate sum i=1...n-1(dist(n,i)).
    Node *nodeN = allNodes[N - 1];
    auto processedNodeN = sumOfPathsFrom(nodeN, nullptr);
    long max = processedNodeN.first; 
    long numDescendants = processedNodeN.second; 
    // Calculate the maximum sum of all paths from node 1.
    Node *node1 = allNodes[0];
    auto processedNode1 = sumOfPathsFrom(node1, nullptr);
    // The maximum sum of the first row is bounded by sum i=1...n-1(dist(n,i)).
    cout << min(processedNode1.first, processedNodeN.first) << endl;
    return 0;
}

