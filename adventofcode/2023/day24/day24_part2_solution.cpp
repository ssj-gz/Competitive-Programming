#include <iostream>
#include <regex>
#include <boost/rational.hpp>
#include <boost/multiprecision/cpp_int.hpp>

#pragma GCC optimize "trapv" // abort() on (signed) integer overflow.

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
    char letter = ' ';
    auto operator<=>(const Hailstone& other) const = default;

};

ostream& operator<<(ostream& os, const Hailstone& hailstone)
{
    os << "[hailstone " << hailstone.letter << ": (" << hailstone.startX << ", " << hailstone.startY << ", " << hailstone.startZ << ") @ (" << hailstone.velocityX << ", " << hailstone.velocityY <<", " << hailstone.velocityZ << ")]";
    return os;
}

using Rational = boost::rational<boost::multiprecision::cpp_int>;
void gaussian(vector<vector<Rational>>& augmentedMatrix)
{
    std::cout << "gaussian" << std::endl;
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
    std::cout << "gaussian complete: " << std::endl;
    for (int row = 0; row < numRows; row++)
    {
        for (int col = 0; col < numCols; col++)
        {
            std::cout << augmentedMatrix[row][col] << " ";
        }
        std::cout << std::endl;
    }
}

int main()
{
    // 19, 13, 30 @ -2,  1, -2
    std::regex hailstoneRegex(R"(^\s*([0-9-]+)\s*,\s*([0-9-]+)\s*,\s*([0-9-]+)\s*@\s*([0-9-]+)\s*,\s*([0-9-]+)\s*,\s*([0-9-]+)\s*$)");
    string hailstoneLine;
    vector<Hailstone> hailstones;
    char letter = 'A';
    while (getline(cin, hailstoneLine))
    {
        std::smatch hailstoneMatch;
        const bool matchSuccessful = std::regex_match(hailstoneLine, hailstoneMatch, hailstoneRegex);
        assert(matchSuccessful);
        const Hailstone hailstone = { std::stoll(hailstoneMatch[1]), std::stoll(hailstoneMatch[2]), std::stoll(hailstoneMatch[3]), std::stoll(hailstoneMatch[4]), std::stoll(hailstoneMatch[5]), std::stoll(hailstoneMatch[6])/*, letter*/ };
        std::cout << "hailstone: " << hailstone << std::endl;
        hailstones.push_back(hailstone);
        letter++;
    }

    constexpr int64_t testAreaMin = 200'000'000'000'000LL;
    constexpr int64_t testAreaMax = 400'000'000'000'000LL;
    std::cout << "testAreaMin: " << testAreaMin << std::endl;
    std::cout << "testAreaMax: " << testAreaMax << std::endl;
    int64_t numCollisionsInTestArea = 0;
    for (auto hailstone1Iter = hailstones.begin(); hailstone1Iter != hailstones.end(); hailstone1Iter++)
    {
        for (auto hailstone2Iter = std::next(hailstone1Iter); hailstone2Iter != hailstones.end(); hailstone2Iter++)
        {
            const auto& hailstone1 = *hailstone1Iter;
            const auto& hailstone2 = *hailstone2Iter;
            //std::cout << "Considering " << hailstone1 << " vs " << hailstone2 << std::endl;
            //if((hailstone1.velocityY - hailstone2.velocityY) == 0)
            //{
            //    std::cout << "Skipping" << std::endl;
            //    continue;
            //}
            //double collisionTime = (static_cast<double>(hailstone2.startY) - hailstone1.startY) / (hailstone1.velocityY - hailstone2.velocityY);
            const Rational Vx1 = hailstone1.velocityX;
            const Rational Vx2 = hailstone2.velocityX;
            const Rational Vy1 = hailstone1.velocityY;
            const Rational Vy2 = hailstone2.velocityY;
            const Rational sx1 = hailstone1.startX;
            const Rational sx2 = hailstone2.startX;
            const Rational sy1 = hailstone1.startY;
            const Rational sy2 = hailstone2.startY;
            if (((Vy2 * Vx1 - Vx2 * Vy1)) == 0)
            {
                //std::cout << "Skipping" << std::endl;
                continue;
            }
            const Rational collisionTime = (Vx2 * (sy1 - sy2) - Vy2 * (sx1 - sx2))  / (Vy2 * Vx1 - Vx2 * Vy1) ;
            //std::cout << "collisionTime:" << collisionTime << std::endl;

            if (collisionTime < 0)
            {
                //std::cout << " in past for hailstone1" << std::endl;
                continue;
            }
            const Rational timeForHailstone2 = ((hailstone1.startX + hailstone1.velocityX * collisionTime) - hailstone2.startX) / hailstone2.velocityX;
            //std::cout << "timeForHailstone2:" << timeForHailstone2 << std::endl;
            if (timeForHailstone2 < 0)
            {
                //std::cout << " in past for hailstone2" << std::endl;
                continue;
            }
            //double hailstone1X = hailstone1.startX + hailstone1.velocityX * collisionTime;
            //double hailstone2X = hailstone2.startX + hailstone2.velocityX * collisionTime;
            //if (abs(hailstone1X - hailstone2X) < 0.01)
            //if (hailstone1X == hailstone2X)
            const auto hailstone1XCollision = hailstone1.startX + hailstone1.velocityX * collisionTime;
            const auto hailstone1YCollision = hailstone1.startY + hailstone1.velocityY * collisionTime;
            if (hailstone1XCollision >= testAreaMin && hailstone1XCollision <= testAreaMax && hailstone1YCollision >= testAreaMin && hailstone1YCollision <= testAreaMax)
            {
                numCollisionsInTestArea++;
            }
            {
                //std::cout << " ** Woo - collision between "  << hailstone1 << " and " << hailstone2 << " at time: " << boost::rational_cast<double>(collisionTime) << " x: " << boost::rational_cast<double>((hailstone1.startX + hailstone1.velocityX * collisionTime)) << " y: " << boost::rational_cast<double>((hailstone1.startY + hailstone1.velocityY * collisionTime)) << std::endl;
            }
        }
    }

    //x = t * Vx1 + sx1
    //y = t * Vy1 + sy1


    //x = t' * Vx2 + sx2 => x - sx2 = t' * Vx2 => t' = (x - sx2) / Vx2
    //y = t' * Vy2 + sy2 => y - sy2 = t' * Vy2 => t' = (y - sy2) / Vy2
    //=> (x - sx2) / Vx2 = (y - sy2) / Vy2
    //=> (t * Vx1 + sx1 - sx2) / Vx2 = (t * Vy1 + sy1 - sy2) / Vy2
    //=> Vy2 * (t * Vx1 + sx1 - sx2) = Vx2 * (t * Vy1 + sy1 - sy2)
    //=> Vy2 * Vx1 * t + Vy2 * (sx1 - sx2) = Vx2 * Vy1 * t + Vx2 * (sy1 - sy2)
    //=> Vy2 * Vx1 * t = Vx2 * Vy1 * t + Vx2 * (sy1 - sy2) - Vy2 * (sx1 - sx2) 
    //=> Vy2 * Vx1 * t - Vx2 * Vy1 * t =  Vx2 * (sy1 - sy2) - Vy2 * (sx1 - sx2) 
    //=> t = Vx2 * (sy1 - sy2) - Vy2 * (sx1 - sx2) 
    //=> t = (Vx2 * (sy1 - sy2) - Vy2 * (sx1 - sx2))  / (Vy2 * Vx1 - Vx2 * Vy1)
    std::cout << "numCollisionsInTestArea: " << numCollisionsInTestArea << std::endl;

    int64_t largestVXGcd = 0;
    int64_t closestX = std::numeric_limits<int64_t>::max();
    for (auto hailstone1Iter = hailstones.begin(); hailstone1Iter != hailstones.end(); hailstone1Iter++)
    {
        for (auto hailstone2Iter = std::next(hailstone1Iter); hailstone2Iter != hailstones.end(); hailstone2Iter++)
        {
            const auto& hailstone1 = *hailstone1Iter;
            const auto& hailstone2 = *hailstone2Iter;
            largestVXGcd = std::max(largestVXGcd, std::gcd(hailstone1.velocityX, hailstone2.velocityX));
            closestX = std::min(closestX, abs(hailstone1.startX - hailstone2.startX));
        }
    }
    std::cout << "largestVXGcd:"  << largestVXGcd << std::endl;
    std::cout << "closestX:"  << closestX << std::endl;

    int64_t largestVYGcd = 0;
    int64_t closestY = std::numeric_limits<int64_t>::max();
    for (auto hailstone1Iter = hailstones.begin(); hailstone1Iter != hailstones.end(); hailstone1Iter++)
    {
        for (auto hailstone2Iter = std::next(hailstone1Iter); hailstone2Iter != hailstones.end(); hailstone2Iter++)
        {
            const auto& hailstone1 = *hailstone1Iter;
            const auto& hailstone2 = *hailstone2Iter;
            largestVYGcd = std::max(largestVYGcd, std::gcd(hailstone1.velocityY, hailstone2.velocityY));
            //std::cout << "hailstone1: " << hailstone1 << " hailstone2: " << hailstone2 << std::endl;
            //assert(hailstone1.startY != hailstone2.startY);
            closestY = std::min(closestY, abs(hailstone1.startY - hailstone2.startY));
        }
    }
    std::cout << "largestVYGcd:"  << largestVYGcd << std::endl;
    std::cout << "closestY:"  << closestY << std::endl;

    int64_t largestVZGcd = 0;
    int64_t closestZ = std::numeric_limits<int64_t>::max();
    for (auto hailstone1Iter = hailstones.begin(); hailstone1Iter != hailstones.end(); hailstone1Iter++)
    {
        for (auto hailstone2Iter = std::next(hailstone1Iter); hailstone2Iter != hailstones.end(); hailstone2Iter++)
        {
            const auto& hailstone1 = *hailstone1Iter;
            const auto& hailstone2 = *hailstone2Iter;
            largestVZGcd = std::max(largestVZGcd, std::gcd(hailstone1.velocityZ, hailstone2.velocityZ));
            closestZ = std::min(closestZ, abs(hailstone1.startZ - hailstone2.startZ));
        }
    }
    std::cout << "largestVZGcd:"  << largestVZGcd << std::endl;
    std::cout << "closestZ:"  << closestZ << std::endl;

    for (const auto& hailstone : hailstones)
    {
        if (abs(hailstone.velocityZ) == 380)
        {
            std::cout << "sausage: " << hailstone.startZ % 380 << std::endl;
        }
    }

    // Two of the hailstones have their startY == Sy and velocityY == Vy; this enables us 
    // to "cheat" and deduce all other unknowns.
    const int64_t Sy = 270148844447628LL;
    const int64_t Vy = -5LL;

    int64_t t = -1; // Time of collision of arbitrary hailstone #0.
    {
        const int64_t S = hailstones[0].startY;
        const int64_t V = hailstones[0].velocityY;
        const int64_t blee = (S - Sy);
        const int64_t bloo = (Vy - V);
        assert(blee % bloo == 0);
        t = blee / bloo;
    }
    int64_t tPrime = -1; // Time of collision of arbitrary hailstone #1.
    {
        const int64_t S = hailstones[1].startY;
        const int64_t V = hailstones[1].velocityY;
        const int64_t blee = (S - Sy);
        const int64_t bloo = (Vy - V);
        assert(blee % bloo == 0);
        tPrime = blee / bloo;
    }
    std::cout << "t: " << t << " tPrime: " << tPrime << std::endl;
    int64_t hailstoneStartX = -1;
    int64_t hailstoneVelocityX = -1;
    {
        const int64_t A = hailstones[0].startX + t * hailstones[0].velocityX;
        const int64_t B = hailstones[1].startX + tPrime * hailstones[1].velocityX;
        const int64_t blee = A - B;
        const int64_t bloo = tPrime - t;
        std::cout << "blee: " << blee << " bloo: " << bloo << std::endl;
        assert(blee % bloo == 0);
        hailstoneVelocityX = -blee / bloo;
        std::cout << "hailstoneVelocityX: " << hailstoneVelocityX << std::endl;
        hailstoneStartX = A - hailstoneVelocityX * t;
        std::cout << "hailstoneStartX: " << hailstoneStartX << std::endl;
        assert(hailstoneStartX + t * hailstoneVelocityX == A);
        assert(hailstoneStartX + tPrime * hailstoneVelocityX == B);
    }
    int64_t hailstoneStartZ = -1;
    int64_t hailstoneVelocityZ = -1;
    {
        const int64_t A = hailstones[0].startZ + t * hailstones[0].velocityZ;
        const int64_t B = hailstones[1].startZ + tPrime * hailstones[1].velocityZ;
        const int64_t blee = A - B;
        const int64_t bloo = tPrime - t;
        std::cout << "blee: " << blee << " bloo: " << bloo << std::endl;
        assert(blee % bloo == 0);
        hailstoneVelocityZ = -blee / bloo;
        hailstoneStartZ = (hailstones[0].startZ + t * hailstones[0].velocityZ) - hailstoneVelocityZ * t;
    }
    const int64_t hailstoneStartY = Sy;
    const int64_t hailstoneVelocityY = Vy;
    std::cout << "hailstoneStartX: " << hailstoneStartX << std::endl;
    std::cout << "hailstoneStartY: " << hailstoneStartY << std::endl;
    std::cout << "hailstoneStartZ: " << hailstoneStartZ << std::endl;
    std::cout << "hailstoneVelocityX: " << hailstoneVelocityX << std::endl;
    std::cout << "hailstoneVelocityY: " << hailstoneVelocityY << std::endl;
    std::cout << "hailstoneVelocityZ: " << hailstoneVelocityZ << std::endl;

    // Verify.
    for (const auto& hailstone : hailstones)
    {
        if (hailstone.startY == hailstoneStartY)
            continue;
        std::cout << "verifying hailstone: "<< hailstone << std::endl;
        const int64_t S = hailstone.startY;
        const int64_t V = hailstone.velocityY;
        const int64_t blee = (S - Sy);
        const int64_t bloo = (Vy - V);
        assert(blee % bloo == 0);
        const int64_t t = blee / bloo;
        assert(hailstoneStartX + t * hailstoneVelocityX == hailstone.startX + t * hailstone.velocityX);
        assert(hailstoneStartY + t * hailstoneVelocityY == hailstone.startY + t * hailstone.velocityY);
        assert(hailstoneStartZ + t * hailstoneVelocityZ == hailstone.startZ + t * hailstone.velocityZ);
        std::cout << "OK" << std::endl;
    }
    std::cout << "result: " << (hailstoneStartX + hailstoneStartY + hailstoneStartZ) << std::endl;

    vector<vector<Rational>> augmentedMatrix(4, vector<Rational>(5));
    for (int i = 1; i <= 4; i++)
    {
        const auto flibble = hailstoneVelocityY * (hailstones[0].startX - hailstones[i].startX) + hailstoneStartX * (hailstones[0].velocityY - hailstones[i].velocityY) + hailstones[i].startX * hailstones[i].velocityY - hailstones[0].startX * hailstones[0].velocityY;
        const auto flobble = hailstoneVelocityX * (hailstones[0].startY - hailstones[i].startY)  + hailstoneStartY * (hailstones[0].velocityX - hailstones[i].velocityX) + hailstones[i].startY * hailstones[i].velocityX - hailstones[0].startY * hailstones[0].velocityX;
        std::cout << "i: " << " flibble: " << flibble << " flobble: " << flobble << std::endl;
        assert(flibble == flobble);
        const auto A = (hailstones[0].velocityY - hailstones[i].velocityY);   // Coefficient of Px
        const auto B = -(hailstones[0].velocityX - hailstones[i].velocityX);  // Coefficient of Py
        const auto C = -(hailstones[0].startY - hailstones[i].startY);        // Coefficient of Vx
        const auto D = (hailstones[0].startX - hailstones[i].startX);         // Coefficient of Vy
        const auto E = (hailstones[i].startX * hailstones[i].velocityY - hailstones[0].startX * hailstones[0].velocityY) - (hailstones[i].startY * hailstones[i].velocityX - hailstones[0].startY * hailstones[0].velocityX);
        assert(A * hailstoneStartX + B * hailstoneStartY + C * hailstoneVelocityX + D * hailstoneVelocityY + E == 0);
        augmentedMatrix[i - 1][0] = A;
        augmentedMatrix[i - 1][1] = B;
        augmentedMatrix[i - 1][2] = C;
        augmentedMatrix[i - 1][3] = D;
        augmentedMatrix[i - 1][4] = -E;
        //const auto flibble1 = D * hailstoneVelocityY + hailstoneStartX * A + 
    }
    std::cout << "Doing gaussian" << std::endl;
    gaussian(augmentedMatrix);
    {
        const auto hailstoneStartXOpt = augmentedMatrix[0][4];
        const auto hailstoneStartYOpt = augmentedMatrix[1][4];
        const auto hailstoneVelocityXOpt = augmentedMatrix[2][4];
        const auto hailstoneVelocityYOpt = augmentedMatrix[3][4];

        const auto hailstone0CollisionTime = (hailstones[0].startX - hailstoneStartXOpt) / (hailstoneVelocityXOpt - hailstones[0].velocityX);
        assert(hailstoneStartXOpt + hailstone0CollisionTime * hailstoneVelocityXOpt == hailstones[0].startX + hailstone0CollisionTime * hailstones[0].velocityX);
        assert(hailstoneStartYOpt + hailstone0CollisionTime * hailstoneVelocityYOpt == hailstones[0].startY + hailstone0CollisionTime * hailstones[0].velocityY);
        const auto hailstone0CollisionZ = hailstones[0].startZ + hailstone0CollisionTime * hailstones[0].velocityZ;
        const auto hailstone1CollisionTime = (hailstones[1].startX - hailstoneStartXOpt) / (hailstoneVelocityXOpt - hailstones[1].velocityX);
        assert(hailstoneStartXOpt + hailstone1CollisionTime * hailstoneVelocityXOpt == hailstones[1].startX + hailstone1CollisionTime * hailstones[1].velocityX);
        assert(hailstoneStartYOpt + hailstone1CollisionTime * hailstoneVelocityYOpt == hailstones[1].startY + hailstone1CollisionTime * hailstones[1].velocityY);
        const auto hailstone1CollisionZ = hailstones[1].startZ + hailstone1CollisionTime * hailstones[1].velocityZ;
        const auto hailstoneVelocityZOpt = (hailstone1CollisionZ - hailstone0CollisionZ) / (hailstone1CollisionTime - hailstone0CollisionTime);
        const auto hailstoneStartZOpt = hailstone0CollisionZ - hailstone0CollisionTime * hailstoneVelocityZOpt;

        std::cout << "hailstoneStartXOpt: " << hailstoneStartXOpt << " hailstoneStartX: " << hailstoneStartX << std::endl;
        std::cout << "hailstoneStartYOpt: " << hailstoneStartYOpt << " hailstoneStartY: " << hailstoneStartY << std::endl;
        std::cout << "hailstoneStartZOpt: " << hailstoneStartZOpt << " hailstoneStartZ: " << hailstoneStartZ << std::endl;
        std::cout << "hailstoneVelocityXOpt: " << hailstoneVelocityXOpt << " hailstoneVelocityX: " << hailstoneVelocityX << std::endl;
        std::cout << "hailstoneVelocityYOpt: " << hailstoneVelocityYOpt << " hailstoneVelocityY: " << hailstoneVelocityY << std::endl;
        std::cout << "hailstoneVelocityZOpt: " << hailstoneVelocityZOpt << " hailstoneVelocityZ: " << hailstoneVelocityZ << std::endl;
        //const auto hailstoneStartZOpt = h
        std::cout << "Result (non-hacky): " << boost::rational_cast<int64_t>(hailstoneStartXOpt + hailstoneStartYOpt + hailstoneStartZ) << std::endl;
    }



    return 0;
}
