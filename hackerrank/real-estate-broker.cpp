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
            Edge *prevEdgeInAugmentingPath = nullptr;
            Node *prevNodeInAugmentingPath = nullptr;

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
    bool followedEdgeCorrectWay = true;
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
    pathSoFar.back().followedEdgeCorrectWay = false;
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

int maxMatch(const vector<NetworkFlow::Node*>& allNodes, NetworkFlow::Node* sourceNode, NetworkFlow::Node* sinkNode)
{
    // Use Ford-Fulkerson to see if we can find targetNumMatches matchings.
    using NetworkFlow::Node;
    using NetworkFlow::Edge;
    for (auto node : allNodes)
    {
        for (auto edge : node->edges)
        {
            if (edge->sourceNode == node)
            {
                node->edgesRightWay.push_back(edge);
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
                const bool followedEdgeCorrectWay = augmentingPathElement.followedEdgeCorrectWay;
                auto node = augmentingPathElement.node;
                Edge* edgeFollowed = (followedEdgeCorrectWay ? node->edgesRightWay[augmentingPathElement.followedEdgeIndex] :
                                                  node->edgesWrongWay[augmentingPathElement.followedEdgeIndex]);
                auto otherNode = (edgeFollowed->sourceNode == node ? edgeFollowed->destNode : edgeFollowed->sourceNode); 
                assert(edgeFollowed->isInMatching == !followedEdgeCorrectWay);
                edgeFollowed->isInMatching = !edgeFollowed->isInMatching;

                if (followedEdgeCorrectWay)
                {
                    node->edgesRightWay.erase(node->edgesRightWay.begin() + augmentingPathElement.followedEdgeIndex);
                    otherNode->edgesWrongWay.push_back(edgeFollowed);
                }
                else
                {
                    node->edgesWrongWay.erase(node->edgesWrongWay.begin() + augmentingPathElement.followedEdgeIndex);
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
                //cout << "Could match house " << house.price << "," << house.area << " with client: " << client.maxPrice << "," << client.minArea << endl;
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
