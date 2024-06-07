#include <iostream>
#include <set>
#include <regex>
#include <limits>
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
    int minX = std::numeric_limits<int>::max();
    int maxX = std::numeric_limits<int>::min();
    int minY = std::numeric_limits<int>::max();
    int maxY = std::numeric_limits<int>::min();
    int minZ = std::numeric_limits<int>::max();
    int maxZ = std::numeric_limits<int>::min();
    while (std::getline(cin, cubeInfoLine))
    {
        std::smatch cubeInfoMatch;
        const bool matchSuccessful = std::regex_match(cubeInfoLine, cubeInfoMatch, cubeInfoRegex);
        assert(matchSuccessful);
        Cube cubeInfo;
        cubeInfo.x = std::stoll(cubeInfoMatch[1]);
        cubeInfo.y = std::stoll(cubeInfoMatch[2]);
        cubeInfo.z = std::stoll(cubeInfoMatch[3]);
        assert(!cubes.contains(cubeInfo));
        cubes.insert(cubeInfo);

        minX = std::min(minX, cubeInfo.x);
        maxX = std::max(maxX, cubeInfo.x);
        minY = std::min(minY, cubeInfo.y);
        maxY = std::max(maxY, cubeInfo.y);
        minZ = std::min(minZ, cubeInfo.z);
        maxZ = std::max(maxZ, cubeInfo.z);
    }

    // Build up locations of initial steam cubes.
    set<Cube> steamCubes;
    // They form a "box" completely enclosing the entirety of the droplet.
    for (int x = minX - 1; x <= maxX + 1; x++)
    {
        for (int y = minY - 1; y <= maxY + 1; y++)
        {
            // Bottom.
            steamCubes.insert({x, y, minZ - 1});
            // Top.
            steamCubes.insert({x, y, maxZ + 1});
        }
    }
    for (int x = minX - 1; x <= maxX + 1; x++)
    {
        for (int z = minZ - 1; z <= maxZ + 1; z++)
        {
            // One side.
            steamCubes.insert({x, minY - 1, z});
            // Opposite side.
            steamCubes.insert({x, maxY + 1, z});
        }
    }

    for (int y = minY - 1; y <= maxY + 1; y++)
    {
        for (int z = minZ - 1; z <= maxZ + 1; z++)
        {
            // One side.
            steamCubes.insert({minX - 1, y, z});
            // Opposite side.
            steamCubes.insert({maxX + 1, y, z});
        }
    }

    const std::initializer_list<std::tuple<int, int, int>> neighbourCubeOffsets = {
        // dx  dy  dz
        {-1, 0, 0},
        {+1, 0, 0},
        {0, -1, 0},
        {0, +1, 0},
        // Top and bottom.
        {0, 0, -1},
        {0, 0, +1}
    };

    // Grow the steam cubes.
    while (true)
    {
        bool addedSteamCube = false;

        for (const auto& steamCube : steamCubes)
        {
            for (const auto [dx, dy, dz] : neighbourCubeOffsets)
            {
                const Cube neighbourCube = { steamCube.x + dx, steamCube.y + dy, steamCube.z + dz };
                if (neighbourCube.x < minX || neighbourCube.x > maxX)
                    continue;
                if (neighbourCube.y < minY || neighbourCube.y > maxY)
                    continue;
                if (neighbourCube.z < minZ || neighbourCube.z > maxZ)
                    continue;
                if (!cubes.contains(neighbourCube) && !steamCubes.contains(neighbourCube))
                {
                    steamCubes.insert(neighbourCube);
                    addedSteamCube = true;
                }
            }
        }

        if (!addedSteamCube)
            break;
    }

    int numExposedFaces = 0;
    for (const auto& cube : cubes)
    {
        for (const auto [dx, dy, dz] : neighbourCubeOffsets)
        {
            const Cube neighbourCube = { cube.x + dx, cube.y + dy, cube.z + dz };
            if (steamCubes.contains(neighbourCube))
                numExposedFaces++;

        }
    }
    std::cout << "numExposedFaces: " << numExposedFaces << std::endl;


}
