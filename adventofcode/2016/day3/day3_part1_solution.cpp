#include <iostream>
#include <sstream>

#include <cassert>

using namespace std;

int main()
{
    string triangleLine;
    int numValidTriangles = 0;
    while (getline(cin, triangleLine))
    {
        istringstream triangleLengthsStream(triangleLine);
        constexpr int numTriangleSides = 3;
        int triangleLengths[numTriangleSides] = {};
        for (int i = 0; i < numTriangleSides; i++)
        {
            triangleLengthsStream >> triangleLengths[i];
        }
        assert(triangleLengthsStream);
        bool isTriangle = true;
        for (int side = 0; side < numTriangleSides; side++)
        {
            int sumOfOtherSides = 0;
            for (int otherSide = 0; otherSide < numTriangleSides; otherSide++)
            {
                if (otherSide != side)
                    sumOfOtherSides += triangleLengths[otherSide];
            }
            std::cout << "side " << side << " has length: " << triangleLengths[side] << "; sum of other sides: " << sumOfOtherSides << std::endl;
            if (sumOfOtherSides <= triangleLengths[side])
                isTriangle = false;

        }
        std::cout << " is triangle: " << isTriangle << std::endl;
        if (isTriangle)
            numValidTriangles++;

    }
    std::cout << "numValidTriangles: " << numValidTriangles << std::endl;
    return 0;
}
