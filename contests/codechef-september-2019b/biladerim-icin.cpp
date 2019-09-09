// Simon St James (ssjgz) - 2019-09-07
// 
// Solution to: https://www.codechef.com/SEPT19B/problems/LAPD
//
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

const int64_t Mod = 1'000'000'007ULL;

class ModNum
{
    public:
        ModNum(int64_t n = 0)
            : m_n{n}
        {
            assert(n >= 0);
        }
        ModNum& operator+=(const ModNum& other)
        {
            m_n = (m_n + other.m_n) % Mod;
            return *this;
        }
        ModNum& operator-=(const ModNum& other)
        {
            m_n = (Mod + m_n - other.m_n) % Mod;
            return *this;
        }
        ModNum& operator*=(const ModNum& other)
        {
            m_n = (m_n * other.m_n) % Mod;
            return *this;
        }
        int64_t value() const { return m_n; };
    private:
        int64_t m_n;
};

ModNum operator+(const ModNum& lhs, const ModNum& rhs)
{
    ModNum result(lhs);
    result += rhs;
    return result;
}

ModNum operator-(const ModNum& lhs, const ModNum& rhs)
{
    ModNum result(lhs);
    result -= rhs;
    return result;
}

ModNum operator*(const ModNum& lhs, const ModNum& rhs)
{
    ModNum result(lhs);
    result *= rhs;
    return result;
}

ostream& operator<<(ostream& os, const ModNum& toPrint)
{
    os << toPrint.value();
    return os;
}

int64_t divCeiling(int64_t x, int64_t y)
{
    if ((x % y) == 0)
    {
        return x / y;
    }
    else
    {
        return x / y + 1;
    }
}

struct CForA
{
    int64_t A = -1;
    int64_t C = std::numeric_limits<int64_t>::max();
    ModNum cumulativeC;
};

struct RepetitionOfC
{
    int64_t C = -1;
    int64_t numReps = -1;
    int64_t finalA = -1;
    ModNum cumulativeCTimesReps;
};

struct LookupForB
{
    vector<CForA> cForA;
    vector<RepetitionOfC> repetitionsOfC;
};

vector<LookupForB> computeLookups(int64_t maxB)
{
    //cout << "computeLookups: " << endl;
    vector<LookupForB> lookup(maxB + 1);


    for (int B = 1; B <= maxB; B++)
    {
        //cout << "B: " << B << endl;
        const int64_t maxA = B * B + 1;
        const int64_t sqrtMaxA = sqrt(maxA);
        //cout << "sqrtMaxA: " << sqrtMaxA << endl;
        //cout << "B: " << B << endl;
        //B = 20;
        auto& lookupForB = lookup[B];
        if (B == 1)
        {
            lookupForB.cForA.resize(1);
            lookupForB.cForA[0].A = 0;
            lookupForB.repetitionsOfC.push_back({std::numeric_limits<int64_t>::max(), 1, 1});
            lookupForB.repetitionsOfC.push_back({3, 1, 2});
        }
        else if (B == 2)
        {
            lookupForB.cForA.resize(sqrtMaxA + 1);
            lookupForB.cForA[2].A = 2;
            lookupForB.cForA[2].C = 6;
            lookupForB.repetitionsOfC.push_back({4, 1, 3});
            lookupForB.repetitionsOfC.push_back({3, 2, 5});
        }
        else
        {
            const int64_t sqrtMaxA = sqrt(maxA);
            lookupForB.cForA.resize(sqrtMaxA + 1);
            //cout << "B: " << B << " maxA: " << maxA << " sqrt(maxA): " << sqrtMaxA << endl;
            for (int A = 2; A <= sqrtMaxA; A++)
            {
                const int C = divCeiling(B * B + 1, A - 1) + 1;
                lookupForB.cForA[A].A = A;
                lookupForB.cForA[A].C = C;
                //cout << "A: " << A << " cForA: " << C << endl;
            }
            const int64_t finalC = lookupForB.cForA.back().C;
            //cout << "finalC: " << finalC << endl;

            int64_t cRepeated = finalC;
            int64_t aAfterCRepeats = sqrtMaxA;
#if 0
            lookupForB.repetitionsOfC.push_back({cRepeated, 1, aAfterCRepeats});
            cRepeated--;
            aAfterCRepeats++;
#endif
            cRepeated--;
            aAfterCRepeats++;
            lookupForB.repetitionsOfC.push_back({cRepeated, 1, aAfterCRepeats});
            cRepeated--;
            aAfterCRepeats++;
            lookupForB.repetitionsOfC.push_back({cRepeated, 1, aAfterCRepeats});
            cRepeated--;
            for (int i = sqrtMaxA - 1; i >= 2; i--)
            {
                //cout << " i: " << i << " aAfterCRepeats:"  << aAfterCRepeats << endl;
                const int64_t numRepetitions = lookupForB.cForA[i].C - lookupForB.cForA[i + 1].C;
                //cout << "numRepetitions: " << numRepetitions << endl;
                assert(numRepetitions >= 1);
                aAfterCRepeats += numRepetitions;
                lookupForB.repetitionsOfC.push_back({cRepeated, numRepetitions, aAfterCRepeats});
                cRepeated--;
            }
        }

        {
            ModNum cumulativeCForA;
            for (int A = 2; A <= sqrtMaxA; A++)
            {
                cumulativeCForA += lookupForB.cForA[A].C;
                lookupForB.cForA[A].cumulativeC = cumulativeCForA;
            }
        }
        {
            ModNum cumulativeCTimesReps;
            for (auto& repetitionOfC : lookupForB.repetitionsOfC)
            {
                cumulativeCTimesReps += ModNum(repetitionOfC.C) * repetitionOfC.numReps;
                repetitionOfC.cumulativeCTimesReps = cumulativeCTimesReps;
            }
        }
    }

    return lookup;
}

