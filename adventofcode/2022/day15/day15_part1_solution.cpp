#include <iostream>
#include <vector>
#include <deque>
#include <queue>
#include <map>
#include <set>
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
};

struct Coord
{
    int64_t x = -1;
    int64_t y = -1;
    auto operator<=>(const Coord& other) const = default;
};

int64_t numCoveredByRanges(const deque<Range>& rangesOrig)
{
    deque<Range> ranges = rangesOrig;
    int64_t numCoveredByRanges = 0;

    sort(ranges.begin(), ranges.end(), [](const auto& lhs, const auto& rhs)
            {
                return lhs.startX < rhs.startX;
            });
    priority_queue<int64_t, std::vector<int64_t>, std::greater<>> yCoordsOfCurrentRangesEnds;
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

        // We might have started processing a range; update yCoordsOfCurrentRangesEnds, regardless.
        while (!ranges.empty() && ranges.front().startX == newCurrentX)
        {
            yCoordsOfCurrentRangesEnds.push(ranges.front().endX);
            ranges.pop_front();
        }

        if (currentX != std::numeric_limits<int64_t>::min())
        {
            assert(newCurrentX >= currentX);
            const int64_t amountToAdd = wasStillWithinRange ? (newCurrentX - currentX) + 1 /* Add the range from newCurrentX -> currentX, inclusive. */ 
                : 1 /* Only add the single cell at newCurrentX. */;
            assert(amountToAdd >= 0);
            numCoveredByRanges += amountToAdd;
        }
        else
        {
            // Only add the single cell at newCurrentX.
            numCoveredByRanges += 1;
        }

        currentX = newCurrentX + 1; // We've processed this currentX, and have added it to numCoveredByRanges; skip to (at least) next cell.
    }
    std::cout << "numCoveredByRanges: " << numCoveredByRanges << std::endl;

    return numCoveredByRanges;
}


int main()
{
    vector<SensorInfo> sensorInfos;

    string sensorInfoLine;
    std::regex sensorInfoRegex("^Sensor at x=([-\\d]+),\\s*y=([-\\d]+): closest beacon is at x=([-\\d]+),\\s*y=([-\\d]+)\\s*$");
    while (std::getline(cin, sensorInfoLine))
    {
        std::smatch sensorInfoMatch;
        std::cout << "line: " << sensorInfoLine << std::endl;
        const bool matchSuccessful = std::regex_match(sensorInfoLine, sensorInfoMatch, sensorInfoRegex);
        assert(matchSuccessful);
        SensorInfo sensorInfo;
        sensorInfo.posX = std::stoll(sensorInfoMatch[1]);
        sensorInfo.posY = std::stoll(sensorInfoMatch[2]);
        sensorInfo.nearestBeaconX = std::stoll(sensorInfoMatch[3]);
        sensorInfo.nearestBeaconY = std::stoll(sensorInfoMatch[4]);
        std::cout << "sensorInfo: " << sensorInfo.posX << ", " << sensorInfo.posY << " nb: " << sensorInfo.nearestBeaconX << ", " << sensorInfo.nearestBeaconY << std::endl;
        sensorInfos.push_back(sensorInfo);
    }

    const int64_t requiredRowNumber = 2000000;

    set<Coord> beaconsOnRow;
    for (const auto& sensorInfo : sensorInfos)
    {
        if (sensorInfo.nearestBeaconY == requiredRowNumber)
            beaconsOnRow.insert({sensorInfo.nearestBeaconX, sensorInfo.nearestBeaconY});
    }
    std::cout << "# beaconsOnRow: " << beaconsOnRow.size() << std::endl;

    std::deque<Range> rangesOnDesiredRow;
    for (const auto& sensorInfo : sensorInfos)
    {
        const int64_t distToNearestBeacon = abs(sensorInfo.posX - sensorInfo.nearestBeaconX) + abs(sensorInfo.posY - sensorInfo.nearestBeaconY);
        if (sensorInfo.posY + distToNearestBeacon < requiredRowNumber || sensorInfo.posY - distToNearestBeacon > requiredRowNumber)
            continue;
        assert(abs(requiredRowNumber - sensorInfo.posY) <= distToNearestBeacon);
        const int64_t projectedRangeWidthAtY = (2 * distToNearestBeacon + 1) - 2 * abs(requiredRowNumber - sensorInfo.posY);
        assert(projectedRangeWidthAtY >= 1);
        assert(projectedRangeWidthAtY % 2 == 1);
        rangesOnDesiredRow.push_back({sensorInfo.posX - (projectedRangeWidthAtY - 1) / 2, sensorInfo.posX + (projectedRangeWidthAtY - 1) / 2});
    }

    const auto numCellsBeaconCannotBeOn = numCoveredByRanges(rangesOnDesiredRow) - beaconsOnRow.size();
    std::cout << numCellsBeaconCannotBeOn << std::endl;

}

