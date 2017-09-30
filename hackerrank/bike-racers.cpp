#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <algorithm>
#include <cassert>

using namespace std;


struct Coord
{
    Coord(long x, long y)
        : x(x), y(y)
    {

    }
    long x, y;
};
struct Edge;
struct Node
{
    enum Type { Bike, Rider} type;
    Node(Type type, const Coord& position, long idAmongstType)
        : type(type), position(position), idAmongstType(idAmongstType)
    {

    }
    Coord position;
    long idAmongstType;
    vector<Edge> edges;
};

struct Edge
{
    Edge(Node* otherNode, long weight)
        : otherNode(otherNode), weight(weight)
    {

    }
    Node *otherNode;
    long weight;
};

namespace NetworkFlow
{
    class Edge;
    class Node
    {
        public:
            Node(::Node* originalNode)
                : originalNode(originalNode)
            {
            }
            ::Node* originalNode;
            list<Edge*> edges;
            Edge *prevEdgeInAugmentingPath = nullptr;
            Node *prevNodeInAugmentingPath = nullptr;
    };
    class Edge
    {
        public:
            Edge(Node* sourceNode, Node* destNode)
                : sourceNode(sourceNode), destNode(destNode)
            {
            }
            Node *sourceNode;
            Node *destNode;
            bool isInMatching = false;
            bool hasBeenVisitedInAugmentingPath = false;
    };
}

struct Pairing
{
    long bikeId;
    long riderId;
    long weight;
};

bool operator<(const Pairing& lhs, const Pairing& rhs)
{
    return lhs.weight < rhs.weight;
}

bool canFindMatching(const vector<NetworkFlow::Node*>& allNodes, NetworkFlow::Node* sourceNode, NetworkFlow::Node* sinkNode, long targetNumMatches)
{
    // Use Ford-Fulkerson to see if we can find targetNumMatches matchings.
    using NetworkFlow::Node;
    using NetworkFlow::Edge;
    set<Edge*> allEdges;
    for (auto node : allNodes)
    {
        for (auto edge : node->edges)
        {
            allEdges.insert(edge);
        }
    }
    long numMatches = 0;
    while (numMatches < targetNumMatches)
    {
        // Find augmenting path.
        list<Node*> nodesToExplore;
        nodesToExplore.push_back(sourceNode);
        bool foundAugmentingPath = false;
        for (auto edge : allEdges)
        {
            edge->hasBeenVisitedInAugmentingPath = false;
        }

        while (!nodesToExplore.empty())
        {
            list<Node*> newNodesToExplore;
            for (auto node : nodesToExplore)
            {
                for (auto edge : node->edges)
                {
                    const bool isForwardFlow = (edge->sourceNode == node);
                    Node *otherNode = (isForwardFlow ? edge->destNode : edge->sourceNode);
                    if (edge->hasBeenVisitedInAugmentingPath)
                    {
                        continue;
                    }
                    edge->hasBeenVisitedInAugmentingPath = true;
                    if (edge->isInMatching == isForwardFlow)
                    {
                        continue;
                    }
                    newNodesToExplore.push_back(otherNode);
                    otherNode->prevEdgeInAugmentingPath = edge;
                    otherNode->prevNodeInAugmentingPath = node;
                    if (otherNode == sinkNode)
                    {
                        foundAugmentingPath = true;
                        break;
                    }
                }
            }
            if (foundAugmentingPath)
                break;
            nodesToExplore = newNodesToExplore;
        }
        if (!foundAugmentingPath)
        {
            return false;
        }
        // Incorporate augmenting path into current matching.
        Node *nodeInAugmentingPath = sinkNode;
        while (nodeInAugmentingPath != sourceNode)
        {
            auto edgeInAugmentingPath = nodeInAugmentingPath->prevEdgeInAugmentingPath;
            edgeInAugmentingPath->isInMatching = !edgeInAugmentingPath->isInMatching;
            nodeInAugmentingPath = nodeInAugmentingPath->prevNodeInAugmentingPath;
        }
        numMatches++;
    }
    return true;
}

