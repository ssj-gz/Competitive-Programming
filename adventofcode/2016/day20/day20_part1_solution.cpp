#include <iostream>
#include <regex>

#include <cassert>

using namespace std;

int main()
{
    struct Range
    {
        int64_t start = -1;
        int64_t end = -1;
    };
    std::regex rangeRegex(R"(^(\d+)-(\d+)$)");
    string rangeDescr;

    vector<Range> blockedRanges;

    while (getline(cin, rangeDescr))
    {
        std::smatch rangeMatch;
        const bool matchSuccessful = std::regex_match(rangeDescr, rangeMatch, rangeRegex);
        assert(matchSuccessful);
        Range range;
        range.start = std::stoll(rangeMatch[1]);
        range.end = std::stoll(rangeMatch[2]);

        blockedRanges.push_back(range);
    }
    sort(blockedRanges.begin(), blockedRanges.end(), [](const auto& lhsRange, const auto& rhsRange) { return lhsRange.start < rhsRange.end; });
    for (const auto& blockedRange : blockedRanges)
    {
        std::cout << blockedRange.start << " - " << blockedRange.end << std::endl;
    }
    int64_t candidateUblockedIp = 0;
    while (true)
    {
        bool wasBlocked = false;
        for (const auto& blockedRange : blockedRanges)
        {
            const bool candidateIsBlocked = !(candidateUblockedIp < blockedRange.start || candidateUblockedIp > blockedRange.end);
            if (candidateIsBlocked)
            {
                std::cout << "The old candidateUblockedIp " << candidateUblockedIp << " was blocked;";
                candidateUblockedIp = blockedRange.end + 1;
                std::cout << " re-trying with " << candidateUblockedIp << std::endl;
                wasBlocked = true;
            }
        }
        if (!wasBlocked)
            break;
    }
    std::cout << "lowest unblocked ip: " << candidateUblockedIp << std::endl;

    return 0;
}
