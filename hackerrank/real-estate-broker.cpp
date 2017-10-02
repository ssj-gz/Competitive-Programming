#include <iostream>
#include <vector>
#include <list>
#include <set>
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

int maxMatch(const vector<NetworkFlow::Node*>& allNodes, NetworkFlow::Node* sourceNode, NetworkFlow::Node* sinkNode)
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
    while (true)
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
            return numMatches;
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
    return numMatches;
}

int maxMatch(vector<Client> clients, vector<House> houses)
{
    // Convert our original graph of Clients and Racers and Pairings to a NetworkFlow graph.
    // A "flow" goes from source -> one of the clients -> one of the houses -> sink.
    // TODO - we only really need to do this once (we can efficiently filter out the edges > maxWeight
    // when doing the Ford-Fulkerson run) - doing it every time is not exactly efficient!
    // Note that there are some minor optimisations here based on the fact that the capacities are always
    // either 0 or 1 (so we use a boolean isInMatching instead of integral flows).
    vector<NetworkFlow::Node*> nfClientNodes;
    vector<NetworkFlow::Node*> nfHouseNodes;

    for (auto& client : clients)
    {
        nfClientNodes.push_back(new NetworkFlow::Node());
        client.networkFlowNode = nfClientNodes.back();
    }
    for (auto& house : houses)
    {
        nfHouseNodes.push_back(new NetworkFlow::Node());
        house.networkFlowNode = nfHouseNodes.back();
    }
    NetworkFlow::Node *sourceNode = new NetworkFlow::Node();
    NetworkFlow::Node *sinkNode = new NetworkFlow::Node();
    for (auto& client : clients)
    {
        auto clientNode = client.networkFlowNode;
        NetworkFlow::Edge * sourceClientEdge = new NetworkFlow::Edge(sourceNode, clientNode);
        sourceNode->edges.push_back(sourceClientEdge);
        clientNode->edges.push_back(sourceClientEdge);
    }
    for (auto& house : houses)
    {
        auto houseNode = house.networkFlowNode;
        NetworkFlow::Edge * houseSinkEdge = new NetworkFlow::Edge(houseNode, sinkNode);
        houseNode->edges.push_back(houseSinkEdge);
        sinkNode->edges.push_back(houseSinkEdge);
    }
    for (auto& client : clients)
    {
        auto clientNode = client.networkFlowNode;
        for (auto& house : houses)
        {
            auto houseNode = house.networkFlowNode;
            if (house.price <= client.maxPrice && house.area >= client.minArea)
            {
                cout << "Could match house " << house.price << "," << house.area << " with client: " << client.maxPrice << "," << client.minArea << endl;
                NetworkFlow::Edge * clientHouseEdge = new NetworkFlow::Edge(clientNode, houseNode);
                clientNode->edges.push_back(clientHouseEdge);
                houseNode->edges.push_back(clientHouseEdge);
            }
        }
    }
    vector<NetworkFlow::Node*> allNFNodes(nfClientNodes);
    allNFNodes.insert(allNFNodes.end(), nfHouseNodes.begin(), nfHouseNodes.end());
    allNFNodes.push_back(sourceNode);
    allNFNodes.push_back(sinkNode);

    return maxMatch(allNFNodes, sourceNode, sinkNode);
}

int main()
{
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