bool canFindMatching(const vector<Node*>& allNodes, long targetNumMatches, long maxWeight)
{
    // Convert our original graph of Bikes and Racers and Pairings to a NetworkFlow graph.
    // A "flow" goes from source -> one of the bikes -> one of the riders -> sink.
    // TODO - we only really need to do this once (we can efficiently filter out the edges > maxWeight
    // when doing the Ford-Fulkerson run) - doing it every time is not exactly efficient!
    // Note that there are some minor optimisations here based on the fact that the capacities are always
    // either 0 or 1 (so we use a boolean isInMatching instead of integral flows).
    long numBikeNodes = 0;
    long numRiderNodes = 0;
    for (auto node : allNodes)
    {
        if (node->type == Node::Bike)
            numBikeNodes++;
        else
            numRiderNodes++;
    }
    vector<NetworkFlow::Node*> nfBikeNodes(numBikeNodes);
    vector<NetworkFlow::Node*> nfRiderNodes(numRiderNodes);
    for (auto node : allNodes)
    {
        if (node->type == Node::Bike)
            nfBikeNodes[node->idAmongstType] = new NetworkFlow::Node(node);
        else
            nfRiderNodes[node->idAmongstType] = new NetworkFlow::Node(node);
    }
    NetworkFlow::Node *sourceNode = new NetworkFlow::Node(nullptr);
    NetworkFlow::Node *sinkNode = new NetworkFlow::Node(nullptr);
    for (auto node : allNodes)
    {
        if (node->type == Node::Bike)
        {
            // Bike <-> Riders, as long as the distance < maxWeight.
            Node *bikeNode = node;
            NetworkFlow::Node* nfBikeNode = nfBikeNodes[bikeNode->idAmongstType];
            for (auto edge : node->edges)
            {
                if (edge.weight > maxWeight)
                    continue;
                Node *riderNode = edge.otherNode;
                NetworkFlow::Node *nfRiderNode = nfRiderNodes[riderNode->idAmongstType];
                NetworkFlow::Edge * bikeRiderEdge = new NetworkFlow::Edge(nfBikeNode, nfRiderNode);
                nfBikeNode->edges.push_back(bikeRiderEdge);
                nfRiderNode->edges.push_back(bikeRiderEdge);
            }

            // Source -> Bike.
            NetworkFlow::Edge *sourceBikerEdge = new NetworkFlow::Edge(sourceNode, nfBikeNode);
            sourceNode->edges.push_back(sourceBikerEdge);
            nfBikeNodes[bikeNode->idAmongstType]->edges.push_back(sourceBikerEdge);
        }
        else if (node->type == Node::Rider)
        {
            // Rider -> Sink.
            Node *riderNode = node;
            NetworkFlow::Node* nfRiderNode = nfRiderNodes[riderNode->idAmongstType];
            NetworkFlow::Edge *riderSinkEdge = new NetworkFlow::Edge(nfRiderNode, sinkNode);
            sinkNode->edges.push_back(riderSinkEdge);
            nfRiderNode->edges.push_back(riderSinkEdge);
        }
    }
    vector<NetworkFlow::Node*> allNFNodes(nfBikeNodes);
    allNFNodes.insert(allNFNodes.end(), nfRiderNodes.begin(), nfRiderNodes.end());
    allNFNodes.push_back(sourceNode);
    allNFNodes.push_back(sinkNode);

    return canFindMatching(allNFNodes, sourceNode, sinkNode, targetNumMatches);
}

