#include <iostream>
#include <regex>
#include <optional>
#include <set>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!


using namespace std;

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
    bool hasInRange(int x, int y, int z) const
    {
        return (abs(this->x - x) + abs(this->y - y) + abs(this->z - z) <= radius);
    }
    auto operator<=>(const Nanobot& other) const = default;

};

struct IntersectionRegion
{
    IntersectionRegion(const Nanobot& nanobot)
    {
        Vec3D planeBasisXY = {1, 1, 0};
        Vec3D planeBasisWithZ = {1, 0, 1};
        auto projectOntoXAxis = [&planeBasisXY, &planeBasisWithZ](const Vec3D& point)
        {
            const Vec3D XYProjection = { point.x - (point.z / planeBasisWithZ.z) * planeBasisWithZ.x, point.y - (point.z / planeBasisWithZ.z) * planeBasisWithZ.y, point.z - (point.z / planeBasisWithZ.z) * planeBasisWithZ.z};
            assert(XYProjection.z == 0);
            Vec3D XAxisProjection = { XYProjection.x - (XYProjection.y / planeBasisXY.y) * planeBasisXY.x, XYProjection.y - (XYProjection.y / planeBasisXY.y) * planeBasisXY.y,  XYProjection.z - (XYProjection.y / planeBasisXY.y) * planeBasisXY.z };
            assert(XAxisProjection.y == 0 && XAxisProjection.z == 0);
            return XAxisProjection.x;
        };
        for (int pairNum = 0; pairNum < numPairs; pairNum++)
        {
            const Vec3D top = {nanobot.x, nanobot.y, nanobot.z + nanobot.radius };
            minMaxPairs[pairNum].first = projectOntoXAxis(top);

            const Vec3D bottom = {nanobot.x, nanobot.y, nanobot.z - nanobot.radius };
            minMaxPairs[pairNum].second = projectOntoXAxis(bottom);

            //std::cout << " pairNum: " << pairNum << " minMaxPairs[pairNum].first: " << minMaxPairs[pairNum].first << " minMaxPairs[pairNum].second: " << minMaxPairs[pairNum].second << std::endl;

            if (pairNum % 4 == 0 || pairNum % 4 == 3)
                assert(minMaxPairs[pairNum].first <= minMaxPairs[pairNum].second);
            else
            {
                assert(minMaxPairs[pairNum].first >= minMaxPairs[pairNum].second);
                swap(minMaxPairs[pairNum].first, minMaxPairs[pairNum].second);
            }
           
            // Rotate bases 90 degrees.
            planeBasisXY = {planeBasisXY.y, -planeBasisXY.x, planeBasisXY.z};
            planeBasisWithZ = {planeBasisWithZ.y, -planeBasisWithZ.x, planeBasisWithZ.z};
        }

    }
    IntersectionRegion() = default;
    bool containsPoint(int x, int y, int z) const
    {
        Vec3D planeBasisXY = {1, 1, 0};
        Vec3D planeBasisWithZ = {1, 0, 1};
        auto projectOntoXAxis = [&planeBasisXY, &planeBasisWithZ](const Vec3D& point)
        {
            const Vec3D XYProjection = { point.x - (point.z / planeBasisWithZ.z) * planeBasisWithZ.x, point.y - (point.z / planeBasisWithZ.z) * planeBasisWithZ.y, point.z - (point.z / planeBasisWithZ.z) * planeBasisWithZ.z};
            assert(XYProjection.z == 0);
            Vec3D XAxisProjection = { XYProjection.x - (XYProjection.y / planeBasisXY.y) * planeBasisXY.x, XYProjection.y - (XYProjection.y / planeBasisXY.y) * planeBasisXY.y,  XYProjection.z - (XYProjection.y / planeBasisXY.y) * planeBasisXY.z };
            assert(XAxisProjection.y == 0 && XAxisProjection.z == 0);
            return XAxisProjection.x;
        };
        for (int pairNum = 0; pairNum < numPairs; pairNum++)
        {
            const int projectedX = projectOntoXAxis({x, y, z});
            if (projectedX < minMaxPairs[pairNum].first || projectedX > minMaxPairs[pairNum].second)
                return false;

            // Rotate bases 90 degrees.
            planeBasisXY = {planeBasisXY.y, -planeBasisXY.x, planeBasisXY.z};
            planeBasisWithZ = {planeBasisWithZ.y, -planeBasisWithZ.x, planeBasisWithZ.z};
        }
        return true;
    }
    IntersectionRegion intersectedWith(const IntersectionRegion& otherRegion) const
    {
        IntersectionRegion result;
        for (int pairNum = 0; pairNum < numPairs; pairNum++)
        {
            result.minMaxPairs[pairNum].first = std::max(minMaxPairs[pairNum].first, otherRegion.minMaxPairs[pairNum].first);
            result.minMaxPairs[pairNum].second = std::min(minMaxPairs[pairNum].second, otherRegion.minMaxPairs[pairNum].second);
        }

        return result;
    }
    bool isEmpty() const
    {
        for (int pairNum = 0; pairNum < numPairs; pairNum++)
        {
            if (minMaxPairs[pairNum].first > minMaxPairs[pairNum].second)
                return true;
        }
        return false;
    }
    void printTo(ostream& os) const
    {
        for (int pairNum = 0; pairNum < numPairs; pairNum++)
        {
            os << "pairNum: " << pairNum << ": " << minMaxPairs[pairNum].first << " - " << minMaxPairs[pairNum].second << std::endl;
        }
    }
    static constexpr int numPairs = 4;
    std::pair<int, int> minMaxPairs[numPairs] = {};
};

