#include <iostream>
#include <regex>
#include <ranges>

#include <cassert>

using namespace std;

int main()
{
    auto valueOfSmushedDigits = [](const std::string& numberList)
    {
        auto smushedAsRange = numberList | std::ranges::views::filter([](const auto letter) { return letter != ' ';});
        return std::stoll(std::string(smushedAsRange.begin(), smushedAsRange.end()));
    };

    string timesLine;
    getline(cin, timesLine);
    std::smatch timesLineMatch;
    std::regex timesLineRegex(R"(^Time:(.*)$)");
    const bool timesMatchSuccessful = std::regex_match(timesLine, timesLineMatch, timesLineRegex);
    assert(timesMatchSuccessful);
    const int64_t time = valueOfSmushedDigits(timesLineMatch[1]);
    std::cout << time << std::endl;


    string distancesLine;
    getline(cin, distancesLine);
    std::smatch distancesLineMatch;
    std::regex distancesLineRegex(R"(^Distance:(.*)$)");
    const bool matchSuccessful = std::regex_match(distancesLine, distancesLineMatch, distancesLineRegex);
    assert(matchSuccessful);
    const int64_t bestDistance = valueOfSmushedDigits(distancesLineMatch[1]);
    std::cout << bestDistance << std::endl;

    assert(cin);

    int numWaysToWinRace = 0;
    for (int timeToHoldDown = 0; timeToHoldDown <= time; timeToHoldDown++)
    {
        const int speed = timeToHoldDown;
        const int64_t distanceTravelled = (time - timeToHoldDown) * speed;
        if (distanceTravelled > bestDistance)
            numWaysToWinRace++;
    }
    std::cout << "numWaysToWinRace: " << numWaysToWinRace << std::endl;
}
