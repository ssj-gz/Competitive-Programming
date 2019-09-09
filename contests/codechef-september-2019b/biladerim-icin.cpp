// Simon St James (ssjgz) - 2019-09-07
// 
// Solution to: https://www.codechef.com/SEPT19B/problems/LAPD
//
#define SUBMISSION
//#define VERIFY_LOOKUPS
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#undef VERIFY_LOOKUPS
//#define NDEBUG
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

bool operator==(const ModNum& lhs, const ModNum& rhs)
{
    return lhs.value() == rhs.value();
}


ostream& operator<<(ostream& os, const ModNum& toPrint)
{
    os << toPrint.value();
    return os;
}


struct Blah
{
    Blah(double x, double y, double z)
        : x(x), y(y), z(z)
    {
    }
    double x, y;
    double z;
};

bool operator<(const Blah& lhs, const Blah& rhs)
{
    if (lhs.z != rhs.z)
        return lhs.z < rhs.z;
    if (lhs.x != rhs.x)
        return lhs.x < rhs.x;
    return lhs.y < rhs.y;
}
ostream& operator<<(ostream& os, const Blah& blah)
{
    os << "z: " << blah.z << " x: " << blah.x << " y: " << blah.y << endl;
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
    ModNum cumulativeC;
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
            for (auto& x : lookupForB.repetitionsOfC)
            {
                cumulativeCTimesReps += ModNum(x.C) * x.numReps;
                x.cumulativeC = cumulativeCTimesReps;
            }
        }



#ifdef VERIFY_LOOKUPS
        vector<int64_t> csBrute(maxA + 1);
        csBrute[0] = numeric_limits<int64_t>::max();
        csBrute[1] = numeric_limits<int64_t>::max();
        for (int A = 2; A <= maxA; A++)
        {
            const int64_t C = divCeiling(B * B + 1, A - 1) + 1;
            assert((C - 1) * (A - 1) >= B *B + 1);
            csBrute[A] = C;
        }
        vector<int64_t> csOpt;
        for (const auto x : lookupForB.cForA)
        {
            csOpt.push_back(x.C);
        }
        int dbgA = lookupForB.cForA.back().A;
        for (const auto x : lookupForB.repetitionsOfC)
        {
            //cout << " C: " << x.C << " numReps: " << x.numReps << endl;
            for (int64_t i = 1; i <= x.numReps; i++)
            {
                csOpt.push_back(x.C);
            }
            dbgA += x.numReps;
            //cout << " dbgA: " << dbgA << " x.aAfterCRepeats: " << x.finalA << endl;
            assert(dbgA == x.finalA);
        }
        assert(dbgA == maxA);

        for (int64_t i = 0; i <= maxA; i++)
        {
       //     cout << "i: " << i << " csBrute: " << csBrute[i] << " csOpt: " << csOpt[i] << endl;
        }
        assert(csBrute == csOpt);
#endif
        //break;
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
                    //cout << " interesting triple: (" << A << ", " << B << ", " << C << ")" << endl;
                    result = (result + 1) % Mod;
                }

            }
        }
    }
#if 0
    while (true)
    {
        const int A = rand() % 20 + 1;
        const int B = rand() % 20 + 1;
        const int C = rand() % 20 + 1;


        const bool isSaddle = (A - 1) * (C - 1) <= B * B;
        cout << "A: " << A << " B: " << B << " C: " << C << endl;
        cout << "(A - 1) * (C - 1): " << (A - 1) * (C - 1) << endl;
        cout << "B * B: " << (B * B) << endl;
        cout << "isSaddle: " << isSaddle << endl;

        //cout << "sausage: " << (double)(C - 1) - (double)(B) * B / ((A - 1)) << endl; 

        auto f = [A, B, C](double x, double y)
        {
            return ((double)A - 1) * x * x + 2 * (double)B * x * y + ((double)C - 1) * y * y;
        };
        //cout << "bleep: " << f(-100, -8.333335) << endl;

        //vector<Blah> blahs;

        Blah minBlah = {0, 0, 3000};

        const double range = 10;
        for (double x = -range; x <= range; x += 0.01)
        {
            for (double y = -range; y <= range; y += 0.01)
            {
                //blahs.push_back({x, y, f(x, y)});
                if (Blah(x, y, f(x,y)) < minBlah)
                {
                    minBlah = Blah(x, y, f(x,y));
                }
            }
        }
        cout << "minBlah: " << minBlah << endl;

        const bool isMinNegative = (minBlah.z < -0.01);
        cout << "minBlah negative: " << isMinNegative << "  isSaddle: " << isSaddle << endl;
        assert(isMinNegative == isSaddle);
#if 0
        sort(blahs.begin(), blahs.end(), [](const auto lhs, const auto rhs)
                {
                if (lhs.z != rhs.z)
                return lhs.z < rhs.z;
                if (lhs.x != rhs.x)
                return lhs.x < rhs.x;
                return lhs.y < rhs.y;
                });
#endif

#if 0
        for (const auto blah : blahs)
        {
            cout << "z: " << blah.z << " x: " << blah.x << " y: " << blah.y << endl;
        }
#endif
    }
