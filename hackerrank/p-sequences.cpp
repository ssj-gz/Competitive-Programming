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
    // Compute divisionChangesOfP.
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

    vector<ModNum> firstNEndingOnDivChangeIndex(divisionChangesOfP.size() + 1, 0);
    for (int i = 0; i < divisionChangesOfP.size(); i++)
    {
        // One P-Sequence of length 1.
        firstNEndingOnDivChangeIndex[i] = 1;
    }
    for (int i = 1; i < N; i++)
    {
        int divChangeIndex = divisionChangesOfP.size();
        int inverseChangeIndex = 0;

        vector<ModNum> nextFirstNEndingOnDivChangeIndex(divisionChangesOfP.size() + 1, 0);
        nextFirstNEndingOnDivChangeIndex[divChangeIndex] = firstNEndingOnDivChangeIndex[inverseChangeIndex];
        // Machinery for incrementally computing the sum of the number of P-Sequences for previous
        // (i-1) case, up to numOfPSequencesNeededInSum.
        ModNum sumOfPreviousPSequences = firstNEndingOnDivChangeIndex[inverseChangeIndex];
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
                    (divisionChangesOfP[inverseChangeIndex] - numPSequencesSummed - 1) * firstNEndingOnDivChangeIndex[inverseChangeIndex - 1]
                    // ... plus the new divisionChangesOfP[inverseChangeIndex].
                    + firstNEndingOnDivChangeIndex[inverseChangeIndex];
                numPSequencesSummed = divisionChangesOfP[inverseChangeIndex];
            }
            // Add the remaining divisionChangesOfP[inverseChangeIndex].
            sumOfPreviousPSequences += (numOfPSequencesNeededInSum - divisionChangesOfP[inverseChangeIndex]) * firstNEndingOnDivChangeIndex[inverseChangeIndex];
            numPSequencesSummed = numOfPSequencesNeededInSum;
            // At this point, sumOfPreviousPSequences is the sum of previous P-Sequences, of length one less than we are calculating (i.e. i-1), whose last element
            // is between 1 and numOfPSequencesNeededInSum.

            assert(divisionChangesOfP[inverseChangeIndex] * divisionChangesOfP[divChangeIndex] <= P);
            nextFirstNEndingOnDivChangeIndex[divChangeIndex] = sumOfPreviousPSequences;
        }
        firstNEndingOnDivChangeIndex = nextFirstNEndingOnDivChangeIndex;
    }

    // Unpack the results, again using the fact that the number of P-sequences ending with X is the same for X between two consecutive divisionChangesOfP.
    ModNum result = 0;
    for (int r = 0; r < divisionChangesOfP.size() - 1; r++)
    {
        result += firstNEndingOnDivChangeIndex[r] * (divisionChangesOfP[r + 1] - divisionChangesOfP[r]);
    }
    // Scoop up the remainder (from the last divisionChangesOfP up to P).
    const auto numRemainingUpToP = (P - divisionChangesOfP.back() + 1);
    result += firstNEndingOnDivChangeIndex[divisionChangesOfP.size() - 1]  * numRemainingUpToP;

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
    // Fundamentally pretty easy, but with some fiddliness that meant I spent ages on it XD
    //
    // So - consider a sequence  of numbers of length N.  This will be a P-Sequence if and
    // only if:
    //  
    //   1) The prefix of length N - 1 is a P-Sequence; and
    //   2) The last element of the prefix times the last element of the original sequence
    //      is less than or equal to P.
    //
    // Therefore, if firstNEndingOnP[N][d] represents the number of P-Sequences of length N
    // those last element is d, then:
    //
    //   firstNEndingOnP[N][d] = sum [1 <= r; r * d <= P] { firstNEndingOnP[N - 1][r] }. (*)
    //
    // Nice and simple, and this would be very easy to calculate except it would be O(N x P),
    // and N is ~1000 and P is ~1'000'000'000 :( In fact, of course, if we naively compute
    // the sum "from scratch" for each d, if would be O(N x P x P) (!), but it should
    // hopefully be obvious that if we start d at P and work downwards, the sum is easily
    // computed incrementally.
    //
    // However, if you compute these values for large-ish d, we see that, as d increases,
    // we get longer and longer runs where firstNEndingOnP[N][d] all give the same result.
    // This is because for large d, the set of r such that 1 <= r, r * d <= P is "often"
    // the same as the set of r such that 1 <= r, r * (d + 1) <= P, and plugging this into
    // (*) we see that if this is the case, then firstNEndingOnP[N][d] == firstNEndingOnP[N][d+1].
    //
    // A little more specifically, firstNEndingOnP[N][d] == firstNEndingOnP[N][d+1] whenever
    // P / d == P / (d + 1) (using integer division, of course!).  Let divisionChangesOfP be the 
    // list of values such that P / divisionChangesOfP[x] != P / (divisionChangesOfP[x] - 1) - I intuited,
    // but can't yet figure out how to prove, that divisionChangesOfP is precisely the union 
    // of the sets:
    //
    //    1 ... sqrt(P) unioned with
    //    P / (x + 1), for 2 <= x <= sqrt(P) + 1.
    //
    // So, the size of divisionChangesOfP is O(sqrt(P)), and we have the important property that
    //
    //   firstNEndingOnP[N][d1] == firstNEndingOnP[N][d2] for all d1 and d2 such that there is
    //      an x such that 1 <= divisionChangesOfP[x - 1] <= d1 <= divisionChangesOfP[x] and
    //                     1 <= divisionChangesOfP[x - 1] <= d2 <= divisionChangesOfP[x] and
    //
    // We can use the fact that, except at the O(sqrt(P)) values of divisionChangesOfP, the values
    // of firstNEndingOnP[N][d] for consecutive values of d are the same to drastically reduce
    // the cost of calculating (*) - assuming incremental calculation of the sum, sumOfPreviousPSequences -
    // to O(N x sqrt(P)), which is much more manageable :)
    //
    // And that's precisely what we do: we calculate firstNEndingOnP[N][d] only for d in divisionChangesOfP
    // (as the values for other d will be "the same" for all d since the last divisionChangesOfP).
    // O(P) would be too big a vector to allocate, so we compute a sparse version of firstNEndingOnP which
    // we call firstNEndingOnDivChangeIndex.  The fiddliness comes from keeping sumOfPreviousPSequences
    // incrementally up to date, but it's not too difficult.





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
