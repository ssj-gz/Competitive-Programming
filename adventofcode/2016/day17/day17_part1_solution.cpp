#include <iostream>
#include <sstream>
#include <limits>
#include <iomanip>

#include <openssl/md5.h>

#include <cassert>

using namespace std;

string md5AsHex(const std::string& input)
{
    unsigned char result[MD5_DIGEST_LENGTH];
    MD5((unsigned char*) input.data(), input.size(), result);
    ostringstream md5HexOut;
    for (auto byte : result)
    {
        md5HexOut << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(byte);
    }
    return md5HexOut.str();
}


using namespace std;

void dfs(int x, int y, string& path, string& toHash, int& shortestPathLen, string& shortestPath)
{
    static constexpr int width = 4;
    static constexpr int height = 4;
    const int pathLen = path.length();
    if (x == width - 1 && y == height - 1)
    {
        if (pathLen < shortestPathLen)
        {
            shortestPathLen = pathLen;
            shortestPath = path;
            std::cout << "New shortestPath: " << shortestPath << std::endl;
        }
        return;
    }

    if (pathLen >= shortestPathLen)
    {
        std::cout << "path exceeds min; aborting" << std::endl;
        return;
    }

    const string hash = md5AsHex(toHash);
    std::cout << "path: " << path << " toHash: " << toHash << " hash: " << hash << " pos: " << x << ", " << y << std::endl;
    int directionIndex = 0;

    for (const auto& [dx, dy] : std::initializer_list<std::pair<int, int>>{ 
            {0, -1}, // Up.
            {0, +1}, // Down.
            {-1, 0}, // Left.
            {+1, 0}  // Right.
            })
    {
        if (hash[directionIndex] >= 'b' && hash[directionIndex] <= 'f')
        {
            const int newX = x + dx;
            const int newY = y + dy;
            if (!(newX < 0 || newX >= width || newY < 0 || newY >= height))
            {

                const char directionChar = "UDLR"[directionIndex];
                std::cout << " moving in direction " << directionChar << " directionIndex: " << directionIndex << " dx: "<< dx << " dy: " << dy << std::endl;

                toHash += directionChar;
                path += directionChar;
                dfs(newX, newY, path, toHash, shortestPathLen, shortestPath);
                path.pop_back();
                toHash.pop_back();
            }

        }
        directionIndex++;
    }
}

int main()
{
    string passcode;
    cin >> passcode;
    assert(cin);

    int shortestPathLen = std::numeric_limits<int>::max();
    string path;
    string shortestPath;
    dfs(0, 0, path, passcode, shortestPathLen, shortestPath);
    std::cout << "shortestPath: " << shortestPath << std::endl;

    return 0;
}
