#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <regex>
#include <limits>
#include <tuple>
#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!
#include <cassert>

using namespace std;

constexpr int maxXDbg = 101;
constexpr int maxYDbg = maxXDbg;
constexpr int maxZDbg = maxXDbg;

vector<vector<vector<string>>> generateDbgTestGrid()
{
    vector<vector<vector<string>>> result(maxXDbg + 1, vector<vector<string>>(maxYDbg + 1, vector<string>(maxZDbg + 1)));
    return result;
}

enum Overlap
{
    None,
    Total,
    Partial
};

class Cuboid
{
    public:
        Cuboid(int xBegin,
                int xEnd,
                int yBegin,
                int yEnd,
                int zBegin,
                int zEnd)
            : xBegin(xBegin),
              xEnd(xEnd),
              yBegin(yBegin),
              yEnd(yEnd),
              zBegin(zBegin),
              zEnd(zEnd)
        {
            normalise();

        }
        Cuboid() = default;

        auto operator<=>(const Cuboid& other) const = default;

        bool contains(const Cuboid& other) const
        {
            if (other.xBegin < xBegin || other.xEnd > xEnd)
                return false;
            if (other.yBegin < yBegin || other.yEnd > yEnd)
                return false;
            if (other.zBegin < zBegin || other.zEnd > zEnd)
                return false;
            return true;
        }

        Cuboid rotatedClockwiseXY(int numTimes) const
        {
            Cuboid rotated(*this);
            for (int i = 0; i  < numTimes; i++)
            {
                swap(rotated.xBegin,rotated.yBegin);
                rotated.yBegin = -rotated.yBegin;
                swap(rotated.xEnd,rotated.yEnd);
                rotated.yEnd = -rotated.yEnd;
            }
            rotated.normalise();
            return rotated;
        }
        Cuboid rotatedClockwiseXZ(int numTimes) const
        {
            Cuboid rotated(*this);
            for (int i = 0; i  < numTimes; i++)
            {
                swap(rotated.xBegin,rotated.zBegin);
                rotated.zBegin = -rotated.zBegin;
                swap(rotated.xEnd,rotated.zEnd);
                rotated.zEnd = -rotated.zEnd;
            }
            rotated.normalise();
            return rotated;
        }
        Cuboid rotatedClockwiseYZ(int numTimes) const
        {
            Cuboid rotated(*this);
            for (int i = 0; i  < numTimes; i++)
            {
                swap(rotated.yBegin,rotated.zBegin);
                rotated.zBegin = -rotated.zBegin;
                swap(rotated.yEnd,rotated.zEnd);
                rotated.zEnd = -rotated.zEnd;
            }
            rotated.normalise();
            return rotated;
        }
        void translate(int dx, int dy, int dz)
        {
            xBegin += dx;
            yBegin += dy;
            zBegin += dz;

            xEnd += dx;
            yEnd += dy;
            zEnd += dz;
            assertNormalised();
        }
        int64_t volume() const
        {
            return (int64_t(abs(xEnd - xBegin)) + 1) * 
                   (abs(yEnd - yBegin) + 1) * 
                   (abs(zEnd - zBegin) + 1);
        }

        

        int xBegin = -1;
        int xEnd = -1;
        int yBegin = -1;
        int yEnd = -1;
        int zBegin = -1;
        int zEnd = -1;

        void normalise()
        {
            if (xBegin > xEnd)
                swap(xBegin, xEnd);
            if (yBegin > yEnd)
                swap(yBegin, yEnd);
            if (zBegin > zEnd)
                swap(zBegin, zEnd);
        }
        
        void assertNormalised() const
        {
            assert(xBegin <= xEnd);
            assert(yBegin <= yEnd);
            assert(zBegin <= zEnd);
        }
};

Overlap overlap(const Cuboid& cuboidA, const Cuboid& cuboidB)
{
    if (cuboidA == cuboidB)
        return Overlap::Total;

    if (cuboidA.xEnd < cuboidB.xBegin || cuboidA.xBegin > cuboidB.xEnd)
        return Overlap::None;
    if (cuboidA.yEnd < cuboidB.yBegin || cuboidA.yBegin > cuboidB.yEnd)
        return Overlap::None;
    if (cuboidA.zEnd < cuboidB.zBegin || cuboidA.zBegin > cuboidB.zEnd)
        return Overlap::None;

    return Overlap::Partial;
}


