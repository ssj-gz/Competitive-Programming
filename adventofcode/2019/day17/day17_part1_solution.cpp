#include "../shared/intcodecomputer.h"

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <limits>

using namespace std;

struct Coord
{
    int x = -1;
    int y = -1;
    auto operator<=>(const Coord&) const = default;
};

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

    IntCodeComputer intCodeComputer(program);
    const auto status = intCodeComputer.run();
    assert(status == IntCodeComputer::Terminated);

    const auto output = intCodeComputer.takeOutput();

    vector<string> map;
    string currentLine;
    for (const auto ascii : output)
    {
        if (ascii == 10)
        {
            map.push_back(currentLine);
            currentLine.clear();
        }
        else 
            currentLine.push_back(static_cast<char>(ascii));
    }
    for (const auto& line : map)
    {
        cout << line << endl;
    }
    while(map.back().empty())
    {
        map.pop_back();
    }
    const int height = static_cast<int>(map.size());
    const int width = static_cast<int>(map[0].size());
    cout << "width: " << width << " height: " << height << endl;

    int alignmentParameterSum = 0;
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            if (map[row][col] != '#')
                continue;

            const std::pair<int, int> directions[] = 
            {
                { -1,  0},
                { +1,  0},
                {  0, -1},
                {  0, +1}
            };

            bool isIntersection = true;
            for (const auto& [dx, dy] : directions)
            {
                const int neighbourRow = row + dy;
                const int neighbourCol = col + dx;
                if (neighbourRow < 0 || neighbourRow >= height)
                    continue;
                if (neighbourCol < 0 || neighbourCol >= width)
                    continue;
                if (map[neighbourRow][neighbourCol] != '#')
                    isIntersection = false;
            }
            if(isIntersection)
            {
                map[row][col] = 'O';
                alignmentParameterSum += row * col;
            }

        }
    }
    for (const auto& line : map)
    {
        cout << line << endl;
    }
    cout << "alignmentParameterSum: " << alignmentParameterSum << endl;
}

