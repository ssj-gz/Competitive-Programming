#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main()
{
    int T;
    cin >> T;
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
                    unknownDistCity->distFromStart = currentLongestPath + 1;
                    numWithNextLongestPath++;
                }
            }
            // Cities with currentLongestPath play no further part; remove them.
            activeCities.erase(remove_if(activeCities.begin(), activeCities.end(), [currentLongestPath](City* city)
                        {
                            return city->distFromStart == currentLongestPath;
                        }), activeCities.end());

            currentLongestPath = currentLongestPath + 1;
            numWithCurrentLongestPath = numWithNextLongestPath;
        }

        for (auto city : allCities)
        {
            if (city.distFromStart == 0)
            {
                // The challenge tells us not to output the start node.
                continue;
            }
            cout << city.distFromStart << " ";
        }
        cout << endl;
    }
}
