#include <iostream>
#include <map>
#include <vector>
#include <regex>
#include <limits>
#include <optional>

#include <cassert>

using namespace std;

struct Range
{
    int64_t destRangeStart = -1;
    int64_t sourceRangeStart = -1;
    int64_t length = -1;
    std::optional<int64_t> destForSource(int64_t sourceValue) const
    {
        if (sourceValue < sourceRangeStart || sourceValue > sourceRangeStart + length - 1)
            return std::optional<int64_t>();
        return (sourceValue - sourceRangeStart + destRangeStart);
    }
};

ostream& operator<<(ostream& os, const Range& range)
{
    os << "sourceRangeStart: " << range.sourceRangeStart << " sourceRangeEnd:" << (range.sourceRangeStart + range.length - 1) << " destRangeStart: " << range.destRangeStart << " length: " << range.length << " destRangeEnd: " << (range.destRangeStart + range.length - 1);
    return os;
}

int main()
{
    std::regex seedRegex(R"(^seeds:(.*)$)");
    std::regex mapHeaderRegex(R"(^\s*([^ ]+)\s*map:\s*$)");

    string seedLine;
    getline(cin, seedLine);
    std::smatch seedLineMatch;
    const bool matchSuccessful = std::regex_match(seedLine, seedLineMatch, seedRegex);
    assert(matchSuccessful);

    auto parseNumbers = [](const std::string& numberListString)
    {
        vector<int64_t> numbers;
        istringstream numberListStream(numberListString);
        int64_t number;
        while (numberListStream >> number)
        {
            numbers.push_back(number);
        }
        return numbers;
    };
    vector<int64_t> seedRangePairs = parseNumbers(seedLineMatch[1]);
    vector<Range> seedRanges;
    while (!seedRangePairs.empty())
    {
        assert(seedRangePairs.size() >= 2);
        const int64_t rangeBegin = seedRangePairs[0];
        const int64_t rangeLength = seedRangePairs[1];
        seedRanges.push_back({rangeBegin, rangeBegin, rangeLength});
        seedRangePairs.erase(seedRangePairs.begin());
        seedRangePairs.erase(seedRangePairs.begin());
    }

    string currentMapBeingParsed;
    string mapLine;
    map<string, vector<Range>> rangesForMap;
    while (getline(cin, mapLine))
    {
        if (mapLine.empty())
        {
            currentMapBeingParsed.clear();
            continue;
        }
        if (currentMapBeingParsed.empty())
        {
            std::smatch mapHeaderMatch;
            const bool matchSuccessful = std::regex_match(mapLine, mapHeaderMatch, mapHeaderRegex);
            assert(matchSuccessful);
            currentMapBeingParsed = mapHeaderMatch[1];
        }
        else
        {
            const vector<int64_t> rangeNumbers = parseNumbers(mapLine);
            assert(rangeNumbers.size() == 3);
            std::cout << "Parsed rangeNumbers for " << currentMapBeingParsed << ": " << mapLine << std::endl;
            rangesForMap[currentMapBeingParsed].push_back({rangeNumbers[0], rangeNumbers[1], rangeNumbers[2]});
        }
    }

    auto destValueForSource = [&rangesForMap](int64_t sourceValue, const std::string& mapCategory)
    {
        assert(rangesForMap.contains(mapCategory));
        int64_t destValue = sourceValue;
        for (const auto& range : rangesForMap[mapCategory])
        {
            if (sourceValue >= range.sourceRangeStart && sourceValue < range.sourceRangeStart + range.length)
            {
                destValue = range.destRangeStart + sourceValue - range.sourceRangeStart;
            }
        }
        //std::cout << " mapped sourceValue: " << sourceValue << " to " << destValue << " using map: " << mapCategory << std::endl;
        return destValue;
    };

#if 0
    int64_t lowestLocation = std::numeric_limits<int64_t>::max();
    for (const auto seed : seedsToPlant)
    {
        std::cout << "seed: " << seed << std::endl;
        int64_t value = seed;
        for (const auto& mapCategory : { "seed-to-soil", "soil-to-fertilizer", "fertilizer-to-water", "water-to-light", "light-to-temperature", 
"temperature-to-humidity", "humidity-to-location" })
        {
            std::cout << " category: " << mapCategory << std::endl;
            value = destValueForSource(value, mapCategory);
        }
        const auto location = value;
        lowestLocation = std::min(lowestLocation, location);
        std::cout << "location: " << value << std::endl;
    }
    std::cout << "lowestLocation: " << lowestLocation << std::endl;
#endif

    auto locationForSeedValue = [&destValueForSource](int64_t seed)
    {
        //std::cout << "seed: " << seed << std::endl;
        int64_t value = seed;
        for (const auto& mapCategory : { "seed-to-soil", "soil-to-fertilizer", "fertilizer-to-water", "water-to-light", "light-to-temperature", 
"temperature-to-humidity", "humidity-to-location" })
        {
            //std::cout << " category: " << mapCategory << std::endl;
            value = destValueForSource(value, mapCategory);
        }
        const auto location = value;
        //std::cout << "location: " << value << std::endl;
        return location;
    };

    auto areRangesNormalised = [](const vector<Range>& ranges)
    {
        assert(!ranges.empty());
        if (ranges.front().sourceRangeStart != 0)
        {
            return false;
        }
        for (auto normalisedRangeIter = std::next(ranges.begin()); normalisedRangeIter != ranges.end(); normalisedRangeIter++)
        {
            const auto range = *normalisedRangeIter;
            const auto previousRange = *std::prev(normalisedRangeIter);
            if (range.sourceRangeStart != previousRange.sourceRangeStart + previousRange.length)
                return false;
        }
        if(ranges.back().sourceRangeStart + ranges.back().length != std::numeric_limits<int64_t>::max())
            return false;

        return true;
    };

    auto normaliseRanges = [&areRangesNormalised](vector<Range>& ranges)
    {
        std::cout << "normaliseRanges: " << std::endl;
        for (const auto& range : ranges)
        {
            std::cout << " " << range << std::endl;
        }
        vector<Range> normalised;
        assert(!ranges.empty());
        sort(ranges.begin(), ranges.end(), [](const auto& lhs, const auto& rhs) { return lhs.sourceRangeStart < rhs.sourceRangeStart;});
        if (ranges.front().sourceRangeStart != 0)
        {
            normalised.push_back({0, 0, ranges.front().sourceRangeStart});
        }
        else
        {
            normalised.push_back(ranges.front());
            ranges.erase(ranges.begin());
        }
        for (const auto& range : ranges)
        {
            const auto previousRange = normalised.back();
            assert(range.sourceRangeStart >= previousRange.sourceRangeStart + previousRange.length);
            if (range.sourceRangeStart >  previousRange.sourceRangeStart + previousRange.length)
            {
                normalised.push_back({previousRange.sourceRangeStart + previousRange.length, previousRange.sourceRangeStart + previousRange.length, range.sourceRangeStart -  (previousRange.sourceRangeStart + previousRange.length)});
            }
            normalised.push_back(range);
        }
        if (ranges.back().sourceRangeStart + ranges.back().length != std::numeric_limits<int64_t>::max())
        {
            normalised.push_back({ranges.back().sourceRangeStart + ranges.back().length, ranges.back().sourceRangeStart + ranges.back().length, std::numeric_limits<int64_t>::max() - (ranges.back().sourceRangeStart + ranges.back().length)});
        }
        assert(areRangesNormalised(normalised));
        ranges = normalised;
    };

    for (auto& [mapCategory_unused, ranges] : rangesForMap)
    {
        normaliseRanges(ranges);
        std::cout << "category: " << mapCategory_unused << " normalised ranges: " << std::endl;
        for (const auto& range : ranges)
        {
            std::cout << " " << range << std::endl;
        }
    }


#if 0
    vector<Range> combinedRanges = rangesForMap["seed-to-soil"];
    for (const auto& mapCategory : { "soil-to-fertilizer", "fertilizer-to-water", "water-to-light", "light-to-temperature", 
            "temperature-to-humidity", "humidity-to-location" })
    {
        const auto& rangesToCombine = rangesForMap[mapCategory];
        vector<Range> newCombinedRanges;
        for (const auto& range : combinedRanges)
        {
            const int64_t destRangeBegin = range.destRangeStart;
            const int64_t destRangeEnd = range.destRangeStart + range.length - 1;
            vector<Range> overlappingRangesToCombine;
            for (const auto& toCombine : rangesToCombine)
            {
                if (destRangeEnd < toCombine.sourceRangeStart || destRangeBegin > toCombine.sourceRangeStart + toCombine.length - 1)
                    continue;
                overlappingRangesToCombine.push_back(toCombine);
            }
            for (const auto& overlappingRange : overlappingRangesToCombine)
            {
                const int64_t overlapSourceBegin = std::max(overlappingRange.sourceRangeStart, destRangeBegin);
                const int64_t overlapSourceEnd = std::min(overlappingRange.sourceRangeStart + overlappingRange.length - 1, destRangeEnd);
                assert(overlapSourceEnd >= overlapSourceBegin);

                Range splitRange;
                splitRange.sourceRangeStart = range.sourceRangeStart + (overlapSourceBegin - destRangeBegin);
                splitRange.length = overlapSourceEnd - overlapSourceBegin + 1;
                splitRange.destRangeBegin = overlapSourceBegin - overlappingRange.sourceRangeStart + overlappingRange.destRangeStart;

                assert (newCombinedRanges.empty() || newCombinedRanges.sourceRangeStart + newCombinedRanges.length <= splitRange.sourceRangeStart);
                if (newCombinedRanges.empty() || newCombinedRanges.back().sourceRangeStart + newCombinedRanges.back().length < splitRange.sourceRangeStart)
                {
                    const int64_t fillerRangeSourceStart = newCombinedRanges.back().sourceRangeStart + newCombinedRanges.back().length;
                    const int64_t fillerRangeLength = splitRange.sourceRangeStart - fillerRangeSourceStart;
                    const int64_t fillerRangeDestStart = fillerRangeSourceStart + (range.destRangeStart - range.sourceRangeStart);
                    newCombinedRanges.push_back({fillerRangeDestStart, fillerRangeSourceStart, fillerRangeLength});
                }
                newCombinedRanges.push_back(splitRange);
            }
            if (newCombinedRanges.empty() || )
            {
            }

        }


    }
#endif

    rangesForMap["initial-seeds"] = seedRanges;
    vector<Range> locationRangesNormalised;
    for (const auto& humidityToLocationRange : rangesForMap[ "humidity-to-location" ])
    {
        locationRangesNormalised.push_back({humidityToLocationRange.destRangeStart, humidityToLocationRange.destRangeStart, humidityToLocationRange.length});
    }
    normaliseRanges(locationRangesNormalised);

    int64_t lowestLocation = std::numeric_limits<int64_t>::max();
    for (const auto& finalRange : locationRangesNormalised)
    {
        std::cout << "finalRange: " << finalRange << std::endl;
        vector<Range> rangesToMapBackwards = { finalRange };

        for (const auto& mapCategory : { "humidity-to-location", "temperature-to-humidity",  "light-to-temperature",  "water-to-light",  "fertilizer-to-water", "soil-to-fertilizer", "seed-to-soil", "initial-seeds" })
        {
            std::cout << "mapCategory: " << mapCategory << " #rangesToMapBackwards: " << rangesToMapBackwards.size() << std::endl;
            vector<Range> nextRangesToMapBackwards;
            const auto& previousRanges = rangesForMap[mapCategory];
            for (const auto& rangeToMapBackwards : rangesToMapBackwards)
            {
                for (const auto& previousRange : previousRanges)
                {
                    const int64_t prevRangeDestStart = previousRange.destRangeStart;
                    const int64_t prevRangeDestEnd = previousRange.destRangeStart + previousRange.length - 1;
                    if (prevRangeDestEnd < rangeToMapBackwards.sourceRangeStart || prevRangeDestStart > rangeToMapBackwards.sourceRangeStart + rangeToMapBackwards.length)
                        continue;
                    // There are values for which this previousRange maps to rangeToMapBackwards.  Let's trim it to find the exact values.
                    const int64_t trimmedStart = std::max(previousRange.sourceRangeStart, previousRange.sourceRangeStart + (rangeToMapBackwards.sourceRangeStart - prevRangeDestStart));
                    const int64_t trimmedEnd = std::min(previousRange.sourceRangeStart + previousRange.length - 1, previousRange.sourceRangeStart + (rangeToMapBackwards.sourceRangeStart + rangeToMapBackwards.length - 1 - prevRangeDestStart));
                    if (trimmedEnd >= trimmedStart)
                    {
                        const int64_t trimmedDestStart = trimmedStart + (previousRange.destRangeStart - previousRange.sourceRangeStart);
                        const Range trimmedRange = {trimmedDestStart, trimmedStart, trimmedEnd - trimmedStart + 1};
                        std::cout << " verifying: trimmedEnd " << trimmedEnd << " trimmedStart: " << trimmedStart << std::endl;
                        for (int64_t i = trimmedStart; i < std::min<int64_t>(trimmedEnd, 100'000'000); i++)
                        {
                            const auto dest = previousRange.destForSource(i);
                            assert(dest.has_value());
                            const auto destValue = dest.value();
                            assert(rangeToMapBackwards.destForSource(destValue).has_value());
                            assert(previousRange.destForSource(i) == dest);
                        }
                        assert(!previousRange.destForSource(trimmedStart - 1).has_value() || !rangeToMapBackwards.destForSource(previousRange.destForSource(trimmedStart - 1).value()).has_value());
                        assert(!previousRange.destForSource(trimmedEnd + 1).has_value() || !rangeToMapBackwards.destForSource(previousRange.destForSource(trimmedEnd + 1).value()).has_value());

                        nextRangesToMapBackwards.push_back(trimmedRange);
                        assert(trimmedRange.length > 0);
                    }

                }
            }
            rangesToMapBackwards = nextRangesToMapBackwards;
        }
        std::cout << "For final range: " << finalRange << " the following initial ranges lead to values in it: " << std::endl;
        for (const auto& seedRange : rangesToMapBackwards)
        {
            int previousLocation = -1;
            for (int64_t seed = seedRange.sourceRangeStart; seed < std::min<int64_t>(seedRange.sourceRangeStart + seedRange.length - 1, 10'000'000); seed++)
            {
                const int64_t location = locationForSeedValue(seed);
                assert(finalRange.destForSource(location).has_value());
                assert(previousLocation == -1 || location == previousLocation + 1);
                previousLocation = location;
            }
            const int64_t lowestLocationForRange = locationForSeedValue(seedRange.sourceRangeStart);
            std::cout << " " << seedRange << " lowestLocationForRange: " << lowestLocationForRange << std::endl;
            lowestLocation = std::min(lowestLocation, lowestLocationForRange);
        }
    }
    std::cout << "lowestLocation: " << lowestLocation << std::endl;


}
