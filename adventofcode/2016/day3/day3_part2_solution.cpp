#include <iostream>
#include <vector>
#include <sstream>

#include <cassert>

using namespace std;

int main()
{
    string triangleLine;
    vector<vector<int>> grid;
    constexpr int numTriangleSides = 3;
    while (getline(cin, triangleLine))
    {
        istringstream triangleLengthsStream(triangleLine);
        int triangleLengths[numTriangleSides] = {};
        for (int i = 0; i < numTriangleSides; i++)
        {
            triangleLengthsStream >> triangleLengths[i];
        }
        assert(triangleLengthsStream);
        grid.push_back({std::begin(triangleLengths), std::end(triangleLengths)});
        //grid.back().insert(grid.back().begin(), std::begin(triangleLengths), std::end(triangleLengths));

    }
    const int height = static_cast<int>(grid.size());
    assert(height % 3 == 0);

    int numValidTriangles = 0;
    for (int triangleRowBegin = 0; triangleRowBegin < height; triangleRowBegin += numTriangleSides)
    {
        for (int column = 0; column < numTriangleSides; column++)
        {
            std::cout << "Triangle: " << std::endl;
            int triangleLengths[numTriangleSides] = {};
            for (int rowIndex = triangleRowBegin; rowIndex < triangleRowBegin + numTriangleSides; rowIndex++)
            {
                triangleLengths[rowIndex - triangleRowBegin] = grid[rowIndex][column];
                std::cout << triangleLengths[rowIndex - triangleRowBegin] << std::endl;
            }
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
    }
    std::cout << "numValidTriangles: " << numValidTriangles << std::endl;
    return 0;
}
