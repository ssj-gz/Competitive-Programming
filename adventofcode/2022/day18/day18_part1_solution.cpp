#include <iostream>
#include <set>
#include <regex>
#include <cassert>

using namespace std;

struct Cube
{
    int x = -1;
    int y = -1;
    int z = -1;
    auto operator<=>(const Cube& other) const = default;
};

int main()
{
    string cubeInfoLine;
    std::regex cubeInfoRegex("^(\\d+),(\\d+),(\\d+)$");
    set<Cube> cubes;
    while (std::getline(cin, cubeInfoLine))
    {
        std::smatch cubeInfoMatch;
        const bool matchSuccessful = std::regex_match(cubeInfoLine, cubeInfoMatch, cubeInfoRegex);
        assert(matchSuccessful);
        Cube cubeInfo;
        cubeInfo.x = std::stoll(cubeInfoMatch[1]);
        cubeInfo.y = std::stoll(cubeInfoMatch[2]);
        cubeInfo.z = std::stoll(cubeInfoMatch[3]);
        std::cout << "Cube: " << cubeInfo.x << "," << cubeInfo.y << "," << cubeInfo.z << std::endl;
        assert(!cubes.contains(cubeInfo));
        cubes.insert(cubeInfo);
    }

    int numExposedFaces = 0;
    for (const auto& cube : cubes)
    {
        const std::initializer_list<std::tuple<int, int, int>> neighbourOffsets = {
            // dx  dy  dz
            {-1, 0, 0},
            {+1, 0, 0},
            {0, -1, 0},
            {0, +1, 0},
            // Top and bottom.
            {0, 0, -1},
            {0, 0, +1}
        };
        for (const auto [dx, dy, dz] : neighbourOffsets)
        {
            const Cube neighbourCube = { cube.x + dx, cube.y + dy, cube.z + dz };
            if (!cubes.contains(neighbourCube))
                numExposedFaces++;

        }
    }
    std::cout << "numExposedFaces: " << numExposedFaces << std::endl;


}
