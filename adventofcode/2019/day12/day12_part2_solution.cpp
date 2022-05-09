#include <iostream>
#include <array>
#include <regex>
#include <cmath>
#include <cassert>

using namespace std;

struct Triple
{
    int x = -1;
    int y = -1;
    int z = -1;
    int absSum() const
    {
        return abs(x) + abs(y) + abs(z);
    }
    auto operator<=>(const Triple&) const = default;
};

struct Moon
{
    Triple position;
    Triple velocity{0, 0, 0};
    int64_t totalEnergy() const
    {
        return static_cast<int64_t>(position.absSum()) * velocity.absSum();
    }
    auto operator<=>(const Moon&) const = default;
};

struct State
{
    vector<Moon> moons;
    auto operator<=>(const State&) const = default;
};

ostream& operator<<(ostream& os, const Triple& triple)
{
    os << "<x=" << triple.x << ", y=" << triple.y << ", z=" << triple.z << ">";
    return os;
}

ostream& operator<<(ostream& os, const Moon& moon)
{
    os << "pos=" << moon.position << ", vel=" << moon.velocity;
    return os;
}

int64_t gcd(int64_t a, int64_t b)
{
    while (b != 0)
    {
        const int64_t t = b;
        b = (a % b);
        a = t;
    }
    return a;
}

int64_t lcm(int64_t a, int64_t b)
{
    return (a / gcd(a, b)) * b;
}


int main()
{
    string moonLine;
    vector<Moon> moons;
    while (getline(cin, moonLine))
    {
        static regex moonRegex(R"(^\s*<\s*x\s*=([\d-]+)\s*,\s*y\s*=\s*([\d-]+)\s*,\s*z\s*=([\d-]+)\s*>\s*$)");
        std::smatch moonMatch;
        const bool matched = std::regex_match(moonLine, moonMatch, moonRegex);
        assert(matched);
        assert(moonMatch.size() == 4);
        const auto x = stoi(moonMatch[1].str());
        const auto y = stoi(moonMatch[2].str());
        const auto z = stoi(moonMatch[3].str());

        moons.push_back({x, y, z});
    }
    const int numMoons = static_cast<int>(moons.size());

    State state;
    state.moons = moons;

    const State initialState = state;
    int64_t stepNum = 0;

    int64_t prevTotalSystemEnergy = 0;
    int64_t largestTotalSystemEnergy = 0;

    constexpr int numComponents = 3; // "x", "y", and "z".
    int64_t stepsForComponentRepetition[numComponents] = { -1, -1, -1 };
    int numComponentsWithUnknownRep = numComponents;

    while (true)
    {
        auto& moons = state.moons;

        // Velocity changes due to gravity.
        for (auto moon1Iter = moons.begin(); moon1Iter != moons.end(); moon1Iter++)
        {
            for (auto moon2Iter = std::next(moon1Iter); moon2Iter != moons.end(); moon2Iter++)
            {
                auto& moon1 = *moon1Iter;
                auto& moon2 = *moon2Iter;

                auto updateVelocityComponents = [](auto& vel1component, auto& vel2component, const auto& pos1component, const auto& pos2component) 
                {
                    if (pos1component < pos2component)
                    {
                        vel1component++;
                        vel2component--;
                    }
                    else if (pos1component > pos2component)
                    {
                        vel1component--;
                        vel2component++;
                    }
                };

                updateVelocityComponents(moon1.velocity.x, moon2.velocity.x, moon1.position.x, moon2.position.x);
                updateVelocityComponents(moon1.velocity.y, moon2.velocity.y, moon1.position.y, moon2.position.y);
                updateVelocityComponents(moon1.velocity.z, moon2.velocity.z, moon1.position.z, moon2.position.z);
            }
        }

        // Position changes.
        for (auto& moon : moons)
        {
            moon.position.x += moon.velocity.x;
            moon.position.y += moon.velocity.y;
            moon.position.z += moon.velocity.z;
        }
        stepNum++;

        // Has a component of the system (x, y or z) repeated for the first time?
        bool allXsRepeated = true;
        bool allYsRepeated = true;
        bool allZsRepeated = true;
        for (int moonIndex = 0; moonIndex < numMoons; moonIndex++)
        {
            const auto& moon = moons[moonIndex];
            const auto& initialMoon = initialState.moons[moonIndex];
            if ((moon.position.x != initialMoon.position.x) || (moon.velocity.x != initialMoon.velocity.x))
            {
                allXsRepeated = false;
            }
            if ((moon.position.y != initialMoon.position.y) || (moon.velocity.y != initialMoon.velocity.y))
            {
                allYsRepeated = false;
            }
            if ((moon.position.z != initialMoon.position.z) || (moon.velocity.z != initialMoon.velocity.z))
            {
                allZsRepeated = false;
            }
        }
        if (allXsRepeated && stepsForComponentRepetition[0] == -1)
        {
            stepsForComponentRepetition[0] = stepNum;
            numComponentsWithUnknownRep--;
        }
        if (allYsRepeated && stepsForComponentRepetition[1] == -1)
        {
            stepsForComponentRepetition[1] = stepNum;
            numComponentsWithUnknownRep--;
        }
        if (allZsRepeated && stepsForComponentRepetition[2] == -1)
        {
            stepsForComponentRepetition[2] = stepNum;
            numComponentsWithUnknownRep--;
        }
        if (numComponentsWithUnknownRep == 0)
        {
            // We know the cycles of all three components; no need to simulate further.
            break;
        }
    }

    int64_t stepsUntilSystemRepeats = 1;
    for (const auto componentRep : stepsForComponentRepetition)
    {
        stepsUntilSystemRepeats = lcm(stepsUntilSystemRepeats, componentRep);
    }
    cout << "stepsUntilSystemRepeats: " << stepsUntilSystemRepeats << endl;
}