void blah(const IntersectionRegion& currentIntersection, std::vector<Nanobot>::iterator nextBot, const std::vector<Nanobot>& nanobots, int numBotsInIntersection, int& maxNumBotsInIntersection)
{
    if (numBotsInIntersection > maxNumBotsInIntersection)
    {
        maxNumBotsInIntersection = numBotsInIntersection;
        std::cout << "new maxNumBotsInIntersection: " << maxNumBotsInIntersection << std::endl;
        currentIntersection.printTo(cout);
    }
    const auto numBotsRemaining = nanobots.end() - nextBot;
    if (numBotsInIntersection + numBotsRemaining < maxNumBotsInIntersection)
    {
        //std::cout << "skipping: numBotsRemaining: " << numBotsRemaining << " numBotsInIntersection: " << numBotsInIntersection << std::endl;
        return;
    }
    while (nextBot != nanobots.end())
    {
        IntersectionRegion newIntersection = currentIntersection.intersectedWith(*nextBot);
        if (!newIntersection.isEmpty())
        {
            // Recurse, using this bot.
            blah(newIntersection, std::next(nextBot), nanobots, numBotsInIntersection + 1, maxNumBotsInIntersection);
        }
        nextBot++;
    }

}

void blah2(const vector<const Nanobot*>& bots, int pairNum, int& highest, IntersectionRegion& highestIntersectionRegion)
{
    if (pairNum == IntersectionRegion::numPairs)
    {
        if (bots.size() > highest)
        {
            highest = bots.size();
            std::cout << "New highest: " << highest << std::endl;
            highestIntersectionRegion = IntersectionRegion(*bots.front());
            for (const auto* bot : bots)
            {
                highestIntersectionRegion = highestIntersectionRegion.intersectedWith(IntersectionRegion(*bot));
            }
            highestIntersectionRegion.printTo(cout); 
        }
        return;
    }
    set<int> events;
    for (const auto* bot : bots)
    {
        const auto region = IntersectionRegion(*bot);
        events.insert(region.minMaxPairs[pairNum].first);
        events.insert(region.minMaxPairs[pairNum].second + 1);
    }
    vector<vector<const Nanobot*>> botsInInterval;
    for (const int event : events)
    {
        botsInInterval.push_back({});
        for (const auto* bot : bots)
        {
            const auto region = IntersectionRegion(*bot);
            if (region.minMaxPairs[pairNum].first <= event && region.minMaxPairs[pairNum].second >= event)
            {
                botsInInterval.back().push_back(bot);
                
            }
        }
    }
    std::sort(botsInInterval.begin(), botsInInterval.end(), [](const auto& lhsBots, const auto& rhsBots)
            { return rhsBots.size() < lhsBots.size();});
    for (const auto& botList : botsInInterval)
    {
        if (botList.size() > highest)
            blah2(botList, pairNum + 1, highest, highestIntersectionRegion);
        else
        {
            std::cout << "Pruning botList of size: " << botList.size() << " @pairNum: " << pairNum << std::endl;
            return;
        }
    }
}

