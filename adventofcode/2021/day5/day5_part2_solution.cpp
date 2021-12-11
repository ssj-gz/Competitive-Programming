#include <iostream>
#include <vector>
#include <regex>
#include <limits>
#include <cassert>

using namespace std;

int main()
{
    string line;
    struct Line
    {
        int x1 = -1;
        int x2 = -1;
        int y1 = -1;
        int y2 = -1;
    };
    vector<Line> lines;
    while (getline(cin, line))
    {
        static regex lineSegmentRegex(R"((\d+),(\d+)\s*->\s*(\d+),(\d+)\s*)");
        std::smatch lineSegmentMatch;
        cout << "Line: " << line << endl;
        if (regex_match(line, lineSegmentMatch, lineSegmentRegex))
        {
            Line line;
            line.x1 = stoi(lineSegmentMatch[1]);
            line.y1 = stoi(lineSegmentMatch[2]);
            line.x2 = stoi(lineSegmentMatch[3]);
            line.y2 = stoi(lineSegmentMatch[4]);
            lines.push_back(line);
        }
        else
        {
            assert(false);
        }

    }

    for (const auto& line : lines)
    {
        cout << "line: (" << line.x1 << ", " << line.y1 << ") -> (" << line.x2 << ", " << line.y2 << ")" << endl;
    }

    int maxX = numeric_limits<int>::min();
    int maxY = numeric_limits<int>::min();
    for (const auto& line : lines)
    {
        maxX = max(maxX, line.x1);
        maxX = max(maxX, line.x2);
        maxY = max(maxY, line.y1);
        maxY = max(maxY, line.y2);
    }
    vector<vector<int>> grid(maxX + 1, vector<int>(maxY + 1, 0));

    auto sgn = [](int x)
    {
        if (x > 0)
            return 1;
        else if (x < 0)
            return -1;
        return 0;
    };

    for (const auto& line : lines)
    {
        int x = line.x1;
        int y = line.y1;
        while (true)
        {
            grid[x][y]++;
            if (x == line.x2 && y == line.y2)
                break;
            x += sgn(line.x2 - line.x1);
            y += sgn(line.y2 - line.y1);
        }
    }

    for (int y = 0; y <= maxY; y++)
    {
        for (int x = 0; x <= maxX; x++)
        {
            if (grid[x][y] == 0)
                cout << '.';
            else
                cout << grid[x][y];
        }
        cout << endl;
    }


    int64_t count = 0;
    for (int y = 0; y <= maxY; y++)
    {
        for (int x = 0; x <= maxX; x++)
        {
            if (grid[x][y] >= 2)
            {
                count++;
            }
            
        }
    }
    cout << count << endl;
}

