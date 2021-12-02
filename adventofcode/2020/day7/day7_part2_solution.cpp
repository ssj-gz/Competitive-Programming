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
        cout << " blee: " << bagDesc << " - " << count << endl;
        result += count * (1 + totalBagsContainedBy(bagDesc, bagsContainedBy, totalBagsContainedByLookup));
    }
    cout << "totalBagsContainedBy " << outerBagDesc << " = " << result << endl;
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
        cout << "outerBagDesc: " << outerBagDesc << endl;
        lineStream >> str;
        cout << " str: " << str << endl;
        assert(str == "bags");
        lineStream >> str;
        assert(str == "contain");
        while (true)
        {
            int numOfInner;
            if (!(lineStream >> numOfInner))
                break;

            string innerBagDesc;
            lineStream >> str;
            innerBagDesc += str;
            lineStream >> str;
            innerBagDesc += "," + str;
            cout << " " << numOfInner << " " << innerBagDesc << endl;

            bagsContainedBy[outerBagDesc].push_back({innerBagDesc, numOfInner});

            lineStream >> str;
            const char punctuation = str.back();

            assert(punctuation == '.' || punctuation == ',');
            if (punctuation == '.')
                break;
        }

    }

    map<string, int64_t> totalBagsContainedByLookup;
    cout << totalBagsContainedBy("shiny,gold", bagsContainedBy, totalBagsContainedByLookup) << endl;
}
