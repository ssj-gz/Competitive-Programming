#include <iostream>
#include <map>
#include <sstream>
#include <algorithm>

#include <cassert>

using namespace std;

int main()
{
    string auntInfoLine;
    std::map<string, int> actualCompoundAmount = 
    {
        {"children", 3},
        {"cats", 7},
        {"samoyeds", 2},
        {"pomeranians", 3},
        {"akitas", 0},
        {"vizslas", 0},
        {"goldfish", 5},
        {"trees", 3},
        {"cars", 2},
        {"perfumes", 1}
    };
    int correctAuntId = -1;
    while (std::getline(cin, auntInfoLine))
    {
        // Sample line: Sue 495: vizslas: 1, akitas: 2, cats: 2
        std::replace(auntInfoLine.begin(), auntInfoLine.end(), ':', ' ');
        std::replace(auntInfoLine.begin(), auntInfoLine.end(), ',', ' ');
        std::cout << "auntInfoLine: " << auntInfoLine << std::endl;
        istringstream auntInfoStream(auntInfoLine);
        string dummy;
        auntInfoStream >> dummy;
        assert(dummy == "Sue");
        int auntId = -1;
        auntInfoStream >> auntId;

        bool couldBeCorrectAunt = true;
        while (true)
        {
            string compoundName;
            auntInfoStream >> compoundName;
            int compoundAmount;
            auntInfoStream >> compoundAmount;
            if (auntInfoStream)
            {
                if (compoundName == "cats" || compoundName == "trees")
                {
                    if (compoundAmount <= actualCompoundAmount[compoundName])
                        couldBeCorrectAunt = false;

                }
                else if (compoundName == "pomeranians" || compoundName == "goldfish")
                {
                    if (compoundAmount >= actualCompoundAmount[compoundName])
                        couldBeCorrectAunt = false;
                }
                else if (actualCompoundAmount[compoundName] != compoundAmount)
                {
                    couldBeCorrectAunt = false;
                }
            }
            else
                break;
        }
        if (couldBeCorrectAunt)
        {
            std::cout << "Could be: " << auntId << std::endl;
            assert(correctAuntId == -1);
            correctAuntId = auntId;

        }
    }
    std::cout << "correctAuntId: " << correctAuntId << std::endl;

    return 0;
}
