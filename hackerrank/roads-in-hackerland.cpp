#include <cmath>
#include <cstdio>
#include <vector>
#include <list>
#include <set>
#include <iostream>
#include <algorithm>
using namespace std;


class Road;
class City
{
    public:
        long id;
        list<Road*> roads;
        bool isInMST = false;
        long numCitiesBridgedInMDS = 0;
};

class Road
{
    public:
        long id;
        City *city1;
        City *city2;
        long numOccurrencesInMDS = 0;
};

string longToBinaryString(long n)
{
    string binaryString;
    while (n > 0)
    {
        if ((n & 1) == 0)
            binaryString.push_back('0');
        else
            binaryString.push_back('1');

        n >>= 1;
    }
    reverse(binaryString.begin(), binaryString.end());
    return binaryString;
}

int main() {
    // Surprisingly involved, this one!  Let G = (V,E) be a graph.  Consider the following
    // way of generating successive subgraphs Gi of G, where G0 = G:
    //  For a graph Gi, let Gi+1 be the graph formed from Gi by removing the edge ei, where
    // ei is the highest-weighted edge in Gi whose removal would not disconnect Gi.

    // I'm pretty sure I have proofs for all the following, but unfortunately this editor is too
    // small to contain them:

    // Theorem 1: The process outlined above terminates, and for a general graph G, yields
    // a Minimum Spanning Tree of G.

    // Let A(G) be the resulting MST of G resulting from applying the process above to a graph
    // G.

    // Theorem 2: For a graph G, define MDS(G) to be the "Minimum Distance Sum" of G, that is,
    // MDS(G) is the sum over all pairs of vertices in V of the weight of the minimum-weighted
    // path connecting that pair in G.  If G is a "hackerland" graph, then MDS(A(G)) = MDS(G).*

    // Corollary: If T is the (unique) MST of a Hackerland graph G, then MDS(T) = MDS(G). 

    // So the problem reduces to computing the MST of a Hackerland graph G (easy with Prim's
    // algorithm) and then computing MDS(MST(G)).  Note first of all that since we are dealing with
    // a tree, the minimum distance path between any pair of vertices in MST is the only path
    // between them.

    // We compute the MDS of the MST by counting the number of times, for each edge e in MST,
    // that e appears in a minimum distance path between a pair of vertices in MST.
    // Imagine that the removal of e separates T into two subtrees, T1 and T2.  Then e does not 
    // appear in the path between any pair of vertices which are both in T1, nor between any pair
    // of vertices which are both in T2.  It does appear |T1||T2| times in the minimum paths between
    // a vertex in T1 and a vertex in T2.  We compute this numOccurrencesInMDS for each edge e by 
    // gradually dismantling, one single-degree vertex at a time, the MST and keeping track of the 
    // numCitiesBridgedInMDS - this is essentially the number of (now-deleted) vertices a given node 
    // acts as a bridge to for the remaining vertices.  This allows us to keep track of |T1| and
    // |T2| for each edge until no more edges remain.

    // Once we have the numOccurrencesInMDS for all edges, it's a simple matter to compute 
    // the binary representation of MDS(MST(G)) (computing the decimal representation would be much
    // harder!).
    long numCities, numRoads;
    cin >> numCities >> numRoads;

    // Read in and build graph.
    vector<City*> citiesById(numCities);
    vector<Road*> roadsById(numRoads);
    for (long c = 0; c < numCities; c++ )
    {
        City* city = new City;
        city->id = c;
        citiesById[c] = city;
    }
    for (long r = 0; r < numRoads; r++)
    {
        long city1Id, city2Id, roadId;
        cin >> city1Id >> city2Id >> roadId;
        city1Id--;
        city2Id--;
        Road *road = new Road;
        road->id = roadId;
        road->city1 = citiesById[city1Id];
        road->city2 = citiesById[city2Id];
        roadsById[roadId] = road;
        road->city1->roads.push_back(road);
        road->city2->roads.push_back(road);
    }

    // Build MST using Prim's Algorithm: as we go, construct list of pairs of 
    // <road, newly-connected city> we added to form our MST - it will be useful
    // later.
    set<Road*, bool(*)(Road*, Road*)> candidateRoadsSortedByWeight([](Road* lhs, Road* rhs) -> bool{ return lhs->id < rhs->id;});
    list<pair<Road*, City*>> addedRoadsAndCitiesInMST;
    auto addIncidentRoads = [&candidateRoadsSortedByWeight](City* city)
    {
        candidateRoadsSortedByWeight.insert(city->roads.begin(), city->roads.end());
        city->isInMST = true;
    };
    // Always incorporate the road with the lowest weight.
    addIncidentRoads(roadsById[0]->city1);
    addIncidentRoads(roadsById[0]->city2);
    addedRoadsAndCitiesInMST.push_back(make_pair(roadsById[0], roadsById[0]->city2));

    while (!candidateRoadsSortedByWeight.empty())
    {
        auto leastWeightCandidate = *candidateRoadsSortedByWeight.begin();
        if ((leastWeightCandidate->city1->isInMST && leastWeightCandidate->city2->isInMST) ||
                (!leastWeightCandidate->city1->isInMST && !leastWeightCandidate->city2->isInMST))
        {
            // Candidate road does not connect vertex in MST with vertex not in MST.
            candidateRoadsSortedByWeight.erase(candidateRoadsSortedByWeight.begin());
            continue;
        }

        auto nextRoadInMST = leastWeightCandidate;
        auto nextCityInMST = nextRoadInMST->city1->isInMST ? nextRoadInMST->city2 : nextRoadInMST->city1;

        addedRoadsAndCitiesInMST.push_back(make_pair(nextRoadInMST, nextCityInMST));
        addIncidentRoads(nextCityInMST);
    }

    // Compute the number of times each road in the MST appears in the MDS.  To do this,
    // we need to be able to "peel off", at each step and starting with our MST, a vertex with 
    // degree 1 and its incident edge: as it happens, the list of vertices and incident edges we
    // added when computing the MST, in reverse, gives us precisely this.
    reverse(addedRoadsAndCitiesInMST.begin(), addedRoadsAndCitiesInMST.end());
    long numCitiesRemaining = numCities;
    for (const auto& x : addedRoadsAndCitiesInMST)
    {
        auto degree1City = x.second; 
        auto linkingRoad = x.first;
        numCitiesRemaining--;
        linkingRoad->numOccurrencesInMDS += (numCities - (degree1City->numCitiesBridgedInMDS + 1))  * (degree1City->numCitiesBridgedInMDS + 1);
        auto otherCity = (linkingRoad->city1 == degree1City) ? linkingRoad->city2 : linkingRoad->city1;
        otherCity->numCitiesBridgedInMDS += (degree1City->numCitiesBridgedInMDS + 1);
    }

    // Compute the binary representation.  For a road r with id i, the contribution of r
    // to the MDS is (r->numOccurrencesInMDS) * (1 << i).  If we go in order of i, then when
    // we reach a road with id j, we know we have everything we need to compute the first 
    // (j - 1) digits of the binary representation of the MDS.  Thus, we don't actually need
    // to compute the MDS itself: we can easily generate the binary digits one by one.
    string MDSBinary;
    long currentValue = 0;
    for (auto road : roadsById)
    {
        currentValue += road->numOccurrencesInMDS;
        if ((currentValue & 1) == 0)
        {
            MDSBinary.push_back('0');
        }
        else
        {
            MDSBinary.push_back('1');
        }
        currentValue >>= 1;
    }
    // We've computed the string so far in reverse; reverse it ...
    reverse(MDSBinary.begin(), MDSBinary.end());
    // ... and prepend the remainder as a binary string.
    MDSBinary = longToBinaryString(currentValue) + MDSBinary;

    // Trim the leading zeros.
    auto firstOne = std::find(MDSBinary.begin(), MDSBinary.end(), '1');
    if (firstOne == MDSBinary.end())
    {
        cout << "0" << endl;
    }
    else
    {
        string trimmedLeadingZeros(firstOne, MDSBinary.end()) ;
        cout << trimmedLeadingZeros << endl;
    }

    return 0;
}

// * - note that this does not hold for arbitrary graphs G - in particular, consider the complete graph on n 
// vertices where each edge has weight 1.  The property of "Hackerland" graphs that makes this hold, very
// informally, is the fact that for any edge e = uv in G, if there is a path P in G from u to v where every
// edge e' in P has w(e') < w(e), then w(P) < w(e).  Essentially, we can find edges that cannot possibly occur
// in the MDS of G, as there is a "cheaper" option than going over that edge.  The inequality above 
// is informally due to the fact that 2^N = 2^0 + 2^1 + ... + 2^(n-1) + 1.  In the description of A(G) above, the
// ei are precisely these vertices that cannot occur in the MDS.

