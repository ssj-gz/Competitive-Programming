#include <iostream>
#include <vector>
#include <map>
#include <regex>

#include <cassert>

using namespace std;

int main()
{
    std::regex claimRegex(R"(^#(\d+)\s*@\s*(\d+),(\d+):\s*(\d+)x(\d+)$)");
    string claimLine;
    struct Coord
    {
        int64_t x = -1;
        int64_t y = -1;
        auto operator<=>(const Coord& other) const = default;
    };
    std::map<Coord, int> numClaimsOnCoord;
    struct Claim
    {
        int id = -1;
        int64_t tlX = -1;
        int64_t tlY = -1;
        int64_t width = -1;
        int64_t height = -1;
    };
    vector<Claim> claims;
    while (getline(cin, claimLine))
    {
        std::cout << "claimLine: " << claimLine << std::endl;
        std::smatch claimMatch;
        const bool matchSuccessful = std::regex_match(claimLine, claimMatch, claimRegex);
        assert(matchSuccessful);
        Claim claim;
        claim.id = stoll(claimMatch[1]);
        claim.tlX = stoll(claimMatch[2]);
        claim.tlY = stoll(claimMatch[3]);
        claim.width = stoll(claimMatch[4]);
        claim.height = stoll(claimMatch[5]);
        claims.push_back(claim);
        for (int64_t x = claim.tlX; x < claim.tlX + claim.width; x++)
        {
            for (int64_t y = claim.tlY; y < claim.tlY + claim.height; y++)

            {
                numClaimsOnCoord[{x, y}]++;
            }
        }
    }

    int unsharedClaimId = -1;
    for (const auto& claim : claims)
    {
        bool isUnshared = true;
        for (int64_t x = claim.tlX; x < claim.tlX + claim.width; x++)
        {
            for (int64_t y = claim.tlY; y < claim.tlY + claim.height; y++)
            {
                if (numClaimsOnCoord[{x, y}] != 1)
                    isUnshared = false;
            }
        }
        if (isUnshared)
        {
            assert(unsharedClaimId == -1);
            unsharedClaimId = claim.id;
        }
    }

    std::cout << "unsharedClaimId: " << unsharedClaimId << std::endl;
    return 0;
}
