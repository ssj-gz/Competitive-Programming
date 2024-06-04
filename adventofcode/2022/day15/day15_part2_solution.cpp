#include <iostream>
#include <vector>
#include <deque>
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

const int64_t beaconMaxX = 20; // TODO - replace both of these with 4'000'000
const int64_t beaconMaxY = 20;

int64_t numCoveredByRanges(const deque<Range>& rangesOrig)
{
    std::cout << "Beginning numCoveredByRanges" << std::endl;
    deque<Range> ranges = rangesOrig;
    int64_t numCoveredByRanges = 1;

    sort(ranges.begin(), ranges.end(), [](const auto& lhs, const auto& rhs)
            {
                return lhs.startX < rhs.startX;
            });
    deque<int64_t> yCoordsOfCurrentRangesEnds;
    int64_t currentX = std::numeric_limits<int64_t>::min();
    //auto rangeIter = ranges.cbegin();
    //auto rangeEndsIter = yCoordsOfCurrentRangesEnds.cbegin();
    while (true)
    {
        // Remove ranges/ y-coords of ranges that are "behind" us.
        while (!ranges.empty() && ranges.front().endX < currentX)
        {
            ranges.pop_front();
        }
        while (!yCoordsOfCurrentRangesEnds.empty() && yCoordsOfCurrentRangesEnds.front() < currentX)
        {
            yCoordsOfCurrentRangesEnds.pop_front();
        }

        if (ranges.empty() && yCoordsOfCurrentRangesEnds.empty())
            break;
        const bool wasStillWithinRange = !yCoordsOfCurrentRangesEnds.empty();

        const int64_t bestRangeStartX = (!ranges.empty() ? ranges.front().startX : std::numeric_limits<int64_t>::max());
        const int64_t bestEndOfCurrentRangeX = (!yCoordsOfCurrentRangesEnds.empty() ? yCoordsOfCurrentRangesEnds.front() : std::numeric_limits<int64_t>::max());
        const int64_t newCurrentX = min(bestRangeStartX, bestEndOfCurrentRangeX);
        assert(newCurrentX != std::numeric_limits<int64_t>::max());

        // Update yCoordsOfCurrentRangesEnds.
        while (!ranges.empty() && ranges.front().startX == newCurrentX)
        {
            yCoordsOfCurrentRangesEnds.push_back(ranges.front().endX);
            ranges.pop_front();
        }
        sort(yCoordsOfCurrentRangesEnds.begin(), yCoordsOfCurrentRangesEnds.end());

        if (currentX != std::numeric_limits<int64_t>::min())
        {
            assert(newCurrentX >= currentX);
            const int64_t amountToAdd = wasStillWithinRange ? (newCurrentX - currentX) + 1 /* Add the range from newCurrentX -> currentX, inclusive. */ 
                          : 1 /* Only add the single cell at newCurrentX. */;
            assert(amountToAdd >= 0);
            //std::cout << " amountToAdd: " << amountToAdd << " currentX: " << currentX << " newCurrentX: " << newCurrentX << " wasStillWithinRange? " << wasStillWithinRange << std::endl;
            numCoveredByRanges += amountToAdd;
        }
        else
        {
            const int64_t amountToAdd = 1;
            //std::cout << " amountToAdd (first): " << amountToAdd << " currentX: " << currentX << " newCurrentX: " << newCurrentX << " wasStillWithinRange? " << wasStillWithinRange << std::endl;
        }

        currentX = newCurrentX + 1; // We've processed this currentX, and have added it to numCoveredByRanges.
    }
    std::cout << "numCoveredByRanges: " << numCoveredByRanges << std::endl;

    return numCoveredByRanges;
}