ostream& operator<<(ostream& os, const Cuboid& cuboid)
{
    os << " x: " << cuboid.xBegin << ".." << cuboid.xEnd << "," << 
          " y: " << cuboid.yBegin << ".." << cuboid.yEnd << "," << 
          " z: " << cuboid.zBegin << ".." << cuboid.zEnd;
    return os;
}

void debugWriteCuboid(vector<vector<vector<string>>>& grid, const Cuboid& cuboid, char letter)
{
    cuboid.assertNormalised();
    for (int x = cuboid.xBegin; x <= cuboid.xEnd; x++)
    {
        for (int y = cuboid.yBegin; y <= cuboid.yEnd; y++)
        {
            for (int z = cuboid.zBegin; z <= cuboid.zEnd; z++)
            {
                grid[x][y][z] += letter;
            }
        }
    }
}

std::tuple<bool, vector<Cuboid>, vector<Cuboid>> sliceDisjointXY(const Cuboid& cuboidA, const Cuboid& cuboidB)
{
    std::tuple<bool, vector<Cuboid>, vector<Cuboid>> result{};

    if (cuboidA.zBegin == cuboidB.zBegin &&
            cuboidA.zEnd == cuboidB.zEnd )
    {
        return {false, vector<Cuboid>(), vector<Cuboid>()};
    }
    if (cuboidA.zEnd < cuboidB.zBegin || cuboidA.zBegin > cuboidB.zEnd)
    {
        return {false, vector<Cuboid>(), vector<Cuboid>()};
    }
    vector<Cuboid> slicesA;
    vector<Cuboid> slicesB;
    vector<int> zCoords;
    zCoords.push_back(cuboidA.zBegin);
    zCoords.push_back(cuboidA.zEnd + 1);
    zCoords.push_back(cuboidB.zBegin);
    zCoords.push_back(cuboidB.zEnd + 1);
    sort(zCoords.begin(), zCoords.end());
    zCoords.erase(unique(zCoords.begin(), zCoords.end()), zCoords.end());
#if 0
    cout << "z coords: " << endl;
    for(const auto z :zCoords)
    {
        cout << " " << z << endl;
    }
#endif
    for (int i = 1; i < static_cast<int>(zCoords.size()); i++)
    {
        if (cuboidA.zBegin <= zCoords[i - 1] && cuboidA.zEnd >= zCoords[i] - 1)
        {
            slicesA.push_back({cuboidA.xBegin, cuboidA.xEnd,
                    cuboidA.yBegin, cuboidA.yEnd,
                    zCoords[i - 1], zCoords[i] - 1});
            slicesA.back().assertNormalised();
        }

        if (cuboidB.zBegin <= zCoords[i - 1] && cuboidB.zEnd >= zCoords[i] - 1)
        {
            slicesB.push_back({cuboidB.xBegin, cuboidB.xEnd,
                    cuboidB.yBegin, cuboidB.yEnd,
                    zCoords[i - 1], zCoords[i] - 1});
            slicesB.back().assertNormalised();
        }
    }


    return {true, slicesA, slicesB};
}

struct Instruction
{
    bool switchOn = false;
    Cuboid cuboid;
};