int main()
{
    // Basic approach: find the smallest weight such that we have a K-Matching where the weight
    // of each match does not exceed that weight - finding K-matching is accomplished using Ford-Fulkerson
    // after modelling the problem as a network flow, and finding the smallest weight uses a binary search
    // on weights. 
    // NB: This took me ages as I'm not keen on problems that require you to use non-trivial (i.e. - ones
    // that are named after their creators!) algorithms that are not part of the STL, and I struggled for 
    // ages trying to avoid Ford-Fulkerson.  Had to admit defeat in the end, though :(
    long N, M, K;
    cin >> N >> M >> K;

    // Build graph.
    vector<Node*> riderNodes(N);
    for (long i = 0; i < N; i++)
    {
        long x, y; 
        cin >> x >> y;
        riderNodes[i] = new Node(Node::Rider, Coord(x, y), i);
    }
    vector<Node*> bikeNodes(M);
    for (long i = 0; i < M; i++)
    {
        long x, y; 
        cin >> x >> y;
        bikeNodes[i] = new Node(Node::Bike, Coord(x, y), i);
    }

    vector<Node*> allNodes;
    allNodes.reserve(N + M);
    allNodes.insert(allNodes.end(), riderNodes.begin(), riderNodes.end());
    allNodes.insert(allNodes.end(), bikeNodes.begin(), bikeNodes.end());

    set<long> distances;
    for (auto riderNode : riderNodes)
    {
        for (auto bikeNode : bikeNodes)
        {
            const long bikeRiderDistSquared = (riderNode->position.x - bikeNode->position.x) * (riderNode->position.x - bikeNode->position.x) + (riderNode->position.y - bikeNode->position.y) * (riderNode->position.y - bikeNode->position.y);
            riderNode->edges.push_back(Edge(bikeNode, bikeRiderDistSquared));
            bikeNode->edges.push_back(Edge(riderNode, bikeRiderDistSquared));
            distances.insert(bikeRiderDistSquared);
        }
    }

    // Make an list of all pairings ordered by weight.
    vector<Pairing> orderedPairings;
    for (auto riderNode : riderNodes)
    {
        for (auto bikeNode : bikeNodes)
        {
            const long bikeRiderDistSquared = (riderNode->position.x - bikeNode->position.x) * (riderNode->position.x - bikeNode->position.x) + (riderNode->position.y - bikeNode->position.y) * (riderNode->position.y - bikeNode->position.y);
            Pairing pairing;
            pairing.bikeId  = bikeNode->idAmongstType;
            pairing.riderId  = riderNode->idAmongstType;
            pairing.weight = bikeRiderDistSquared;
            orderedPairings.push_back(pairing);
        }
    }
    sort(orderedPairings.begin(), orderedPairings.end());


    // Calculate rough upper (bestKnownPairing) and lower (minPlausiblePairing) pairing weights (and indices into orderedPairings).
    vector<bool> usedRiders(N, false);
    vector<bool> usedBikes(M, false);
    long pairingsRemaining = K;
    long index = 0;
    long bestKnownPairing = numeric_limits<long>::max();
    long minPlausiblePairing = numeric_limits<long>::max();
    long bestKnownPairingIndex = -1;
    long minPlausiblePairingIndex = -1;
    long numUsedBikes = 0;
    long numUsedRiders = 0;
    for (const auto& pairing : orderedPairings)
    {
        if (!usedRiders[pairing.riderId])
        {
            numUsedRiders++;
        }
        if (!usedBikes[pairing.bikeId])
        {
            numUsedBikes++;
        }
        if (numUsedBikes >= K && numUsedRiders >= K && pairing.weight < minPlausiblePairing)
        {
            minPlausiblePairing = pairing.weight;
            minPlausiblePairingIndex = index;
        }
        if (!usedRiders[pairing.riderId] && !usedBikes[pairing.bikeId])
        {
            usedRiders[pairing.riderId] = true;
            usedBikes[pairing.bikeId] = true;
            pairingsRemaining--;
            if (pairingsRemaining == 0)
            {
                bestKnownPairing = pairing.weight;
                bestKnownPairingIndex = index;
                break;
            }
        }
        index++;
    }
    // Binary chop on all possible weights between bestKnownPairing and minPlausiblePairing weights W to see if we can
    // find a K-matching after disallowing matches with weights > W.
    // When we find a weight W that does have a K-matching, but the next lowest weight does not, we have our answer.
    long maxPairingWeightIndex = bestKnownPairingIndex;
    long minPairingWeightIndex = minPlausiblePairingIndex;
    long lowestPairingWeightIndex = numeric_limits<long>::max();
    while (maxPairingWeightIndex >= minPairingWeightIndex)
    {
        const long pairingWeightIndex = minPairingWeightIndex + (maxPairingWeightIndex - minPairingWeightIndex) / 2;
        const long maxAllowedWeight = orderedPairings[pairingWeightIndex].weight;
        if (canFindMatching(allNodes, K, maxAllowedWeight))
        {
            // Found a K-matching; decrease maxAllowedWeight and retry.
            maxPairingWeightIndex = pairingWeightIndex - 1;
            if (pairingWeightIndex < lowestPairingWeightIndex)
                lowestPairingWeightIndex = pairingWeightIndex;
        }
        else
        {
            // Increase maxAllowedWeight and retry.
            minPairingWeightIndex = pairingWeightIndex + 1;
        }
    }
    cout << orderedPairings[lowestPairingWeightIndex].weight << endl;
}
