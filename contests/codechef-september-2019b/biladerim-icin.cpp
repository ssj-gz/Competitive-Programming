// Simon St James (ssjgz) - 2019-09-07
// 
// Solution to: https://www.codechef.com/SEPT19B/problems/LAPD
//
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
//#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

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
    int64_t C = -1;
};

struct RepetitionOfC
{
    int64_t C = -1;
    int64_t numReps = -1;
    int64_t finalA = -1;
};

struct LookupForB
{
    vector<CForA> cForA;
    vector<RepetitionOfC> repetitionsOfC;
};

vector<LookupForB> computeLookups(int64_t maxB)
{
    vector<LookupForB> lookup(maxB + 1);

    for (int B = 2; B <= maxB; B++)
    {
        const int64_t maxA = B * B + 1;
        const int64_t sqrtMaxA = sqrt(maxA);
        auto& lookupForB = lookup[B];
        lookupForB.cForA.resize(sqrtMaxA + 1);
        cout << "B: " << B << " maxA: " << maxA << " sqrt(maxA): " << sqrtMaxA << endl;
        vector<int> csBrute(maxA + 1);
        for (int A = 2; A <= maxA; A++)
        {
            const int C = divCeiling(B * B + 1, A - 1);
            csBrute[A] = C;
        }
        vector<int> csOpt(maxA + 1);
        for (int A = 2; A <= sqrtMaxA; A++)
        {
            const int C = divCeiling(B * B + 1, A - 1);
            csOpt[A] = C;
            lookupForB.cForA[A].A = A;
            lookupForB.cForA[A].C = C;
        }
        const int64_t finalC = lookupForB.cForA.back().C;
        cout << "finalC: " << finalC << endl;

        vector<int64_t> diffs;
        diffs.push_back(1);
        diffs.push_back(1);
        for (int i = sqrtMaxA - 1; i >= 0; i--)
        {
            diffs.push_back(csOpt[i] - csOpt[i + 1]);
        }
        for (int i = 0; i < diffs.size(); i++)
        {
            cout << "i: " << i << " diff: " << diffs[i] << endl;
        }

        int64_t cRepeated = finalC;
        int64_t aAfterCRepeats = sqrtMaxA + 1;
        lookupForB.repetitionsOfC.push_back({cRepeated, 1, aAfterCRepeats});
        cRepeated--;
        aAfterCRepeats++;
        lookupForB.repetitionsOfC.push_back({cRepeated, 1, aAfterCRepeats});
        cRepeated--;
        aAfterCRepeats++;
        for (int i = sqrtMaxA - 1; i >= 0; i--)
        {
            lookupForB.repetitionsOfC.push_back({cRepeated, lookupForB.cForA[i].C - lookupForB.cForA[i + 1].C, aAfterCRepeats});
            cRepeated--;
            aAfterCRepeats++;
        }

        int currentC = finalC;
        int numReps = 0;
        int diffIndex = 0;
        for (int A = sqrtMaxA + 1; A <= maxA; A++)
        {
            if (numReps == 0)
            {
                cout << "A: " << A << " numReps = 0; taking diff from diffs[" << (diffIndex) << "] = " << diffs[diffIndex] << endl;
                numReps = diffs[diffIndex];
                diffIndex++;
                currentC--;
            }

            csOpt[A] = currentC;
            numReps--;
        }

        for (int i = 0; i <= maxA; i++)
        {
            cout << "i: " << i << " csBrute: " << csBrute[i] << " csOpt: " << csOpt[i] << endl;
        }
        assert(csBrute == csOpt);
    }

    return lookup;
};

