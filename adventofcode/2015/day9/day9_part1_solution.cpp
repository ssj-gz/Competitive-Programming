#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <regex>
#include <limits>

#include <cassert>

using namespace std;

struct Destination
{
    string name;
    int64_t distance = -1;
};

void calcShortestDistance(const string& currentPlace, const set<string>& allPlaces, const int64_t distanceSoFar, set<string>& alreadyVisited, map<string, vector<Destination>>& destinationsForOrigin, int64_t& shortestDistance)
{
    alreadyVisited.insert(currentPlace);

    if (alreadyVisited == allPlaces)
    {
        if (distanceSoFar < shortestDistance)
        {
            shortestDistance = distanceSoFar;
            std::cout << "New shortestDistance: " << shortestDistance << std::endl; 
        }
    }

    for (const auto& destination : destinationsForOrigin[currentPlace])
    {
        if (alreadyVisited.contains(destination.name))
            continue;

        calcShortestDistance(destination.name, allPlaces, distanceSoFar + destination.distance, alreadyVisited, destinationsForOrigin, shortestDistance);
    }
    alreadyVisited.erase(currentPlace);
}

int main()
{
    std::regex distanceRegex(R"(^(\w+) to (\w+) = (\d+)$)");
    string distanceLine;

    map<string, vector<Destination>> destinationsForOrigin;
    set<string> allPlaces;

    while (getline(cin, distanceLine))
    {
        std::smatch distanceMatch;
        const bool matchSuccessful = std::regex_match(distanceLine, distanceMatch, distanceRegex);
        assert(matchSuccessful);

        const string originName = distanceMatch.str(1);
        const string destinationName = distanceMatch.str(2);
        const auto distance = std::stoll(distanceMatch.str(3));

        destinationsForOrigin[originName].push_back({destinationName, distance});
        destinationsForOrigin[destinationName].push_back({originName, distance});
        allPlaces.insert(originName);
        allPlaces.insert(destinationName);
    }

    int64_t shortestDistance = std::numeric_limits<int64_t>::max();
    set<string> alreadyVisited;
    for (const auto& startPlaceName : allPlaces)
    {
        calcShortestDistance(startPlaceName, allPlaces, 0, alreadyVisited, destinationsForOrigin, shortestDistance);
    }
    std::cout << "shortestDistance: " << shortestDistance << std::endl;
    return 0;
}
