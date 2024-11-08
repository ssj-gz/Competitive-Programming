#include <iostream>
#include <regex>

#include <cassert>

using namespace std;

int main()
{
    std::regex instructionRegex(R"(^(turn on|turn off|toggle) (\d+),(\d+) through (\d+),(\d+)$)");
    string instruction;
    constexpr auto width = 1'000;
    constexpr auto height = 1'000;
    vector<vector<int>> lightBrightness(width, vector<int>(height, 0));
    while (getline(cin, instruction))
    {
        std::cout << "instruction: " << instruction << std::endl;
        std::smatch instructionMatch;
        const bool matchSuccessful = std::regex_match(instruction, instructionMatch, instructionRegex);
        assert(matchSuccessful);
        const auto command = instructionMatch[1];
        const int rectangleTLX = std::stoi(instructionMatch[2]);
        const int rectangleTLY = std::stoi(instructionMatch[3]);
        const int rectangleBRX = std::stoi(instructionMatch[4]);
        const int rectangleBRY = std::stoi(instructionMatch[5]);
        assert(rectangleTLX <= rectangleBRX);
        assert(rectangleTLY <= rectangleBRY);
        if (command == "turn on")
        {
            for (int x = rectangleTLX; x <= rectangleBRX; x++)
            {
                for (int y = rectangleTLY; y <= rectangleBRY; y++)
                {
                    lightBrightness[x][y]++;
                }
            }
        }
        else if (command == "turn off")
        {
            for (int x = rectangleTLX; x <= rectangleBRX; x++)
            {
                for (int y = rectangleTLY; y <= rectangleBRY; y++)
                {
                    lightBrightness[x][y] = std::max(lightBrightness[x][y] - 1, 0);
                }
            }
        }
        else if (command == "toggle")
        {
            for (int x = rectangleTLX; x <= rectangleBRX; x++)
            {
                for (int y = rectangleTLY; y <= rectangleBRY; y++)
                {
                    lightBrightness[x][y] += 2;
                }
            }
        }
        else
            assert(false);

    }

    int totalBrightness = 0;
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            totalBrightness += lightBrightness[x][y];
        }
    }
    std::cout << "totalBrightness: " << totalBrightness << std::endl;
    return 0;
}
