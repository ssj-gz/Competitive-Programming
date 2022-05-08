#include <iostream>
#include <regex>
#include <map>
#include <limits>
#include <cassert>

using namespace std;

void markDistances(const string& object, int distanceTravelled, const map<string, string>& orbitedBy, map<string, int>& distanceTo)
{
    if (!orbitedBy.contains(object))
        return;

    cout << "distanceTo: " << object << " = " << distanceTravelled << endl;
    distanceTo[object] = distanceTravelled;

    const auto [object_unused, orbitee] = *orbitedBy.find(object);
    markDistances(orbitee, distanceTravelled + 1, orbitedBy, distanceTo);
}

int main()
{
    string orbitInfoLine;
    map<string, string> orbitedBy;
    while (getline(cin, orbitInfoLine))
    {
        static regex orbitRegex(R"(^(.*)\)(.*)$)");
        std::smatch orbitMatch;
        const bool matched = std::regex_match(orbitInfoLine, orbitMatch, orbitRegex);
        assert(matched);
        assert(orbitMatch.size() == 3);
        const string orbitee = orbitMatch[1].str();
        const string orbiter = orbitMatch[2].str();
        assert(!orbitedBy.contains(orbiter));
        orbitedBy[orbiter] = orbitee;
    }

    map<string, int> distanceFromYou;
    markDistances("YOU", 0, orbitedBy, distanceFromYou);
    map<string, int> distanceFromSan;
    markDistances("SAN", 0, orbitedBy, distanceFromSan);

    cout << "All distances computed" << endl;
    int bestDistance = std::numeric_limits<int>::max();
    for (const auto& [orbiter, distance] : distanceFromYou)
    {
        if (!distanceFromSan.contains(orbiter))
        {
            cout << "The object " << orbiter << " can be reached from YOU, but not from SAN" << endl;
            continue;
        }
        cout << "Distance YOU -> " << orbiter << ": " << distance << ". Distance SAN -> " << orbiter << ": " << distanceFromSan[orbiter] << endl;
        const int totalDistance = (distanceFromYou[orbiter] - 1) + (distanceFromSan[orbiter] - 1);
        bestDistance = min(bestDistance, totalDistance);
    }
    cout << "bestDistance: " << bestDistance << endl;


}
