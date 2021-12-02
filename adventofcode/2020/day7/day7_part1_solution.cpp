#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <cassert>

using namespace std;

bool canContainDirectlyOrIndirectly(const string& outerBagDesc, const string& bagDescToContain, map<string, vector<string>>& bagsContainedBy)
{
    for (const auto& bagDesc : bagsContainedBy[outerBagDesc])
    {
        if (bagDesc == bagDescToContain)
            return true;
        if (canContainDirectlyOrIndirectly(bagDesc, bagDescToContain, bagsContainedBy))
            return true;
    }
    return false;
}

int main()
{
    string line;
    map<string, vector<string>> bagsContainedBy;
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

            bagsContainedBy[outerBagDesc].push_back(innerBagDesc);

            lineStream >> str;
            const char punctuation = str.back();

            assert(punctuation == '.' || punctuation == ',');
            if (punctuation == '.')
                break;
        }

    }

    int numPossibleOuterBags = 0;
    for (const auto& [outerBagDesc, unused] : bagsContainedBy)
    {
        if (outerBagDesc == "shiny,gold")
            continue;
        if (canContainDirectlyOrIndirectly(outerBagDesc, "shiny,gold", bagsContainedBy))
        {
            numPossibleOuterBags++;
            cout << " Possible outerBagDesc: " << outerBagDesc << endl;
        }
    }
    cout << numPossibleOuterBags << endl;

}
