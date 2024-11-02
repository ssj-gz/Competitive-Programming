#include <iostream>
#include <vector>

#include <cassert>

using namespace std;

int main()
{
    constexpr int keypadWidth = 5;
    constexpr int keypadHeight = 5;
    vector<vector<char>> keypadButtons(keypadWidth, vector<char>(keypadHeight, ' '));

    int keypadNum = 1;
    int posX = -1;
    int posY = -1;
    for (int y = 0; y < keypadHeight; y++)
    {
        const int currentRowLen = keypadWidth - 2 * abs(keypadHeight / 2 - y);
        const int rowStartX = keypadWidth / 2 - currentRowLen / 2;
        std::cout << "y: " << y << " currentRowLen:  " << currentRowLen << " rowStartX: " << rowStartX << std::endl;
        for (int x = rowStartX; x < rowStartX + currentRowLen; x++)
        {
            if (keypadNum < 10)
                keypadButtons[x][y] = static_cast<char>('0' + keypadNum);
            else
                keypadButtons[x][y] = static_cast<char>('A' + (keypadNum - 10));

            if (keypadNum == 5)
            {
                posX = x;
                posY = y;
            }
            keypadNum++;
        }
    }
    std::cout << "keypad: " << std::endl;
    for (int y = 0; y < keypadHeight; y++)
    {
        for (int x = 0; x < keypadWidth; x++)
        {
            std::cout << keypadButtons[x][y];
        }
        std::cout << std::endl;
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
            if (posX < 0 || posX >= keypadWidth || posY < 0 || posY >= keypadHeight || keypadButtons[posX][posY] == ' ')
            {
                posX = oldX;
                posY = oldY;
            }
        }
        bathroomCode += keypadButtons[posX][posY];
    }
    std::cout << "bathroomCode: " << bathroomCode << std::endl;


    return 0;
}
