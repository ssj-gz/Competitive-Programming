// Simon St James (ssjgz) - 2017-08-12 11:13
#include <iostream>
#include <vector>
#include <queue>
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
    // A nice, easy 80 points :)
    // Firstly: the rather cryptic description of fares boils down to "the fare of a path P from station A to station B
    // is equal to the largest fare between consecutive stations in P" - this is almost easily spottable
    // from the example they give, and also is very easily proven by induction on the length of P (proof omitted).
    // Now: imagine we add links between stations in order of the (direct) cost between them: if firstStation and lastStation
    // end up in the same component when we add in link X, then that means that there is a path P from firstStation to lastStation
    // where all cost are at most equal to the cost of link X i.e. there is a path P from firstStation to lastStation whose
    // total fare is the cost of link X.
    // But adding in links successively in order of cost and tracking which nodes belong to the same component is almost precisely
    // Kruskal's algorithm - we just need to modify it to stop when firstStation and lastStation are in the same component!
    // So I pretty much just copied my solution from "Kruskal's Really Special Subtree" XD
    // If firstStation and lastStation never end up in the same component after adding all links, then there cannot be a path between
    // them.
    ios::sync_with_stdio(false);
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

    // Start preparing for Kruskal.
    auto edgeCompare = [](const Edge* lhs, const Edge* rhs)
    {
        if (lhs->fare != rhs->fare)
            return lhs->fare > rhs->fare;

        // Allow us to store multiple edges with the same
        // fare.
        return lhs > rhs;
    };

    priority_queue<Edge*, vector<Edge*>, decltype(edgeCompare)> unaddedLinks(edgeCompare);

    vector<vector<Station*>> stationsInComponent;

    for (auto& edge : allEdges)
    {
        unaddedLinks.push(&edge);
    }

    int smallestFare = -1;
    while (!unaddedLinks.empty())
    {
        const Edge* edge = unaddedLinks.top();
        unaddedLinks.pop();

        Station *stationA = edge->stationA;
        Station *stationB = edge->stationB;

        if (stationA->componentNum == -1 && stationB->componentNum == -1)
        {
            // Join stations, and create brand-new component.
            stationsInComponent.push_back({stationA, stationB});
            stationA->componentNum = stationsInComponent.size() - 1;
            stationB->componentNum = stationsInComponent.size() - 1;
            continue;
        }
        if (stationA->componentNum == stationB->componentNum)
        {
            // This link doesn't do anything; ignore.
            continue;
        }
        // Have two stations, at least one of which is in a component, and they are
        // not in the same component.
        int absorbingComponent = -1;
        int absorbedComponent = -1;
        Station *absorbedStation = nullptr;
        // If either Station has no component, then it is absorbed into the other stations' component; otherwise, the larger
        // component absorbs the smaller.
        if (stationA->componentNum == -1 || (stationB->componentNum != -1 && stationsInComponent[stationA->componentNum].size() < stationsInComponent[stationB->componentNum].size() ))
        {
            absorbingComponent = stationB->componentNum;
            absorbedComponent = stationA->componentNum;
            absorbedStation = stationA;
        }
        else
        {
            absorbingComponent = stationA->componentNum;
            absorbedComponent = stationB->componentNum;
            absorbedStation = stationB;
        }
        assert(absorbingComponent != -1);
        if (absorbedComponent != -1)
        {
            for (auto stationToAbsorb : stationsInComponent[absorbedComponent])
            {
                stationToAbsorb->componentNum = absorbingComponent;
                stationsInComponent[absorbingComponent].push_back(stationToAbsorb);
            }
        }
        else
        {
            absorbedStation->componentNum = absorbingComponent;
            stationsInComponent[absorbingComponent].push_back(absorbedStation);
        }

        if ((firstStation->componentNum != -1 && lastStation->componentNum != -1) && firstStation->componentNum == lastStation->componentNum)
        {
            // We've found the cheapest path between firstStation and lastStation - stop here!
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