int64_t solveBruteForce(int64_t maxA, int64_t maxB, int64_t maxC)
{
    // Looks like minimum of f will be < 0 if (a - 1) * (c - 1) < b * b.
    int64_t result = 0;

    for (int64_t B = 1; B <= maxB; B++)
    {
        for (int64_t A = 1; A <= maxA; A++)
        {
            if ((A % 100) == 0)
                cout << "B: " << B << " A: " << A << endl;
            for (int64_t C = 1; C <= maxC; C++)
            {
                if ((A - 1) * (C - 1) > B * B)
                {
                    result = (result + 1) % Mod;
                }

            }
        }
    }
    
    return result;
}



int64_t solveOptimised(int64_t maxA, int64_t maxB, int64_t maxC, const vector<LookupForB>& lookup)
{
    ModNum result = 0;
    bool finished = false;
    for (int64_t B = 1; B <= maxB && !finished; B++)
    {
        auto& lookupForB = lookup[B];

        const int maxIndex = min<int64_t>(lookupForB.cForA.size() - 1, maxA);
        {
            int beginIndex = -1;
            int endIndex = -1;
            for (int64_t A = 2; A <= maxIndex; A++)
            {
                if (lookupForB.cForA[A].C <= maxC && beginIndex == -1)
                {
                    beginIndex = A;
                }
                if (A <= maxA)
                {
                    endIndex = A;
                }
            }

            if (beginIndex != -1 && endIndex != -1 && endIndex >= beginIndex)
            {
                result += ModNum(maxC + 1) * (endIndex - beginIndex + 1);
                result -= lookupForB.cForA[endIndex].cumulativeC;
                if (beginIndex > 0)
                {
                    result += lookupForB.cForA[beginIndex - 1].cumulativeC;
                }
            }
        }

        {
            int beginIndex = -1;
            int endIndex = -1;

            for (int i = 0; i < lookupForB.repetitionsOfC.size(); i++)
            {
                if (lookupForB.repetitionsOfC[i].C <= maxC && beginIndex == -1)
                {
                    beginIndex = i;
                }
                if (lookupForB.repetitionsOfC[i].finalA <= maxA)
                {
                    endIndex = i;
                }
            }
            if (endIndex != -1 && lookupForB.repetitionsOfC[endIndex].finalA < maxA && endIndex + 1 < lookupForB.repetitionsOfC.size())
                endIndex++;

            if (beginIndex != -1 && endIndex != -1 && endIndex >= beginIndex)
            {
                const int64_t numAsInRange = (lookupForB.repetitionsOfC[endIndex].finalA - (lookupForB.repetitionsOfC[beginIndex].finalA - lookupForB.repetitionsOfC[beginIndex].numReps));
                result += ModNum(maxC + 1) * numAsInRange - lookupForB.repetitionsOfC[endIndex].cumulativeCTimesReps;
                if (beginIndex > 0)
                {
                    result += lookupForB.repetitionsOfC[beginIndex - 1].cumulativeCTimesReps;
                }
                if (lookupForB.repetitionsOfC[endIndex].finalA > maxA)
                {
                    const int64_t numAOverCounted = lookupForB.repetitionsOfC[endIndex].finalA - maxA;
                    assert(numAOverCounted >= 0);
                    // Remove overcount.
                    result -= (ModNum(maxC + 1) - lookupForB.repetitionsOfC[endIndex].C) * numAOverCounted;
                }
            }
        }

        const int64_t lastProcessedA = lookupForB.repetitionsOfC.back().finalA;
        if (lastProcessedA < maxA)
        {
            result += (ModNum(maxA) - lastProcessedA) * (maxC - 1);
        }

    }
    
    return result.value();
}


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        // TODO - generate randomised test.
        //const int T = rand() % 100 + 1;
        const int T = 1;
        cout << T << endl;


        for (int t = 0; t < T; t++)
        {
            cout << ((rand() % 300) + 1) << " " << ((rand() % 100) + 1) << " " << ((rand() % 300) + 1) << endl;
        }

        return 0;
    }
    
    // TODO - read in testcase.
    const auto T = read<int>();

    struct Query
    {
        int64_t maxA;
        int64_t maxB;
        int64_t maxC;
    };

    vector<Query> queries;

    int64_t largestMaxB = 0;


    for (int t = 0; t < T; t++)
    {
        queries.push_back({read<int64_t>(), read<int64_t>(), read<int64_t>()});
        largestMaxB = max(largestMaxB, queries.back().maxB);
    }

    const auto& lookups = computeLookups(largestMaxB);

    for (int t = 0; t < T; t++)
    {
        const int64_t maxA = queries[t].maxA;
        const int64_t maxB = queries[t].maxB;
        const int64_t maxC = queries[t].maxC;

        //const int64_t maxA = read<int64_t>();
        //const int64_t maxB = read<int64_t>();
        //const int64_t maxC = read<int64_t>();
#ifdef BRUTE_FORCE
        const auto solutionBruteForce = solveBruteForce(maxA, maxB, maxC);
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
        const auto solutionOptimised = solveOptimised(maxA, maxB, maxC, lookups);
        cout << "solutionOptimised:  " << solutionOptimised << endl;

        assert(solutionOptimised == solutionBruteForce);
#else
        const auto solutionOptimised = solveOptimised(maxA, maxB, maxC, lookups);
        cout << solutionOptimised << endl;
#endif
    }

    // ∂  f(x) = fxx
    // ∂x
    assert(cin);
}
