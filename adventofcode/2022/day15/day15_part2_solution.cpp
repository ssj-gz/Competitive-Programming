#include <iostream>
#include <vector>
#include <deque>
#include <map>
#include <set>
#include <queue>
#include <regex>
#include <limits>
#include <cassert>

using namespace std;

struct SensorInfo
{
    int64_t posX = -1;
    int64_t posY = -1;
    int64_t nearestBeaconX = -1;
    int64_t nearestBeaconY = -1;
};

struct Range
{
    int64_t startX = -1;
    int64_t endX = -1;
    auto operator<=>(const Range& other) const = default;
};

struct Coord
{
    int64_t x = -1;
    int64_t y = -1;
    auto operator<=>(const Coord& other) const = default;
};

const int64_t beaconMaxX = 4'000'000;
const int64_t beaconMaxY = 4'000'000;

int64_t findSoleXNotCoveredByRanges(const vector<Range>& rangesOrig)
{
    deque<Range> ranges(rangesOrig.begin(), rangesOrig.end());
    int64_t uncoveredX = -1;
    sort(ranges.begin(), ranges.end(), [](const auto& lhs, const auto& rhs)
            {
                return lhs.startX < rhs.startX;
            });
    for (auto& range : ranges)
    {
        range.startX = max<int64_t>(0, range.startX);
        range.endX = min(beaconMaxX, range.endX);
    }
    ranges.erase(std::remove_if(ranges.begin(), ranges.end(), [](const auto& range) {
                return range.startX > beaconMaxX || range.endX < 0 || range.startX > range.endX;
            }), ranges.end());
    std::priority_queue<int64_t, vector<int64_t>, std::greater<>> yCoordsOfCurrentRangesEnds;
    int64_t currentX = std::numeric_limits<int64_t>::min();
    while (true)
    {
        // Remove ranges/ y-coords of ranges that are "behind" us.
        while (!ranges.empty() && ranges.front().endX < currentX)
        {
            ranges.pop_front();
        }
        while (!yCoordsOfCurrentRangesEnds.empty() && yCoordsOfCurrentRangesEnds.top() < currentX)
        {
            yCoordsOfCurrentRangesEnds.pop();
        }

        if (ranges.empty() && yCoordsOfCurrentRangesEnds.empty())
            break;
        const bool wasStillWithinRange = !yCoordsOfCurrentRangesEnds.empty();

        const int64_t bestRangeStartX = (!ranges.empty() ? ranges.front().startX : std::numeric_limits<int64_t>::max());
        const int64_t bestEndOfCurrentRangeX = (!yCoordsOfCurrentRangesEnds.empty() ? yCoordsOfCurrentRangesEnds.top() : std::numeric_limits<int64_t>::max());
        const int64_t newCurrentX = min(bestRangeStartX, bestEndOfCurrentRangeX);
        assert(newCurrentX != std::numeric_limits<int64_t>::max());

        // We may have chosen a range; update yCoordsOfCurrentRangesEnds regardless.
        while (!ranges.empty() && ranges.front().startX == newCurrentX)
        {
            yCoordsOfCurrentRangesEnds.push(ranges.front().endX);
            ranges.pop_front();
        }

        if (currentX != std::numeric_limits<int64_t>::min())
        {
            assert(newCurrentX >= currentX);
            if (!wasStillWithinRange)
            {
                assert(newCurrentX == currentX + 1);
                assert(uncoveredX == -1);
                uncoveredX = newCurrentX - 1;
                std::cout << "Found uncoveredX: " << uncoveredX << std::endl;
            }
        }

        currentX = newCurrentX + 1; // We've processed this currentX, and have checked that it's not uncovered; move on to (at least) the next X.
    }

    return uncoveredX;
}

