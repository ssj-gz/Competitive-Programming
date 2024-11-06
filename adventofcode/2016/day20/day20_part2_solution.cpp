#include <iostream>
#include <regex>
#include <optional>

#include <cassert>

// TODO - remove this!
#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

//#define BRUTE_FORCE

using namespace std;

struct Range
{
    int64_t start = -1;
    int64_t end = -1;
};

constexpr int64_t minIp = 0;
constexpr int64_t maxIp = 4294967295; // TODO - reinstate this!
//constexpr int64_t maxIp = 100;

#ifdef BRUTE_FORCE
int64_t calcNumAllowedIpsBruteForce(const vector<Range>& blockedRanges)
{
    int64_t numAllowedIps = 0;
    for (int64_t ip = minIp; ip <= maxIp; ip++)
    {
        bool isAllowed = true;
        for (const auto& blockedRange : blockedRanges)
        {
            if (ip >= blockedRange.start && ip <= blockedRange.end)
                isAllowed = false;
        }
        if (isAllowed)
            numAllowedIps++;
        std::cout << " ip: " << ip << " isAllowed: " << isAllowed << std::endl;
    }
    return numAllowedIps;
}
#endif

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
        std::cout << "  flushing: " << currentMergedRange.value().start << " - " << currentMergedRange.value().end << std::endl;
        mergedBlockedRanges.push_back(currentMergedRange.value());
        currentMergedRange.reset();
    };

    for (const auto& blockedRange : blockedRanges)
    {
        std::cout << "blockedRange: " << blockedRange.start << " - " << blockedRange.end << std::endl;
        if (!currentMergedRange.has_value())
        {
            currentMergedRange = blockedRange;
            std::cout << " setting currentMergedRange to it" << std::endl;
        }
        else
        {
            if (blockedRange.end <= currentMergedRange.value().start)
            {
                std::cout << " ignoring" << std::endl;
                continue;
            }
            if (blockedRange.start > currentMergedRange.value().end + 1)
            {
                flushCurrentMergedRange();
                currentMergedRange = blockedRange;
                std::cout << "  reset currentMergedRange; now: " << currentMergedRange.value().start << " - " << currentMergedRange.value().end << std::endl;
                continue;
            }

            currentMergedRange.value().end = std::max(currentMergedRange.value().end, blockedRange.end);
            std::cout << "  extended currentMergedRange; now: " << currentMergedRange.value().start << " - " << currentMergedRange.value().end << std::endl;
        }
    }
    flushCurrentMergedRange();

    std::cout << "mergedBlockedRanges: " << std::endl;
    for (const auto& blockedRange : mergedBlockedRanges)
    {
        std::cout << " " << blockedRange.start << " - " << blockedRange.end << std::endl;
    }
    int64_t numAllowedIps = 0;
    int64_t prevValidBlockBegin = minIp;
    std::cout << "computing numAllowedIps " << std::endl;
    for (const auto& blockedRange : mergedBlockedRanges)
    {
        std::cout << " prevValidBlockBegin: " << prevValidBlockBegin << " blockedRange: " << blockedRange.start << " - " << blockedRange.end << std::endl;
        assert(prevValidBlockBegin == 0 || blockedRange.start > prevValidBlockBegin);
        numAllowedIps += blockedRange.start - prevValidBlockBegin;
        std::cout <<" blockedRange.start - prevValidBlockBegin: " << (blockedRange.start - prevValidBlockBegin) << std::endl;
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

#ifdef BRUTE_FORCE
    struct timeval time;
    gettimeofday(&time,NULL);
    srand(static_cast<unsigned int>((time.tv_sec * 1000) + (time.tv_usec / 1000)));
    const int numRanges = rand() % 10 + 1;
    for (int i = 1; i <= numRanges; i++)
    {
        Range range;
        range.start = rand() % (maxIp + 1);
        range.end = rand() % (maxIp + 1);
        if (range.start > range.end)
            swap(range.start, range.end);
        blockedRanges.push_back(range);
    }
    sort(blockedRanges.begin(), blockedRanges.end(), [](const auto& lhsRange, const auto& rhsRange) { return lhsRange.start < rhsRange.start; });
    std::cout << "Blocked ranges: " << std::endl;
    for (const auto x : blockedRanges) std::cout << " " << x.start << " - " << x.end << std::endl;
    const int64_t numAllowedIpsBruteForce = calcNumAllowedIpsBruteForce(blockedRanges);
    const int64_t numAllowedIps = calcNumAllowedIps(blockedRanges);
    std::cout << "numAllowedIpsBruteForce: " << numAllowedIpsBruteForce << std::endl;
    std::cout << "numAllowedIps          : " << numAllowedIps << std::endl;
    assert(numAllowedIpsBruteForce == numAllowedIps);
    return 0;
#else
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
#endif

    return 0;
}
