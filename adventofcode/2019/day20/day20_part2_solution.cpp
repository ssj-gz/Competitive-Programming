#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <cassert>

using namespace std;

struct Coord
{
    int x = -1;
    int y = -1;
    auto operator<=>(const Coord& other) const = default;
};

ostream& operator<<(ostream& os, const Coord& coord)
{
    os << "(" << coord.x << "," << coord.y << ")";
    return os;
}

map<string, vector<Coord>> parseHorizontalLabels(const vector<string>& input)
{
    const int width = input.front().size();
    const int height = input.size();
    map<string, vector<Coord>> labelInfo;

    auto isLetter = [](const char character)
    {
        return 'A' <= character && character <= 'Z';
    };

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            const char cellContents = input[y][x];
            if (x < 2 || x >= width - 1 || y < 2 || y >= height - 1)
            {
                assert(isLetter(cellContents) || cellContents == ' ');
            }
            if (isLetter(cellContents))
            {
                if (isLetter(input[y][x + 1]))
                {
                    //std::cout << "Found letter pair: " << cellContents << input[y][x + 1] << std::endl;
                    const auto label = input[y].substr(x, 2);
                    std::cout << "Found letter pair: " << label << std::endl;
                    if (x - 1 >= 0 && input[y][x - 1] == '.')
                    {
                        labelInfo[label].push_back(Coord{x - 1, y});
                        std::cout << "  associated with Coord: " << Coord{x - 1, y} << std::endl;
                    }
                    else
                    {
                        assert(x + 2 < width && input[y][x + 2] == '.');
                        labelInfo[label].push_back(Coord{x + 2, y});
                        std::cout << "  associated with Coord: " << Coord{x + 2, y} << std::endl;
                    }
                    // Skip over the other letter in this pair.
                    x++;
                }
            }
        }
    }

    return labelInfo;
}