Coord hiddenBeaconPosition(const std::vector<SensorInfo>& sensorInfos)
{
    struct ProjectedRange
    {
        enum GrowthDirection { Expanding, Contracting } ;
        GrowthDirection growthDirection;
        Range range;
    };
    set<int64_t> eventsYCoords;
    for (const auto& sensorInfo : sensorInfos)
    {
        const int64_t distToNearestBeacon = abs(sensorInfo.posX - sensorInfo.nearestBeaconX) + abs(sensorInfo.posY - sensorInfo.nearestBeaconY);
        eventsYCoords.insert(sensorInfo.posY - distToNearestBeacon); // Topmost y-coord of this block of '#'s.
        eventsYCoords.insert(sensorInfo.posY + 1); // One-past-middle i.e. where this block of '#'s starts to narrow as y increases, instead of expanding.
        eventsYCoords.insert(sensorInfo.posY + distToNearestBeacon); // First y where this block of '#'s has disappeared.
    }
    for (const auto yCoord : eventsYCoords)
    {
        if (yCoord < 0 || yCoord > beaconMaxY)
            continue;

        // Find the ranges projected onto this yCoord.
        vector<ProjectedRange> projectedRanges;
        for (const auto& sensorInfo : sensorInfos)
        {
            const int64_t distToNearestBeacon = abs(sensorInfo.posX - sensorInfo.nearestBeaconX) + abs(sensorInfo.posY - sensorInfo.nearestBeaconY);
            if (sensorInfo.posY + distToNearestBeacon < yCoord || sensorInfo.posY - distToNearestBeacon > yCoord)
                continue;
            const auto growthDirection = (sensorInfo.posY > yCoord ? ProjectedRange::GrowthDirection::Contracting : ProjectedRange::GrowthDirection::Expanding);

            assert(abs(yCoord - sensorInfo.posY) <= distToNearestBeacon);
            const int64_t projectedRangeWidthAtY = (2 * distToNearestBeacon + 1) - 2 * abs(yCoord - sensorInfo.posY);
            assert(projectedRangeWidthAtY >= 1);
            assert(projectedRangeWidthAtY % 2 == 1);
            Range projectedRange(sensorInfo.posX - (projectedRangeWidthAtY - 1) / 2, sensorInfo.posX + (projectedRangeWidthAtY - 1) / 2);
            projectedRange.startX = max<int64_t>(projectedRange.startX, 0);
            projectedRange.endX = min(projectedRange.endX, beaconMaxX);
            if (projectedRange.startX <= projectedRange.endX)
                projectedRanges.push_back({growthDirection, projectedRange});

        }

        set<Coord> beaconsOnRow;
        for (const auto& sensorInfo : sensorInfos)
        {
            if (sensorInfo.nearestBeaconY == yCoord)
                beaconsOnRow.insert({sensorInfo.nearestBeaconX, sensorInfo.nearestBeaconY});
        }



    }

    return {-1, -1};

}
Coord hiddenBeaconPositionBruteForce(const std::vector<SensorInfo>& sensorInfos)
{
    static std::map<Coord, char> cellContents;
    static int64_t minX = std::numeric_limits<int64_t>::max();
    static int64_t minY = std::numeric_limits<int64_t>::max();
    static int64_t maxX = std::numeric_limits<int64_t>::min();
    static int64_t maxY = std::numeric_limits<int64_t>::min();
    if (cellContents.empty())
    {
        for (const auto& sensorInfo : sensorInfos)
        {
            const int64_t distToNearestBeacon = abs(sensorInfo.posX - sensorInfo.nearestBeaconX) + abs(sensorInfo.posY - sensorInfo.nearestBeaconY);
            for (int64_t x = sensorInfo.posX - distToNearestBeacon; x <= sensorInfo.posX + distToNearestBeacon; x++)
            {
                for (int64_t y = sensorInfo.posY - distToNearestBeacon; y <= sensorInfo.posY + distToNearestBeacon; y++)
                {
                    if (abs(sensorInfo.posX - x) + abs(sensorInfo.posY - y) <= distToNearestBeacon)
                    {
                        cellContents[{x, y}] = '#';
                    }
                }
            }
        }
        for (const auto& sensorInfo : sensorInfos)
        {
            cellContents[{sensorInfo.posX, sensorInfo.posY}] = 'S';
            cellContents[{sensorInfo.nearestBeaconX, sensorInfo.nearestBeaconY}] = 'B';

        }

        for (const auto& [cellCoord, cellContent] : cellContents)
        {
            minX = min(minX, cellCoord.x);
            minY = min(minY, cellCoord.y);
            maxX = max(maxX, cellCoord.x);
            maxY = max(maxY, cellCoord.y);
        }
        minX = max<int64_t>(minX, 0);
        minY = max<int64_t>(minY, 0);
        maxX = min(maxX, beaconMaxX);
        maxY = min(maxY, beaconMaxY);
        for (int y = minY; y <= maxY; y++)
        {
            string yLabel = std::to_string(y);
            while (yLabel.size() < 4)
            {
                yLabel = ' ' + yLabel;
            }
            std::cout << yLabel;
            for (int x = minX; x <= maxX; x++)
            {
                if (!cellContents.contains({x, y}))
                    cellContents[{x, y}] = '.';
                cout << cellContents[{x, y}];
            }
            std::cout << endl;
        }
    }

#if 0
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
#endif

    //return numPlacesCannotBePresent;
    Coord hiddenBeaconPosition = { -1, - 1};
    for (int64_t x = minX; x <= maxX; x++)
    {
        for (int64_t y = minY; y <= maxY; y++)
        {
            if (cellContents[{x, y}] == '.')
            {
                assert(hiddenBeaconPosition == Coord({-1, -1}));
                hiddenBeaconPosition = {x, y};
            }

        }
    }
    return hiddenBeaconPosition;
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

    const auto bruteForce = hiddenBeaconPositionBruteForce(sensorInfos);
    std::cout << "hiddenBeaconPositionBruteForce: " << bruteForce.x << ", " << bruteForce.y << std::endl;

#if 0
    //const int64_t requiredRowNumber = 2000000;
    const int maxX = 20; // TODO - replace both of these with 4'000'000
    const int maxY = 20;

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

    const auto optimised = numCoveredByRanges(rangesOnDesiredRow);
    const auto bruteForce = numCoveredByRangesBruteForce(rangesOnDesiredRow);
    std::cout << "optimised: " << optimised << " bruteForce: " << bruteForce << std::endl;
    std::cout << "Answer: " << (optimised - beaconsOnRow.size()) << std::endl;;
#endif

#if 0

    numNotPresentInRowBruteForce(sensorInfos, requiredRowNumber);

    std::cout << "Woo" << std::endl;
    assert(numCoveredByRanges({{1, 10}, {15, 20}}) == 10 + 6);
    assert(numCoveredByRanges({{1, 10}, {11, 20}}) == 20);
    assert(numCoveredByRanges({{1, 10}, {10, 20}}) == 20);
    assert(numCoveredByRanges({{1, 10}, {2, 20}}) == 20);
    assert(numCoveredByRanges({{1, 10}, {12, 20}}) == 19);
    assert(numCoveredByRanges({{1, 10}, {1, 20}}) == 20);

    while (true)
    {
        int minX = rand() % 100 - 50;
        int maxX = rand() % 100 - 50;
        if (minX > maxX)
            swap(minX, maxX);
        if (minX == maxX)
            continue;
        const int num = rand() % 100 + 1;
        deque<Range> ranges;
        for (int i = 0; i < num; i++)
        {
            int startX = (rand() % (maxX - minX)) + minX;
            int endX = (rand() % (maxX - minX)) + minX;
            assert(startX >= minX && startX <= maxX);
            assert(endX >= minX && endX <= maxX);
            if (startX > endX)
                swap(startX, endX);
            ranges.push_back({startX, endX});
        }

        const auto optimised = numCoveredByRanges(ranges);
        const auto bruteForce = numCoveredByRangesBruteForce(ranges);
        std::cout << "optimised: " << optimised << " bruteForce: " << bruteForce << std::endl;
        if(optimised != bruteForce)
        {
            std::cout << "Failure: " << std::endl;
            for (const auto range : ranges)
            {
                std::cout << " " << range.startX << ", " << range.endX << std::endl;
            }
            break;
        }
    }
#endif
}

