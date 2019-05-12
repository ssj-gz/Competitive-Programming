// Simon St James (ssjgz) - 2019-05-12
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <cassert>

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
        ModNum& operator-=(const ModNum& other)
        {
            m_n += ::modulus;
            assert(m_n >= other.m_n);
            m_n = (m_n - other.m_n) % ::modulus;
            return *this;
        }
        ModNum& operator*=(const ModNum& other)
        {
            m_n = (m_n * other.m_n) % ::modulus;
            return *this;
        }
        ModNum operator++(int)
        {
            m_n += 1;
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

bool operator==(const ModNum& lhs, const ModNum& rhs)
{
    return lhs.value() == rhs.value();
}



int numChanges(const string& a, const string& b)
{
    int numChanges = 0;
    assert(a.size() == b.size());
    for (int i = 0; i < a.size(); i++)
    {
        if (a[i] != b[i])
            numChanges++;
    }
    return numChanges;

}

vector<int> periods(const string& a)
{
    vector<int> periods;
    for (int length = 1; length < a.size(); length++)
    {
        int startPos = length;
        bool periodIsLength = true;
        while (startPos < a.size())
        {
            if (a.substr(0, length) != a.substr(startPos, length))
            {
                periodIsLength = false;
                break;
            }
            startPos += length;
        }
        if (periodIsLength)
            periods.push_back(length);
    }
    return periods;
}

bool isPeriodic(const string& a)
{
    return !periods(a).empty();
}

vector<int> numWithPeriod;

void blah(string& alteredStringSoFar, int nextIndex, int K, const string& originalString, ModNum& numNonPeriodicFound, ModNum& totalFound)
{
    if (nextIndex == originalString.size())
    {
        if (numChanges(alteredStringSoFar, originalString) <= K && !isPeriodic(alteredStringSoFar))
        {
            numNonPeriodicFound++;
        }
        if (numChanges(alteredStringSoFar, originalString) <= K)
        {
            totalFound++;
            if (isPeriodic(alteredStringSoFar))
            {
                cout << "Made periodic string " << alteredStringSoFar << "(periods: ";
                for (const auto period : periods(alteredStringSoFar))
                {
                    cout << " " << period;
                    numWithPeriod[period]++;
                }
                cout <<  ") with " << numChanges(alteredStringSoFar, originalString) << " changes" << endl;
            }
        }
        return;
    }
    blah(alteredStringSoFar, nextIndex + 1, K, originalString, numNonPeriodicFound, totalFound);

    alteredStringSoFar[nextIndex] = '0' + ('1' - alteredStringSoFar[nextIndex]);
    blah(alteredStringSoFar, nextIndex + 1, K, originalString, numNonPeriodicFound, totalFound);
    alteredStringSoFar[nextIndex] = '0' + ('1' - alteredStringSoFar[nextIndex]);
} 

ModNum solveBruteForce(const string& binaryString, int N, int K)
{
    numWithPeriod.clear();
    numWithPeriod.resize(N + 1);
    string alteredStringSoFar = binaryString;
    ModNum numNonPeriodicFound = 0;
    ModNum totalFound = 0;
    blah(alteredStringSoFar, 0, K, binaryString, numNonPeriodicFound, totalFound);
    cout << "totalFound: " << totalFound << endl;
    return numNonPeriodicFound;
}

ModNum computeNumStringsWithUpToKChanges(int N, int K)
{
    vector<vector<ModNum>> numOfLengthWithChanges(N + 1, vector<ModNum>(K + 1, 0));
    numOfLengthWithChanges[1][0] = 1;
    if (K > 0)
        numOfLengthWithChanges[1][1] = 1;
    for (int i = 2; i <= N; i++)
    {
        for (int numChanges = 0; numChanges <= min(K, i); numChanges++)
        {
            numOfLengthWithChanges[i][numChanges] = numOfLengthWithChanges[i - 1][numChanges];
            if (numChanges != 0)
                numOfLengthWithChanges[i][numChanges] += numOfLengthWithChanges[i - 1][numChanges - 1];
        }
    }
    ModNum numStrings = 0;
    for (int numChanges = 0; numChanges <= K; numChanges++)
    {
        numStrings += numOfLengthWithChanges[N][numChanges];
    }
    //cout << "total optimised: " <<  numStrings << endl;
    return numStrings;
}

ModNum solveOptimised(const string& binaryString, int N, int K)
{
    const auto totalStringsMadeWithChanges = computeNumStringsWithUpToKChanges(N, K);

    vector<vector<int>> factorsOf(N + 1);
    for (int factor = 1; factor <= N; factor++)
    {
        int mutiplied = 1 * factor;
        while (mutiplied <= N)
        {
            factorsOf[mutiplied].push_back(factor);
            mutiplied += factor;
        }
    }

    vector<int> blockSizes;
    for (int i = 1; i < N; i++)
    {
        if ((N % i) == 0)
        {
            blockSizes.push_back(i);
        }
    }

    vector<ModNum> periodicStringsMadeBy(N + 1);

    for (const auto blockSize : blockSizes)
    {
        const auto numBlocks = N / blockSize;

        vector<ModNum> periodicLastWithNumChanges(K + 1, 0);

        for (int posInBlock = 0; posInBlock < blockSize; posInBlock++)
        {
            vector<ModNum> nextPeriodicLastWithNumChanges(K + 1, 0);
            auto numChangesIfDontChange = 0;
            auto numChangesIfChange = 1; // At least one change if we change the digit at this posInBlock!

            auto posInString = posInBlock + blockSize;
            while (posInString < N)
            {
                if (binaryString[posInString] == binaryString[posInBlock])
                {
                    numChangesIfChange++;
                }
                else
                {
                    numChangesIfDontChange++;
                }
                posInString += blockSize;
            }

            if (posInBlock == 0)
            {
                if (numChangesIfDontChange <= K)
                    nextPeriodicLastWithNumChanges[numChangesIfDontChange]++;
                if (numChangesIfChange <= K)
                    nextPeriodicLastWithNumChanges[numChangesIfChange]++;
            }
            else
            {
                for (int numChanges = 0; numChanges <= K; numChanges++)
                {
                    if (numChanges - numChangesIfDontChange >= 0)
                        nextPeriodicLastWithNumChanges[numChanges] += periodicLastWithNumChanges[numChanges - numChangesIfDontChange];
                    if (numChanges - numChangesIfChange >= 0)
                        nextPeriodicLastWithNumChanges[numChanges] += periodicLastWithNumChanges[numChanges - numChangesIfChange];
                }
            }
            periodicLastWithNumChanges = nextPeriodicLastWithNumChanges;

        }
        ModNum periodicStringsMadeWithBlocksize = 0;
        for (int numChanges = 0; numChanges <= K; numChanges++)
        {
            periodicStringsMadeWithBlocksize += periodicLastWithNumChanges[numChanges];
        }

        periodicStringsMadeBy[blockSize] = periodicStringsMadeWithBlocksize;
    }
    for (const auto blockSize : blockSizes)
    {
        assert(numWithPeriod[blockSize] == periodicStringsMadeBy[blockSize]);
    }

    vector<ModNum> F(periodicStringsMadeBy);
    for (const auto blockSize : blockSizes)
    {
        int factor = 2 * blockSize;
        while (factor <= N)
        {
            F[factor] -= F[blockSize];
            factor += blockSize;
        }
    }

    ModNum periodicStringsMade = 0;
    for (const auto blockSize : blockSizes)
    {
        periodicStringsMade += F[blockSize];
    }
    return totalStringsMadeWithChanges - periodicStringsMade;
}

int main(int argc, char* argv[])
{


    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int N = rand() % 15 + 1;
        const int K = rand() % (N + 1);

        string binaryString;
        for (int i = 0; i < N; i++)
        {
            binaryString += '0' + (rand() % 2);
        }
        cout << 1 << endl;
        cout << N << " " << K << endl;
        cout << binaryString << endl;

        return 0;

    }
    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        //cout << "t: " << t << endl;
        int N;
        cin >> N;
        int K;
        cin >> K;
        string binaryString;
        cin >> binaryString;

#ifdef BRUTE_FORCE
        const auto solutionBruteForce = solveBruteForce(binaryString, N, K);
#endif

        const auto solutionOptimised = solveOptimised(binaryString, N, K);
#ifdef BRUTE_FORCE
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
        cout << "solutionOptimised: " << solutionOptimised << endl;
#else
        cout << solutionOptimised << endl;
#endif
        
        assert(solutionOptimised == solutionBruteForce);
    }


}
