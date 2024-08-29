#include <iostream>
#include <regex>

#include <cassert>

using namespace std;

int main()
{
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

    string timesLine;
    getline(cin, timesLine);
    std::smatch timesLineMatch;
    std::regex timesLineRegex(R"(^Time:(.*)$)");
    const bool timesMatchSuccessful = std::regex_match(timesLine, timesLineMatch, timesLineRegex);
    assert(timesMatchSuccessful);
    const vector<int64_t> times = parseNumbers(timesLineMatch[1]);

    string distancesLine;
    getline(cin, distancesLine);
    std::smatch distancesLineMatch;
    std::regex distancesLineRegex(R"(^Distance:(.*)$)");
    const bool matchSuccessful = std::regex_match(distancesLine, distancesLineMatch, distancesLineRegex);
    assert(matchSuccessful);
    const vector<int64_t> bestDistances = parseNumbers(distancesLineMatch[1]);

    assert(cin);

    assert(bestDistances.size() == times.size());
    const int numRaces = times.size();

    int64_t product = 1;
    for (int i = 0; i < numRaces; i++)
    {
        std::cout << "race: " << (i + 1) << " time: " << times[i] << " distance: " << bestDistances[i] << std::endl;
        int numWaysToWinRace = 0;
        for (int timeToHoldDown = 0; timeToHoldDown <= times[i]; timeToHoldDown++)
        {
            const int speed = timeToHoldDown;
            const int64_t distanceTravelled = (times[i] - timeToHoldDown) * speed;
            std::cout << " timeToHoldDown: " << timeToHoldDown << " distanceTravelled: " << distanceTravelled << std::endl;
            if (distanceTravelled > bestDistances[i])
                numWaysToWinRace++;
        }
        product *= numWaysToWinRace;
    }
    std::cout << product << std::endl;
}
