#pragma GCC optimize "trapv" // abort() on (signed) integer overflow.
#include <iostream>
#include <regex>
#include <limits>
#include <boost/rational.hpp>

#include <cassert>

using namespace std;

struct Vec3D
{
    int64_t x = -1;
    int64_t y = -1;
    int64_t z = -1;
    auto operator<=>(const Vec3D& other) const = default;
};

struct Particle
{
    Vec3D pos;
    Vec3D velocity;
    Vec3D acceleration;
    bool isAlive = true;

    Vec3D posAtTime(int64_t t) const
    {
        return { posAtTime(pos.x, velocity.x, acceleration.x, t),
                 posAtTime(pos.y, velocity.y, acceleration.y, t),
                 posAtTime(pos.z, velocity.z, acceleration.z, t)};
                 
    }

    static int64_t posAtTime(int64_t p, int64_t v, int64_t a, int64_t t)
    {
        // See below for formula.
        return p + t * v + a * (t * (t + 1)) / 2;
    }

    int id = -1;
};

int64_t intSquareRoot(int64_t x)
{
    int64_t squareRoot = 1;
    while (squareRoot * squareRoot < x)
    {
        squareRoot++;
    }
    if (squareRoot * squareRoot == x)
        return squareRoot;
    else 
        return -1;
}

using Rational = boost::rational<int64_t>;
std::pair<int, vector<int64_t>> getCollisionTime(int64_t p1, int64_t v1, int64_t a1,
                         int64_t p2, int64_t v2, int64_t a2)
{
    // Equation for particle with start point p, velocity v, and acceleration at at time t:
    //
    //    p + t * v + a * (t * (t + 1)) / 2
    //
    // (derived using the formula for the sum of Arithmetic Progression: 
    //  https://en.wikipedia.org/wiki/Arithmetic_progression#Sum)
    // where x0 is initial x pos, v0 is initial x velocity.
    //  
    //  = a * t * t / 2 + (a / 2 + v) * t + p
    //
    // The first form will always give a correct result even if we use integer arithmetic;
    // the other will not (need to use rationals).
    //
    // We can use this to derive a quadratic (in t) equation to find when two particles 
    // collide.
    const auto a1x = Rational(a1) / 2;
    const auto b1x = (Rational(a1) / 2 + v1);
    const auto c1x = p1;

    const auto a2x = Rational(a2) / 2;
    const auto b2x = (Rational(a2) / 2 + v2);
    const auto c2x = p2;

    const auto A = a1x - a2x;
    const auto B = b1x - b2x;
    const auto C = c1x - c2x;

    if (A != 0)
    {
        // Quadratic equation.  For integer roots,
        // need numerator/ denominator of determinant
        // to be perfect squares, and determinant to
        // be non-negative.
        const auto determinant = (B * B) - (4 * A * C);
        if (determinant < 0)
            return {-1, {}};
        const Rational numeratorSqRoot = intSquareRoot(abs(determinant.numerator()));
        const Rational denominatorSqRoot = intSquareRoot(abs(determinant.denominator()));
        if (numeratorSqRoot == -1 || denominatorSqRoot == -1)
            return {-1, {}};

        const auto t1 = (-B + (numeratorSqRoot / denominatorSqRoot)) / (2 * A);
        const auto t2 = (-B - (numeratorSqRoot / denominatorSqRoot)) / (2 * A);
        assert(A * t1 * t1 + B * t1 + C == 0);
        assert(A * t2 * t2 + B * t2 + C == 0);
        const bool t1Valid = (t1 >= 0) && (t1.denominator() == 1);
        const bool t2Valid = (t2 >= 0) && (t2.denominator() == 1);
        if (!t1Valid && !t2Valid)
            return {-1, {}};
        vector<int64_t> solutions;
        if (t1Valid)
            solutions.push_back(boost::rational_cast<int64_t>(t1));
        if (t2Valid)
            solutions.push_back(boost::rational_cast<int64_t>(t2));
        return {solutions.size(), solutions};

    }
    else if (B != 0)
    {
        // Linear equation.
        const auto t = -C / B;
        if (t < 0 || t.denominator() != 1)
            return {-1, {}};
        assert(A * t * t + B * t + C == 0);
        return {1, {boost::rational_cast<int64_t>(t)}};
    }
    else if (C != 0)
    {
        return {-1, {}};
    }
    else
    {
        // Special code - -2 means "the two sets of parameters
        // are identical, so always collide".
        return {-2, {}};
    }
}