int64_t solveBruteForce(int64_t maxA, int64_t maxB, int64_t maxC)
{
    // Looks like minimum of f will be < 0 if (a - 1) * (c - 1) < b * b.
    int64_t result = 0;

    for (int64_t B = 1; B <= maxB; B++)
    {
        for (int64_t A = 1; A <= maxA; A++)
        {
            for (int64_t C = 1; C <= maxC; C++)
            {
                if ((A - 1) * (C - 1) > B * B)
                {
//                    cout << " interesting triple: (" << A << ", " << B << ", " << C << ")" << endl;
                    result++;
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



int64_t solveOptimised(int64_t maxA, int64_t maxB, int64_t maxC)
{
    ModNum result = 0;
    bool finished = false;
    for (int64_t B = 1; B <= maxB && !finished; B++)
    {
        //B = 899;
        int64_t lastC = -1;
        vector<int64_t> diffsBetweenCs;
        vector<int64_t> lengthsOfRunsOfCs;
        int64_t currentCRunLength = 1;

        for (int64_t A = 2; A <= maxA && !finished; A++)
        {
            //cout << "B: " << B << " A: " << A << endl;
            //int64_t C = ((B * B) + 1) / (A - 1);
            int64_t C = divCeiling(B * B + 1, A - 1) + 1;

            auto isValidC = [&A, &B, maxC](const int C)
            {
                //cout <<  "  isValidC? A: " << A << " B: " << B << " C: " << C << " B * B: " << B * B << " (A - 1) * (C - 1): " << ((A - 1) * (C - 1)) << endl;
                return C >= 1 && C <= maxC && ((A - 1) * (C - 1) > B * B);
            };

            //cout << " choosing initial C = " << C << " isValidC: " << isValidC(C) << endl;

            if (!isValidC(C))
            {
                //cout << " Could not find C for A: " << A << " B: " << B << endl;
                continue;
            }
            //cout << "Found valid C: " << C << endl;
            if (A - 1 >= sqrt(B * B + 1))
            {
                if (C == lastC)
                {
                    currentCRunLength++;
                }
                else
                {
                    lengthsOfRunsOfCs.push_back(currentCRunLength);
                    currentCRunLength = 1;
                }
            }
            else
            {
                if (lastC != -1)
                {
                    diffsBetweenCs.push_back(abs(C - lastC));
                    if (diffsBetweenCs.back() == 0)
                    {
                        //cout << "Whoo: A: " << A << " B: " << B << endl;
                    }
                }
            }
            //if (C == 2)
            {
                // Done - all remaining A's paired with all possible C's will do.
                //cout << "Found C == 2 for A: " << A << " B: " << B << " -  done" << endl;
                //cout << "result so far: " << result << " maxA - A: " << (maxA - A) << " maxC: " << maxC << endl;
                //result += ModNum(maxA - A + 1) * ModNum(maxC - 1);
                //finished = true;
                //assert(false);
                //break;
            }
            //else
            {
                result += maxC - C + 1;
            }
            
            lastC = C;

        }

        //cout << "B:" << B << endl;
        //cout << " diffsBetweenCs:" << endl;
        for (const auto x : diffsBetweenCs)
        {
            //cout << x << " ";
        }
        //cout << endl;
        reverse(lengthsOfRunsOfCs.begin(), lengthsOfRunsOfCs.end());
        //cout << " lengthsOfRunsOfCs:" << endl;
        for (const auto x : lengthsOfRunsOfCs)
        {
            //cout << x << " ";
        }
        //cout << endl;
        //break;



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
            cout << ((rand() % 10) + 1) << " " << ((rand() % 10) + 1) << " " << ((rand() % 10) + 1) << endl;
        }

        return 0;
    }
    
    // TODO - read in testcase.
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {

        const int64_t maxA = read<int64_t>();
        const int64_t maxB = read<int64_t>();
        const int64_t maxC = read<int64_t>();
#ifdef BRUTE_FORCE
#if 1
        const auto solutionBruteForce = solveBruteForce(maxA, maxB, maxC);
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
#endif
#if 1
        const auto solutionOptimised = solveOptimised(maxA, maxB, maxC);
        cout << "solutionOptimised:  " << solutionOptimised << endl;

        assert(solutionOptimised == solutionBruteForce);
#endif
#else
        const auto solutionOptimised = solveOptimised(maxA, maxB, maxC);
        cout << solutionOptimised << endl;
#endif
    }

    // ∂  f(x) = fxx
    // ∂x
    assert(cin);
}
