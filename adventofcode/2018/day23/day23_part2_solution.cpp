#include <iostream>
#include <regex>
#include <optional>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!


using namespace std;

int main()
{
    struct Vec3D
    {
        int64_t x = -1;
        int64_t y = -1;
        int64_t z = -1;
    };
    struct Nanobot
    {
        int64_t x = -1;
        int64_t y = -1;
        int64_t z = -1;
        int64_t radius = -1;
        std::pair<int, int> oppositePairs[2] = {};
        bool hasInRange(int x, int y) const
        {
            return (abs(this->x - x) + abs(this->y - y) <= radius);
        }
        auto operator<=>(const Nanobot& other) const = default;

    };
    vector<Nanobot> nanobots;
    string nanobotDescription;
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
    }

    const int width = 200;
    const int height = 100;
    const int minX = -width / 2;
    const int maxX = width / 2 - 1;
    const int minY = -height / 2;
    const int maxY = height / 2 - 1;
    std::cout << "minX: " << minX << " maxX: " << maxX << " minY: " << minY << " maxY: " << maxY << std::endl;

    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand(static_cast<unsigned int>((time.tv_sec * 1000) + (time.tv_usec / 1000)));

        vector<vector<char>> grid(width, vector<char>(height, ' '));
        const int numBots = 2;
        vector<Nanobot> nanobots;
        for (int i = 0; i < numBots; i++)
        {
            while (true)
            {
                const int x = minX + rand() % width;
                const int y = minY + rand() % height;
                const int radius = (rand() % std::min(width, height)) + 1;
                if (x - radius < minX || x + radius > maxX || y - radius < minY || y + radius > maxY)
                {
                    //std::cout << "Nope: " << x << ", " << y << " R: " << radius << std::endl;
                    continue;
                }
                nanobots.push_back({x, y, 0, radius});
                break;
            }
        }


        for (auto& bot : nanobots)
        {
            Vec3D xyDiag = {1, 1, 0};
            auto xIntersectForPoint = [&xyDiag](const int x, const int y)
            {
                return x - y / xyDiag.y;
            };
            for (int pairNum = 0; pairNum < 2; pairNum++)
            {
                {
                    const int botTopX = bot.x;
                    const int botTopY = bot.y + bot.radius;
                    bot.oppositePairs[pairNum].first = xIntersectForPoint(botTopX, botTopY);
                }
                {
                    const int botBottomX = bot.x;
                    const int botBottomY = bot.y - bot.radius;
                    bot.oppositePairs[pairNum].second = xIntersectForPoint(botBottomX, botBottomY);
                }
                if (pairNum % 2 == 0)
                    assert(bot.oppositePairs[pairNum].first <= bot.oppositePairs[pairNum].second);
                else
                {
                    assert(bot.oppositePairs[pairNum].first >= bot.oppositePairs[pairNum].second);
                    std::swap(bot.oppositePairs[pairNum].first, bot.oppositePairs[pairNum].second);
                }
                std::cout << "bot.oppositePairs[" << pairNum << "].first: " << bot.oppositePairs[pairNum].first << " bot.oppositePairs[" << pairNum << "].second: " << bot.oppositePairs[pairNum].second << std::endl;
                assert(bot.oppositePairs[pairNum].first <= bot.oppositePairs[pairNum].second);
                xyDiag = {xyDiag.y, -xyDiag.x, 0};
            }

        }

        const int intersectionPair0Min = std::max(nanobots[0].oppositePairs[0].first, nanobots[1].oppositePairs[0].first);
        const int intersectionPair0Max = std::min(nanobots[0].oppositePairs[0].second, nanobots[1].oppositePairs[0].second);
        const int intersectionPair1Min = std::max(nanobots[0].oppositePairs[1].first, nanobots[1].oppositePairs[1].first);
        const int intersectionPair1Max = std::min(nanobots[0].oppositePairs[1].second, nanobots[1].oppositePairs[1].second);
        bool intersection = !(intersectionPair0Min > intersectionPair0Max || intersectionPair1Min > intersectionPair1Max);
        std::cout << "intersectionPair0Min: " << intersectionPair0Min << " intersectionPair0Max: " << intersectionPair0Max << std::endl;
        std::cout << "intersectionPair1Min: " << intersectionPair1Min << " intersectionPair1Max: " << intersectionPair1Max << std::endl;
        if (!intersection)
        {
            std::cout << "No intersection" << std::endl;
        }
        else
        {
            std::cout << "Intersection!" << std::endl;
        }
        vector<std::pair<int, int>> intersectionPoints;
        for (int y = maxY; y >= minY; y--)
        {
            for (int x = minX; x <= maxX; x++)
            {
                int numPairsSatisfied = 0;
                for (int botIndex = 0; botIndex < numBots; botIndex++)
                {
                    const auto& bot = nanobots[botIndex];
                    Vec3D xyDiag = {1, 1, 0};
                    auto xIntersectForPoint = [&xyDiag](const int x, const int y)
                    {
                        return x - y / xyDiag.y;
                    };
                    for (int pairNum = 0; pairNum < 2; pairNum++)
                    {
                        const int xIntersect = xIntersectForPoint(x, y);
                        if (xIntersect>= bot.oppositePairs[pairNum].first && xIntersect<= bot.oppositePairs[pairNum].second)
                            numPairsSatisfied+=1;

                        xyDiag = {xyDiag.y, -xyDiag.x, 0};
                    }
                }
                assert(numPairsSatisfied <= 4);
                if (numPairsSatisfied == 4)
                {
                    intersectionPoints.push_back({x, y});
                }
            }
            std::cout << std::endl;
        }

        assert(nanobots.size() == 2);
        bool debugIntersection = false;
        vector<std::pair<int, int>> dbgIntersectionPoints;
        for (int y = maxY; y >= minY; y--)
        {
            for (int x = minX; x <= maxX; x++)
            {
                char letter = ' ';
                for (int botIndex = 0; botIndex < numBots; botIndex++)
                {
                    const auto& bot = nanobots[botIndex];
                    if (bot.hasInRange(x, y))
                    {
                        if (botIndex == 0)
                            letter = 'A';
                        else if (botIndex == 1)
                        {
                            if (letter == ' ')
                                letter = 'B';
                            else
                            {
                                letter = 'C';
                                debugIntersection = true;
                                dbgIntersectionPoints.push_back({x, y});
                            }
                        }
                    }
                }
                std::cout << letter;
            }
            std::cout << std::endl;
        }
        std::cout << "intersection: " << intersection << " debugIntersection: " << debugIntersection << " #intersectionPoints: " << intersectionPoints.size() << " #dbgIntersectionPoints: " << dbgIntersectionPoints.size() << std::endl;
        assert(intersection == debugIntersection);
        assert(intersectionPoints == dbgIntersectionPoints);
    }


    return 0;
}
