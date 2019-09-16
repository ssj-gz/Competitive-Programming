// Simon St James (ssjgz) - 2019-09-07
// 
// Solution to: https://www.codechef.com/SEPT19B/problems/LAPD
//
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>

#include <cassert>

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
    int64_t c = std::numeric_limits<int64_t>::max();
    ModNum cumulativeC;
};

struct RepetitionOfC
{
    int64_t c = -1;
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
    vector<LookupForB> lookup(maxB + 1);

    for (int b = 1; b <= maxB; b++)
    {
        const int64_t alwaysValidA = b * b + 1; // (alwaysValidA - 1) * (c - 2) > b * b for all c >= 1.
        const int64_t sqrtAlwaysValidA = sqrt(alwaysValidA); // The value "sqrtAlwaysValidA" is ~= b.
        auto& lookupForB = lookup[b];
        if (b == 1)
        {
            // Ugly - special case for b == 1.
            lookupForB.cForA.resize(1);
            lookupForB.repetitionsOfC.push_back({std::numeric_limits<int64_t>::max(), 1, 1});
            lookupForB.repetitionsOfC.push_back({3, 1, 2});
        }
        else if (b == 2)
        {
            // Ugly - special case for b == 2.
            lookupForB.cForA.resize(sqrtAlwaysValidA + 1);
            lookupForB.cForA[2].c = 6;
            lookupForB.repetitionsOfC.push_back({4, 1, 3});
            lookupForB.repetitionsOfC.push_back({3, 2, 5});
        }
        else
        {
            // Compute cForA.
            lookupForB.cForA.resize(sqrtAlwaysValidA + 1);
            for (int a = 2; a <= sqrtAlwaysValidA; a++)
            {
                const int c = divCeiling(b * b + 1, a - 1) + 1;
                lookupForB.cForA[a].c = c;
            }
            const int64_t finalC = lookupForB.cForA.back().c;

            int64_t cRepeated = finalC;
            int64_t aAfterCRepeats = sqrtAlwaysValidA;

            {
                // This is a bit of a bodge - not sure why it's needed, to be honest XD
                cRepeated--;
                aAfterCRepeats++;
                lookupForB.repetitionsOfC.push_back({cRepeated, 1, aAfterCRepeats});
                cRepeated--;
                aAfterCRepeats++;
                lookupForB.repetitionsOfC.push_back({cRepeated, 1, aAfterCRepeats});
                cRepeated--;
            }
            // Use the "symmetry" to compute repetitionsOfC using cForA.
            for (int i = sqrtAlwaysValidA - 1; i >= 2; i--)
            {
                const int64_t numRepetitions = lookupForB.cForA[i].c - lookupForB.cForA[i + 1].c;
                assert(numRepetitions >= 1);
                aAfterCRepeats += numRepetitions;
                lookupForB.repetitionsOfC.push_back({cRepeated, numRepetitions, aAfterCRepeats});
                cRepeated--;
            }
        }

        // Calculate cumulativeC/ cumulativeCTimesReps for cForA/ repetitionsOfC.
        {
            ModNum cumulativeCForA;
            for (int a = 2; a <= sqrtAlwaysValidA; a++)
            {
                cumulativeCForA += lookupForB.cForA[a].c;
                lookupForB.cForA[a].cumulativeC = cumulativeCForA;
            }
        }
        {
            ModNum cumulativeCTimesReps;
            for (auto& repetitionOfC : lookupForB.repetitionsOfC)
            {
                cumulativeCTimesReps += ModNum(repetitionOfC.c) * repetitionOfC.numReps;
                repetitionOfC.cumulativeCTimesReps = cumulativeCTimesReps;
            }
        }
    }

    return lookup;
}