int main()
{
    string line;
    vector<string> input;
    while (std::getline(cin, line))
    {
        input.push_back(line);
    }
    const int rawWidth = input.front().size();
    const int rawHeight = input.size();

    std::cout << "Original input: " << std::endl;
    for (const auto& row : input)
    {
        cout << row << std::endl;
    }

    auto horizontalLabels = parseHorizontalLabels(input);
    vector<string> inputRotated(rawWidth, string(rawHeight, ' '));

    for (int x = 0; x < rawWidth; x++)
    {
        for (int y = 0; y < rawHeight; y++)
        {
            inputRotated[x][y] = input[y][x];
        }
    }

    std::cout << "Rotated input: " << std::endl;
    for (const auto& row : inputRotated)
    {
        cout << row << std::endl;
    }

    map<string, vector<Coord>> allLabelPositions;
    auto unadjustedVerticalLabels = parseHorizontalLabels(inputRotated);
    for (auto& [label, coords] : unadjustedVerticalLabels)
    {
        for (auto& coord : coords)
        {
            std::swap(coord.x, coord.y);
        }
        allLabelPositions[label] = coords;
    }
    for (const auto& [label, coords] : horizontalLabels)
    {
        allLabelPositions[label].insert(allLabelPositions[label].begin(), coords.begin(), coords.end());
    }

    // Cut off border; build warp-table.
    std::map<Coord, Coord> warps;
    for (auto& [label, coords] : allLabelPositions)
    {
        cout << "Label: " << label << " at ";
        for (auto& coord : coords)
        {
            coord.x -= 2;
            coord.y -= 2;
        }
        cout << std::endl;
        assert((coords.size() == 2) || (label == "AA" || label == "ZZ"));
        assert((coords.size() == 1) || !(label == "AA" || label == "ZZ"));
        if (label != "AA" && label != "ZZ")
        {
            assert(coords.size() == 2);
            warps[coords[0]] = coords[1];
            warps[coords[1]] = coords[0];
        }
        else
        {
            assert(coords.size() == 1);
        }
    }
    const int width = rawWidth - 4;
    const int height = rawHeight - 4;
    vector<string> maze(height, string(width, ' '));
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            if ((input[y + 2][x + 2] < 'A') || (input[y + 2][x + 2] > 'z'))
                maze[y][x] = input[y + 2][x + 2];
        }
    }
    std::cout << "maze: " << std::endl;
    for (const auto& row : maze)
    {
        cout << row << std::endl;
    }

    const auto startCoord = allLabelPositions["AA"].front();
    const auto endCoord = allLabelPositions["ZZ"].front();

    struct Pos
    {
        Coord coord;
        int level = -1;
        auto operator<=>(const Pos& other) const = default;
    };

    int time = 0;
    int bestTime = -1;
    bool foundExit = false;
    vector<Pos> toExplore = { {startCoord, 0} };
    set<Pos> alreadyVisited = { {startCoord, 0} };

    while (!toExplore.empty() && !foundExit)
    {
        std::cout << "loop at time: " << time << std::endl;
        vector<Pos> nextToExplore;
        std::cout << "toExplore: " << toExplore.size() << std::endl;
        for (const auto pos : toExplore)
        {
            //std::cout << "pos: " << pos.coord << " level: " << pos.level << " mapId: " << pos.mapId << std::endl;
            if (pos.coord == endCoord && pos.level == 0)
            {
                std::cout << "huzzah!" << time << std::endl;
                bestTime = time;
                foundExit = true;
                break;
            }

            const int directions[][2] = { {-1, 0}, { 1, 0}, { 0, -1}, {0, 1} };
            for (const auto dxdy : directions )
            {
                Coord neighbourCoord = { pos.coord.x + dxdy[0], pos.coord.y + dxdy[1] };
                //std::cout << "   neighbour: " << neighbour.x  << "," << neighbour.y << std::endl;
                if (neighbourCoord.x < 0 || neighbourCoord.x >= width)
                    continue;
                if (neighbourCoord.y < 0 || neighbourCoord.y >= height)
                    continue;
                auto& cellContents = maze[neighbourCoord.y][neighbourCoord.x];
                //std::cout << " neighbourCoord: " << neighbourCoord << " cellContents: " << cellContents << std::endl;
                if (cellContents != '.')
                    continue;
                if (alreadyVisited.contains({neighbourCoord, pos.level}))
                    continue;
                if (pos.level != 0 && (pos.coord == startCoord || pos.coord == endCoord))
                    continue;
                //std::cout << "    cell: " << cellContents << std::endl;
                //if (warps.contains(neighbour))
                    //neighbour = warps[neighbour];

                //cellContents = 'X';
                alreadyVisited.insert({neighbourCoord, pos.level});
                nextToExplore.push_back({neighbourCoord, pos.level});

            }
            if (warps.contains(pos.coord))
            {
                //nextToExplore.push_back(warps[coord]);
                const bool isOuterWarp = ((pos.coord.x == 0) || (pos.coord.x == width - 1) || (pos.coord.y == 0) || (pos.coord.y == height - 1) );
                if (isOuterWarp) {
                    if (pos.level != 0) {
                        //std::cout << "Back up to " << pos.level - 1 << " due to entering warp: at pos: " << pos.coord << " level: " << pos.level << " exiting at: " << warps[pos.coord] << std::endl;
                        nextToExplore.push_back({warps[pos.coord], pos.level - 1});
                    }
                }
                else
                {
                    //std::cout << "New map: " << mapId << " due to entering warp: at pos: " << pos.coord << " level: " << pos.level << " mapId:" << pos.mapId << " exiting at: " << warps[pos.coord] << std::endl;
                    alreadyVisited.insert({pos.coord, pos.level});
                    nextToExplore.push_back({warps[pos.coord], pos.level + 1});
                }

            }
        }

        toExplore = nextToExplore;
        time++;
    }
    assert(bestTime != -1);
    cout << " bestTime: " << bestTime << std::endl;

}
