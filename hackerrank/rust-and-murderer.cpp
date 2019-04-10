#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main()
{
    int T;
    cin >> T;
    // Nice, easy 70 points :) What it's essentially asking is to perform a single-source
    // shortest path on the complement(?) of G.
    // Note first of all that given the maximum constraints (200000 cities; 5550 edges), 
    // a *huge* amount of cities will be reached in the first step from S - even if 
    // S is incident with all edges, there are still 200000 - 5550 cities that will be 
    // reached.  So in general, this should terminate after very few steps from S.
    // Now: assume that we have iterated k times in a BFS from S: then a given city has either
    // not yet been reached, or its distance from S has been calculated as some number <= k.
    // In the next iteration, given a city c whose distance is not yet known, when will c not
    // be visited this iteration? (NB: if it *is* visited this interation, it will be marked as being k + 1 from S).
    // It is not visited this iteration when and only when *it is connected to every single city whose distance 
    // from S is k* - a pretty tall order :) In other words, c will not be visited if and only if the number of cities directly
    // linked to c which have dist k from S is equal to the total number of cities which have 
    // dist k from S, all of which we can compute very easily.
    // Note that cities with distance < k play no part in this in future iterations: as an optimisation,
    // I remove them from the list of cities.
    struct City
    {
        int distFromStart = -1;
        vector<City*> linkedCities;
    };
    for (int t = 0; t < T; t++)
    {
        long N, M;
        cin >> N >> M;
        vector<City> allCities(N);
        for (int i = 0; i < M; i++)
        {
            long x, y;
            cin >> x >> y;
            // Make 0-relative.
            x--;
            y--;

            allCities[x].linkedCities.push_back(&allCities[y]);
            allCities[y].linkedCities.push_back(&allCities[x]);
        }

        long S;
        cin >> S;
        // Make 0-relative.
        S--;

        allCities[S].distFromStart = 0;
        int currentLongestPath = 0;
        int numWithCurrentLongestPath = 1;

        vector<City*> activeCities;
        for (auto& city : allCities)
        {
            activeCities.push_back(&city);
        }

        while (!activeCities.empty())
        {
            int numWithNextLongestPath = 0;
            for (auto unknownDistCity : activeCities)
            {
                if (unknownDistCity->distFromStart != -1)
                    continue;
                long numLinkedCitiesWithCurrentLongestPath = 0;
                for (auto linkedCity : unknownDistCity->linkedCities)
                {
                    if (linkedCity->distFromStart == currentLongestPath)
                    {
                        numLinkedCitiesWithCurrentLongestPath++;
                    }
                }
                if (numLinkedCitiesWithCurrentLongestPath != numWithCurrentLongestPath)
                {
                    // This city is visited this iteration, and so now we know its length.
                    unknownDistCity->distFromStart = currentLongestPath + 1;
                    numWithNextLongestPath++;
                }
            }

            // Finished updating cities for this iteration; those with distFromS == currentLongestPath 
            // play no further part; remove them as an optimisation.
            activeCities.erase(remove_if(activeCities.begin(), activeCities.end(), 
                        [currentLongestPath](City* city)
                        {
                        return city->distFromStart == currentLongestPath;
                        }), 
                    activeCities.end());

            currentLongestPath = currentLongestPath + 1;
            numWithCurrentLongestPath = numWithNextLongestPath;
        }

        for (auto city : allCities)
        {
            if (city.distFromStart == 0)
            {
                // The output format tells us not to output the start node's distance.
                continue;
            }
            cout << city.distFromStart << " ";
        }
        cout << endl;
    }
}


