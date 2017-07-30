#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;


int main() {
    int numQueries;
    cin >> numQueries;
    for (int query = 0; query < numQueries; query++)
    {
        long numCities, numRoads, costOfLibrary, costOfRoad;
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
        long cost = 0;
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

