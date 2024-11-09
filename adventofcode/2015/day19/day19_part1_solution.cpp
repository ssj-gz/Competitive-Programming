#include <iostream>
#include <set>
#include <map>
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
    set<string> distinctReplacedStrings;

    for (const auto& [beforeString, afterString] : replacements)
    {
        string::size_type nextBeforeStringSearchPos = 0;
        std::cout << "Using replacement " << beforeString << " => " << afterString << std::endl;
        while (true)
        {
            const auto beforeStringPos = startingString.find(beforeString, nextBeforeStringSearchPos);
            if (beforeStringPos == string::npos)
            {
                break;
            }
            string replacedString = startingString;
            replacedString.replace(beforeStringPos, beforeString.size(), afterString);
            std::cout << " replacedString: " << replacedString << std::endl;
            distinctReplacedStrings.insert(replacedString);
            nextBeforeStringSearchPos = beforeStringPos + 1;
        }
    }
    std::cout << "# distinct replacedStrings: " << distinctReplacedStrings.size() << std::endl;

    return 0;
}
