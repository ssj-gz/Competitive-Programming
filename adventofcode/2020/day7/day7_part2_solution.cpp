#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <cassert>

using namespace std;

int64_t totalBagsContainedBy(const string& outerBagDesc, map<string, vector<pair<string, int>>>& bagsContainedBy, map<string, int64_t>& totalBagsContainedByLookup)
{
    if (totalBagsContainedByLookup.find(outerBagDesc) != totalBagsContainedByLookup.end())
    {
        return totalBagsContainedByLookup[outerBagDesc];
    }
    int64_t result = 0;
    for (const auto& [bagDesc, count] : bagsContainedBy[outerBagDesc])
    {
        result += count * (1  // The directly contained bag itself ...
                           + totalBagsContainedBy(bagDesc, bagsContainedBy, totalBagsContainedByLookup) // ... plus all bags which the directly contained bag contains.
                          );
    }
    // Memo-ise so we can look up the answer if we're asked for it again.
    totalBagsContainedByLookup[outerBagDesc] = result;
    return result;
}

int main()
{
    string line;
    map<string, vector<pair<string, int>>> bagsContainedBy;
    while (std::getline(cin, line))
    {
        istringstream lineStream(line);
        string outerBagDesc;
        string str;
        lineStream >> str;
        outerBagDesc += str;
        lineStream >> str;
        outerBagDesc += "," + str;

        lineStream >> str;
        assert(str == "bags");
        lineStream >> str;
        assert(str == "contain");

        while (true)
        {
            int numOfInner;
            if (!(lineStream >> numOfInner))
            {
                // This outer bag contains no bags.
                break;
            }

            string innerBagDesc;
            lineStream >> str;
            innerBagDesc += str;
            lineStream >> str;
            innerBagDesc += "," + str;
            bagsContainedBy[outerBagDesc].push_back({innerBagDesc, numOfInner});

            lineStream >> str;
            const char punctuation = str.back();

            // Do we have more contained bags? ("," == yes; "." == no").
            assert(punctuation == '.' || punctuation == ',');
            if (punctuation == '.')
                break;
        }

    }

    map<string, int64_t> totalBagsContainedByLookup;
    cout << totalBagsContainedBy("shiny,gold", bagsContainedBy, totalBagsContainedByLookup) << endl;
}
