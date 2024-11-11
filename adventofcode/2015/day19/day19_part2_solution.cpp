#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <regex>

#include <cassert>

using namespace std;

int main()
{
    std::regex replacementRegex(R"(^(\w+)\s+=>\s+(\w+)$)");
    string replacementString;
    vector<std::pair<string, string>> replacements;
    while (getline(cin, replacementString) && !replacementString.empty())
    {
        std::smatch replacementMatch;
        const bool matchSuccessful = std::regex_match(replacementString, replacementMatch, replacementRegex);
        assert(matchSuccessful);

        replacements.push_back({replacementMatch.str(1), replacementMatch.str(2)});
    }

    string startingString;
    getline(cin, startingString);
    assert(!startingString.empty());
    std::cout << "startingString: " << startingString << std::endl << std::endl;

    vector<string> toExplore = {startingString};
    set<string> seenStrings = {startingString};

    int numSteps = 1;
    while (!toExplore.empty())
    {
        std::cout << "#toExplore: " << toExplore.size() << std::endl;
        vector<string> nextToExplore;
        for (const auto& str : toExplore)
        {
            if (str == "e")
            {
                std::cout << "Done! numSteps: " << numSteps << std::endl;
                return 0;
            }
            std::cout << " str: " << str << std::endl;
            // Make the leftmost replacement we can.
            bool leftmostReplacementMade = false;
            for (string::size_type pos = 0; pos < str.size() && !leftmostReplacementMade; pos++)
            {
                for (const auto& [beforeString, afterString] : replacements)
                {
                    auto addReplacedString = [&seenStrings, &nextToExplore](const auto& str, const auto beforeStringPos, const auto& toReplace, const auto& replacement)
                    {
                        assert(toReplace.size() >= replacement.size());
                        string replacedString = str;
                        replacedString.replace(beforeStringPos, toReplace.size(), replacement);
                        assert(replacedString.size() <= str.size());
                        if (!seenStrings.contains(replacedString))
                        {
                            seenStrings.insert(replacedString);
                            nextToExplore.push_back(replacedString);
                        }
                    };
                    if (str.find(afterString) == pos)
                    {
                        std::cout << " found first replacement to be made at  " << pos << " " << afterString << " => " << beforeString << std::endl;
                        addReplacedString(str, pos, afterString, beforeString);
                        // Also add any replacements in range [pos, pos + afterString.size()], inclusive.
                        const auto replacementPosBegin = pos;
                        const auto replacementPosEnd = pos + afterString.size();
                        for (string::size_type insideReplacementPos = replacementPosBegin; insideReplacementPos < replacementPosEnd; insideReplacementPos++)
                        {
                            for (const auto& [beforeString, afterString] : replacements)
                            {
                                if (str.substr(insideReplacementPos, afterString.size()) == afterString)
                                {
                                    addReplacedString(str, insideReplacementPos, afterString, beforeString);
                                }
                            }
                        }

                        leftmostReplacementMade = true;
                        break;
                    }
                }


            }
        }
        toExplore = nextToExplore;
        numSteps++;
    }

    return 0;
}
