// Simon St James (ssjgz) 2017-7-14 09:41
#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;


int main() {
    // Easy one, though I made a few schoolboy errors along the way XD
    // So, we have a graph where nodes are cities and edges are (broken)
    // roads.  I claim the following:
    //
    //  In each connected component of a graph, the optimal arrangement is:
    //   a) fixing roads forming a spanning tree of the component and adding
    //      a library if costOfRoad < costOfLibrary; or
    //   b) fixing no roads and adding a library to every city if costOfRoad >= costOfLibrary.
    //
    // Sketch proof: in either case (i.e. costOfRoad < costOfLibrary vs not), an optimal arrangement
    // will contain no cycles of roads - else we could remove a road that is part of a 
    // cycle while preserving reachability of libraries, contradicting optimality of the arrangement.
    // Imagine that costOfRoad < costOfLibrary, and take an optimal arrangement.  Let OC1, OC2, ... OCk
    // be the original components (formed of broken roads), and C1, C2, ... , Cr be the components (formed
    // of repaired roads) in the optimal arrangement.  From above, any Ci contains no cycles and since it 
    // is connected, must be formed of a spanning tree of the nodes in Ci.
    // First, we claim that each Ci has exactly one library.  It must have at least one else no cities in
    // Ci would be able to reach one.  Imagine it has more than one: then we could remove one, and each
    // city in Ci would still be able to reach one of the remaining ones, contradicting optimality, hence result.
    // Second, we claim that {C1, C2, ... Cr} == { OC1, OC2, ... OCr}.  Assume otherwise.
    // Note that each Ci is contained within exactly one OCj: thus, by assumption, one of the OCi must contain
    // more than one of the Cjs - call these Ca and Cb.
    // Since Ca and Cb are contained in Ci, then there is a node (a, say) in Ca an a node(b, say) in Cb
    // such that a and b have a broken road between them.  But then we could form a new arrangement by
    // repairing this road and removing the library from Cb - the total "increase" in cost is 
    // costOfRoad - costOfLibrary which, since costOfRoad < costOfLibrary, is a decrease, contradicting
    // minimality of the arrangement.
    // Thus, each Ci is contained in exactly one OCj, and each OCj contains exactly one Ci - thus, 
    // {C1, C2, ... Cr} = {OC1, OC2, ... OCk}.
    // This proves what was needed for the case where costOfRoad < costOfLibrary; now for the case where
    // costOfRoad >= costOfLibrary.
    // Take an optimal arrangement: we claim that there can be no repaired roads in this arrangement.  Assume
    // otherwise.  First, we claim that in such an arrangement, there must be a pair of cities a and b connected
    // by a road R, where at least one of a and b have a library.  Consider the case where a' and b' are connected
    // by a road but neither a' nor b' has a library; then there must be some path from a' to a city c such that c
    // has a library. Let R be the last road in this path; a be c; and b be the city on the other end of R to c. 
    // Second, we  claim that it can't be the case that *both* a and b have a library.  Again, assume otherwise -
    // then we could remove the library from a: it can then reach the library in b, as could any other city whose reachable
    // library used to be a, so this is a valid arrangement that is cheaper than the "optimal" one.
    // To re-cap, then: if costOfRoad >= costOfLibrary, then given an optimal arrangement containing a road, we can find cities a and b
    // connected by a road R such that exactly one of a and b has a library.  Without loss of generality, we can
    // assume it is a.
    // But then we could form a new arrangement by removing the road R and adding a library to a: this preserves connectedness,
    // as any path between a city c and a library that relied on R is still connected to the library either at a or b.
    // The "increase" in cost is costOfLibrary - costOfRoad which, since costOfRoad >= costOfLibrary, is either the same
    // or a decrease.
    // Thus, if costOfRoad >= costOfLibrary, then an optimal arrangement can be found that has no repaired roads. Since
    // all cities must have a reachable library, then each city must contain a library.
    // Putting it all together:a 
    //  - if costOfRoad >= costOfLibrary, then the optimal arrangement has no repaired roads and a library in every city:
    //    cost is numCities * costOfLibrary;
    //  - if costOfRoad < costOfLibrary, then the optimal arrangement is a the cost of repairing a spanning tree of each 
    //    component in the original graph, plus the cost of adding a library to that component; if OC1, OC2, ... OCk
    //    are the components in the original graph, then the cost is k * costOfLibraries + sum over i = 1 ... k 
    // {costOfRoad * (number of cities in OCi - 1)}.
    int numQueries;
    cin >> numQueries;
    for (int query = 0; query < numQueries; query++)
    {
        int64_t numCities, numRoads, costOfLibrary, costOfRoad;
        cin >> numCities >> numRoads >> costOfLibrary >> costOfRoad;


        struct City
        {
            vector<City*> neighbours;
            bool isInKnownComponent = false;
        };
        vector<City> cities(numCities);
        for (int road = 0; road < numRoads; road++)
        {
            int u, v;
            cin >> u >> v;
            u--; // Make 0-relative.
            v--;
            cities[u].neighbours.push_back(&cities[v]);
            cities[v].neighbours.push_back(&cities[u]);
        }

        if (costOfRoad >= costOfLibrary)
        {
            cout << numCities * costOfLibrary << endl;
            continue;
        }

        int numConnectedComponents = 0;
        int64_t cost = 0;
        for (auto& city : cities)
        {
            if (city.isInKnownComponent)
                continue;

            city.isInKnownComponent = true;
            numConnectedComponents++;
            int numInComponent = 1;

            vector<City*> citiesToExplore = {&city};
            while (!citiesToExplore.empty())
            {
                vector<City*> nextCitiesToExplore;
                for (auto toExplore : citiesToExplore)
                {
                    for (auto neighbourCity : toExplore->neighbours)
                    {
                        if (!neighbourCity->isInKnownComponent)
                        {
                            numInComponent++;
                            neighbourCity->isInKnownComponent = true;
                            nextCitiesToExplore.push_back(neighbourCity);
                        }
                    }
                }
                citiesToExplore = nextCitiesToExplore;
            }

            cost += costOfLibrary + (numInComponent - 1) * costOfRoad;
        }
        cout << cost << endl;
    }
    return 0;
}
