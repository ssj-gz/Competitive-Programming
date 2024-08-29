#include <iostream>
#include <map>
#include <vector>
#include <regex>
#include <limits>

#include <cassert>

using namespace std;

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
    const vector<int64_t> seedsToPlant = parseNumbers(seedLineMatch[1]);

    string currentMapBeingParsed;
    string mapLine;
    struct Range
    {
        int64_t destRangeStart = -1;
        int64_t sourceRangeStart = -1;
        int64_t length = -1;
    };
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
        std::cout << " mapped sourceValue: " << sourceValue << " to " << destValue << " using map: " << mapCategory << std::endl;
        return destValue;
    };

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
}
