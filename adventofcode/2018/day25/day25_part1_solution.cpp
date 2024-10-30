#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

#include <cassert>

using namespace std;

int main()
{
    string fixedPointLine;
    struct FixedPoint
    {
        int x = -1;
        int y = -1;
        int z = -1;
        int t = -1;
    };
    vector<FixedPoint> fixedPoints;
    while (getline(cin, fixedPointLine))
    {
        for (auto& character : fixedPointLine)
        {
            if (character == ',')
                character = ' ';
        }
        std::cout << "fixedPointLine: " << fixedPointLine << std::endl;
        istringstream fixedPointStream(fixedPointLine);
        FixedPoint fixedPoint;
        fixedPointStream >> fixedPoint.x;
        fixedPointStream >> fixedPoint.y;
        fixedPointStream >> fixedPoint.z;
        fixedPointStream >> fixedPoint.t;
        assert(fixedPointStream);
        std::cout << fixedPoint.x << ", " << fixedPoint.y << ", " << fixedPoint.z << ", " << fixedPoint.t << std::endl;
        fixedPoints.push_back(fixedPoint);
    }

    vector<FixedPoint*> remainingFixedPoints;
    vector<FixedPoint*> currentConstellation;
    for (auto& fixedPoint : fixedPoints)
        remainingFixedPoints.push_back(&fixedPoint);
    int numConstellations = 0;
    while (!remainingFixedPoints.empty())
    {
        if (currentConstellation.empty())
        {
            currentConstellation.push_back(remainingFixedPoints.front());
            remainingFixedPoints.erase(remainingFixedPoints.begin());
            numConstellations++;
        }
        else
        {
            bool addedNew = false;
            for (auto* fixedPoint : remainingFixedPoints)
            {
                for (auto* constellationPoint : currentConstellation)
                {
                    if (abs(fixedPoint->x - constellationPoint->x) + 
                            abs(fixedPoint->y - constellationPoint->y) +
                            abs(fixedPoint->z - constellationPoint->z) +
                            abs(fixedPoint->t - constellationPoint->t) <= 3)
                    {
                        addedNew = true;
                        currentConstellation.push_back(fixedPoint);
                        break;
                    }
                }
                if (addedNew)
                    break;
            }
            if (addedNew)
            {
                remainingFixedPoints.erase(std::find(remainingFixedPoints.begin(), remainingFixedPoints.end(), currentConstellation.back()));
            }
            else
            {
                std::cout << "constellation complete: " << currentConstellation.size() << std::endl;
                currentConstellation.clear();

            }
        }
    }
    std::cout << "result: " << numConstellations << std::endl;


    return 0;
}
