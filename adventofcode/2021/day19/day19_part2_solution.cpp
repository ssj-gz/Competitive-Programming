#include <iostream>
#include <vector>
#include <regex>
#include <cassert>

using namespace std;

class Coord
{
    public:
        Coord(int x, int y, int z)
            :x{x}, y{y}, z{z}
        {

        }
        Coord()
            :x{-1}, y{-1}, z{-1}
        {
        }

        void rotateClockwiseXY(int numTimes)
        {
            for (int i = 0; i  < numTimes; i++)
            {
                swap(x,y);
                y = -y;
            }
        }
        void rotateClockwiseXZ(int numTimes)
        {
            for (int i = 0; i  < numTimes; i++)
            {
                swap(x,z);
                z = -z;
            }
        }
        void rotateClockwiseYZ(int numTimes)
        {
            for (int i = 0; i  < numTimes; i++)
            {
                swap(y,z);
                z = -z;
            }
        }
        void translate(const Coord& delta)
        {
            x += delta.x;
            y += delta.y;
            z += delta.z;
        }
        Coord operator-() const
        {
            return {-x, -y, -z};
        }
        bool operator==(const Coord& other) const
        {
            return (x == other.x) &&
                   (y == other.y) &&
                   (z == other.z);
        }
        friend ostream& operator<<(ostream& os, const Coord& coord)
        {
            os << "(" << coord.x << "," << coord.y << "," << coord.z << ")";
            return os;
        }

        int x = -1;
        int y = -1;
        int z = -1;
};

int countIntersections(const vector<Coord>& coordsA, const vector<Coord>& coordsB)
{
    int result = 0;
    for (const auto coordA : coordsA)
    {
        if (find(coordsB.begin(), coordsB.end(), coordA) != coordsB.end())
            result++;
    }
    return result;
}


std::pair<vector<Coord>, Coord> attemptMatch(const vector<Coord>& knownPositions, const vector<Coord>& unknownPositionsOriginal, const Coord& knownCoord, const Coord& unknownCoord)
{
    std::pair<vector<Coord>, Coord> result;
    for (int numXYRotations = 0; numXYRotations < 4; numXYRotations++)
    {
        for (int numYZRotations = 0; numYZRotations < 4; numYZRotations++)
        {
            for (int numXZRotations = 0; numXZRotations < 4; numXZRotations++)
            {
                vector<Coord> unknownTransformed(unknownPositionsOriginal);
                auto transformToMatchKnown = [=](auto& coord)
                {
                    coord.translate(-unknownCoord);
                    coord.rotateClockwiseXY(numXYRotations);
                    coord.rotateClockwiseYZ(numYZRotations);
                    coord.rotateClockwiseXZ(numXZRotations);
                    coord.translate(knownCoord);
                };
                for (auto& coord : unknownTransformed)
                {
                    transformToMatchKnown(coord);
                }
                if (countIntersections(knownPositions, unknownTransformed) >= 12)
                {
                    Coord scannerPos{0,0,0};
                    transformToMatchKnown(scannerPos);
                    result.first = unknownTransformed;
                    result.second = scannerPos;

                }
            }
        }
    }
    return result;
}

std::pair<vector<Coord>, Coord> findOverlapping(const vector<Coord>& knownPositions, const vector<Coord>& unknownPositions)
{
    std::pair<vector<Coord>, Coord> result;
    for (const auto knownCoord : knownPositions)
    {
        for (const auto unknownCoord : unknownPositions)
        {
            const auto matchResult = attemptMatch(knownPositions, unknownPositions, knownCoord, unknownCoord);
            if (!matchResult.first.empty())
            {
                result = matchResult;
            }
        }
    }
    return result;
}

int main()
{
    string line;
    int currentScannerId = -1;
    vector<vector<Coord>> coordsForScanners;
    while (getline(cin, line))
    {
        static regex scannerIdRegex(R"(.*scanner\s*(\d+)\s+.*)");
        std::smatch scannerIdMatch;

        static regex coordRegex(R"(([-\d]+),([-\d]+),([-\d]+))");
        std::smatch coordMatch;

        if (regex_match(line, scannerIdMatch, scannerIdRegex))
        {
            currentScannerId = stoi(scannerIdMatch[1]);
            assert(currentScannerId == static_cast<int>(coordsForScanners.size()));
            coordsForScanners.push_back(vector<Coord>());
        }
        else if (regex_match(line, coordMatch, coordRegex))
        {
            coordsForScanners.back().emplace_back(stoi(coordMatch[1]), stoi(coordMatch[2]), stoi(coordMatch[3]));
        }
        else
        {
            assert(line.empty());
        }
    }

    vector<vector<Coord>> knownBeaconPositions = { coordsForScanners.front() };
    vector<Coord> scannerPositions = { {0, 0, 0} };

    vector<vector<Coord>> unknownBeaconPositions(coordsForScanners.begin() + 1, coordsForScanners.end());

    while (!unknownBeaconPositions.empty())
    {
        cout << "# unknownBeaconPositions: " << unknownBeaconPositions.size() << endl;
        for (const auto& knownPositions : knownBeaconPositions)
        {
            for (int i = 0; i < static_cast<int>(unknownBeaconPositions.size()); i++)
            {
                cout << "Trying with unknown coords: " << i << endl;
                const auto result = findOverlapping(knownPositions, unknownBeaconPositions[i]);
                if (!result.first.empty())
                {
                    knownBeaconPositions.push_back(result.first);
                    unknownBeaconPositions.erase(unknownBeaconPositions.begin() + i);
                    scannerPositions.push_back(result.second);
                    cout << "Mapped an unknown scanner into known scanner's frame of reference!" << endl;
                    break;
                }
            }
        }
    }
    vector<Coord> uniqueBeaconCoords;
    for (const auto& knownPositions : knownBeaconPositions)
    {
        for (const auto beaconCoord : knownPositions)
        {
            if (find(uniqueBeaconCoords.begin(), uniqueBeaconCoords.end(), beaconCoord) == uniqueBeaconCoords.end())
            {
                uniqueBeaconCoords.push_back(beaconCoord);
            }
        }
    }
    cout << "#uniqueBeaconCoords:" << uniqueBeaconCoords.size() << endl;
    int64_t maxScannerDistance = 0;
    for (const auto scannerA : scannerPositions)
    {
        for (const auto scannerB : scannerPositions)
        {
            const int64_t distance = abs(scannerA.x - scannerB.x) + 
                                     abs(scannerA.y - scannerB.y) + 
                                     abs(scannerA.z - scannerB.z);
            maxScannerDistance = max(maxScannerDistance, distance);
        }
    }
    cout << "maxScannerDistance: " << maxScannerDistance << endl;
}
