// Simon St James (ssjgz) - 2017-10-02
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;

namespace NetworkFlow
{
    class Node;
}

struct Client
{
    int minArea = 0;
    int maxPrice = 0;
    NetworkFlow::Node* networkFlowNode = nullptr;
};

struct House
{
    int area = 0;
    int price = 0;
    NetworkFlow::Node* networkFlowNode = nullptr;
};

namespace NetworkFlow
{
    class Edge;
    class Node
    {
        public:
            Node()
            {
            }
            vector<Edge*> edges;

            vector<Edge*> edgesRightWay;
            vector<Edge*> edgesWrongWay;
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

struct PathElement
{
    NetworkFlow::Node* node = nullptr;
    NetworkFlow::Edge* edgeFollowed = nullptr;
    bool followedEdgeRightWay = true;
    int followedEdgeIndex = -1;
};

bool findAugmentPath(NetworkFlow::Node* currentNode, NetworkFlow::Node* sinkNode, vector<PathElement>& pathSoFar, vector<NetworkFlow::Edge*>& visitedEdges)
{
    if (currentNode == sinkNode)
        return true;

    PathElement pathElement;
    pathElement.node = currentNode;
    pathSoFar.push_back(pathElement);

    // Try going in the right direction first.
    for (int i = 0; i < currentNode->edgesRightWay.size(); i++)
    {
        auto edge = currentNode->edgesRightWay[i];
        assert(!edge->isInMatching);
        if (edge->hasBeenVisitedInAugmentingPath)
            continue;

        pathSoFar.back().followedEdgeIndex = i;
        edge->hasBeenVisitedInAugmentingPath = true;
        visitedEdges.push_back(edge);
        assert(edge->sourceNode == currentNode);
        auto otherNode = edge->destNode;
        const bool augmentingPathFound = findAugmentPath(otherNode, sinkNode, pathSoFar, visitedEdges);
        if (augmentingPathFound)
            return true;
    }
    // Wrong direction.
    pathSoFar.back().followedEdgeRightWay = false;
    for (int i = 0; i < currentNode->edgesWrongWay.size(); i++)
    {
        auto edge = currentNode->edgesWrongWay[i];
        if (edge->hasBeenVisitedInAugmentingPath)
            continue;
        if (!edge->isInMatching)
            continue;
        pathSoFar.back().followedEdgeIndex = i;
        edge->hasBeenVisitedInAugmentingPath = true;
        visitedEdges.push_back(edge);
        assert(edge->destNode == currentNode);
        auto otherNode = edge->sourceNode;
        const bool augmentingPathFound = findAugmentPath(otherNode, sinkNode, pathSoFar, visitedEdges);
        if (augmentingPathFound)
            return true;
    }

    pathSoFar.pop_back();
    return false;
}

vector<PathElement> findAugmentPath(NetworkFlow::Node* sourceNode, NetworkFlow::Node* sinkNode)
{
    vector<PathElement> augmentingPath;
    vector<NetworkFlow::Edge*> visitedEdges;
    const bool found = findAugmentPath(sourceNode, sinkNode, augmentingPath, visitedEdges);

    for (auto edge : visitedEdges)
    {
        edge->hasBeenVisitedInAugmentingPath = false;
    }

    if (found)
        return augmentingPath;
    else 
        return vector<PathElement>();
}

int maxMatch(vector<NetworkFlow::Node>& allNodes, NetworkFlow::Node* sourceNode, NetworkFlow::Node* sinkNode)
{
    // Use Ford-Fulkerson to find the maximal match.
    using NetworkFlow::Node;
    using NetworkFlow::Edge;
    for (auto& node : allNodes)
    {
        for (auto edge : node.edges)
        {
            if (edge->sourceNode == &node)
            {
                node.edgesRightWay.push_back(edge);
            }
        }
    }
    long numMatches = 0;
    while (true)
    {
        auto augmentingPath = findAugmentPath(sourceNode, sinkNode);
        if (augmentingPath.empty())
            break;
        else
        {
            // Incorporate augmenting path into current matching.
            reverse(augmentingPath.begin(), augmentingPath.end());
            for (const auto& augmentingPathElement : augmentingPath)
            {
                const bool followedEdgeRightWay = augmentingPathElement.followedEdgeRightWay;
                auto node = augmentingPathElement.node;
                Edge* edgeFollowed = (followedEdgeRightWay ? node->edgesRightWay[augmentingPathElement.followedEdgeIndex] :
                                                  node->edgesWrongWay[augmentingPathElement.followedEdgeIndex]);
                auto otherNode = (edgeFollowed->sourceNode == node ? edgeFollowed->destNode : edgeFollowed->sourceNode); 
                assert(edgeFollowed->isInMatching == !followedEdgeRightWay);
                edgeFollowed->isInMatching = !edgeFollowed->isInMatching;

                if (followedEdgeRightWay)
                {
                    // Erase the followed edge from the list of edges we can follow in the wrong direction.
                    swap(node->edgesRightWay[augmentingPathElement.followedEdgeIndex], node->edgesRightWay.back());
                    node->edgesRightWay.pop_back();

                    // ... add add to the list of edges that the other node can follow in the wrong direction.
                    otherNode->edgesWrongWay.push_back(edgeFollowed);
                }
                else
                {
                    swap(node->edgesWrongWay[augmentingPathElement.followedEdgeIndex], node->edgesWrongWay.back());
                    node->edgesWrongWay.pop_back();

                    otherNode->edgesRightWay.push_back(edgeFollowed);
                }
            }
            numMatches++;
        }
    }
    return numMatches;
}

int maxMatch(vector<Client> clients, vector<House> houses)
{
    // Build the flow network representing the possible matchings between clients and houses.
    const int numNodes = 1 + // Source
                         1 + // Sink
                         clients.size() +
                         houses.size();

    vector<NetworkFlow::Node> allNodes;
    allNodes.reserve(numNodes);

    allNodes.push_back(NetworkFlow::Node{});
    auto sourceNode = &(allNodes.back());
    allNodes.push_back(NetworkFlow::Node{});
    auto sinkNode = &(allNodes.back());

    for (auto& client : clients)
    {
        allNodes.push_back(NetworkFlow::Node{});
        client.networkFlowNode = &(allNodes.back());
    }
    for (auto& house : houses)
    {
        allNodes.push_back(NetworkFlow::Node{});
        house.networkFlowNode = &(allNodes.back());
    }
    int numEdges = clients.size() + // Source -> Client
                   houses.size();   // House -> Sink.

    auto canMatchHouseAndClient = [](const Client& client, const House& house)
        {
            return (house.price <= client.maxPrice && house.area >= client.minArea);
        };

    for (auto& client : clients)
    {
        for (auto& house : houses)
        {
            if (canMatchHouseAndClient(client, house))
                numEdges++;
        }
    }

    vector<NetworkFlow::Edge> allEdges;
    allEdges.reserve(numEdges);

    for (auto& client : clients)
    {
        auto clientNode = client.networkFlowNode;
        allEdges.push_back(NetworkFlow::Edge(sourceNode, clientNode));
        auto sourceClientEdge = &(allEdges.back());
        sourceNode->edges.push_back(sourceClientEdge);
        clientNode->edges.push_back(sourceClientEdge);
    }
    for (auto& house : houses)
    {
        auto houseNode = house.networkFlowNode;
        allEdges.push_back(NetworkFlow::Edge(houseNode, sinkNode));
        auto houseSinkEdge = &(allEdges.back());
        houseNode->edges.push_back(houseSinkEdge);
        sinkNode->edges.push_back(houseSinkEdge);
    }
    for (auto& client : clients)
    {
        auto clientNode = client.networkFlowNode;
        for (auto& house : houses)
        {
            auto houseNode = house.networkFlowNode;
            if (canMatchHouseAndClient(client, house))
            {
                allEdges.push_back(NetworkFlow::Edge(clientNode, houseNode));
                auto clientHouseEdge = &(allEdges.back());
                clientNode->edges.push_back(clientHouseEdge);
                houseNode->edges.push_back(clientHouseEdge);
            }
        }
    }
    // Find max matchings for this network.
    return maxMatch(allNodes, sourceNode, sinkNode);
}

int main()
{
    // "Easy" one; just need to be careful with implementation!
    // It's basically just a standard maximal matching problem; I use Ford-Fulkerson
    // with a depth-first search to find the augmenting path, and some
    // optimisations (edgesRightWay, etc) for cutting down the number of 
    // edges we need to explore from each Node.
    // Not much to say beyond that!
    int numClients, numHouses;
    cin >> numClients >> numHouses;

    vector<Client> clients;
    for (int i = 0; i < numClients; i++)
    {
        Client client;
        cin >> client.minArea;
        cin >> client.maxPrice;

        clients.push_back(client);
    }

    vector<House> houses;
    for (int i = 0; i < numHouses; i++)
    {
        House house;
        cin >> house.area;
        cin >> house.price;

        houses.push_back(house);
    }

    cout << maxMatch(clients, houses) << endl;
}
