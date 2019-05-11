// Simon St James (ssjgz) 2019-05-11.
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#define NDEBUG
#undef BRUTE_FORCE
#endif
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>

#include <sys/time.h>

using namespace std;


const int64_t modulus = 1'000'000'007ULL;

class ModNum
{
    public:
        ModNum(int64_t n = 0)
            : m_n{n}
        {
        }
        ModNum& operator+=(const ModNum& other)
        {
            m_n = (m_n + other.m_n) % ::modulus;
            return *this;
        }
        ModNum& operator*=(const ModNum& other)
        {
            m_n = (m_n * other.m_n) % ::modulus;
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

bool operator==(const ModNum& lhs, const ModNum& rhs)
{
    return lhs.value() == rhs.value();
}


ModNum solutionOptimised(int N, int P)
{
    vector<int> factorsOfP;
    for (int i = 1; i <= sqrt(P); i++)
    {
        factorsOfP.push_back(i);
    }
    for (int i = sqrt(P) + 1; i > 1; i--)
    {
        factorsOfP.push_back(P / i + 1);
    }
    factorsOfP.erase(std::unique(factorsOfP.begin(), factorsOfP.end()), factorsOfP.end());
    assert(std::is_sorted(factorsOfP.begin(), factorsOfP.end()));
    vector<ModNum> firstNEndingOnFactorIndex(factorsOfP.size() + 1, 0);
    for (int i = 0; i < factorsOfP.size(); i++)
    {
        firstNEndingOnFactorIndex[i] = 1;
    }
    for (int i = 1; i < N; i++)
    {
        vector<ModNum> nextEndingOnFactorIndex(factorsOfP.size() + 1, 0);
        int lastFactorIndex = 0;
        ModNum sumUpToLast = firstNEndingOnFactorIndex[lastFactorIndex];
        int summedSoFar = factorsOfP[lastFactorIndex];
        int newFactorIndex = factorsOfP.size();
        nextEndingOnFactorIndex[newFactorIndex] = firstNEndingOnFactorIndex[0];

        while (newFactorIndex >= 1)
        {
            newFactorIndex--;
            const auto diffFromPreviousLastFactor = factorsOfP[lastFactorIndex] - factorsOfP[lastFactorIndex - 1];
            const auto needSumUpTo = P / factorsOfP[newFactorIndex];
            while (lastFactorIndex + 1 < factorsOfP.size() && factorsOfP[lastFactorIndex + 1] <= needSumUpTo)
            {
                assert(lastFactorIndex + 1 < factorsOfP.size());
                lastFactorIndex++;
                const auto globble = (factorsOfP[lastFactorIndex] - summedSoFar - 1) * firstNEndingOnFactorIndex[lastFactorIndex - 1]
                     + firstNEndingOnFactorIndex[lastFactorIndex];
                sumUpToLast += globble;
                summedSoFar = factorsOfP[lastFactorIndex];
            }
            const auto globble = (needSumUpTo - factorsOfP[lastFactorIndex]) * firstNEndingOnFactorIndex[lastFactorIndex];
            sumUpToLast += globble;
            summedSoFar = needSumUpTo;

            const auto diffUntilNextFactor = factorsOfP[newFactorIndex + 1] - factorsOfP[newFactorIndex];
            assert(factorsOfP[lastFactorIndex] * factorsOfP[newFactorIndex] <= P);
            nextEndingOnFactorIndex[newFactorIndex] = sumUpToLast * 1;
        }
        firstNEndingOnFactorIndex = nextEndingOnFactorIndex;
    }

    ModNum result = 0;
    for (int r = 0; r < factorsOfP.size(); r++)
    {
        if (r + 1 < factorsOfP.size())
        {
            result += firstNEndingOnFactorIndex[r] * (factorsOfP[r + 1] - factorsOfP[r]);
        }
    }
    result += firstNEndingOnFactorIndex[factorsOfP.size() - 1] * (P - factorsOfP.back() + 1);

    return result;
}

ModNum solutionBruteForce(int N, int P)
{
    ModNum result = 0;
    vector<vector<ModNum>> firstNEndingOnP(N, vector<ModNum>(P + 1, 0));
    for (int r = 0; r <= P; r++)
    {
        firstNEndingOnP[0][r] = 1;
    }
    for (int i = 1; i < N; i++)
    {
        for (int q = 1; q <= P; q++)
        {
            for (int r = 1; r * q <= P; r++)
            {
                firstNEndingOnP[i][q] += firstNEndingOnP[i - 1][r];
            }
            //cout << "firstNEndingOnP[" << i << "][" << q << "] = " << firstNEndingOnP[i][q] << endl;
        }
    }

    for (int r = 1; r <= P; r++)
    {
        result += firstNEndingOnP[N - 1][r];
    }
    return result;
}


int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));


        const int N = rand() % 100 + 1;
        const int P = rand() % 100 + 1;
        cout << N << " " << P << endl;
        return 0;
    }
    int N;
    cin >> N;

    int P;
    cin >> P;

#ifdef BRUTE_FORCE
    const auto bruteForceResult = solutionBruteForce(N, P);
    cout << "bruteForceResult: " << bruteForceResult << endl;
#endif

    const auto optimisedResult = solutionOptimised(N, P);

#ifdef BRUTE_FORCE
    cout << "optimisedResult: " << optimisedResult << endl;
    assert(optimisedResult == bruteForceResult);
#else
    cout << optimisedResult << endl;
#endif

}