int main()
{
    vector<Particle> particles;

    std::regex particleInfoRegex(R"(^p=<([-0-9]+),([-0-9]+),([-0-9]+)>, v=<([-0-9]+),([-0-9]+),([-0-9]+)>, a=<([-0-9]+),([-0-9]+),([-0-9]+)>$)");
    string particleInfoLine;
    while (getline(cin, particleInfoLine))
    {
        std::smatch particleInfoMatch;
        const bool matchSuccessful = std::regex_match(particleInfoLine, particleInfoMatch, particleInfoRegex);
        assert(matchSuccessful);

        Particle particle;
        particle.pos.x = std::stoll(particleInfoMatch[1]);
        particle.pos.y = std::stoll(particleInfoMatch[2]);
        particle.pos.z = std::stoll(particleInfoMatch[3]);

        particle.velocity.x = std::stoll(particleInfoMatch[4]);
        particle.velocity.y = std::stoll(particleInfoMatch[5]);
        particle.velocity.z = std::stoll(particleInfoMatch[6]);

        particle.acceleration.x = std::stoll(particleInfoMatch[7]);
        particle.acceleration.y = std::stoll(particleInfoMatch[8]);
        particle.acceleration.z = std::stoll(particleInfoMatch[9]);

        particle.id = particles.size();
        //particle.v0 = particle.vx;
        //particle.x0 = particle.x;

        particles.push_back(particle);
    }

    std::map<int64_t, vector<std::pair<Particle*, Particle*>>> collisionsAtTime;

    for (auto particle1Iter = particles.begin(); particle1Iter != particles.end(); particle1Iter++)
    {
        auto& particle1 = *particle1Iter;
        for (auto particle2Iter = std::next(particle1Iter); particle2Iter != particles.end(); particle2Iter++)
        {
            auto& particle2 = *particle2Iter;
            vector<int64_t> collisionTimes;
            if (const auto [numCols, xColTimes] = getCollisionTime(particle1.pos.x, particle1.velocity.x, particle1.acceleration.x,
                                                          particle2.pos.x, particle2.velocity.x, particle2.acceleration.x); numCols != -2)
            {
                collisionTimes.insert(collisionTimes.begin(),  xColTimes.begin(), xColTimes.end());
            }
            else if (const auto [numCols, yColTimes] = getCollisionTime(particle1.pos.y, particle1.velocity.y, particle1.acceleration.y,
                                                          particle2.pos.y, particle2.velocity.y, particle2.acceleration.y); numCols != -2)
            {
                collisionTimes.insert(collisionTimes.begin(),  yColTimes.begin(), yColTimes.end());
            }
            else if (const auto [numCols, zColTimes] = getCollisionTime(particle1.pos.z, particle1.velocity.z, particle1.acceleration.z,
                                                          particle2.pos.z, particle2.velocity.z, particle2.acceleration.z); numCols != -2)
            {
                collisionTimes.insert(collisionTimes.begin(),  zColTimes.begin(), zColTimes.end());
            }
            else
            {
                // These particles are identical, so "collide" at t = 0.
                collisionTimes.push_back(0);
            }
            for (int64_t collisionTime : collisionTimes)
            {
                // This is the collision time for one dimension; need to make sure the other dimensions collide, too!
                if (particle1.posAtTime(collisionTime) == particle2.posAtTime(collisionTime))
                {
                    std::cout << "Particles " << particle1.id << " & " << particle2.id << " collide at time: " << collisionTime << std::endl;
                    collisionsAtTime[collisionTime].push_back({&particle1, &particle2});
                }

            }
        }

    }

    for (auto& [time, collisions] : collisionsAtTime)
    {
        collisions.erase(std::remove_if(collisions.begin(), collisions.end(), [](const auto& particlePair)
                    {
                        return !particlePair.first->isAlive || !particlePair.second->isAlive;
                    }), collisions.end());
        // Every particle mentioned in collisions definitely collide with *something*.
        for (auto [particle1, particle2] : collisions)
        {
            particle1->isAlive = false;
            particle2->isAlive = false;
        }
    }

    const auto result = std::count_if(particles.begin(),particles.end(), [](const auto& particle) { return particle.isAlive; });
    std::cout << "result: " << result << std::endl;



    // Manual-ish check.
#if 0
    {
        int t = 1;
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

                // tv0 + a(1/2 * t ( t + 1)) = 
                const auto estParticleX = particle.x0 + t * particle.v0 + particle.ax * (t * (t + 1)) / 2;
                const auto estParticleX2 = Rational(particle.ax) * t * t / 2 + (Rational(particle.ax) / 2 + particle.v0) * t + particle.x0;
                std::cout << "particle.x: " << particle.x << " estParticleX: " << estParticleX << " estParticleX2: " << estParticleX2 << std::endl;
                assert(estParticleX == particle.x);
                assert(estParticleX2 == particle.x);

                const auto distance = abs(particle.x) + abs(particle.y) + abs(particle.z);
                if (distance < shortestDistance)
                {
                    shortestDistance = distance;
                    nearestParticle = particle;
                }

            }

            std::cout << "at end of time: " << t << " shortest distance is " << shortestDistance << " held by particle " << nearestParticle.id << " with acceleration: " << nearestParticle.ax << ", " << nearestParticle.ay << ", " << nearestParticle.az << std::endl;

            t++;
        }
    }
#endif
    return 0;
}
