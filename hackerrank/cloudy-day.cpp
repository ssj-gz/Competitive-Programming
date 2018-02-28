// Simon St James (ssjgz) - 2018-02-28
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>

using namespace std;

int main()
{
    int numTowns;
    cin >> numTowns;

    struct Town
    {
        int location = -1;
        int population = -1;
        int townIndex = -1;
    };

    vector<Town> towns(numTowns);
    for (int i = 0; i < numTowns; i++)
    {
        cin >> towns[i].population;
        towns[i].townIndex = i;
    }

    for (int i = 0; i < numTowns; i++)
    {
        cin >> towns[i].location;
    }

    int numClouds;
    cin >> numClouds;

    struct Cloud
    {
        int location = -1;
        int range = -1;
        int cloudIndex = -1;

        int startPoint = -1;
        int endPoint = -1;
    };

    vector<Cloud> clouds(numClouds);
    for (int cloudIndex = 0; cloudIndex < numClouds; cloudIndex++)
    {
        auto& cloud = clouds[cloudIndex];
        cin >> cloud.location;
        cloud.cloudIndex = cloudIndex;
    }
    for (int cloudIndex = 0; cloudIndex < numClouds; cloudIndex++)
    {
        auto& cloud = clouds[cloudIndex];
        cin >> cloud.range;
        cloud.startPoint = cloud.location - cloud.range;
        cloud.endPoint = cloud.location + cloud.range;
    }

    vector<Cloud> cloudsByStartPoint(clouds);
    sort(cloudsByStartPoint.begin(), cloudsByStartPoint.end(), [](const Cloud& lhs, const Cloud& rhs)
            {
                return lhs.startPoint < rhs.startPoint;
            });

    auto compareEndpoints = [](const Cloud& lhs, const Cloud& rhs)
        {
            if (lhs.endPoint != rhs.endPoint)
                return lhs.endPoint < rhs.endPoint;

            // Allow multiple Clouds with the same endpoint!
            return lhs.cloudIndex < rhs.cloudIndex;
        };
    set<Cloud, decltype(compareEndpoints)> cloudsCoveringCurrentPosByEndpoint(compareEndpoints);

    auto nextCloudIter = cloudsByStartPoint.begin();

    vector<Town> townsByLocation(towns);
    sort(townsByLocation.begin(), townsByLocation.end(), [](const Town& lhs, const Town& rhs)
            {
                if (lhs.location != rhs.location)
                    return lhs.location < rhs.location;
                // Allow multiple towns at the same location!
                return lhs.townIndex < rhs.townIndex;
            });

    int populationOriginallySunny = 0;
    vector<int> populationCoveredJustByThisCloud(numClouds, 0);
    for (const auto& town : townsByLocation)
    {
        while (nextCloudIter != cloudsByStartPoint.end() && nextCloudIter->startPoint <= town.location)
        {
            cloudsCoveringCurrentPosByEndpoint.insert(*nextCloudIter);
            nextCloudIter++;
        }
        while (!cloudsCoveringCurrentPosByEndpoint.empty() && cloudsCoveringCurrentPosByEndpoint.begin()->endPoint < town.location)
        {
            cloudsCoveringCurrentPosByEndpoint.erase(cloudsCoveringCurrentPosByEndpoint.begin());
        }

        if (cloudsCoveringCurrentPosByEndpoint.size() == 1)
        {
            const auto soleCoveringCloudIndex = cloudsCoveringCurrentPosByEndpoint.begin()->cloudIndex;
            populationCoveredJustByThisCloud[soleCoveringCloudIndex] += town.population;
        }
        else if (cloudsCoveringCurrentPosByEndpoint.empty())
        {
            populationOriginallySunny += town.population;
        }
    }
    const int maxNewSunnyByRemovingCloud = *max_element(populationCoveredJustByThisCloud.begin(), populationCoveredJustByThisCloud.end());
    const int maxSunnyAfterRemovingOneCloud = populationOriginallySunny + maxNewSunnyByRemovingCloud;

    cout << maxSunnyAfterRemovingOneCloud << endl;
}
