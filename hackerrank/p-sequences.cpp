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


ModNum calcNumPSequences(int N, int P)
{
    vector<int> divisionChangesOfP;
    for (int i = 1; i <= sqrt(P); i++)
    {
        divisionChangesOfP.push_back(i);
    }
    for (int i = sqrt(P) + 1; i > 1; i--)
    {
        divisionChangesOfP.push_back(P / i + 1);
    }
    divisionChangesOfP.erase(std::unique(divisionChangesOfP.begin(), divisionChangesOfP.end()), divisionChangesOfP.end());
    assert(std::is_sorted(divisionChangesOfP.begin(), divisionChangesOfP.end()));
    vector<ModNum> firstNEndingOnFactorIndex(divisionChangesOfP.size() + 1, 0);
    for (int i = 0; i < divisionChangesOfP.size(); i++)
    {
        firstNEndingOnFactorIndex[i] = 1;
    }
    for (int i = 1; i < N; i++)
    {
        int divChangeIndex = divisionChangesOfP.size();
        int inverseChangeIndex = 0;

        vector<ModNum> nextFirstNEndingOnFactorIndex(divisionChangesOfP.size() + 1, 0);
        nextFirstNEndingOnFactorIndex[divChangeIndex] = firstNEndingOnFactorIndex[inverseChangeIndex];
        // Machinery for incrementally computing the sum of the number of P-Sequences for previous
        // (i-1) case, up to numOfPSequencesNeededInSum.
        ModNum sumOfPreviousPSequences = firstNEndingOnFactorIndex[inverseChangeIndex];
        int numPSequencesSummed = divisionChangesOfP[inverseChangeIndex];

        while (divChangeIndex >= 1)
        {
            divChangeIndex--;
            const auto numOfPSequencesNeededInSum = P / divisionChangesOfP[divChangeIndex];
            while (inverseChangeIndex + 1 < divisionChangesOfP.size() && divisionChangesOfP[inverseChangeIndex + 1] <= numOfPSequencesNeededInSum)
            {
                assert(inverseChangeIndex + 1 < divisionChangesOfP.size());
                inverseChangeIndex++;
                sumOfPreviousPSequences += 
                    // The remainder of divisionChangesOfP[inverseChangeIndex - 1], using the fact that the number of P-sequences ending with X 
                    // is the same for X between two consecutive divisionChangesOfP ...
                    (divisionChangesOfP[inverseChangeIndex] - numPSequencesSummed - 1) * firstNEndingOnFactorIndex[inverseChangeIndex - 1]
                    // ... plus the new divisionChangesOfP[inverseChangeIndex].
                    + firstNEndingOnFactorIndex[inverseChangeIndex];
                numPSequencesSummed = divisionChangesOfP[inverseChangeIndex];
            }
            // Add the remaining divisionChangesOfP[inverseChangeIndex].
            sumOfPreviousPSequences += (numOfPSequencesNeededInSum - divisionChangesOfP[inverseChangeIndex]) * firstNEndingOnFactorIndex[inverseChangeIndex];
            numPSequencesSummed = numOfPSequencesNeededInSum;
            // At this point, sumOfPreviousPSequences is the sum of previous P-Sequences, of length one less than we are calculating (i.e. i-1), whose last element
            // is between 1 and numOfPSequencesNeededInSum.

            assert(divisionChangesOfP[inverseChangeIndex] * divisionChangesOfP[divChangeIndex] <= P);
            nextFirstNEndingOnFactorIndex[divChangeIndex] = sumOfPreviousPSequences;
        }
        firstNEndingOnFactorIndex = nextFirstNEndingOnFactorIndex;
    }

    // Unpack the results, again using the fact that the number of P-sequences ending with X is the same for X between two consecutive divisionChangesOfP.
    ModNum result = 0;
    for (int r = 0; r < divisionChangesOfP.size() - 1; r++)
    {
        result += firstNEndingOnFactorIndex[r] * (divisionChangesOfP[r + 1] - divisionChangesOfP[r]);
    }
    // Scoop up the remainder (from the last divisionChangesOfP up to P).
    const auto numRemainingUpToP = (P - divisionChangesOfP.back() + 1);
    result += firstNEndingOnFactorIndex[divisionChangesOfP.size() - 1]  * numRemainingUpToP;

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

    const auto optimisedResult = calcNumPSequences(N, P);

#ifdef BRUTE_FORCE
    cout << "optimisedResult: " << optimisedResult << endl;
    assert(optimisedResult == bruteForceResult);
#else
    cout << optimisedResult << endl;
#endif

}
