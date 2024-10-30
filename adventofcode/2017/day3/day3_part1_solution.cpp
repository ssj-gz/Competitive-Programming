#include <iostream>

#include <cassert>

using namespace std;

int main()
{
    int64_t number = -1;
    cin >> number;

    int squareSideLen = 1;
    while (true)
    {
        if (number >= squareSideLen * squareSideLen)
            squareSideLen += 2;
        else
        {
            const int outerSquareSideLen = squareSideLen;
            std::cout << "outerSquareSideLen: " << outerSquareSideLen << std::endl;
            const int innerSquareSideLen = outerSquareSideLen - 2;
            number -= (innerSquareSideLen * innerSquareSideLen);
            std::cout << "number: " << number << std::endl;
            // Bottom right corner of the square either containing original number (if original number
            // was a square) or contained within the square that contains the original number.
            // The number "1" is at (0, 0) - increase y => move *downwards*.
            const int squareBRX = (innerSquareSideLen - 1) / 2;
            const int squareBRY = (innerSquareSideLen - 1) / 2;
            std::cout << "squareBRX: " << squareBRX << " squareBRY: " << squareBRY << std::endl;
            int numberXCoord = squareBRX;
            int numberYCoord = squareBRY;
            auto sgn = [](const int x)
            {
                if (x < 0)
                    return -1;
                else if (x == 0)
                    return 0;
                else 
                    return 1;

            };

            for (const auto& [squareX, squareY] : std::initializer_list<std::pair<int, int>>{ 
                    // Move right one.
                    {squareBRX + 1, squareBRY},
                    // Move up to top-right corner of outer square.
                    {squareBRX + 1, squareBRY - outerSquareSideLen + 2},
                    // Move left to top-left corner of outer square.
                    {squareBRX + 1 - outerSquareSideLen + 1, squareBRY - outerSquareSideLen + 2},
                    // Move down to bottom-left corner of outer square.
                    {squareBRX + 1 - outerSquareSideLen + 1, squareBRY + 1},
                    // Move right to bottom-right corner of outer square.
                    {squareBRX + 1, squareBRY + 1},

                    })
            {
                const int numStepsToMove = std::min<int>(number, abs(squareX - numberXCoord) + abs(squareY - numberYCoord));
                std::cout << " moving " << numStepsToMove << " steps towards: " << squareX << ", " << squareY << std::endl;
                if (numStepsToMove == 0)
                   break; 
                numberXCoord += sgn(squareX - numberXCoord) * numStepsToMove;
                numberYCoord += sgn(squareY - numberYCoord) * numStepsToMove;
                std::cout << "Now at: " << numberXCoord << ", " << numberYCoord << std::endl;
                number -= numStepsToMove;
            }
            assert(number == 0);
            std::cout << "numberXCoord: " << numberXCoord << " numberYCoord: " << numberYCoord << std::endl;

            std::cout << "result: " << abs(numberXCoord - 0) + abs(numberYCoord - 0) << std::endl;


            break;
        }
    }
    return 0;
}
