#include <iostream>
#include <vector>

#include <cassert>

using namespace std;

int main()
{
    constexpr int keypadWidth = 3;
    constexpr int keypadHeight = 3;
    vector<vector<int>> keypadButtons(keypadWidth, vector<int>(keypadHeight));

    int keypadNum = 1;
    int posX = -1;
    int posY = -1;
    for (int y = 0; y < keypadHeight; y++)
    {
        for (int x = 0; x < keypadWidth; x++)
        {
            keypadButtons[x][y] = keypadNum;
            if (keypadNum == 5)
            {
                posX = x;
                posY = y;
            }
            keypadNum++;
        }
    }

    string directionsLine;
    string bathroomCode;
    while(std::getline(cin, directionsLine))
    {
        for (const auto& directionChar : directionsLine)
        {
            const int oldX = posX;
            const int oldY = posY;
            switch (directionChar)
            {
                case 'U':
                    posY--;
                    break;
                case 'R':
                    posX++;
                    break;
                case 'D':
                    posY++;
                    break;
                case 'L':
                    posX--;
                    break;
            }
            if (posX < 0 || posX >= keypadWidth || posY < 0 || posY >= keypadHeight)
            {
                posX = oldX;
                posY = oldY;
            }
        }
        bathroomCode += static_cast<char>('0' + keypadButtons[posX][posY]);
    }
    std::cout << "bathroomCode: " << bathroomCode << std::endl;


    return 0;
}
