#pragma GCC optimize "trapv" // abort() on (signed) integer overflow.
#include <iostream>
#include <regex>
#include <limits>

#include <cassert>

using namespace std;

int main()
{
    struct Particle
    {
        int64_t x = -1;
        int64_t y = -1;
        int64_t z = -1;
        int64_t vx = -1;
        int64_t vy = -1;
        int64_t vz = -1;
        int64_t ax = -1;
        int64_t ay = -1;
        int64_t az = -1;
        int id = -1;
    };
    vector<Particle> particles;

    std::regex particleInfoRegex(R"(^p=<([-0-9]+),([-0-9]+),([-0-9]+)>, v=<([-0-9]+),([-0-9]+),([-0-9]+)>, a=<([-0-9]+),([-0-9]+),([-0-9]+)>$)");
    string particleInfoLine;
    while (getline(cin, particleInfoLine))
    {
        std::smatch particleInfoMatch;
        const bool matchSuccessful = std::regex_match(particleInfoLine, particleInfoMatch, particleInfoRegex);
        assert(matchSuccessful);

        Particle particle;
        particle.x = std::stoll(particleInfoMatch[1]);
        particle.y = std::stoll(particleInfoMatch[2]);
        particle.z = std::stoll(particleInfoMatch[3]);

        particle.vx = std::stoll(particleInfoMatch[4]);
        particle.vy = std::stoll(particleInfoMatch[5]);
        particle.vz = std::stoll(particleInfoMatch[6]);

        particle.ax = std::stoll(particleInfoMatch[7]);
        particle.ay = std::stoll(particleInfoMatch[8]);
        particle.az = std::stoll(particleInfoMatch[9]);

        particle.id = particles.size();

        particles.push_back(particle);
    }


    int64_t smallestAbsAccel = std::numeric_limits<int64_t>::max();
    int longTermNearestParticleId = -1;
    for (auto& particle : particles)
    {
        const auto absAccel = abs(particle.ax) + abs(particle.ay) + abs(particle.az);
        if (absAccel < smallestAbsAccel)
        {
            smallestAbsAccel = absAccel;
            longTermNearestParticleId = particle.id;
        }
    }
    std::cout << "longTermNearestParticleId: " << longTermNearestParticleId << std::endl;

    // Manual-ish check.
#if 0
    {
        int t = 0;
        while (true)
        {
            int64_t shortestDistance = std::numeric_limits<int64_t>::max();
            Particle nearestParticle;

            for (auto& particle : particles)
            {
                particle.vx += particle.ax;
                particle.vy += particle.ay;
                particle.vz += particle.az;

                particle.x += particle.vx;
                particle.y += particle.vy;
                particle.z += particle.vz;

                const auto distance = abs(particle.x) + abs(particle.y) + abs(particle.z);
                if (distance < shortestDistance)
                {
                    shortestDistance = distance;
                    nearestParticle = particle;
                }

            }

            std::cout << "at end of time: " << t << " shortest distance is " << shortestDistance << " held by particle " << nearestParticle.id << " with acceleration: " << nearestParticle.ax << ", " << nearestParticle.ay << ", " << nearestParticle.az << std::endl;

        }
    }
#endif
    return 0;
}
