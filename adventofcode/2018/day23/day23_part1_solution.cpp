#include <iostream>
#include <regex>
#include <optional>

#include <cassert>

using namespace std;

int main()
{
    struct Nanobot
    {
        int64_t x = -1;
        int64_t y = -1;
        int64_t z = -1;
        int64_t radius = -1;
        bool hasInRange(const Nanobot& other) const
        {
            return (abs(other.x - x) + abs(other.y - y) + abs(other.z - z)) <= radius;
        }
        auto operator<=>(const Nanobot& other) const = default;

    };
    vector<Nanobot> nanobots;
    string nanobotDescription;
    std::optional<Nanobot> strongestNanobot;
    while (getline(cin, nanobotDescription))
    {
        std::smatch nanobotMatch;
        const bool matchSuccessful = std::regex_match(nanobotDescription, nanobotMatch, std::regex(R"(^pos=<([-0-9]+),([-0-9]+),([-0-9]+)>,\s*r=(\d+)$)"));
        assert(matchSuccessful);
        Nanobot nanobot;
        nanobot.x = std::stoll(nanobotMatch[1]);
        nanobot.y = std::stoll(nanobotMatch[2]);
        nanobot.z = std::stoll(nanobotMatch[3]);
        nanobot.radius = std::stoll(nanobotMatch[4]);
        nanobots.push_back(nanobot);
        if (!strongestNanobot.has_value() || strongestNanobot.value().radius < nanobot.radius)
            strongestNanobot = nanobot;
    }

    assert(strongestNanobot.has_value());
    int numInRangeOfStrongest = 0;
    // We apparently don't preclude strongestNanobot from the count.
    for (const auto& nanobot : nanobots)
    {
        if (strongestNanobot.value().hasInRange(nanobot))
            numInRangeOfStrongest++;
    }
    std::cout << "numInRangeOfStrongest: " << numInRangeOfStrongest << std::endl;

    return 0;
}
