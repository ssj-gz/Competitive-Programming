#include "../shared/intcodecomputer.h"

#include <iostream>
#include <vector>
#include <map>
#include <limits>

using namespace std;

int main()
{
    vector<int64_t> program;
    int64_t programInput;
    while (true)
    {
        cin >> programInput;
        program.push_back(programInput);
        assert(cin);
        char comma;
        cin >> comma;
        if (!cin)
            break;
        assert(comma == ',');
    }

    program[0] = 2;
    IntCodeComputer intCodeComputer(program);
    int frame = 0;
    int score = 0;

    struct Coord
    {
        int x = -1;
        int y = -1;
        auto operator<=>(const Coord&) const = default;
    };

    map<Coord, int> tileContents;
    int paddlePos = -1;
    int ballPos = -1;
    while (true)
    {
        cout << "Frame: " << frame << endl;
        frame++;

        const auto status = intCodeComputer.run();
        auto output = intCodeComputer.takeOutput();

        assert(output.size() % 3 == 0);

        while (!output.empty())
        {
            const Coord coord = {static_cast<int>(output[0]), static_cast<int>(output[1])};
            if (coord.x == -1 && coord.y == 0)
            {
                score = static_cast<int>(output[2]);
            }
            else
            {
                const int contents = static_cast<int>(output[2]);
                tileContents[coord] = contents;
                if (contents == 3)
                {
                    paddlePos = coord.x;
                }
                else if (contents == 4)
                {
                    ballPos = coord.x;
                }
            }

            output.erase(output.begin(), output.begin() + 3);
        }

        cout << "paddlePos: " << paddlePos << " ballPos: " << ballPos << endl;

        // Attempt to move paddle so that it's x-coord matches that of the ball.
        if (paddlePos < ballPos)
            intCodeComputer.addInput(1);
        else if (paddlePos > ballPos)
            intCodeComputer.addInput(-1);
        else
        {
            intCodeComputer.addInput(0);
        }


        // Draw.
        int minX = std::numeric_limits<int>::max();
        int minY = std::numeric_limits<int>::max();
        int maxX = std::numeric_limits<int>::min();
        int maxY = std::numeric_limits<int>::min();
        for (const auto& [coord, contents] : tileContents)
        {
            minX = min(minX, coord.x);
            minY = min(minY, coord.y);
            maxX = max(maxX, coord.x);
            maxY = max(maxY, coord.y);
        }
        const int width = maxX - minX + 1;
        const int height = maxY - minY + 1;
        cout << "Score: " << score << endl;
        vector<string> gameDisplay(height, string(width, ' '));
        for (const auto& [coord, contents] : tileContents)
        {
            const int row = coord.y - minY;
            const int col = coord.x - minX;
            char code = '\0';
            switch(contents)
            {
                case 0:
                    code = ' ';
                    break;
                case 1:
                    code = '#';
                    break;
                case 2:
                    code = 'x';
                    break;
                case 3:
                    code = '-';
                    break;
                case 4:
                    code = 'o';
                    break;
            }
            gameDisplay[row][col] = code;
        }
        for (const auto& gameRow : gameDisplay)
            cout << gameRow << endl;

        if(status == IntCodeComputer::Terminated)
        {
            break;
        }

    }

}