std::pair<vector<Cuboid>, vector<Cuboid>> split(const Cuboid& cuboidA, const Cuboid& cuboidB)
{
    assert(overlap(cuboidA, cuboidB) == Overlap::Partial);
    vector<Cuboid> aPieces = { cuboidA };
    vector<Cuboid> bPieces = { cuboidB };
    bool changeMade = false;
    do
    {
        changeMade = false;
        for (auto aIter = aPieces.begin(); aIter != aPieces.end(); aIter++)
        {
            for (auto bIter = bPieces.begin(); bIter != bPieces.end(); bIter++)
            {
                if (overlap(*aIter, *bIter) == Overlap::Partial)
                {
                    {
                        // Do these ned to be sliced in the X-Y Plane?
                        const auto [sliceOccurred, slicedA, slicedB] = sliceDisjointXY(*aIter, *bIter);
                        if (sliceOccurred)
                        {
                            aPieces.erase(aIter);
                            bPieces.erase(bIter);
                            aPieces.insert(aPieces.end(), slicedA.begin(), slicedA.end());
                            bPieces.insert(bPieces.end(), slicedB.begin(), slicedB.end());
                            changeMade = true;
                            break;
                        }
                    }
                    {
                        // No - but how about X-Z?
                        const auto aPieceRotated = aIter->rotatedClockwiseXZ(1);
                        const auto bPieceRotated = bIter->rotatedClockwiseXZ(1);
                        const auto [sliceOccurred, slicedA, slicedB] = sliceDisjointXY(aPieceRotated, bPieceRotated);
                        if (sliceOccurred)
                        {
                            aPieces.erase(aIter);
                            bPieces.erase(bIter);
                            for (const auto& newAPiece : slicedA)
                            {
                                aPieces.push_back(newAPiece.rotatedClockwiseXZ(3));
                            }
                            for (const auto& newBPiece : slicedB)
                            {
                                bPieces.push_back(newBPiece.rotatedClockwiseXZ(3));
                            }
                            changeMade = true;
                            break;
                        }
                    }
                    {
                        // No - how about the final plane?
                        const auto aPieceRotated = aIter->rotatedClockwiseYZ(1);
                        const auto bPieceRotated = bIter->rotatedClockwiseYZ(1);
                        const auto [sliceOccurred, slicedA, slicedB] = sliceDisjointXY(aPieceRotated, bPieceRotated);
                        if (sliceOccurred)
                        {
                            aPieces.erase(aIter);
                            bPieces.erase(bIter);
                            for (const auto& newAPiece : slicedA)
                            {
                                aPieces.push_back(newAPiece.rotatedClockwiseYZ(3));
                            }
                            for (const auto& newBPiece : slicedB)
                            {
                                bPieces.push_back(newBPiece.rotatedClockwiseYZ(3));
                            }
                            changeMade = true;
                            break;
                        }
                    }

                    

                    break;
                }
            }
            if (changeMade)
                break;
        }
    } while(changeMade);
    return {aPieces, bPieces};
}

void handleInstruction(const Instruction& originalInstruction, map<Cuboid, bool>& currentRegions)
{
    list<Instruction> toAdd = { originalInstruction };
    while (!toAdd.empty())
    {
        //cout << "currentRegions.size(): " << currentRegions.size() << " toAdd: " << toAdd.size() << endl;
        const auto instruction = toAdd.front();
        toAdd.pop_front();

        bool shatterOccurred = false;
        for (auto& [existingCuboid, value]: currentRegions)
        {
            const auto overlapType = overlap(existingCuboid, instruction.cuboid);
            if (overlapType == Overlap::Total)
            {
                // No need to shatter; just update.
                value = instruction.switchOn;
            }
            else if (overlapType == Overlap::Partial)
            {
#if 1
                if (instruction.cuboid.contains(existingCuboid))
                {
                    // No need to shatter - just remove existing cuboid - we'll overwrite it later.
                    currentRegions.erase(existingCuboid);
                    // Can't keep iterating, though.
                    toAdd.push_back(instruction);
                    break;
                }
                else
#endif
                {
                    shatterOccurred = true;

                    const auto [oldPieces, newPieces] = split(existingCuboid, instruction.cuboid);

                    currentRegions.erase(existingCuboid);
                    for (const auto& oldPiece : oldPieces)
                    {
                        if (!instruction.cuboid.contains(oldPiece))
                        {
                            // Preserve this old piece.
                            currentRegions[oldPiece] = value;
                        }
                    }
#if 0
                    for (const auto& newPiece : newPieces)
                    {
                        toAdd.push_back({instruction.switchOn, newPiece});
                    }
#endif
                    // Didn't add successfully - schedule for re-try.
                    toAdd.push_back(instruction);

                }

                break;
            }
        }

        if (!shatterOccurred)
        {
            currentRegions[instruction.cuboid] = instruction.switchOn;
        }
    }
}


