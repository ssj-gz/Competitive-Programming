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
    vector<LookupForB> lookup(maxB + 1);

    for (int B = 1; B <= maxB; B++)
    {
        const int64_t maxA = B * B + 1;
        const int64_t sqrtMaxA = sqrt(maxA);
        auto& lookupForB = lookup[B];
        if (B == 1)
        {
            // Ugly - special case for B == 1.
            lookupForB.cForA.resize(1);
            lookupForB.repetitionsOfC.push_back({std::numeric_limits<int64_t>::max(), 1, 1});
            lookupForB.repetitionsOfC.push_back({3, 1, 2});
        }
        else if (B == 2)
        {
            // Ugly - special case for B == 2.
            lookupForB.cForA.resize(sqrtMaxA + 1);
            lookupForB.cForA[2].C = 6;
            lookupForB.repetitionsOfC.push_back({4, 1, 3});
            lookupForB.repetitionsOfC.push_back({3, 2, 5});
        }
        else
        {
            lookupForB.cForA.resize(sqrtMaxA + 1);
            for (int A = 2; A <= sqrtMaxA; A++)
            {
                const int C = divCeiling(B * B + 1, A - 1) + 1;
                lookupForB.cForA[A].C = C;
            }
            const int64_t finalC = lookupForB.cForA.back().C;

            int64_t cRepeated = finalC;
            int64_t aAfterCRepeats = sqrtMaxA;

            // This is a bit of a bodge - not sure why it's needed, to be honest XD
            cRepeated--;
            aAfterCRepeats++;
            lookupForB.repetitionsOfC.push_back({cRepeated, 1, aAfterCRepeats});
            cRepeated--;
            aAfterCRepeats++;
            lookupForB.repetitionsOfC.push_back({cRepeated, 1, aAfterCRepeats});
            cRepeated--;
            for (int i = sqrtMaxA - 1; i >= 2; i--)
            {
                const int64_t numRepetitions = lookupForB.cForA[i].C - lookupForB.cForA[i + 1].C;
                assert(numRepetitions >= 1);
                aAfterCRepeats += numRepetitions;
                lookupForB.repetitionsOfC.push_back({cRepeated, numRepetitions, aAfterCRepeats});
                cRepeated--;
            }
        }

        // Calculate cumulativeC/ cumulativeCTimesReps for cForA/ repetitionsOfC.
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

int64_t solveOptimised(int64_t maxA, int64_t maxB, int64_t maxC, const vector<LookupForB>& lookup)
{
    ModNum result = 0;
    for (int64_t B = 1; B <= maxB; B++)
    {
        auto& lookupForB = lookup[B];

        const int maxIndex = min<int64_t>(lookupForB.cForA.size() - 1, maxA);
        {
            const auto& cForALookup = lookupForB.cForA;


            // NB: since cForALookup is in increasing order of A and non-increasing order
            // of C, we could find beginIndex and endIndex in O(log2 maxA).
            // Can't be bothered, though :)
            int beginIndex = -1;
            int endIndex = -1;
            for (int64_t A = 2; A <= maxIndex; A++)
            {
                if (cForALookup[A].C <= maxC && beginIndex == -1)
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

            // NB: since repetitionsOfC is in increasing order of A and non-increasing order
            // of C, we could find beginIndex and endIndex in O(log2 maxA).
            // Can't be bothered, though :)
            for (int i = 0; i < repetitionOfCLookup.size(); i++)
            {
                if (repetitionOfCLookup[i].C <= maxC && beginIndex == -1)
                {
                    beginIndex = i;
                }
                if (repetitionOfCLookup[i].finalA <= maxA)
                {
                    endIndex = i;
                }
            }
            if (endIndex != -1 && repetitionOfCLookup[endIndex].finalA < maxA && endIndex + 1 < repetitionOfCLookup.size())
            {
                // Now endIndex is the index such that *either*
                //  i) it is the index with finalA == maxA.
                //  ii) it is the first index with finalA > maxA if there is no such finalA == maxA (the "overcount" case; see below).
                endIndex++;
            }

            if (beginIndex != -1 && endIndex != -1 && endIndex >= beginIndex)
            {
                const int64_t numAsInRange = (repetitionOfCLookup[endIndex].finalA - (repetitionOfCLookup[beginIndex].finalA - repetitionOfCLookup[beginIndex].numReps));
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
                    // Remove overcount.
                    result -= (ModNum(maxC + 1) - repetitionOfCLookup[endIndex].C) * numAOverCounted;
                }
            }
        }

        const int64_t lastProcessedA = lookupForB.repetitionsOfC.back().finalA;
        if (lastProcessedA < maxA)
        {
            // There are A such that finalA < A <= maxA; incorporate them into the results - each such
            // A is satisfied by all C.
            result += (ModNum(maxA) - lastProcessedA) * (maxC - 1);
        }

    }
    
    return result.value();
}


int main(int argc, char* argv[])
{
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
