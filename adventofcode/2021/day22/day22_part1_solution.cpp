#include <iostream>
#include <vector>
#include <regex>
#include <cassert>

using namespace std;

int main()
{
    static regex instructionRegex(R"((on|off)\s*x=([-\d]+)\.\.([-\d]+),y=([-\d]+)\.\.([-\d]+),z=([-\d]+)\.\.([-\d]+).*)");

    std::smatch instructionMatch;
    string instructionLine;
    const int maxCoord = 50;
    vector<vector<vector<bool>>> grid(2 * maxCoord + 1, vector<vector<bool>>(2 * maxCoord + 1, vector<bool>(2 * maxCoord + 1, false)));
    while (getline(cin, instructionLine))
    {
        cout << "instructionLine: " << instructionLine << endl;

        if (regex_match(instructionLine, instructionMatch, instructionRegex))
        {
            const bool on = (instructionMatch[1] == "on");

            const int xBegin = max(stoi(instructionMatch[2]), -maxCoord);
            const int xEnd = min(stoi(instructionMatch[3]), maxCoord);

            const int yBegin = max(stoi(instructionMatch[4]), -maxCoord);
            const int yEnd = min(stoi(instructionMatch[5]), maxCoord);

            const int zBegin = max(stoi(instructionMatch[6]), -maxCoord);
            const int zEnd = min(stoi(instructionMatch[7]), maxCoord);

            for (int x = xBegin; x <= xEnd; x++)
            {
                for (int y = yBegin; y <= yEnd; y++)
                {
                    for (int z = zBegin; z <= zEnd; z++)
                    {
                        grid[x + maxCoord][y + maxCoord][z + maxCoord] = on;
                    }
                }
            }

        }
        else
        {
            assert(false);
        }

    }

    int64_t count = 0;
    for (int x = -maxCoord; x <= maxCoord; x++)
    {
        for (int y = -maxCoord; y <= maxCoord; y++)
        {
            for (int z = -maxCoord; z <= maxCoord; z++)
            {
                if (grid[x + maxCoord][y + maxCoord][z + maxCoord] )
                    count++;
            }
        }
    }
    cout << count << endl;


}

