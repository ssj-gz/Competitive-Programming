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
    return numStrings;
}

ModNum findPossibleSourceMessages(const string& binaryString, int N, int K)
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

    vector<ModNum> numWithPeriod(N + 1);

    for (const auto blockSize : blockSizes)
    {
        const auto numBlocks = N / blockSize;

        vector<ModNum> periodicLastWithNumChanges(K + 1, 0);

        for (int posInBlock = 0; posInBlock < blockSize; posInBlock++)
        {
            vector<ModNum> nextPeriodicWithNumChanges(K + 1, 0);
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
                    nextPeriodicWithNumChanges[numChangesIfDontChange]++;
                if (numChangesIfChange <= K)
                    nextPeriodicWithNumChanges[numChangesIfChange]++;
            }
            else
            {
                for (int numChanges = 0; numChanges <= K; numChanges++)
                {
                    if (numChanges - numChangesIfDontChange >= 0)
                        nextPeriodicWithNumChanges[numChanges] += periodicLastWithNumChanges[numChanges - numChangesIfDontChange];
                    if (numChanges - numChangesIfChange >= 0)
                        nextPeriodicWithNumChanges[numChanges] += periodicLastWithNumChanges[numChanges - numChangesIfChange];
                }
            }
            periodicLastWithNumChanges = nextPeriodicWithNumChanges;

        }
        ModNum periodicStringsMadeWithBlocksize = 0;
        for (int numChanges = 0; numChanges <= K; numChanges++)
        {
            periodicStringsMadeWithBlocksize += periodicLastWithNumChanges[numChanges];
        }

        numWithPeriod[blockSize] = periodicStringsMadeWithBlocksize;
    }

    // Calculate numWithMinPeriod[blockSize] for each blockSize: this is the number
    // of strings of period blockSize which do *not* also have a period less than blockSize. 
    vector<ModNum> numWithMinPeriod(numWithPeriod);
    for (const auto blockSize : blockSizes)
    {
        int factor = 2 * blockSize;
        while (factor <= N)
        {
            // All periodics strings with period "factor" also give rise to numWithMinPeriod[blockSize]
            // strings with period blockSize - subtract.
            numWithMinPeriod[factor] -= numWithMinPeriod[blockSize];
            factor += blockSize;
        }
    }

    // Final tally - numWithMinPeriod is used instead of numWithPeriod as the latter overcounts
    // e.g. numWithPeriod[4] would also include numWithPeriod[2] and numWithPeriod[1],
    // whereas numWithMinPeriod[4] does not.
    ModNum periodicStringsMade = 0;
    for (const auto blockSize : blockSizes)
    {
        periodicStringsMade += numWithMinPeriod[blockSize];
    }
    return totalStringsMadeWithChanges - periodicStringsMade;
}

