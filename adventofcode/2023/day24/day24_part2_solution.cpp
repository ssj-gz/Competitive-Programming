#include <iostream>
#include <regex>
#include <boost/rational.hpp>
#include <boost/multiprecision/cpp_int.hpp>

#include <cassert>

using namespace std;

struct Hailstone
{
    int64_t startX = -1;
    int64_t startY = -1;
    int64_t startZ = -1;
    int64_t velocityX = -1;
    int64_t velocityY = -1;
    int64_t velocityZ = -1;
    auto operator<=>(const Hailstone& other) const = default;
};

ostream& operator<<(ostream& os, const Hailstone& hailstone)
{
    os << "[hailstone " << hailstone.startX << ", " << hailstone.startY << ", " << hailstone.startZ << ") @ (" << hailstone.velocityX << ", " << hailstone.velocityY <<", " << hailstone.velocityZ << ")]";
    return os;
}

using Rational = boost::rational<boost::multiprecision::cpp_int>;
void gaussian(vector<vector<Rational>>& augmentedMatrix)
{
    const int numRows = augmentedMatrix.size();
    const int numCols = augmentedMatrix.front().size();
    for (int rowToKeep = 0; rowToKeep < numRows; rowToKeep++)
    {
        const int colToKeep = rowToKeep;
        assert(augmentedMatrix[rowToKeep][colToKeep] != 0);
        for (int row = 0; row < numRows; row++)
        {
            if (row == rowToKeep)
                continue;
            const auto rowToSubstractMultiplier = augmentedMatrix[row][colToKeep] / augmentedMatrix[rowToKeep][colToKeep];
            for (int col = 0; col < numCols; col++)
            {
                augmentedMatrix[row][col] -= rowToSubstractMultiplier * augmentedMatrix[rowToKeep][col];
            }
        }
        // Preceding columns are 0; make this one a "1" by dividing all columns through.
        const auto makeOneDivisor = augmentedMatrix[rowToKeep][colToKeep];
        for (int col = 0; col < numCols; col++)
        {
            augmentedMatrix[rowToKeep][col] /= makeOneDivisor;
        }
    }
}

int main()
{
    std::regex hailstoneRegex(R"(^\s*([0-9-]+)\s*,\s*([0-9-]+)\s*,\s*([0-9-]+)\s*@\s*([0-9-]+)\s*,\s*([0-9-]+)\s*,\s*([0-9-]+)\s*$)");
    string hailstoneLine;
    vector<Hailstone> hailstones;
    while (getline(cin, hailstoneLine))
    {
        std::smatch hailstoneMatch;
        const bool matchSuccessful = std::regex_match(hailstoneLine, hailstoneMatch, hailstoneRegex);
        assert(matchSuccessful);
        const Hailstone hailstone = { std::stoll(hailstoneMatch[1]), std::stoll(hailstoneMatch[2]), std::stoll(hailstoneMatch[3]), std::stoll(hailstoneMatch[4]), std::stoll(hailstoneMatch[5]), std::stoll(hailstoneMatch[6])};
        hailstones.push_back(hailstone);
    }

    vector<vector<Rational>> augmentedMatrix(4, vector<Rational>(5));
    for (int i = 1; i <= 4; i++)
    {
        const auto A = (hailstones[0].velocityY - hailstones[i].velocityY);   // Coefficient of Px
        const auto B = -(hailstones[0].velocityX - hailstones[i].velocityX);  // Coefficient of Py
        const auto C = -(hailstones[0].startY - hailstones[i].startY);        // Coefficient of Vx
        const auto D = (hailstones[0].startX - hailstones[i].startX);         // Coefficient of Vy
        const auto E = (hailstones[i].startX * hailstones[i].velocityY - hailstones[0].startX * hailstones[0].velocityY) - (hailstones[i].startY * hailstones[i].velocityX - hailstones[0].startY * hailstones[0].velocityX);
        augmentedMatrix[i - 1][0] = A;
        augmentedMatrix[i - 1][1] = B;
        augmentedMatrix[i - 1][2] = C;
        augmentedMatrix[i - 1][3] = D;
        augmentedMatrix[i - 1][4] = -E;
    }
    gaussian(augmentedMatrix);
    const auto smasherStartX = boost::rational_cast<int64_t>(augmentedMatrix[0][4]);
    const auto smasherStartY = boost::rational_cast<int64_t>(augmentedMatrix[1][4]);
    const auto smasherVelocityX = boost::rational_cast<int64_t>(augmentedMatrix[2][4]);
    const auto smasherVelocityY = boost::rational_cast<int64_t>(augmentedMatrix[3][4]);

    // Deduce smasherStartZ & smasherVelocityZ.
    const auto hailstone0CollisionTime = (hailstones[0].startX - smasherStartX) / (smasherVelocityX - hailstones[0].velocityX);
    assert(smasherStartX + hailstone0CollisionTime * smasherVelocityX == hailstones[0].startX + hailstone0CollisionTime * hailstones[0].velocityX);
    assert(smasherStartY + hailstone0CollisionTime * smasherVelocityY == hailstones[0].startY + hailstone0CollisionTime * hailstones[0].velocityY);
    const auto hailstone0CollisionZ = hailstones[0].startZ + hailstone0CollisionTime * hailstones[0].velocityZ;
    const auto hailstone1CollisionTime = (hailstones[1].startX - smasherStartX) / (smasherVelocityX - hailstones[1].velocityX);
    assert(smasherStartX + hailstone1CollisionTime * smasherVelocityX == hailstones[1].startX + hailstone1CollisionTime * hailstones[1].velocityX);
    assert(smasherStartY + hailstone1CollisionTime * smasherVelocityY == hailstones[1].startY + hailstone1CollisionTime * hailstones[1].velocityY);
    const auto hailstone1CollisionZ = hailstones[1].startZ + hailstone1CollisionTime * hailstones[1].velocityZ;
    const auto smasherVelocityZ = (hailstone1CollisionZ - hailstone0CollisionZ) / (hailstone1CollisionTime - hailstone0CollisionTime);
    const auto smasherStartZ = hailstone0CollisionZ - hailstone0CollisionTime * smasherVelocityZ;

    for (const auto& hailstone : hailstones)
    {
        assert((hailstone.startX - smasherStartX) % (smasherVelocityX - hailstone.velocityX) == 0);
        const int64_t t = (hailstone.startX - smasherStartX) / (smasherVelocityX - hailstone.velocityX);
        assert(smasherStartX + t * smasherVelocityX == hailstone.startX + t * hailstone.velocityX);
        assert(smasherStartY + t * smasherVelocityY == hailstone.startY + t * hailstone.velocityY);
        assert(smasherStartZ + t * smasherVelocityZ == hailstone.startZ + t * hailstone.velocityZ);
        std::cout << "Collision with " << hailstone << " verified OK" << std::endl;
    }

    std::cout << "Result: " << smasherStartX + smasherStartY + smasherStartZ << std::endl;

    return 0;
}