int64_t doInstructions(const vector<Instruction>& instructions)
{
    map<Cuboid, bool> currentRegions;
    const int numInstructions = static_cast<int>(instructions.size());
    for (int i = 0; i < numInstructions; i++)
    {
        cout << "Handling instruction #" << i << " of " << numInstructions << endl;
        const auto& instruction = instructions[i];
        handleInstruction(instruction, currentRegions);
    }
    int64_t count = 0;
    for (const auto& [cuboid, isOn] : currentRegions)
    {
        if (isOn)
            count += cuboid.volume();
    }
    return count;
}

int64_t doInstructionsBruteForce(const vector<Instruction>& instructions)
{
    vector<vector<vector<bool>>> grid(maxXDbg + 1, vector<vector<bool>>(maxYDbg + 1, vector<bool>(maxZDbg + 1, false)));
    for (const auto& instruction : instructions)
    {
        const auto cuboid = instruction.cuboid;
            for (int x = cuboid.xBegin; x <= cuboid.xEnd; x++)
            {
                for (int y = cuboid.yBegin; y <= cuboid.yEnd; y++)
                {
                    for (int z = cuboid.zBegin; z <= cuboid.zEnd; z++)
                    {
                        grid[x][y][z] = instruction.switchOn;
                    }
                }
            }
    }

    int64_t count = 0;
    for (int x = 0; x <= maxXDbg; x++)
    {
        for (int y = 0; y <= maxYDbg; y++)
        {
            for (int z = 0; z <= maxZDbg; z++)
            {
                if (grid[x][y][z] )
                    count++;
            }
        }
    }
    return count;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand(static_cast<unsigned int>((time.tv_sec * 1000) + (time.tv_usec / 1000)));

        auto genRandomCuboid = []()
        {
            return Cuboid(
                    rand() % maxXDbg,
                    rand() % maxXDbg,
                    rand() % maxYDbg,
                    rand() % maxYDbg,
                    rand() % maxZDbg,
                    rand() % maxZDbg
                    );
        };

        auto cuboidA = genRandomCuboid();
        auto cuboidB = genRandomCuboid();
        //Cuboid cuboidA = Cuboid(2, 4, 5,7,2,9);
        //Cuboid cuboidB = Cuboid(0, 9, 3,8,5,7);


        cout << "cuboidA: " << cuboidA << endl;
        cout << "cuboidB: " << cuboidB << endl;

        auto correctTestGrid = generateDbgTestGrid();
        debugWriteCuboid(correctTestGrid, cuboidA, 'A');
        debugWriteCuboid(correctTestGrid, cuboidB, 'B');

#if 0
        const auto [sliceOccurred, slicedA, slicedB] = sliceDisjointXY(cuboidA, cuboidB);
        if (sliceOccurred)
        {
            cout << "slice occurred!" << endl;
            auto actualTestGrid = generateDbgTestGrid();
            for (const auto& sliceA : slicedA)
            {
                debugWriteCuboid(actualTestGrid, sliceA, 'A');
                cout << " sliceA: " << sliceA << endl;
            }
            for (const auto& sliceB : slicedB)
            {
                debugWriteCuboid(actualTestGrid, sliceB, 'B');
                cout << " sliceB: " << sliceB << endl;
            }
            assert(correctTestGrid == actualTestGrid);

            vector<Cuboid> all(slicedA);
            all.insert(all.end(), slicedB.begin(), slicedB.end());
            for (const auto x : all)
            {
                for (const auto y : all)
                {
                    assert(!get<0>(sliceDisjointXY(x, y)));
                }
            }

        }
        else
        {
            cout << "slice did *not* occur!" << endl;
        }
#endif
        if (overlap(cuboidA, cuboidB) != Overlap::Partial)
        {
            cout << "Didn't bother to split" << endl;
        }
        else
        {
            const auto [aPieces, bPieces] = split(cuboidA, cuboidB);
            auto actualTestGrid = generateDbgTestGrid();
            for (const auto& sliceA : aPieces)
            {
                debugWriteCuboid(actualTestGrid, sliceA, 'A');
                cout << " sliceA: " << sliceA << endl;
            }
            for (const auto& sliceB : bPieces)
            {
                debugWriteCuboid(actualTestGrid, sliceB, 'B');
                cout << " sliceB: " << sliceB << endl;
            }
            assert(correctTestGrid == actualTestGrid);
            vector<Cuboid> all(aPieces);
            all.insert(all.end(), bPieces.begin(), bPieces.end());
            for (const auto x : all)
            {
                for (const auto y : all)
                {
                    assert(overlap(x, y) != Overlap::Partial);
                }
            }
            cout << "Split into " << all.size() << " pieces" << endl;

        }



        return 0;
    }

    static regex instructionRegex(R"((on|off)\s*x=([-\d]+)\.\.([-\d]+),y=([-\d]+)\.\.([-\d]+),z=([-\d]+)\.\.([-\d]+).*)");

    std::smatch instructionMatch;
    string instructionLine;
    const int maxCoord = 50;
    vector<vector<vector<bool>>> grid(2 * maxCoord + 1, vector<vector<bool>>(2 * maxCoord + 1, vector<bool>(2 * maxCoord + 1, false)));
    int minX = numeric_limits<int>::max(), minY = numeric_limits<int>::max(), minZ = numeric_limits<int>::max();

    vector<Instruction> instructions;

    while (getline(cin, instructionLine))
    {
        cout << "instructionLine: " << instructionLine << endl;

        if (regex_match(instructionLine, instructionMatch, instructionRegex))
        {
            const bool on = (instructionMatch[1] == "on");

            const int xBegin = stoi(instructionMatch[2]);
            const int xEnd = stoi(instructionMatch[3]);
            cout << "xBegin: " << xBegin << " xEnd: " << xEnd << endl;
            assert(xBegin <= xEnd);

            const int yBegin = stoi(instructionMatch[4]);
            const int yEnd = stoi(instructionMatch[5]);
            assert(yBegin <= yEnd);

            const int zBegin = stoi(instructionMatch[6]);
            const int zEnd = stoi(instructionMatch[7]);
            assert(zBegin <= zEnd);

            minX = min(minX, xBegin);
            minY = min(minY, yBegin);
            minZ = min(minZ, zBegin);

#if 0
            for (int x = xBegin; x <= xEnd; x++)
            {
                for (int y = yBegin; y <= yEnd; y++)
                {
                    for (int z = zBegin; z <= zEnd; z++)
                    {
                        grid[x + maxCoord][y + maxCoord][z + maxCoord] = on;
                    }
                }
            }
#endif
            instructions.push_back({on, {xBegin, xEnd, yBegin, yEnd, zBegin, zEnd}});

        }
        else
        {
            assert(false);
        }

    }

    for (auto& instruction : instructions)
    {
        instruction.cuboid.translate(-minX, -minY, -minZ);
    }
    for (auto& instruction : instructions)
    {
        cout << "switch " << (instruction.switchOn ? "on" : "off") << " cuboid: " << instruction.cuboid << endl;
        instruction.cuboid.assertNormalised();
    }

    //const auto resultBruteForce = doInstructionsBruteForce(instructions);
    //cout << "brute force: " << resultBruteForce << endl;
    const auto resultOptimised = doInstructions(instructions);
    cout << "optimised  : " << resultOptimised << endl;

#if 0
    int64_t count = 0;
    for (int x = -maxCoord; x <= maxCoord; x++)
    {
        for (int y = -maxCoord; y <= maxCoord; y++)
        {
            for (int z = -maxCoord; z <= maxCoord; z++)
            {
                if (grid[x + maxCoord][y + maxCoord][z + maxCoord] )
                    count++;
            }
        }
    }
    cout << count << endl;
#endif

#if 0
    struct Blah
    {
        vector<int> blee;
    };

    cout << "Doing stuff" << endl;
    for (int i = 0; i < 456533; i++)
    {
        vector<vector<Blah>> bloo(1'000, vector<Blah>(1'000));
        bloo[0][0].blee.resize(rand() % 500);
        cout << "i:" << i << " - " << bloo[0][0].blee.size() << endl;
    }
    cout << "Done" << endl;
#endif


}

