#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <utility>
#include <limits>
#include "experiments/heap.hpp"

using namespace std;

int main()
{
    int N, M, numFishTypes;
    cin >> N >> M >> numFishTypes;

    // Represent the fish sold at a Shopping Center by a set of bits,
    // with each fishType bit 1 .. numFishTypes being on or off.
    const int maxFishCollectionValue = (1 << numFishTypes);

    struct ShoppingCenter
    {
        int fishCollection = 0;
        // first == shopping center id; second = travel time.
        vector<pair<int, int>> roads;
    };

    vector<ShoppingCenter> shoppingCenters;

    for (int i = 0; i < N; i++)
    {
        int numFishTypesSold;
        cin >> numFishTypesSold;

        ShoppingCenter shoppingCenter;

        for (int j = 0; j < numFishTypesSold; j++)
        {
            int fishType;
            cin >> fishType;

            // Make fishType 0-relative.
            fishType--;
            shoppingCenter.fishCollection |= (1 << fishType);
        }

        shoppingCenters.push_back(shoppingCenter);
    }

    for (int i = 0; i < M; i++)
    {
        int firstShoppingCentreId, secondShoppingCentreId, travelTime;
        cin >> firstShoppingCentreId >> secondShoppingCentreId >> travelTime;
        // Make 0-relative.
        firstShoppingCentreId--;
        secondShoppingCentreId--;

        shoppingCenters[firstShoppingCentreId].roads.push_back(make_pair(secondShoppingCentreId, travelTime));
        shoppingCenters[secondShoppingCentreId].roads.push_back(make_pair(firstShoppingCentreId, travelTime));
    }

    // Right - we've read in all the data - now to begin!
    // The solution is relatively straightforward, though takes a bit of work to implement.
    // Firstly: the presence of two cats is a red-herring (pardon the pun :)): if one cat makes it to N
    // with set of fish F1 and taking time t1, and the other makes it to N with a set of 
    // fish F2 and taking  time t2, and F1 union F2 == full set of fish, then the time is
    // max(t1, t2).  Thus, if one reaches N at time t1 and with fish collection F1, then we need only
    // check to see how long it takes to reach N with each fish collection F that is a *complement*
    // to F1 i.e. such that F1 union F == full set of fish i.e. we only need to consider one cat at a  time.
    // Obviously, we should assume that each cat buys up all the fish it can at each step.

    // As for finding out the min time to reach the ending with a given fish collection, we use Dijkstra 
    // on a special graph that models the problem: the nodes of the graph are pairs (Shopping Center, Fish Collection)
    // and there is an edge (s1, f1) -> (s2, f2) with weight w if and only if there is an edge with weight w between
    // s1 and s2 in the original graph, and f1 union [fish sold at s2] == f2.  As we take a path P = s1s2...s_l
    // through the original graph and through the corresponding new graph (P' = (s1,f1)(s2,f2)...(s_l,f_l)) we see 
    // that the f_i match the fish we would have collected if we bought all fish travelling along path P - in 
    // particular, if s_l == N, then the f_l is the fish collection we finish with.

    struct Node;
    struct Edge
    {
        Edge(Node *otherNode, int travelTime)
            : otherNode(otherNode), travelTime(travelTime)
        {

        }
        Node *otherNode = nullptr;
        int travelTime = -1;
    };
    struct Node
    {
        int shoppingCenterId;
        int fishCollection;
        vector<Edge> edges;

        bool visitedInBFS = false;

        // For Dijkstra's Algorithm.
        long d = std::numeric_limits<long>::max();
        bool completed = false;
        bool isInHeap = false;
        int heapHandleId = -1;
    };

    // Main "graph": each cell is a Node, and is indexed by shoppingCenterId x fishCollection.
    vector<vector<Node>> graph(N, vector<Node>(maxFishCollectionValue + 1));
    // Build it up via a BFS.
    Node& initialNode = graph[0][shoppingCenters[0].fishCollection];
    initialNode.shoppingCenterId = 0;
    initialNode.fishCollection = shoppingCenters[0].fishCollection;

    vector<Node*> nodesToExplore = { &initialNode };

    while (!nodesToExplore.empty())
    {
        vector<Node*> newNodesToExplore;
        for (auto& node : nodesToExplore)
        {
            if (node->visitedInBFS)
                continue;
            node->visitedInBFS = true;
            const ShoppingCenter& nodeShoppingCenter = shoppingCenters[node->shoppingCenterId];
            node->edges.reserve(nodeShoppingCenter.roads.size());
            for (auto road : nodeShoppingCenter.roads)
            {
                const int otherShoppingCenterId = road.first;
                const ShoppingCenter& otherShoppingCenter = shoppingCenters[otherShoppingCenterId];
                const int otherNodeFishCollection = node->fishCollection | otherShoppingCenter.fishCollection;
                Node& otherNode = graph[otherShoppingCenterId][otherNodeFishCollection];

                node->edges.push_back(Edge(&otherNode, road.second));

                if (!otherNode.visitedInBFS)
                {
                    newNodesToExplore.push_back(&otherNode);
                    otherNode.shoppingCenterId = otherShoppingCenterId;
                    otherNode.fishCollection = otherNodeFishCollection;
                }
            }
        }
        nodesToExplore = newNodesToExplore;
    }

    // Dijkstra time!
    const int endingShoppingCentreId = N - 1;
    vector<long> minEndingsWithFishCollection(maxFishCollectionValue + 1, std::numeric_limits<long>::max());
    long minFullFishEndingTime = std::numeric_limits<long>::max();
    initialNode.d = 0;
    struct NodeDComparison
    {
        bool operator()(const Node* lhs, const Node* rhs) const
        { 
            // Want to sort by d ...
            if (lhs->d != rhs->d)
            {
                return lhs->d < rhs->d; 
            }
            // ... but may want to include multiple nodes with same "d"!
            return lhs < rhs;
        }
    };
    auto decreaseKeyBy = [](Node* nodeToDecrease, int decreaseKeyBy)
    {
        nodeToDecrease->d -= decreaseKeyBy;
    };
    Heap<Node*, NodeDComparison, decltype(decreaseKeyBy) > nodesByD(graph.size() * graph[0].size(), NodeDComparison(), decreaseKeyBy);
    nodesByD.add(&initialNode);

    while (!nodesByD.empty())
    {
        //cout << " # nodes in heap: " << nodesByD.size() << endl;
        Node *lowestDNode = nodesByD.min();
        nodesByD.extractMin();

        // Optimise: don't process nodes that can't possibly beat the current record.
        if (lowestDNode->d >= minFullFishEndingTime)
            continue;

        vector<Node*> newNodes;
        for (const auto& edge : lowestDNode->edges)
        {
            Node *otherNode = edge.otherNode;
            if (otherNode->completed)
                continue;

            const int otherNodeNewMinDist = lowestDNode->d + edge.travelTime;
            if (otherNode->d > otherNodeNewMinDist)
            {
                if (otherNode->isInHeap)
                {
                    nodesByD.decreaseKey(nodesByD.handleById(otherNode->heapHandleId), otherNode->d - otherNodeNewMinDist);
                }
                otherNode->d = otherNodeNewMinDist;
            }
            // Optimise: don't add nodes that can't possibly beat the current record.
            if (otherNode->d <= minFullFishEndingTime)
                newNodes.push_back(otherNode);
        }

        lowestDNode->completed = true;

        for (auto newNode : newNodes)
        {
            if (newNode->completed || newNode->isInHeap)
                continue;
            const int handleId = nodesByD.add(newNode).id();
            newNode->heapHandleId = handleId;
            newNode->isInHeap = true;
        }


        if (lowestDNode->shoppingCenterId == endingShoppingCentreId)
        {
            const int fishCollection = lowestDNode->fishCollection;
            const long time = lowestDNode->d;
            long& minEndingWithTheseFish = minEndingsWithFishCollection[fishCollection];
            minEndingWithTheseFish = min(minEndingWithTheseFish, time);
            // Find all min ending times with fish collections that are complements to ours.
            const int fullFishCollection = (1 << numFishTypes) - 1;
            for (int complementFishCollection = 0; complementFishCollection <= maxFishCollectionValue; complementFishCollection++)
            {
                if ((complementFishCollection | fishCollection) != fullFishCollection)
                    continue;
                const long endingTime = max(time, minEndingsWithFishCollection[complementFishCollection]);
                minFullFishEndingTime = min(minFullFishEndingTime, endingTime);
            }
        }
    }

    cout << minFullFishEndingTime << endl;
}
