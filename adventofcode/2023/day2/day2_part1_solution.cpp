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
    int validGameIdSum = 0;
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
        bool gameIsValid = true;
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

            if ((numOfColourCubeDrawn["red"] <= 12) && (numOfColourCubeDrawn["green"] <= 13) && (numOfColourCubeDrawn["blue"] <= 14))
            {
                std::cout << "  draw is valid" << std::endl;
            }
            else
            {
                std::cout << "  draw is *NOT* valid" << std::endl;
                gameIsValid = false;
            }
        }
        std::cout << " gameIsValid: " << gameIsValid << std::endl;
        if (gameIsValid)
            validGameIdSum += gameId;
    }
    std::cout << validGameIdSum << std::endl;
}
