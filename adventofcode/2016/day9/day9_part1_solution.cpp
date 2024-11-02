#include <iostream>
#include <regex>

#include <cassert>

using namespace std;

int main()
{
    string compressed;
    std::getline(cin, compressed);
    std::string::size_type pos = 0;
    std::regex markerRegex(R"(^(\d+)x(\d+)$)");
    string decompressed;
    while (pos < compressed.size())
    {
        const auto character = compressed[pos];
        if (character == '(')
        {
            const auto markerBeginPos = pos + 1;
            while (compressed[pos] != ')')
            {
                pos++;
            }
            const auto markerContents = compressed.substr(markerBeginPos, pos - markerBeginPos);
            std::cout << " markerContents: " << markerContents << std::endl;
            std::smatch markerMatch;
            const bool matchSuccessful = std::regex_match(markerContents, markerMatch, markerRegex);
            assert(matchSuccessful);
            const int repeatedblockSize = std::stoi(markerMatch[1]);
            const int numReps = std::stoi(markerMatch[2]);
            std::cout << "repeatedblockSize: " << repeatedblockSize << std::endl;
            std::cout << "numReps: " << numReps << std::endl;
            // Skip over ")".
            pos++;
            const auto block = compressed.substr(pos, repeatedblockSize);
            std::cout << "block: " << block << std::endl;
            for (int i = 1; i <= numReps; i++)
            {
                decompressed += block;
            }
            // Skip over block.
            pos += repeatedblockSize;
        }
        else
        {
            decompressed += character;
            pos++;
        }
    }
    std::cout << "decompressed; " << decompressed << std::endl;
    std::cout << "length: " << decompressed.size() << std::endl;
    return 0;
}
