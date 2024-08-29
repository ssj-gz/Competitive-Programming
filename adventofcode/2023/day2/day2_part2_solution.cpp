#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <ranges>
#include <regex>
#include <cassert>

using namespace std;

int main()
{
    string gameLine;
    std::regex gameIdRegex(R"(^Game\s*(\d+)\s*:(.*)$)");
    int64_t sumOfPowers = 0;
    while (getline(cin, gameLine))
    {
        std::cout << "gameLine: " << gameLine << std::endl;
        std::smatch gameIdMatch;
        const bool matchSuccessful = std::regex_match(gameLine, gameIdMatch, gameIdRegex);
        assert(matchSuccessful);
        const int gameId = std::stoi(gameIdMatch[1]);
        const auto gameCubeDrawsDesc = gameIdMatch[2];
        std::cout << " gameId: " << gameId << std::endl;

        auto gameCubeDraws = std::views::split(std::string(gameCubeDrawsDesc), ';');
        map<string, int> minOfColourToBeValid;
        for (const auto& draw : gameCubeDraws)
        {
            string drawDesc(draw.begin(), draw.end());
            std::cout << " draw: "<< std::string(drawDesc) << std::endl;
            auto coloursDrawn = std::views::split(std::string(drawDesc), ',');

            map<string, int> numOfColourCubeDrawn;
            for (const auto& colourDraw : coloursDrawn)
            {
                string colourDrawDesc(colourDraw.begin(), colourDraw.end());
                istringstream colourDrawDescStream(colourDrawDesc);
                int numDrawn = -1;
                colourDrawDescStream >> numDrawn;
                assert(colourDrawDescStream);
                string colour;
                colourDrawDescStream >> colour;
                assert(colourDrawDescStream);
                assert(colour == "red" || colour == "green" || colour == "blue");
                numOfColourCubeDrawn[colour] += numDrawn;
                std::cout << "  drew " << numDrawn << " of colour: " << colour << std::endl;
            }
            for (const auto& [colour, numDrawn] : numOfColourCubeDrawn)
            {
                minOfColourToBeValid[colour] = std::max(minOfColourToBeValid[colour], numDrawn);
            }

        }
        const int64_t power = (minOfColourToBeValid["red"] * minOfColourToBeValid["green"] * minOfColourToBeValid["blue"]);
        std::cout << "  power: " << power << std::endl;
        sumOfPowers += power;
    }
    std::cout << sumOfPowers << std::endl;
}
