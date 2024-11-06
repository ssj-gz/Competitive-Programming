#include <iostream>
#include <regex>
#include <optional>

#include <cassert>

using namespace std;

struct Range
{
    int64_t start = -1;
    int64_t end = -1;
};

constexpr int64_t minIp = 0;
constexpr int64_t maxIp = 4294967295;

int64_t calcNumAllowedIps(const vector<Range>& blockedRangesOrig)
{
    auto blockedRanges = blockedRangesOrig;
    sort(blockedRanges.begin(), blockedRanges.end(), [](const auto& lhsRange, const auto& rhsRange) { return lhsRange.start < rhsRange.start; });

    std::optional<Range> currentMergedRange;
    vector<Range> mergedBlockedRanges;
    auto flushCurrentMergedRange = [&currentMergedRange, &mergedBlockedRanges]()
    {
        if (!currentMergedRange.has_value())
            return;
        mergedBlockedRanges.push_back(currentMergedRange.value());
        currentMergedRange.reset();
    };

    for (const auto& blockedRange : blockedRanges)
    {
        if (!currentMergedRange.has_value())
        {
            currentMergedRange = blockedRange;
        }
        else
        {
            if (blockedRange.end <= currentMergedRange.value().start)
            {
                continue;
            }
            if (blockedRange.start > currentMergedRange.value().end + 1)
            {
                flushCurrentMergedRange();
                currentMergedRange = blockedRange;
                continue;
            }

            currentMergedRange.value().end = std::max(currentMergedRange.value().end, blockedRange.end);
        }
    }
    flushCurrentMergedRange();

    int64_t numAllowedIps = 0;
    int64_t prevValidBlockBegin = minIp;
    for (const auto& blockedRange : mergedBlockedRanges)
    {
        assert(prevValidBlockBegin == 0 || blockedRange.start > prevValidBlockBegin);
        numAllowedIps += blockedRange.start - prevValidBlockBegin;
        prevValidBlockBegin = blockedRange.end + 1;
    }
    assert(!mergedBlockedRanges.empty());
    numAllowedIps += maxIp - mergedBlockedRanges.back().end;

    return numAllowedIps;
}

int main()
{
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
    const int64_t numAllowedIps = calcNumAllowedIps(blockedRanges);
    std::cout << "numAllowedIps: " << numAllowedIps << std::endl;

    return 0;
}
