#include <iostream>
#include <regex>

#include <cassert>

using namespace std;

//#define BRUTE_FORCE

std::regex markerRegex(R"(^(\d+)x(\d+)$)");

int64_t decompressedLength(const std::string& compressed)
{
    int64_t result = 0;
    std::string::size_type pos = 0;
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
            const auto lengthOfDecompressedBlock = decompressedLength(compressed.substr(pos, repeatedblockSize));
            result += lengthOfDecompressedBlock * numReps;
            // Skip over block.
            pos += repeatedblockSize;
        }
        else
        {
            result++;
            pos++;
        }
    }
    return result;
}

#ifdef BRUTE_FORCE
string decompress(const std::string& compressed)
{
    string result;
    std::string::size_type pos = 0;
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
            const auto block = decompress(compressed.substr(pos, repeatedblockSize));
            std::cout << "block: " << block << std::endl;
            for (int i = 1; i <= numReps; i++)
            {
                result += block;
            }
            // Skip over block.
            pos += repeatedblockSize;
        }
        else
        {
            result += character;
            pos++;
        }
    }
    return result;
}
#endif

int main()
{
    string compressed;
    std::getline(cin, compressed);
    const auto decompressedLength = ::decompressedLength(compressed);
    std::cout << "decompressedLength: " << decompressedLength << std::endl;
#ifdef BRUTE_FORCE
    const string decompressed = decompress(compressed);
    std::cout << "BRUTE_FORCE decompressed; " << decompressed << std::endl;
    std::cout << "BRUTE_FORCE length: " << decompressed.size() << std::endl;
    assert(decompressed.size() == decompressedLength);
#endif
    return 0;
}