int main()
{
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

    for (int bot1Index = 0; bot1Index < static_cast<int>(nanobots.size()); bot1Index++)
    {
        const auto& bot1 = nanobots[bot1Index];
        for (int bot2Index = bot1Index + 1; bot2Index < static_cast<int>(nanobots.size()); bot2Index++)
        {
            const auto& bot2 = nanobots[bot2Index];
            if (!IntersectionRegion(bot1).intersectedWith(bot2).isEmpty())
            {
                //std::cout << "Bots " << bot1Index << " and " << bot2Index << " intersect glarp" << std::endl;
            }

        }
    }

#if 0
    const int width = 200;
    const int height = 100;
    const int depth = 100;
#else
    const int width = 50;
    const int height = 50;
    const int depth = 50;
#endif
    const int minX = -width / 2;
    const int maxX = width / 2 - 1;
    const int minY = -height / 2;
    const int maxY = height / 2 - 1;
    const int minZ = -depth / 2;
    const int maxZ = depth / 2 - 1;
    std::cout << "minX: " << minX << " maxX: " << maxX << " minY: " << minY << " maxY: " << maxY << std::endl;

    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand(static_cast<unsigned int>((time.tv_sec * 1000) + (time.tv_usec / 1000)));

        //vector<vector<vector<char>>> grid(width, vectorvector<char>(height, vector<char>(depth, ' '));
        const int numBots = 3;
        vector<Nanobot> nanobots;
        for (int i = 0; i < numBots; i++)
        {
            while (true)
            {
                const int x = minX + rand() % width;
                const int y = minY + rand() % height;
                const int z = minZ + rand() % depth;
                const int radius = (rand() % std::min(depth, std::min(width, height))) + 1;
                if (x - radius < minX || x + radius > maxX || y - radius < minY || y + radius > maxY || z - radius < minZ || z + radius > maxZ)
                {
                    //std::cout << "Nope: " << x << ", " << y << " R: " << radius << std::endl;
                    continue;
                }
                nanobots.push_back({x, y, z, radius});
                IntersectionRegion blah(nanobots.back());
                break;
            }
        }



        for (auto& bot : nanobots)
        {
#if 0
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
#endif
            for (int z = maxZ; z >= minZ; z--)
            {
                for (int y = maxY; y >= minY; y--)
                {
                    for (int x = minX; x <= maxX; x++)
                    {
                        assert(bot.hasInRange(x, y, z) == IntersectionRegion(bot).containsPoint(x, y, z));
                        //if (bot.hasInRange(x, y, z))
                        //std::cout << "Ooh - intersection!" << "x: " << x << " y: " << y << " z: " << z << std::endl;
                    }
                }
            }

        }

#if 0
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
#endif
        int maxNumBotsContaining = 0;
        const auto threeBotsIntersection = IntersectionRegion(nanobots[0]).intersectedWith(nanobots[1]).intersectedWith(nanobots[2]);
        for (int z = maxZ; z >= minZ; z--)
        {
            for (int y = maxY; y >= minY; y--)
            {
                for (int x = minX; x <= maxX; x++)
                {
                    int numBotsContaining = 0;
                    int dbgNumBotsContaining = 0;
                    for (const auto& bot : nanobots)
                    {
                        if (bot.hasInRange(x, y, z))
                            dbgNumBotsContaining++;
                        if (IntersectionRegion(bot).containsPoint(x, y, z))
                            numBotsContaining++;
                    }
                    assert(numBotsContaining == dbgNumBotsContaining);
                    maxNumBotsContaining = std::max(maxNumBotsContaining, numBotsContaining);
                    if (numBotsContaining == 3)
                    {
                        assert(threeBotsIntersection.containsPoint(x, y, z));
                    }
                    else
                        assert(!threeBotsIntersection.containsPoint(x, y, z));

                }
            }
        }
        std::cout << "maxNumBotsContaining: " << maxNumBotsContaining << std::endl;

#if 0
        assert(nanobots.size() == 2);
        bool debugIntersection = false;
        vector<std::pair<int, int>> dbgIntersectionPoints;
        for (int y = maxY; y >= minY; y--)
        {
            for (int x = minX; x <= maxX; x++)
            {
                std::cout << letter;
            }
            std::cout << std::endl;
        }
        std::cout << "intersection: " << intersection << " debugIntersection: " << debugIntersection << " #intersectionPoints: " << intersectionPoints.size() << " #dbgIntersectionPoints: " << dbgIntersectionPoints.size() << std::endl;
        assert(intersection == debugIntersection);
        assert(intersectionPoints == dbgIntersectionPoints);
#endif
    }
