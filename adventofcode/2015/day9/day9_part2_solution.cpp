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

void calcLongestDistance(const string& currentPlace, const set<string>& allPlaces, const int64_t distanceSoFar, set<string>& alreadyVisited, map<string, vector<Destination>>& destinationsForOrigin, int64_t& longestDistance)
{
    alreadyVisited.insert(currentPlace);

    if (alreadyVisited == allPlaces)
    {
        if (distanceSoFar > longestDistance)
        {
            longestDistance = distanceSoFar;
            std::cout << "New longestDistance: " << longestDistance << std::endl; 
        }
    }

    for (const auto& destination : destinationsForOrigin[currentPlace])
    {
        if (alreadyVisited.contains(destination.name))
            continue;

        calcLongestDistance(destination.name, allPlaces, distanceSoFar + destination.distance, alreadyVisited, destinationsForOrigin, longestDistance);
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

    int64_t longestDistance = std::numeric_limits<int64_t>::min();
    set<string> alreadyVisited;
    for (const auto& startPlaceName : allPlaces)
    {
        calcLongestDistance(startPlaceName, allPlaces, 0, alreadyVisited, destinationsForOrigin, longestDistance);
    }
    std::cout << "longestDistance: " << longestDistance << std::endl;
    return 0;
}
