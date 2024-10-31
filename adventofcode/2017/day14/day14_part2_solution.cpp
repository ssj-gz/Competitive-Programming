#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>

#include <cassert>

using namespace std;

std::string calcKnotHash(const vector<int>& origLengths)
{
    auto lengths = origLengths;
    lengths.push_back(17); lengths.push_back(31); lengths.push_back(73); lengths.push_back(47); lengths.push_back(23);

    constexpr int numElements = 256;
    vector<int> elements;
    for (int i = 0; i < numElements; i++)
    {
        elements.push_back(i);
    }

    int currentPosition = 0;
    int skipSize = 0;
    for (int round = 1; round <= 64; round++)
    {
        for (const auto& length : lengths)
        {
            int reversalBeginPos = currentPosition;
            int reversalEndPos = (currentPosition + length - 1) % numElements;
            for (int i = 0; i < length / 2; i++)
            {
                swap(elements[reversalBeginPos], elements[reversalEndPos]);
                reversalBeginPos = (reversalBeginPos + 1) % numElements;
                reversalEndPos = (reversalEndPos - 1 + numElements) % numElements;
            }
            currentPosition = (currentPosition + length + skipSize) % numElements;
            skipSize++;
            std::cout << "After processing length: " << length << " elements are: " << std::endl;
            for (const auto& x : elements)
                std::cout << x << " ";
            std::cout << std::endl;
        }
    }
    constexpr int blockSize = 16;
    ostringstream outputHexStream;
    while (!elements.empty())
    {
        const vector<int> block(elements.begin(), elements.begin() + blockSize);
        elements.erase(elements.begin(), elements.begin() + blockSize);
        assert(elements.size() % blockSize == 0);

        int hashValue = 0;
        for (const auto& value : block)
        {
            hashValue ^= value;
        }

        outputHexStream << std::setfill('0') << std::setw(2) << std::hex << hashValue;
    }
    return outputHexStream.str();
}


int main()
{
    string keyString;
    getline(cin, keyString);

    const int gridSize = 128;
    vector<vector<int>> grid(gridSize, vector<int>(gridSize, -1));
    for (int hashNum = 0; hashNum < gridSize; hashNum++)
    {
        const auto hashInput = keyString + "-" + std::to_string(hashNum);
        vector<int> lengths;
        for (const auto& character : hashInput)
        {
            lengths.push_back(static_cast<int>(character));
        }
        const auto hash = calcKnotHash(lengths);

        string hashAsBinary;
        for (const auto& hexDigit : hash)
        {
            int decimal = 0;
            if (hexDigit >= 'a' && hexDigit <= 'f')
                decimal = hexDigit - 'a' + 10;
            else if (hexDigit >= '0' && hexDigit <= '9')
                decimal = hexDigit - '0';
            else
                assert(false);
            const int origDigit = decimal;

            string digitAsBinary;
            for (int i = 0; i < 4; i++)
            {
                if (decimal % 2 == 0)
                    digitAsBinary.insert(digitAsBinary.begin(), '0');
                else
                    digitAsBinary.insert(digitAsBinary.begin(), '1');

                decimal >>= 1;
            }
            std::cout << " hex: " << hex << " decimal: " << origDigit << " binary: " << digitAsBinary << std::endl;
            hashAsBinary += digitAsBinary;
        }
        assert(hashAsBinary.size() == gridSize);
        std::cout << "hashNum: " << hashNum << std::endl;
        std::cout << "binary: " << hashAsBinary << std::endl;
        for (int x = 0; x < gridSize; x++)
        {
            grid[x][hashNum] = (hashAsBinary[x] == '0' ? 0 : 1);
        }
    }

    struct Coord
    {
        int x = -1;
        int y = -1;
    };

    int numRegions = 0;
    for (int x = 0; x < gridSize; x++)
    {
        for (int y = 0; y < gridSize; y++)
        {
            if (grid[x][y] == 1)
            {
                numRegions++;
                grid[x][y] = -1;
                vector<Coord> toExplore = { {x, y} };
                while (!toExplore.empty())
                {
                    vector<Coord> nextToExplore;
                    for (const auto& coord : toExplore)
                    {
                        for (const auto& [dx, dy] : std::initializer_list<std::pair<int, int>>{ {-1, 0},
                                {+1, 0},
                                {0, -1},
                                {0, +1}
                                })
                        {
                            const int newX = coord.x + dx;
                            const int newY = coord.y + dy;
                            if (newX < 0 || newX >= gridSize || newY < 0 || newY >= gridSize)
                                continue;
                            if (grid[newX][newY] == 1)
                            {
                                grid[newX][newY] = -1;
                                nextToExplore.push_back({newX, newY});
                            }
                        }
                    }
                    toExplore = nextToExplore;
                }
            }
        }
    }
    std::cout << dec << "numRegions: " << numRegions << std::endl;
    return 0;
}