int64_t solveOptimised(int64_t maxA, int64_t maxB, int64_t maxC, const vector<LookupForB>& lookup)
{
    ModNum result = 0;
    for (int64_t b = 1; b <= maxB; b++)
    {
        auto& lookupForB = lookup[b];

        const int maxIndex = min<int64_t>(lookupForB.cForA.size() - 1, maxA);
        {
            const auto& cForALookup = lookupForB.cForA;


            // NB: since cForALookup is in increasing order of a and non-increasing order
            // of c, we could find beginIndex and endIndex in O(log2 maxA).
            // Can't be bothered, though :)
            int beginIndex = -1;
            int endIndex = -1;
            for (int64_t a = 2; a <= maxIndex; a++)
            {
                if (cForALookup[a].c <= maxC && beginIndex == -1)
                {
                    beginIndex = a;
                }
                if (a <= maxA)
                {
                    endIndex = a;
                }
            }

            if (beginIndex != -1 && endIndex != -1 && endIndex >= beginIndex)
            {
                result += ModNum(maxC + 1) * (endIndex - beginIndex + 1);
                result -= cForALookup[endIndex].cumulativeC;
                if (beginIndex > 0)
                {
                    result += cForALookup[beginIndex - 1].cumulativeC;
                }
            }
        }

        {
            const auto& repetitionOfCLookup = lookupForB.repetitionsOfC;

            int beginIndex = -1;
            int endIndex = -1;

            // NB: since repetitionsOfC is in increasing order of a and non-increasing order
            // of c, we could find beginIndex and endIndex in O(log2 maxA).
            // Can't be bothered, though :)
            //
            if (maxA >= repetitionOfCLookup.back().finalA)
            {
                // maxA is >= than all finalA's.  If this is not the case, then we need to find the index of the first finalA which
                // is strictly larger than maxA, which is done in the loop below, and then deal with the resulting overcounting.
                endIndex = repetitionOfCLookup.size() - 1;
            }
            for (int i = 0; i < repetitionOfCLookup.size(); i++)
            {
                if (repetitionOfCLookup[i].c <= maxC && beginIndex == -1)
                {
                    beginIndex = i;
                }
                if (i > 0 && repetitionOfCLookup[i].finalA > maxA && repetitionOfCLookup[i - 1].finalA <= maxA)
                {
                    endIndex = i;
                }
            }

            if (beginIndex != -1 && endIndex != -1 && endIndex >= beginIndex)
            {
                const int64_t AAtBeginningOfRange = repetitionOfCLookup[beginIndex].finalA - repetitionOfCLookup[beginIndex].numReps;
                const int64_t numAsInRange = repetitionOfCLookup[endIndex].finalA - AAtBeginningOfRange;
                ModNum sumOfCTimesRepsInRange = repetitionOfCLookup[endIndex].cumulativeCTimesReps;
                if (beginIndex > 0)
                {
                    sumOfCTimesRepsInRange -= repetitionOfCLookup[beginIndex - 1].cumulativeCTimesReps;
                }
                result += ModNum(maxC + 1) * numAsInRange - sumOfCTimesRepsInRange;
                if (repetitionOfCLookup[endIndex].finalA > maxA)
                {
                    const int64_t numAOverCounted = repetitionOfCLookup[endIndex].finalA - maxA;
                    assert(numAOverCounted >= 0);
                    // Overcounted the contribution of c >= repetitionOfCLookup[endIndex].c by numAOverCounted times - correct for this.
                    result -= (ModNum(maxC + 1) - repetitionOfCLookup[endIndex].c) * numAOverCounted;
                }
            }
        }

        const int64_t lastProcessedA = lookupForB.repetitionsOfC.back().finalA;
        if (lastProcessedA < maxA)
        {
            // There exist a such that finalA < a <= maxA; incorporate them into the results - each such
            // a is satisfied by all c.
            result += (ModNum(maxA) - lastProcessedA) * (maxC - 1);
        }

    }
    
    return result.value();
}


int main(int argc, char* argv[])
{
    //   X: 31 Y: 1 divCeiling(X, Y):  31 
    //   X: 31 Y: 2 divCeiling(X, Y):  16 (31 - 16 = 15)
    //   X: 31 Y: 3 divCeiling(X, Y):  11 (16 - 11 = 5)
    //   X: 31 Y: 4 divCeiling(X, Y):  8  (11 - 8  = 3)
    //   X: 31 Y: 5 divCeiling(X, Y):  7  (8 - 7   = 1)   ┐ 
    //   X: 31 Y: 6 divCeiling(X, Y):  6  (7 - 6   = 1)   ┘ sqrt(X) is someone around here.
    //   X: 31 Y: 7 divCeiling(X, Y):  5  ] 1 repetitions
    //   X: 31 Y: 8 divCeiling(X, Y):  4  ┐
    //   X: 31 Y: 9 divCeiling(X, Y):  4  ├ 3 repetitions
    //   X: 31 Y: 10 divCeiling(X, Y): 4  ┘
    //   X: 31 Y: 11 divCeiling(X, Y): 3  ┐
    //   X: 31 Y: 12 divCeiling(X, Y): 3  │
    //   X: 31 Y: 13 divCeiling(X, Y): 3  ├ 5 repetitions
    //   X: 31 Y: 14 divCeiling(X, Y): 3  │
    //   X: 31 Y: 15 divCeiling(X, Y): 3  ┘
    //   X: 31 Y: 16 divCeiling(X, Y): 2  ┐
    //   X: 31 Y: 17 divCeiling(X, Y): 2  │
    //   X: 31 Y: 18 divCeiling(X, Y): 2  │
    //   X: 31 Y: 19 divCeiling(X, Y): 2  │
    //   X: 31 Y: 20 divCeiling(X, Y): 2  │
    //   X: 31 Y: 21 divCeiling(X, Y): 2  │
    //   X: 31 Y: 22 divCeiling(X, Y): 2  ├ 15 repetitions
    //   X: 31 Y: 23 divCeiling(X, Y): 2  │
    //   X: 31 Y: 24 divCeiling(X, Y): 2  │
    //   X: 31 Y: 25 divCeiling(X, Y): 2  │
    //   X: 31 Y: 26 divCeiling(X, Y): 2  │
    //   X: 31 Y: 27 divCeiling(X, Y): 2  │
    //   X: 31 Y: 28 divCeiling(X, Y): 2  │
    //   X: 31 Y: 29 divCeiling(X, Y): 2  │
    //   X: 31 Y: 30 divCeiling(X, Y): 2  ┘
    //   X: 31 Y: 31 divCeiling(X, Y): 1
    //
    //      Diagram showing symmetry of divCeiling(X, Y)
    ios::sync_with_stdio(false);
    
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

        const auto solutionOptimised = solveOptimised(maxA, maxB, maxC, lookups);
        cout << solutionOptimised << endl;
    }

    assert(cin);
}
