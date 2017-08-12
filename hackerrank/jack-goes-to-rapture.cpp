// Simon St James (ssjgz) - 2017-08-12
#include <iostream>
#include <vector>
#include <set>
#include <cassert>

using namespace std;
struct Station;
struct Edge
{
    Edge() = default;
    Edge(Station* stationA, Station *stationB, int fare)
        : stationA(stationA), stationB(stationB), fare(fare)
    {

    }
    Station *stationA = nullptr;
    Station *stationB = nullptr;
    int fare = -1;
};
struct Station
{
    vector<Edge*> edges;
    int componentNum = -1;
};

int main() {
    int N, E;
    cin >> N >> E;
    vector<Station> allStation(N, Station{});
    vector<Edge> allEdges(E);
    for (int i = 0; i < E; i++)
    {
        int stationAIndex, stationBIndex, fare;
        cin >> stationAIndex >> stationBIndex >> fare;

        Station *stationA = &(allStation[stationAIndex - 1]);
        Station *stationB = &(allStation[stationBIndex - 1]);

        allEdges[i] = Edge(stationA, stationB, fare);

        stationA->edges.push_back(&(allEdges[i]));
        stationB->edges.push_back(&(allEdges[i]));
    }
    Station* firstStation = &(allStation.front());
    Station* lastStation = &(allStation.back());

    auto edgeCompare = [](const Edge* lhs, const Edge* rhs)
    {
        if (lhs->fare != rhs->fare)
            return lhs->fare < rhs->fare;

        // Allow us to store multiple edges with the same
        // fare.
        return lhs < rhs;
    };

    set<Edge*, decltype(edgeCompare)> unprocessedEdges(edgeCompare);

    vector<vector<Station*>> nodesInComponent;

    for (auto& edge : allEdges)
    {
        unprocessedEdges.insert(&edge);
    }

    int smallestFare = -1;
    while (!unprocessedEdges.empty())
    {
        const Edge* edge = *unprocessedEdges.begin();
        unprocessedEdges.erase(unprocessedEdges.begin());

        Station *stationA = edge->stationA;
        Station *stationB = edge->stationB;

        if (stationA->componentNum == -1 && stationB->componentNum == -1)
        {
            // Join nodes, and create brand-new component.
            nodesInComponent.push_back({stationA, stationB});
            stationA->componentNum = nodesInComponent.size() - 1;
            stationB->componentNum = nodesInComponent.size() - 1;
            continue;
        }
        if (stationA->componentNum == stationB->componentNum)
        {
            // Don't create a cycle.
            continue;
        }
        // Have two nodes, at least one of which is in a component, and they are
        // not in the same component.
        int absorbingComponent = -1;
        int absorbedComponent = -1;
        Station *absorbedNode = nullptr;
        // If either Station has no component, then it is absorbed into the other nodes' component; otherwise, the larger
        // component absorbs the smaller.
        if (stationA->componentNum == -1 || (stationB->componentNum != -1 && nodesInComponent[stationA->componentNum].size() < nodesInComponent[stationB->componentNum].size() ))
        {
            absorbingComponent = stationB->componentNum;
            absorbedComponent = stationA->componentNum;
            absorbedNode = stationA;
        }
        else
        {
            absorbingComponent = stationA->componentNum;
            absorbedComponent = stationB->componentNum;
            absorbedNode = stationB;
        }
        assert(absorbingComponent != -1);
        if (absorbedComponent != -1)
        {
            for (auto nodeToAbsorb : nodesInComponent[absorbedComponent])
            {
                nodeToAbsorb->componentNum = absorbingComponent;
                nodesInComponent[absorbingComponent].push_back(nodeToAbsorb);
            }
        }
        else
        {
            absorbedNode->componentNum = absorbingComponent;
            nodesInComponent[absorbingComponent].push_back(absorbedNode);
        }

        if ((firstStation->componentNum != -1 && lastStation->componentNum != -1) && firstStation->componentNum == lastStation->componentNum)
        {
            smallestFare = edge->fare;
            break;
        }
    }

    if (smallestFare >= 0)
        cout << smallestFare << endl;
    else
        cout << "NO PATH EXISTS" << endl;


    return 0;
}



