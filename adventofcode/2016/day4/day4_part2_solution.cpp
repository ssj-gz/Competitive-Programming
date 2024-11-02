#include <iostream>
#include <regex>

#include <cassert>

using namespace std;

int main()
{
    std::regex roomRegex(R"(^([-a-z]+)-(\d+)\[([a-z]+)\]$)");
    string roomDescr;
    int64_t result = -1;
    while (std::getline(cin, roomDescr))
    {
        std::smatch roomMatch;
        std::cout << "roomDescr: " << roomDescr << std::endl;
        const bool matchSuccessful = std::regex_match(roomDescr, roomMatch, roomRegex);
        assert(matchSuccessful);

        const auto encryptedName = std::string(roomMatch[1]);
        std::cout << "encryptedName: " << encryptedName << std::endl;
        const auto sectorId = std::stoll(roomMatch[2]);
        std::cout << "sectorId: " << sectorId << std::endl;
        const auto checksum = std::string(roomMatch[3]);
        std::cout << "checksum: " << checksum << std::endl;
        constexpr int numLowercaseLetters = 26;

        string decryptedName = encryptedName;
        for (auto& letter : decryptedName)
        {
            if (letter == '-')
                letter = ' ';
            else
            {
                const int decryptedLetterIndex = ((letter - 'a') + sectorId) % numLowercaseLetters;
                letter = 'a' + decryptedLetterIndex;
            }
        }
        std::cout << "decryptedName: " << decryptedName << std::endl;
        if (decryptedName == "northpole object storage")
        {
            assert(result == -1);
            result = sectorId;
        }
    }
    std::cout << "result: " << result << std::endl;
    return 0;
}
