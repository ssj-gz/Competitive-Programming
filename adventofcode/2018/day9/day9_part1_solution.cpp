#define _GLIBCXX_DEBUG       // Iterator safety; out-of-bounds access for Containers, etc.
#include <iostream>
#include <vector>
#include <regex>

#include <cassert>

using namespace std;

int main()
{
    std::regex gameSpecRegex(R"(^(\d+) players; last marble is worth (\d+) points$)");
    string gameSpecLine;
    getline(cin, gameSpecLine);
    std::smatch gameSpecMatch;
    const bool matchSuccessful = std::regex_match(gameSpecLine, gameSpecMatch, gameSpecRegex);
    assert(matchSuccessful);
    const int numPlayers = std::stoi(gameSpecMatch[1]);
    const int highestMarble = std::stoi(gameSpecMatch[2]);
    std::cout << "numPlayers: " << numPlayers << " highestMarble: " << highestMarble << std::endl;
    vector<int> marbles = { 0 };
    int nextMarbleValue = 1;
    int nextPlayerIndex = 0;
    int currentMarbleIndex = 0;
    vector<int64_t> playerScores(numPlayers);
    while (nextMarbleValue <= highestMarble)
    {
        std::cout << "nextMarbleValue: " << nextMarbleValue << std::endl;
        if (nextMarbleValue % 23 != 0)
        {
            const int marblePlacementIndex = (currentMarbleIndex + 2) % static_cast<int>(marbles.size());
            std::cout << "Not divisible by 23; inserting " << nextMarbleValue << " at index: " << marblePlacementIndex << std::endl;
            marbles.insert(marbles.begin() + marblePlacementIndex, nextMarbleValue);
            currentMarbleIndex = (marblePlacementIndex) % static_cast<int>(marbles.size());
        }
        else
        {
            std::cout << "Divisible by 23; removing" << std::endl;
            playerScores[nextPlayerIndex] += nextMarbleValue;
            const int marbleToRemoveIndex = (7 * static_cast<int>(marbles.size()) + currentMarbleIndex - 7) % static_cast<int>(marbles.size());
            std::cout << " marbleToRemoveIndex: " << marbleToRemoveIndex << " #marbles: " << marbles.size() << std::endl;
            std::cout << " marble to remove:" << (*(marbles.begin() + marbleToRemoveIndex)) << std::endl;
            playerScores[nextPlayerIndex] += *(marbles.begin() + marbleToRemoveIndex);
            marbles.erase(marbles.begin() + marbleToRemoveIndex);
            currentMarbleIndex = marbleToRemoveIndex % static_cast<int>(marbles.size());
            std::cout << "player index " << nextPlayerIndex << " now has score: " << playerScores[nextPlayerIndex] << std::endl;
        }
#if 0
        std::cout << "Marbles: "<<  std::endl;
        for (int i = 0; i < static_cast<int>(marbles.size()); i++)
        {
            if (i == currentMarbleIndex)
                std::cout << "(" << marbles[i] << ") ";
            else
                std::cout << marbles[i] << " ";
        }
        std::cout << endl;
#endif

        nextMarbleValue++;
        nextPlayerIndex = (nextPlayerIndex + 1) % numPlayers;
    }
    std::cout << " highest score: " << *std::max_element(playerScores.begin(), playerScores.end()) << std::endl;
    return 0;
}
