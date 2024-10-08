#include <iostream>
#include <regex>
#include <set>

#include <cassert>

using namespace std;

struct Coord3D
{
    int x = -1;
    int y = -1;
    int z = -1;
    auto operator<=>(const Coord3D& other) const = default;
};

struct Brick
{
    Brick(const Coord3D& endCoord1, const Coord3D& endCoord2, char letter = ' ')
        : endCoord1{endCoord1},
          endCoord2{endCoord2},
          m_letter{letter}
          
    {
    }

    bool canMoveDown(const vector<Brick>& allBricks) const
    {
        if (minZ() == 1)
            return false;

        Brick movedDown = *this;
        movedDown.endCoord1.z--;
        movedDown.endCoord2.z--;

        for (const auto& otherBrick : allBricks)
        {
            if (otherBrick == *this)
                continue;
            if (movedDown.intersectsWith(otherBrick))
                return false;
        }
        return true;
    }

    auto operator<=>(const Brick& other) const = default;

    int minX() const
    {
        return std::min(endCoord1.x, endCoord2.x);
    }
    int maxX() const
    {
        return std::max(endCoord1.x, endCoord2.x);
    }
    int minY() const
    {
        return std::min(endCoord1.y, endCoord2.y);
    }
    int maxY() const
    {
        return std::max(endCoord1.y, endCoord2.y);
    }
    int minZ() const
    {
        return std::min(endCoord1.z, endCoord2.z);
    }
    int maxZ() const
    {
        return std::max(endCoord1.z, endCoord2.z);
    }

    char letter() const
    {
        return m_letter;
    }

    bool intersectsWith(const Brick& other) const
    {
        if (other.minX() > maxX())
            return false;
        if (other.minY() > maxY())
            return false;
        if (other.minZ() > maxZ())
            return false;
        if (other.maxX() < minX())
            return false;
        if (other.maxY() < minY())
            return false;
        if (other.maxZ() < minZ())
            return false;

        return true;
    }

    Coord3D endCoord1;
    Coord3D endCoord2;
    char m_letter;
};

ostream& operator<<(ostream& os, const Brick& brick)
{
    os << "[brick " << brick.letter() << ": (" << brick.endCoord1.x << ", " << brick.endCoord1.y << ", " << brick.endCoord1.z << ")-(" << brick.endCoord2.x << ", " << brick.endCoord2.y << ", " << brick.endCoord2.z << ")]";
    return os;
}

int simulateFallingBricks(vector<Brick>& bricks)
{
    sort(bricks.begin(), bricks.end(), [](const auto& lhsBrick, const auto& rhsBrick) { return lhsBrick.minZ() < rhsBrick.minZ(); });
    set<Brick*> fallenBricks;

    bool brickMoved = false;
    do
    {
        brickMoved = false;
        for (auto& brick : bricks)
        {
            if (brick.canMoveDown(bricks))
            {
                brick.endCoord1.z--;
                brick.endCoord2.z--;
                std::cout << " Brick: " << brick.letter() << " moved downwards" << std::endl;
                brickMoved = true;
                fallenBricks.insert(&brick);

                if (brick.minZ() == 1)
                {
                    std::cout << " Brick: " << brick.letter() << " reached floor" << std::endl;
                }
            }
            else
            {
                std::cout << " Brick: " << brick.letter() << " could not move downwards" << std::endl;
            }
        }
    } while (brickMoved);

    return fallenBricks.size();
}

int main()
{
    std::regex brickRegex(R"(^(\d+),(\d+),(\d+)~(\d+),(\d+),(\d+)$)");
    string brickLine;
    vector<Brick> bricks;
    char letter = 'A';
    while (getline(cin, brickLine))
    {
        std::smatch brickMatch;
        const bool matchSuccessful = std::regex_match(brickLine, brickMatch, brickRegex);
        assert(matchSuccessful);

        const Brick brick({std::stoi(brickMatch[1]), std::stoi(brickMatch[2]), std::stoi(brickMatch[3])}, {std::stoi(brickMatch[4]), std::stoi(brickMatch[5]), std::stoi(brickMatch[6])}/*, letter*/);
        bricks.push_back(brick);
        letter++;
    }

    // Let them all come to rest.
    simulateFallingBricks(bricks);

    int result = 0;
    for (const auto& brickToRemove : bricks)
    {
        std::cout << "Removing brick: " << brickToRemove.letter() << std::endl;
        vector<Brick> bricksMinusToRemove = bricks;
        bricksMinusToRemove.erase(std::remove(bricksMinusToRemove.begin(), bricksMinusToRemove.end(), brickToRemove), bricksMinusToRemove.end());
        assert(bricksMinusToRemove.size() + 1 == bricks.size());

        const int numBricksFallen = simulateFallingBricks(bricksMinusToRemove);
        std::cout << " after removing brick: " << brickToRemove.letter() << " " << numBricksFallen << " fell" << std::endl;
        result += numBricksFallen;
    }
    std::cout << "result: " << result << std::endl;

    return 0;
}
