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
    auto operator<=>(const Range& other) const = default;
};

struct Coord
{
    int64_t x = -1;
    int64_t y = -1;
    auto operator<=>(const Coord& other) const = default;
};

#define BRUTE_FORCE

#if 0
const int64_t beaconMaxX = 20; // TODO - replace both of these with 4'000'000
const int64_t beaconMaxY = 20;
#else
const int64_t beaconMaxX = 4'000'000;
const int64_t beaconMaxY = 4'000'000;
#undef BRUTE_FORCE
#endif

#ifdef BRUTE_FORCE
int64_t numCoveredByRangesBruteForce(const vector<Range>& rangesOrig)
{
    int64_t minX = std::numeric_limits<int64_t>::max();
    int64_t maxX = std::numeric_limits<int64_t>::min();
    for (const auto& range : rangesOrig)
    {
        minX = min(minX, range.startX);
        maxX = max(maxX, range.endX);
        assert(range.startX <= range.endX);
    }
    assert(maxX >= minX);
    std::cout << "minX: " << minX << " maxX: " << maxX << std::endl;
    string cellContents(maxX - minX + 1, '.');
    assert(!cellContents.empty());
    for (const auto& range : rangesOrig)
    {
        for (int x = range.startX; x <= range.endX; x++)
        {

            assert(x - minX >= 0); 
            assert(x - minX < cellContents.size());
            cellContents[x - minX] = '#';
        }
    }

    std::cout << "numCoveredByRangesBruteForce cellContents: " << cellContents << " # ranges: " << rangesOrig.size() << " minX: " << minX << " maxX: " << maxX << " size: " << cellContents.size() << std::endl;
    return std::count(cellContents.begin(), cellContents.end(), '#');
}
#endif


