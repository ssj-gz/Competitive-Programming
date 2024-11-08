#include <iostream>
#include <regex>

#include <cassert>

using namespace std;

int main()
{
    struct Reindeer
    {
        string name;
        int speed = -1;
        int flightDuration = -1;
        int restDuration = -1;

        int64_t distanceTravelled = 0;
        bool isFlying = true;
        int flightCountdown = -1;
        int restCountdown = -1;
    };
    std::regex reindeerRegex(R"(^(\w+) can fly (\d+) km/s for (\d+) seconds, but then must rest for (\d+) seconds.$)");
    string reindeerLine;
    vector<Reindeer> allReindeer;
    while(std::getline(cin, reindeerLine))
    {
        std::smatch reindeerMatch;
        const bool matchSuccessful = std::regex_match(reindeerLine, reindeerMatch, reindeerRegex);
        assert(matchSuccessful);

        Reindeer reindeer;
        reindeer.name = reindeerMatch.str(1);
        reindeer.speed = std::stoi(reindeerMatch.str(2));
        reindeer.flightDuration = std::stoi(reindeerMatch.str(3));
        reindeer.restDuration = std::stoi(reindeerMatch.str(4));
        reindeer.flightCountdown = reindeer.flightDuration;
        
        allReindeer.push_back(reindeer);
    }

    int64_t farthestDistanceTravelled = 0;
    for (int t = 1; t <= 2'503; t++)
    {
        for (auto& reindeer : allReindeer)
        {
            if (reindeer.isFlying)
            {
                reindeer.distanceTravelled += reindeer.speed;
                reindeer.flightCountdown--;
                if (reindeer.flightCountdown == 0)
                {
                    reindeer.isFlying = false;
                    reindeer.restCountdown = reindeer.restDuration;
                }
            }
            else
            {
                reindeer.restCountdown--;
                if (reindeer.restCountdown == 0)
                {
                    reindeer.isFlying = true;
                    reindeer.flightCountdown = reindeer.flightDuration;
                }
            }
            std::cout << "After " << t << " seconds, reindeer: " << reindeer.name << " has travelled: " << reindeer.distanceTravelled << "km" << std::endl;
            farthestDistanceTravelled = std::max(farthestDistanceTravelled, reindeer.distanceTravelled);
        }
    }
    std::cout << "farthestDistanceTravelled: " << farthestDistanceTravelled << std::endl;


    return 0;
}