#endif


    
    return result;
}



int64_t solveOptimised(int64_t maxA, int64_t maxB, int64_t maxC, const vector<LookupForB>& lookup)
{
    //cout << "maxA: " << maxA << " maxB: " << maxB << " maxC: " << maxC << endl;
    ModNum result = 0;
    bool finished = false;
    for (int64_t B = 1; B <= maxB && !finished; B++)
    {
        auto& lookupForB = lookup[B];

        //cout << "B: " << B << endl;
        ModNum dbgToAddFromFirstPhase;
        ModNum toAddFromFirstPhase;
        {
#if 0
            for (int64_t A = 2; A <= min<int64_t>(lookupForB.cForA.size() - 1, maxA); A++)
            {
                const auto C = lookupForB.cForA[A].C;
                if (C >= 1 && C <= maxC)
                {
                    //cout << " C: " << C << " maxC + 1: " << (maxC + 1) << endl;
                    const ModNum amountToAdd = ModNum(maxC + 1) - ModNum(C);
                    //cout << " added " << amountToAdd << " for A: " << A << " C: " << C << endl;
                    dbgToAddFromFirstPhase += amountToAdd;
                }
            }
#endif
            const int maxIndex = min<int64_t>(lookupForB.cForA.size() - 1, maxA);
            int dbgBeginIndex = -1;
            int dbgEndIndex = -1;
            for (int64_t A = 2; A <= maxIndex; A++)
            {
                if (lookupForB.cForA[A].C <= maxC && dbgBeginIndex == -1)
                {
                    dbgBeginIndex = A;
                }
                if (A <= maxA)
                {
                    dbgEndIndex = A;
                }
            }


            int beginIndex = dbgBeginIndex;
            int endIndex = dbgEndIndex;
            if (beginIndex != -1 && endIndex != -1 && endIndex >= beginIndex)
            {
                toAddFromFirstPhase += ModNum(maxC + 1) * (endIndex - beginIndex + 1);
                toAddFromFirstPhase -= lookupForB.cForA[endIndex].cumulativeC;
                if (beginIndex > 0)
                {
                    toAddFromFirstPhase += lookupForB.cForA[beginIndex - 1].cumulativeC;
                }

                //cout << "beginIndex: " << beginIndex << " endIndex: " << endIndex << " maxC: " << maxC << endl;
                for (int K = max(beginIndex - 1, 0); K <= endIndex; K++)
                {
                    //assert(K <= maxA && lookupForB.cForA[K].C <= maxC);
                    //toAddFromFirstPhase += ModNum(maxC + 1) - lookupForB.cForA[K].C;
                    //cout << " K: " << K << " C: " << lookupForB.cForA[K].C << " cumulativeC: " << lookupForB.cForA[K].cumulativeC << endl;
                }
            }
            //cout << "toAddFromFirstPhase: " << toAddFromFirstPhase << " dbgToAddFromFirstPhase: " << dbgToAddFromFirstPhase << endl;
            //assert(toAddFromFirstPhase == dbgToAddFromFirstPhase);

        }
        //cout << " result after first block: " << result << endl;
        //ModNum dbgToAddFromSecondPhase;
        ModNum toAddFromSecondPhase;
        {
            int64_t previousA = lookupForB.cForA.back().A;
            //cout << " previousA: " << previousA << endl;
#if 0
            for (const auto& x : lookupForB.repetitionsOfC)
            {
                const auto C = x.C;
                if (C >= 1 && C <= maxC)
                {

                    if (x.finalA <= maxA)
                    {
                        const ModNum amountToAdd = (ModNum(maxC + 1) - C) * x.numReps;
                        //cout << " found " << x.numReps << " repetitions of C: " << C << " ending at A: " << x.finalA << " adding: " << amountToAdd << endl;
                        dbgToAddFromSecondPhase += amountToAdd;
                    }
                    else
                    {
                        const int64_t truncatedReps = maxA - previousA;
                        //cout << " found " << x.numReps << " repetitions of C: " << C << " ending at A: " << x.finalA << " which is greater than maxA: " << maxA << "  truncated to reps: " << truncatedReps << endl;
                        //assert(truncatedReps >= 0);
                        if (truncatedReps > 0)
                        {
                            const ModNum amountToAdd = (ModNum(maxC + 1) - ModNum(C)) * truncatedReps;
                            //cout << " adding: " << amountToAdd << endl;
                            dbgToAddFromSecondPhase += amountToAdd;
                        }
                        break;
                    }
                }
                previousA = x.finalA;
                //cout << " updated previousA: " << previousA << endl;

            }
#endif

            int beginIndex = -1;
            int endIndex = -1;

            for (int i = 0; i < lookupForB.repetitionsOfC.size(); i++)
            {
                //cout << " i:" << i << " repetitionsOfC C:" << lookupForB.repetitionsOfC[i].C << " repetitionsOfC finalA:" << lookupForB.repetitionsOfC[i].finalA << " repetitionsOfC C: " << lookupForB.repetitionsOfC[i].numReps << " cumulativeC: " << lookupForB.repetitionsOfC[i].cumulativeC << endl;
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


            //cout << "phase 2: beginIndex: " << beginIndex << " endIndex: " << endIndex << " maxC: " << maxC << " maxA: " << maxA << endl;
            if (beginIndex != -1 && endIndex != -1 && endIndex >= beginIndex)
            {
                //cout << " finalA at endIndex: " << lookupForB.repetitionsOfC[endIndex].finalA << endl;
                const int64_t numAsInRange = (lookupForB.repetitionsOfC[endIndex].finalA - (lookupForB.repetitionsOfC[beginIndex].finalA - lookupForB.repetitionsOfC[beginIndex].numReps));
                //cout << "numAsInRange: " << numAsInRange << endl;
                toAddFromSecondPhase += ModNum(maxC + 1) * numAsInRange - lookupForB.repetitionsOfC[endIndex].cumulativeC;
                if (beginIndex > 0)
                {
                    toAddFromSecondPhase += lookupForB.repetitionsOfC[beginIndex - 1].cumulativeC;
                }
                if (lookupForB.repetitionsOfC[endIndex].finalA > maxA)
                {
                    const int64_t numAOverCounted = lookupForB.repetitionsOfC[endIndex].finalA - maxA;
                    assert(numAOverCounted >= 0);
                    //cout << "numAOverCounted: " << numAOverCounted << endl;
                    // Remove overcount.
                    toAddFromSecondPhase -= (ModNum(maxC + 1) - lookupForB.repetitionsOfC[endIndex].C) * numAOverCounted;
                    //toAddFromSecondPhase += lookupForB.repetitionsOfC[endIndex].C * overcount;
                }
            }
            //cout << "toAddFromSecondPhase: " << toAddFromSecondPhase << " dbgToAddFromSecondPhase: " << dbgToAddFromSecondPhase << endl;
            //assert(toAddFromSecondPhase == dbgToAddFromSecondPhase);

        }
        result += toAddFromFirstPhase + toAddFromSecondPhase;

        //cout << "last processed A: " << lookupForB.repetitionsOfC.back().finalA << " maxA: " << maxA << endl;
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