Coord hiddenBeaconPosition(const std::vector<SensorInfo>& sensorInfos)
{
    // Basic approach - as we imagine scanning down from 0 to beaconMaxY, 
    // certain (rare-ish) "events" occur - it is only during such events that the gap we are looking
    // for can appear.
    //
    // See the insertions into eventsYCoords for more info.
    Coord answer = {-1, -1};
    set<int64_t> eventsYCoords;
    for (const auto& sensorInfo : sensorInfos)
    {
        const int64_t distToNearestBeacon = abs(sensorInfo.posX - sensorInfo.nearestBeaconX) + abs(sensorInfo.posY - sensorInfo.nearestBeaconY);
        eventsYCoords.insert(sensorInfo.posY - distToNearestBeacon); // Topmost y-coord of this block of '#'s.
        eventsYCoords.insert(sensorInfo.posY + 1); // One-past-middle i.e. where this block of '#'s starts to narrow as y increases, instead of expanding.
        eventsYCoords.insert(sensorInfo.posY + distToNearestBeacon + 1); // First y where this block of '#'s has disappeared.
    }

    for (const auto yCoord : eventsYCoords)
    {
        if (yCoord < 0 || yCoord > beaconMaxY)
            continue;
        // An event occurred! Check to see if this is where the gap happens.

        // Find the ranges projected onto this yCoord.
        vector<Range> expandingRanges;
        vector<Range> contractingRanges;
        for (const auto& sensorInfo : sensorInfos)
        {
            const int64_t distToNearestBeacon = abs(sensorInfo.posX - sensorInfo.nearestBeaconX) + abs(sensorInfo.posY - sensorInfo.nearestBeaconY);
            if (sensorInfo.posY + distToNearestBeacon < yCoord || sensorInfo.posY - distToNearestBeacon > yCoord)
                continue;

            assert(abs(yCoord - sensorInfo.posY) <= distToNearestBeacon);
            const int64_t projectedRangeWidthAtY = (2 * distToNearestBeacon + 1) - 2 * abs(yCoord - sensorInfo.posY);
            assert(projectedRangeWidthAtY >= 1);
            assert(projectedRangeWidthAtY % 2 == 1);
            Range projectedRange(sensorInfo.posX - (projectedRangeWidthAtY - 1) / 2, sensorInfo.posX + (projectedRangeWidthAtY - 1) / 2);

            if (projectedRange.startX <= projectedRange.endX)
            {
                if (sensorInfo.posY > yCoord)
                {
                    expandingRanges.push_back(projectedRange);
                }
                else
                {
                    contractingRanges.push_back(projectedRange);
                }
            }

        }

        vector<Range> allRanges = expandingRanges;
        allRanges.insert(allRanges.end(), contractingRanges.begin(), contractingRanges.end());
        for (const auto& sensorInfo : sensorInfos)
        {
            if (sensorInfo.nearestBeaconY == yCoord && (sensorInfo.nearestBeaconX >= 0 && sensorInfo.nearestBeaconX <= beaconMaxX) )
                allRanges.push_back({sensorInfo.nearestBeaconX, sensorInfo.nearestBeaconX});
        }

        sort(allRanges.begin(), allRanges.end(), [](const auto& lhs, const auto& rhs)
                {
                return lhs.startX < rhs.startX;
                });

        // We've projected all the #-diamonds onto this yCoord to get the range
        // they each cover: let's see if they leave precisely one cell uncovered!
        const int64_t notCoveredX = findSoleXNotCoveredByRanges(allRanges);
        if (notCoveredX != -1)
        {
            // Found the answer, but keep looking just to check that it is indeed
            // unique.
            assert((answer == Coord{-1, -1}));
            answer = {notCoveredX, yCoord};
        }

        // Generate further events to check: said events will be those y coords where two pairs of ranges - 
        // either both expanding, or both contracting - have a gap of precisely one square between them.
        const std::initializer_list<std::pair<vector<Range>, int>> expandingAndContractingRanges = {{expandingRanges, +1}, {contractingRanges, -1}};
        for (const auto& [ranges, dirOfGrowth] : expandingAndContractingRanges)
        {
            for (const auto& leftRangeOrig : ranges)
            {
                for (const auto& rightRangeOrig : ranges)
                {
                    Range leftRange = leftRangeOrig;
                    Range rightRange = rightRangeOrig;

                    if (leftRange.startX > rightRange.startX)
                        swap(leftRange, rightRange);
                    if ((abs(rightRange.startX - leftRange.endX) % 2) == 1)
                        continue;
                    const int64_t yOffsetUntil1Gap = dirOfGrowth * (rightRange.startX - leftRange.endX - 2) / 2;
                    if (yOffsetUntil1Gap < 0)
                    {
                        // This pair of ranges won't have a 1-gap in the future; ignore.
                        continue;
                    }

                    eventsYCoords.insert(yCoord + yOffsetUntil1Gap);
                }
            }
        }
    }

    return answer;

}

int main()
{
    vector<SensorInfo> sensorInfos;

    string sensorInfoLine;
    std::regex sensorInfoRegex("^Sensor at x=([-\\d]+),\\s*y=([-\\d]+): closest beacon is at x=([-\\d]+),\\s*y=([-\\d]+)\\s*$");
    while (std::getline(cin, sensorInfoLine))
    {
        std::smatch sensorInfoMatch;
        const bool matchSuccessful = std::regex_match(sensorInfoLine, sensorInfoMatch, sensorInfoRegex);
        assert(matchSuccessful);
        SensorInfo sensorInfo;
        sensorInfo.posX = std::stoll(sensorInfoMatch[1]);
        sensorInfo.posY = std::stoll(sensorInfoMatch[2]);
        sensorInfo.nearestBeaconX = std::stoll(sensorInfoMatch[3]);
        sensorInfo.nearestBeaconY = std::stoll(sensorInfoMatch[4]);
        sensorInfos.push_back(sensorInfo);
    }

    const auto answer = hiddenBeaconPosition(sensorInfos);
    std::cout << answer.x * beaconMaxX  + answer.y << std::endl;

}

