#include <iostream>
#include <regex>
#include <map>
#include <cassert>

using namespace std;

int numOrbits(const string& object, map<string, int>& numOrbitsLookup, const map<string, string>& orbitedBy)
{
    if (numOrbitsLookup.contains(object))
        return numOrbitsLookup[object];

    if (!orbitedBy.contains(object))
        return 0;

    const auto [object_unused, orbitee] = *orbitedBy.find(object);
    const int numOrbitsForObject = 1 + numOrbits(orbitee, numOrbitsLookup, orbitedBy);
    numOrbitsLookup[object] = numOrbitsForObject;

    return numOrbitsForObject;
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

    map<string, int> numOrbitsLookup;
    numOrbitsLookup["COM"] = 0;

    int totalOrbits = 0;
    for (const auto& [orbiter, orbitee] : orbitedBy)
    {
        totalOrbits += numOrbits(orbiter, numOrbitsLookup, orbitedBy);
    }

    cout << "totalOrbits: " << totalOrbits << endl;

}
