// Simon St James (ssjgz) - 2018-02-28
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>

using namespace std;

int main()
{
    // Very easy one - for each town from left to right, find the set of clouds that are covering that town
    // i.e. the number of intervals (where each interval is the region covered by a cloud - its startPoint to endPoint)
    // that contain the town's location.  There is a well-known algorithm for this involving sorting the intervals by their
    // left endpoint, and separately by their right: maintain an iterator (nextCloudIter) to the first interval that hasn't covered a town yet
    // (or which hasn't been shown not to cover a town) - when we reach a new town, iterate through all clouds in order
    // of their startPoint to find all that have their startPoint occur before the town's location (these may cover the town), and add them to
    // cloudsCoveringCurrentPosByEndpoint.
    // After doing this, find all clouds in cloudsCoveringCurrentPosByEndpoint whose endpoint ends before the current town location - easy as they
    // are sorted by their endpoints!
    // After this, cloudsCoveringCurrentPosByEndpoint is precisely the set of clouds that covers the current town.
    // If there is exactly one cloud c covering the current town, then make a note that removing c will make the population of the current town
    // sunny (i.e. add the population to populationCoveredJustByThisCloud).  
    // If there are no clouds covering the current town, then this town will always be sunny, so add it to populationOriginallySunny.
    // The final result is populationOriginallySunny plus the population made sunny by removing the cloud that makes the most new people sunny
    // i.e. the highest populationCoveredJustByThisCloud.
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
                if (lhs.startPoint != rhs.startPoint)
                    return lhs.startPoint < rhs.startPoint;
                // Allow multiple clouds with the same startpoint!
                return lhs.cloudIndex < rhs.cloudIndex;
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

    int64_t populationOriginallySunny = 0;
    vector<int64_t> populationCoveredJustByThisCloud(numClouds, 0);
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
    const auto maxNewSunnyByRemovingCloud = *max_element(populationCoveredJustByThisCloud.begin(), populationCoveredJustByThisCloud.end());
    const auto maxSunnyAfterRemovingOneCloud = populationOriginallySunny + maxNewSunnyByRemovingCloud;

    cout << maxSunnyAfterRemovingOneCloud << endl;
}
