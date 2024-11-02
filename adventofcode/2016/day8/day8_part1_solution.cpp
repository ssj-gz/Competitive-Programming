#include <iostream>
#include <regex>

#include <cassert>

using namespace std;

int mod(int x, int modulus)
{
    while (x < 0)
        x += modulus;
    return x % modulus;
};

int main()
{
    constexpr int screenWidth = 50;
    constexpr int screenHeight = 6;
    //constexpr int screenWidth = 7;
    //constexpr int screenHeight = 3;
    vector<vector<char>> screen(screenWidth, vector<char>(screenHeight, '.'));
    auto printScreen = [&screen, screenWidth, screenHeight]()
    {
        for (int y = 0; y < screenHeight; y++)
        {
            for (int x = 0; x < screenWidth; x++)
            {
                std::cout << screen[x][y];
            }
            std::cout << std::endl;
        }
    };

    string instructionLine;
    std::regex instructionRegex(R"(^(rect (\d+)x(\d+))|(rotate row y=(\d+) by (\d+))|(rotate column x=(\d+) by (\d+))$)");
    while (getline(cin, instructionLine))
    {
        std::cout << "instructionLine: " << instructionLine << std::endl;
        std::smatch instructionMatch;
        const bool matchSuccessful = std::regex_match(instructionLine, instructionMatch, instructionRegex);
        assert(matchSuccessful);
        if (!std::string(instructionMatch[1]).empty())
        {
            const int rectWidth = std::stoi(instructionMatch[2]);
            const int rectHeight = std::stoi(instructionMatch[3]);
            for (int x = 0; x < rectWidth; x++)
            {
                for (int y = 0; y < rectHeight; y++)
                {
                    screen[x][y] = '#';
                }
            }
            std::cout << "rect: " << rectWidth << "x" << rectHeight << std::endl; 
        }
        else if (!std::string(instructionMatch[4]).empty())
        {
            const int row = std::stoi(instructionMatch[5]);
            assert(row >= 0 && row < screenHeight);
            const int amount = std::stoi(instructionMatch[6]);
            vector<int> newRow;
            for (int col = 0; col < screenWidth; col++)
            {
                newRow.push_back(screen[mod(col - amount, screenWidth)][row]);
            }
            for (int col = 0; col < screenWidth; col++)
            {
                screen[col][row] = newRow[col];
            }
            std::cout << "rotate row y = " << row << " by " << amount << std::endl;
        }
        else if (!std::string(instructionMatch[7]).empty())
        {
            const int col = std::stoi(instructionMatch[8]);
            assert(col >= 0 && col < screenWidth);
            const int amount = std::stoi(instructionMatch[9]);
            vector<int> newCol;
            for (int row = 0; row < screenHeight; row++)
            {
                newCol.push_back(screen[col][mod(row - amount, screenHeight)]);
            }
            for (int row = 0; row < screenHeight; row++)
            {
                screen[col][row] = newCol[row];
            }
            std::cout << "rotate col x = " << col << " by " << amount << std::endl;
        }
        else
        {
            assert(false);
        }
        printScreen();
    }
    int numLitPixels = 0;
    for (int y = 0; y < screenHeight; y++)
    {
        for (int x = 0; x < screenWidth; x++)
        {
            if (screen[x][y] == '#')
                numLitPixels++;
        }
    }
    std::cout << "numLitPixels: " << numLitPixels << std::endl;

    return 0;
}
