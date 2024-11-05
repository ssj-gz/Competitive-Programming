#include <iostream>
#include <set>
#include <vector>

#include <cassert>

using namespace std;

int64_t numBitsInBinary(int64_t x)
{
    int64_t numBits = 0;
    while (x > 0)
    {
        if (x & 1)
            numBits++;
        x >>= 1;
    }
    return numBits;
}

int main()
{
    int favouriteNumber = -1;
    cin >> favouriteNumber;
    assert(cin);
    std::cout << "favouriteNumber: " << favouriteNumber << std::endl;
#if 0
    for (int y = 0; y < 10; y++)
    {
        for (int x = 0; x < 10; x++)
        {
            if (numBitsInBinary(x*x + 3*x + 2*x*y + y + y*y + favouriteNumber) % 2 == 0)
                std::cout << '.';
            else
                std::cout << '#';
        }
        std::cout << endl;
    }
#endif
    struct Coord
    {
        int x = -1;
        int y = -1;
        auto operator<=>(const Coord& other) const = default;
    };

    const Coord initialCoord = { 1, 1 };
    const Coord finalCoord = { 31, 39 };
    //const Coord finalCoord = { 7, 4 };

    vector<Coord> toExplore = { initialCoord };
    set<Coord> visited = { initialCoord };

    int numSteps = 0;
    while (!toExplore.empty())
    {
        vector<Coord> nextToExplore;
        for (const auto& coord : toExplore)
        {
            if (coord == finalCoord)
            {
                std::cout << " Reached " << finalCoord.x << ", " << finalCoord.y << " in " << numSteps << " steps" << std::endl;
                return 0;
            }
            else
            {
                for (const auto& [dx, dy] : std::initializer_list<std::pair<int, int>>{ {-1, 0},
                        {+1, 0},
                        {0, -1},
                        {0, +1}
                        })
                {
                    const Coord neighbourCoord = { coord.x + dx, coord.y + dy };
                    if (neighbourCoord.x < 0 || neighbourCoord.y < 0)
                        continue;
                    const bool isEmptySpace = (numBitsInBinary(neighbourCoord.x*neighbourCoord.x + 3*neighbourCoord.x + 2*neighbourCoord.x*neighbourCoord.y + neighbourCoord.y + neighbourCoord.y*neighbourCoord.y + favouriteNumber) % 2 == 0);
                    if (!isEmptySpace)
                        continue;

                    if (!visited.contains(neighbourCoord))
                    {
                        visited.insert(neighbourCoord);
                        nextToExplore.push_back(neighbourCoord);
                    }

                }

            }
        }

        toExplore = nextToExplore;
        numSteps++;

    }
    


    return 0;
}