#if 0
    set<int> events;
    for (const auto& nanobot : nanobots)
    {
        const auto region = IntersectionRegion(nanobot);
        events.insert(region.minMaxPairs[0].first);
        events.insert(region.minMaxPairs[0].second + 1);
    }
    int maxBlah = 0;
    for (const int event : events)
    {
        int blah = 0;
        for (const auto& nanobot : nanobots)
        {
            const auto region = IntersectionRegion(nanobot);
            if (region.minMaxPairs[0].first <= event && region.minMaxPairs[0].second >= event)
            {
                blah++;
            }
        }
        //if (blah >= 980)
            std::cout << "event: " << event << " blah: " << blah << std::endl;
        maxBlah = std::max(maxBlah, blah);
    }
    std::cout << "maxBlah: " << maxBlah << std::endl;
#endif


#if 0
    int maxNumBotsInIntersection = 0;
    for (auto botIter = nanobots.begin(); botIter != nanobots.end(); botIter++)
    {
        std::cout << "Another bot" << std::endl;
        IntersectionRegion botRegion = IntersectionRegion(*botIter);
        blah(botRegion, std::next(botIter), nanobots, 1, maxNumBotsInIntersection);
    }
#endif

    vector<const Nanobot*> bots;
    for (const auto& bot : nanobots)
        bots.push_back(&bot);
    int highest = 0;
    IntersectionRegion highestIntersectionRegion;
    blah2(bots, 0, highest, highestIntersectionRegion);

    int radius = 1;
    while (IntersectionRegion({0, 0, 0, radius}).intersectedWith(highestIntersectionRegion).isEmpty())
    {
        radius *= 2;
    }
    std::cout << "Initial radius: " << radius << std::endl;
    int highRadius = radius;
    int lowRadius = 0;
    while (highRadius != lowRadius)
    {
        const int medRadius = lowRadius + (highRadius - lowRadius) / 2;
        if (IntersectionRegion({0, 0, 0, medRadius}).intersectedWith(highestIntersectionRegion).isEmpty())
        {
            lowRadius = medRadius + 1;
        }
        else
        {
            highRadius = medRadius;
        }
        std::cout << "medRadius: " << medRadius << " lowRadius: " << lowRadius << " highRadius: " << highRadius << std::endl;
    }
    const int result = highRadius;
    assert(IntersectionRegion({0, 0, 0, result - 1}).intersectedWith(highestIntersectionRegion).isEmpty());
    assert(!IntersectionRegion({0, 0, 0, result}).intersectedWith(highestIntersectionRegion).isEmpty());

    std::cout << "result: " << result << std::endl;


    return 0;
}
