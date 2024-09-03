#define _GLIBCXX_DEBUG       // Iterator safety; out-of-bounds access for Containers, etc.
#pragma GCC optimize "trapv" // abort() on (signed) integer overflow.


#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>

#include <cassert>

using namespace std;

enum Direction { Up, Right, Down, Left };
struct Beam
{
    int x = -1;
    int y = -1;

    Direction direction;

    bool active = true;
};

int calcNumEnergised(const vector<vector<char>>& map, const deque<Beam>& initialBeams)
{
    const int width = map.size();
    const int height = map.front().size();

    vector<vector<vector<Direction>>> beamDirectionsAt(width, vector<vector<Direction>>(height));
    auto printBeamPathMap = [&beamDirectionsAt, &map, width, height]()
    {
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                const auto& beamDirectionsAtCoord = beamDirectionsAt[x][y];
                if (map[x][y] != '.')
                {
                    cout << map[x][y];
                    continue;
                }
                assert(beamDirectionsAtCoord.size() <= 4);
                if (beamDirectionsAtCoord.empty())
                    cout << '.';
                else if (beamDirectionsAtCoord.size() >= 2)
                {
                    cout << static_cast<char>('0' + beamDirectionsAtCoord.size());
                }
                else
                {
                    char directionChar = '\0';
                    switch(beamDirectionsAtCoord.front())
                    {
                        case Up:
                            directionChar = '^';
                            break;
                        case Right:
                            directionChar = '>';
                            break;
                        case Down:
                            directionChar = 'v';
                            break;
                        case Left:
                            directionChar = '<';
                            break;
                    }
                    cout << directionChar;
                }
            }
            std::cout << endl;
        }
    };
    auto beams = initialBeams;
    while (!beams.empty())
    {
        //std::cout << "Loop" << std::endl;
        bool didSomething = false;
        vector<Beam> newBeams;
        for (int beamIndex = 0; beamIndex < static_cast<int>(beams.size()); beamIndex++)
        {
            auto& beam = beams[beamIndex];
            if (!beam.active)
                continue;
            if (beam.x < 0 || beam.x >= width || beam.y < 0 || beam.y >= height)
            {
                //std::cout << " erasing beam" << std::endl;
                //std::cout << " due to beam: x: " << beam.x << " y: " << beam.y << " direction: " << static_cast<int>(beam.direction) << "  width: "<< width << " height: " << height << std::endl;
                //beams.erase(beams.begin() + beamIndex);
                //beam.active = false;
                continue;
            }
            auto& beamDirectionsAtCoord = beamDirectionsAt[beam.x][beam.y];
            if (std::find(beamDirectionsAtCoord.begin(), beamDirectionsAtCoord.end(), beam.direction) == beamDirectionsAtCoord.end())
            {
                beamDirectionsAtCoord.push_back(beam.direction);
            }
            else
            {
                //std::cout << " erasing beam" << std::endl;
                //std::cout << " due to repeated direction " << std::endl;
                //beams.erase(beams.begin() + beamIndex);
                beam.active = false;
                continue;
            }
            switch (map[beam.x][beam.y])
            {
                case '.':
                    break;
                case '/':
                    switch (beam.direction)
                    {
                        case Up:
                            beam.direction = Right;
                            break;
                        case Right:
                            beam.direction = Up;
                            break;
                        case Down:
                            beam.direction = Left;
                            break;
                        case Left:
                            beam.direction = Down;
                            break;
                        default:
                            assert(false);
                    }
                    break;
                case '\\':
                    switch (beam.direction)
                    {
                        case Up:
                            beam.direction = Left;
                            break;
                        case Right:
                            beam.direction = Down;
                            break;
                        case Down:
                            beam.direction = Right;
                            break;
                        case Left:
                            beam.direction = Up;
                            break;
                        default:
                            assert(false);
                    }
                    break;
                case '-':
                    if ((beam.direction == Up) || (beam.direction == Down))
                    {
                        beam.direction = Right;
                        //std::cout << "  splitting beams" << std::endl;
                        newBeams.push_back({beam.x, beam.y, Left});
                    }
                    break;
                case '|':
                    if ((beam.direction == Left) || (beam.direction == Right))
                    {
                        beam.direction = Down;
                        //std::cout << "  splitting beams" << std::endl;
                        newBeams.push_back({beam.x, beam.y, Up});
                    }
                    break;
                default:
                    assert(false);
            }

            didSomething = true;
            //std::cout << "beam: x: " << beam.x << " y: " << beam.y << " direction: " << static_cast<int>(beam.direction) << std::endl;
            //if(beam.x >= 0 && beam.x < width && beam.y >= 0 && beam.y < height);
            //isEnergised[beam.x][beam.y] = true;
            switch (beam.direction)
            {
                case Up:
                    beam.y--;
                    break;
                case Right:
                    beam.x++;
                    break;
                case Down:
                    beam.y++;
                    break;
                case Left:
                    beam.x--;
                    break;
                default:
                    assert(false);
            }

            //beamIndex++;
        }
        //printBeamPathMap();
        beams.insert(beams.end(), newBeams.begin(), newBeams.end());
        if (!didSomething)
            break;
    }
    //std::cout << "Beam path map: " << std::endl;
    //printBeamPathMap();
    std::cout << "Energised map: " << std::endl;
    int numEnergised = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            const auto& beamDirectionsAtCoord = beamDirectionsAt[x][y];
            if (beamDirectionsAtCoord.empty())
                cout << '.';
            else
            {
                cout << '#';
                numEnergised++;
            }
        }
        std::cout << endl;
    }
    std::cout << "numEnergised: " << numEnergised << std::endl;
    return numEnergised;
}

int main()
{
    vector<string> rawMap;
    string mapRow;
    while (getline(cin, mapRow))
    {
        rawMap.push_back(mapRow);
    }
    const int width = rawMap.front().size();
    const int height = rawMap.size();

    vector<vector<char>> map(width, vector<char>(height, ' '));
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            map[x][y] = rawMap[y][x];
        }
    }


    int highestNumEnergised = 0;
    for (int x = 0; x < width; x++)
    {
        highestNumEnergised = std::max(highestNumEnergised, calcNumEnergised(map, { {x, 0, Down} }));
        highestNumEnergised = std::max(highestNumEnergised, calcNumEnergised(map, { {x, height - 1, Up} }));
    }
    for (int y = 0; y < height; y++)
    {
        highestNumEnergised = std::max(highestNumEnergised, calcNumEnergised(map, { {0, y, Right} }));
        highestNumEnergised = std::max(highestNumEnergised, calcNumEnergised(map, { {width - 1, y, Left} }));
    }

    std::cout << "highestNumEnergised: " << highestNumEnergised << std::endl;

}