int64_t numCoveredByRanges(const vector<Range>& rangesOrig)
{
    std::cout << "Beginning numCoveredByRanges" << std::endl;
    deque<Range> ranges(rangesOrig.begin(), rangesOrig.end());
    int64_t numCoveredByRanges = 1;
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
    std::cout << "Beginning hiddenBeaconPosition" << std::endl;
    //std::vector<SensorInfo> sensorInfos = { sensorInfosOrig.front() };
    Coord answer = {-1, -1};
    set<int64_t> eventsYCoords;
    for (const auto& sensorInfo : sensorInfos)
    {
        std::cout << "sensorInfo: " << sensorInfo.posX << ", " << sensorInfo.posY << " nb: " << sensorInfo.nearestBeaconX << ", " << sensorInfo.nearestBeaconY << std::endl;
        const int64_t distToNearestBeacon = abs(sensorInfo.posX - sensorInfo.nearestBeaconX) + abs(sensorInfo.posY - sensorInfo.nearestBeaconY);
        std::cout << "distToNearestBeacon: " << distToNearestBeacon << std::endl;
        eventsYCoords.insert(sensorInfo.posY - distToNearestBeacon); // Topmost y-coord of this block of '#'s.
        eventsYCoords.insert(sensorInfo.posY + 1); // One-past-middle i.e. where this block of '#'s starts to narrow as y increases, instead of expanding.
        eventsYCoords.insert(sensorInfo.posY + distToNearestBeacon + 1); // First y where this block of '#'s has disappeared.
    }

    {
        // TODO - remove this!
        //eventsYCoords.insert(2658763);
        //eventsYCoords.insert(2658764);
    }

    for (const auto yCoord : eventsYCoords)
    {
        if (yCoord < 0 || yCoord > beaconMaxY)
            continue;
        std::cout << "yCoord: " << yCoord << std::endl;

        // Find the ranges projected onto this yCoord.
        vector<Range> expandingRanges;
        vector<Range> contractingRanges;
        for (const auto& sensorInfo : sensorInfos)
        {
            std::cout << "sensorInfo: " << sensorInfo.posX << ", " << sensorInfo.posY << " nb: " << sensorInfo.nearestBeaconX << ", " << sensorInfo.nearestBeaconY << std::endl;
            const int64_t distToNearestBeacon = abs(sensorInfo.posX - sensorInfo.nearestBeaconX) + abs(sensorInfo.posY - sensorInfo.nearestBeaconY);
            if (sensorInfo.posY + distToNearestBeacon < yCoord || sensorInfo.posY - distToNearestBeacon > yCoord)
                continue;

            assert(abs(yCoord - sensorInfo.posY) <= distToNearestBeacon);
            const int64_t projectedRangeWidthAtY = (2 * distToNearestBeacon + 1) - 2 * abs(yCoord - sensorInfo.posY);
            assert(projectedRangeWidthAtY >= 1);
            assert(projectedRangeWidthAtY % 2 == 1);
            Range projectedRange(sensorInfo.posX - (projectedRangeWidthAtY - 1) / 2, sensorInfo.posX + (projectedRangeWidthAtY - 1) / 2);
            //projectedRange.startX = max<int64_t>(projectedRange.startX, 0);
            //projectedRange.endX = min(projectedRange.endX, beaconMaxX);
            std::cout << " blah: " << projectedRange.startX << ", " << projectedRange.endX << ")" << std::endl;
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

        std::cout << "Expanding ranges: " << std::endl;
        for (const auto range : expandingRanges)
        {
            std::cout << " (" << range.startX << ", " << range.endX << ")" << std::endl;
        }
        std::cout << "Contracting ranges: " << std::endl;
        for (const auto range : contractingRanges)
        {
            std::cout << " (" << range.startX << ", " << range.endX << ")" << std::endl;
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
        std::cout << "Ranges: " << std::endl;
        for (const auto range : allRanges)
        {
            std::cout << " (" << range.startX << ", " << range.endX << ")" << std::endl;
        }


#ifdef BRUTE_FORCE
        assert(numCoveredByRanges(allRanges) == numCoveredByRangesBruteForce(allRanges));
#endif
        const int64_t numPlacesCannotBe = numCoveredByRanges(allRanges);
        const int64_t numPlacesCouldBe = beaconMaxX + 1 - numPlacesCannotBe;
        //assert(numPlacesCouldBe == 0 || numPlacesCouldBe == 1);
        if (numPlacesCouldBe == 1)
        {
            std::cout << "woohoo: yCoord: " << yCoord << std::endl;
            assert((answer == Coord{-1, -1}));
            answer = {-1, /*TODO*/ yCoord};
        }

        for (const auto& leftExpandingRangeOrig : expandingRanges)
        {
            for (const auto& rightExpandingRangeOrig : expandingRanges)
            {
                Range leftExpandingRange = leftExpandingRangeOrig;
                Range rightExpandingRange = rightExpandingRangeOrig;

                std::cout << "leftExpandingRange: " << leftExpandingRange.startX << ", " << leftExpandingRange.endX << std::endl;
                std::cout << "rightExpandingRange: " << rightExpandingRange.startX << ", " << rightExpandingRange.endX << std::endl;
                if (leftExpandingRange.startX > rightExpandingRange.startX)
                    swap(leftExpandingRange, rightExpandingRange);
                if ((abs(rightExpandingRange.startX - leftExpandingRange.endX) % 2) == 1)
                    continue;
                int64_t yIncreaseUntil1Gap = (rightExpandingRange.startX - leftExpandingRange.endX) / 2;
                if (yIncreaseUntil1Gap < 0)
                    continue;
                yIncreaseUntil1Gap--;
                auto leftCopy = leftExpandingRange;
                auto rightCopy = rightExpandingRange;
                leftCopy.startX -= yIncreaseUntil1Gap;
                leftCopy.endX += yIncreaseUntil1Gap;
                rightCopy.startX -= yIncreaseUntil1Gap;
                rightCopy.endX += yIncreaseUntil1Gap;
                assert(rightCopy.startX == leftCopy.endX + 2);
                eventsYCoords.insert(yCoord + yIncreaseUntil1Gap);
            }
        }
        for (auto leftContractingRange : contractingRanges)
        {
            for (auto rightContractingRange : contractingRanges)
            {
                if (leftContractingRange == rightContractingRange)
                    continue;
                if (leftContractingRange.startX > rightContractingRange.startX)
                    swap(leftContractingRange, rightContractingRange);
                if ((abs(rightContractingRange.startX - leftContractingRange.endX) % 2) == 1)
                    continue;
                int64_t yIncreaseUntil1Gap = -(rightContractingRange.startX - leftContractingRange.endX) / 2;
                if (yIncreaseUntil1Gap < 0)
                    continue;
                yIncreaseUntil1Gap++;
                auto leftCopy = leftContractingRange;
                auto rightCopy = rightContractingRange;
                leftCopy.startX += yIncreaseUntil1Gap;
                leftCopy.endX -= yIncreaseUntil1Gap;
                rightCopy.startX += yIncreaseUntil1Gap;
                rightCopy.endX -= yIncreaseUntil1Gap;
                //std::cout << "yIncreaseUntil1Gap: " << yIncreaseUntil1Gap << std::endl;
                //std::cout << "leftContractingRange: " << leftContractingRange.startX << ", " << leftContractingRange.endX << std::endl;
                //std::cout << "rightContractingRange: " << rightContractingRange.startX << ", " << rightContractingRange.endX << std::endl;
                //std::cout << "rightCopy.startX: " << rightCopy.startX << " leftCopy.endX: " << leftCopy.endX << std::endl;
                assert(rightCopy.startX == leftCopy.endX + 2);


                eventsYCoords.insert(yCoord + yIncreaseUntil1Gap);
            }
        }
    }

    return answer;

}

#ifdef BRUTE_FORCE
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
#endif

int main()
{
    std::cout << "glomph: " << (3226902 - (2391592 - 2276711)) << std::endl;
    std::cout << "glamph: " << (3226902 + (2391592 - 2276711)) << std::endl;
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

#ifdef BRUTE_FORCE
    const auto bruteForce = hiddenBeaconPositionBruteForce(sensorInfos);
    std::cout << "hiddenBeaconPositionBruteForce: " << bruteForce.x << ", " << bruteForce.y << std::endl;
#endif
    const auto optimised = hiddenBeaconPosition(sensorInfos);
    std::cout << "hiddenBeaconPosition: " << optimised.x << ", " << optimised.y << std::endl;

}