int main(int argc, char* argv[])
{
    // Fundamentally fairly easy, but the final hurdle - the bit that assembles the "numWithPeriod"
    // information into the final answer - was quite hard :)
    //
    // So: as is probably obvious, the easiest approach is to count the number of *periodic* strings
    // that can be formed by up to K changes and then subtract that from the number of *all* strings
    // (totalStringsMadeWithChanges) that can be formed by up to K changes to give the final result.
    //
    // If a string has a length N, then the only possible periods it can have are the proper factors
    // of N, so it makes sense to try, for each proper factor (blockSize, say) of N, to find the number of 
    // strings with period blockSize that can be formed from that string.
    //
    // So divide N into N / blockSize chunks e.g. for N = 12, blockSize = 4 (a proper factor of N):
    //
    //    1101 1111 1001
    //
    // A string s has period blockSize if and only if all of the following hold:
    //
    //    s[0] == s[blockSize] == s[blockSize * 2] ... == s[N - blockSize]
    //    s[1] == s[blockSize + 1] == s[blockSize * 2 + 1] ... == s[N - blockSize + 1]
    //     ...
    //    s[blockSize - 1] == s[blockSize + (blockSize - 1)] == s[blockSize * 2 + (blockSize - 1)] ... == s[N - blockSize + (blockSize - 1)]
    //
    // That is, for a string with period blockSize, all digits from blockSize up to N - 1 are *dictated* by the values 0 ... blockSize - 1.
    // So let's concentrate on these first blockSize digits.
    //
    // Let F[x][y] represent the number of ways of changing the first x elements (and then, accordingly, the elements 
    // s[blockSize], s[2 * blockSize] ... s[N - blockSize]; s[blockSize + 1], s[blockSize + 2] ... s[N - blockSize + 1]; ...
   //  s[blockSize + (x - 1)], s[2 * blockSize + (x - 1)] ... s[N - blockSize + (x - 1)] of s) so that
    //   
    //    s[0] == s[blockSize] == s[blockSize * 2] ... == s[N - blockSize]
    //    s[1] == s[blockSize + 1] == s[blockSize * 2 + 1] ... == s[N - blockSize + 1]
    //    ...
    //    s[x - 1] == s[blockSize + (x - 1)] == s[blockSize * 2 + (x - 1)] ... == s[N - blockSize + (x - 1)] (*)
    //
    // and so that the total number of changes made is precisely equal to y; then the number of ways of generating a string
    // with period blockSize by making up to K changes to s would be:
    //
    //   F[blockSize][0] + F[blockSize][1] ... F[blockSize][K].
    //
    // How do we calculate F[x][y]? Assume we've calculated F[x - 1][y] for all y <= K.
    // We need to change, or leave alone, s[x-1], and then change, or leave alone, 
    // the elements s[blockSize + (x - 1)], s[2 * blockSize + (x - 1)] ... s[N - blockSize + (x - 1)] so as to ensure that (*) holds
    //
    // If we leave s[x-1] unchanged, then we need to change all elements s[z * blockSize + (x - 1)] (z > 1)
    // such that s[z * blockSize + (x - 1)] != s[x-1].  This number of changes is easily computable, and is called numChangesIfDontChange
    // in the code.
    //
    // Similarly, if we do change s[x-1], then we need to change all elements s[z * blockSize + (x - 1)] (z > 1)
    // such that s[z * blockSize + (x - 1)] == s[x-1], and s[x-1] itself.  This number of changes is called numChangesIfChange in the code.
    //
    // Thus:
    //
    //   for y = 0, 1, ... K, F[x][y] = F[x-1][y - numChangesIfChange] + F[x-1][y - numChangesIfDontChange]
    //
    // So this is all easily computable.  Note that computing F[x][y] only requires F[x-1][y'], so we don't need a two dimensional array.
    // The one-dimensional version of F is called periodicLastWithNumChanges in the code.
    //
    // So we can compute F very easily, and from this we can find numWithPeriod[blockSize]; now we can just sum numWithPeriod[blockSize] for
    // each proper factor blockSize of N and get the result, yes?
    //
    // Not quite that simple, alas: the "period" of a string is not very well-defined.
    //
    // Consider the string s of length N = 8, below: 
    //
    //   1010 1010
    //
    // It's clearly periodic, with period 4.  But it's also periodic with period 2:
    //
    //   10 10 10 10
    //
    // and so naively summing the numWithPeriod[blockSize] for each factor blockSize would count this particular string multiple times: once
    // for blockSize = 4; again for blockSize = 2.
    //
    // An even worse example:
    //
    //   11111111
    //
    // This has periods 1, 2 and 4, so would be counted three times.
    //
    // This is a little tricky to deal with: the central insight is that if a string has period blockSize, then it has period blockSize * factor
    // for each factor such that blockSize * factor is still a factor of N.  For example, the 11111111 has period blockSize = 1, and also has 
    // period blockSize * 2 = 4 (factor == 2), and period blockSize * 4 = 4 (factor == 4).  Thus, let's create a new array, numWithMinPeriod:
    // numWithMinPeriod[x] will be the number of strings formable of period x, but not of period less than x.  It's hopefully easy to see that
    // we can form numWithMinPeriod fairly simply: first set numWithMinPeriod = numWithPeriod.  Then note that numWithMinPeriod[1] == numWithPeriod[1]
    // (can't have a period less than 1!).  Then, whenever we have definitely computed numWithMinPeriod[x], subtract numWithMinPeriod[x] from
    // all numWithMinPeriod[y] such that y is a multiple of x and a factor of N.  We have "definitely computed" numWithMinPeriod[x] when we have
    // definitely computed numWithMinPeriod[f] for each proper factor of f.

    // This essential filters out the overcounting once and only once for each string that would have been overcounted had we just naively tallied numWithPeriod,
    // so tallying numWithMinPeriod now gives the correct result.
    //
    // And that's it!
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

        const auto solution = findPossibleSourceMessages(binaryString, N, K);
#ifdef BRUTE_FORCE
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
        cout << "solutionOptimised: " << solution << endl;
#else
        cout << solution << endl;
#endif
        
        assert(solution == solutionBruteForce);
    }
}
