#include <iostream>
#include <regex>

#include <cassert>

using namespace std;

int main()
{
    std::regex roomRegex(R"(^([-a-z]+)-(\d+)\[([a-z]+)\]$)");
    string roomDescr;
    int64_t result = 0;
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
        char letterFrequency[numLowercaseLetters] = {};

        for (const auto& letter : encryptedName)
        {
            if (letter != '-')
            {
                assert(letter >= 'a' && letter <= 'z');
                letterFrequency[letter - 'a']++;
            }
        }
        string expectedCheckSum;
        for (char letter = 'a'; letter <= 'z'; letter++)
        {
            expectedCheckSum.push_back(letter);
        }
        sort(expectedCheckSum.begin(), expectedCheckSum.end(), [letterFrequency](const auto& lhsLetter, const auto& rhsLetter)
                {
                if (letterFrequency[lhsLetter - 'a'] != letterFrequency[rhsLetter - 'a'])
                    return letterFrequency[rhsLetter - 'a'] < letterFrequency[lhsLetter - 'a'];
                return lhsLetter < rhsLetter;
                });
        expectedCheckSum.erase(expectedCheckSum.begin() + 5, expectedCheckSum.end());
        std::cout << "expectedCheckSum: " << expectedCheckSum << std::endl;
        std::cout << "Real room? " << (checksum == expectedCheckSum) << std::endl;
        if (checksum == expectedCheckSum)
            result += sectorId;
    }
    std::cout << "result: " << result << std::endl;
    return 0;
}
