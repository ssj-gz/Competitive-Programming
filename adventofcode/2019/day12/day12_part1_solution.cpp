#include <iostream>
#include <regex>
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
};

struct Moon
{
    Triple position;
    Triple velocity{0, 0, 0};
    int64_t totalEnergy() const
    {
        return static_cast<int64_t>(position.absSum()) * velocity.absSum();
    }
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

    for (int stepNum = 0; stepNum <= 1000; stepNum++)
    {
        cout << "After " << stepNum << " steps:" << endl;
        for (const auto& moon : moons)
        {
            cout << moon << endl;
        }
        cout << "Energy after " << stepNum << " steps:" << endl;
        int64_t totalSystemEnergy = 0;
        for (const auto& moon : moons)
        {
            cout << moon.totalEnergy() << endl;
            totalSystemEnergy += moon.totalEnergy();
        }
        cout << "totalSystemEnergy: " << totalSystemEnergy << endl;

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
    }
}

