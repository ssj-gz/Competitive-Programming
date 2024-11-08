#include <iostream>
#include <map>
#include <set>
#include <regex>
#include <limits>

#include <cassert>

using namespace std;

int main()
{
    std::regex happinessRegex(R"(^(\w+) would (lose|gain) (\d+) happiness units by sitting next to (\w+)\.$)");
    string happinessLine;

    map<string, map<string, int>> happinessChangeWithNeighbour;
    set<string> allPeople;

    while (std::getline(cin, happinessLine))
    {
        std::cout << "happinessLine: " << happinessLine << std::endl;
        std::smatch happinessMatch;
        const bool matchSuccessful = std::regex_match(happinessLine, happinessMatch, happinessRegex);
        assert(matchSuccessful);

        const auto person = happinessMatch.str(1);
        const auto neighbour = happinessMatch.str(4);
        const auto absoluteHappinessChange = std::stoi(happinessMatch.str(3));
        const auto changeMultiplier = (happinessMatch.str(2) == "lose" ? -1 : +1);

        happinessChangeWithNeighbour[person][neighbour] = absoluteHappinessChange * changeMultiplier;
        allPeople.insert(person);
        allPeople.insert(neighbour);
        std::cout << person << "'s happiness changes by " << happinessChangeWithNeighbour[person][neighbour] << " if they sit next to " << neighbour << std::endl;
    }

    vector<string> seating(allPeople.begin(), allPeople.end());
    const auto originalSeating = seating;
    const int numPeople = static_cast<int>(allPeople.size());

    int64_t bestScore = std::numeric_limits<int64_t>::min();
    do
    {
        int64_t score = 0;
        for (int personIndex = 0; personIndex < numPeople; personIndex++)
        {
            const auto& person = seating[personIndex];
            const auto& neighbour1 = seating[(personIndex + 1) % numPeople];
            const auto& neighbour2 = seating[(personIndex + numPeople - 1) % numPeople];
            score += happinessChangeWithNeighbour[person][neighbour1];
            score += happinessChangeWithNeighbour[person][neighbour2];
        }
        if (score > bestScore)
        {
            bestScore = score;
            std::cout << "new bestScore: " << bestScore << std::endl;
        }

        std::next_permutation(seating.begin(), seating.end());
    } while (seating != originalSeating);

    std::cout << "bestScore: " << bestScore << std::endl;
    return 0;
}
