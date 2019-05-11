#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

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
bool isPeriodic(const string& a)
{
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
            return true;
    }
    return false;
}

void blah(string& alteredStringSoFar, int nextIndex, int K, const string& originalString, uint64_t& numNonPeriodicFound, uint64_t& totalFound)
{
    if (nextIndex == originalString.size())
    {
        if (numChanges(alteredStringSoFar, originalString) <= K && !isPeriodic(alteredStringSoFar))
        {
            numNonPeriodicFound++;
        }
        if (numChanges(alteredStringSoFar, originalString) <= K)
            totalFound++;
        return;
    }
    blah(alteredStringSoFar, nextIndex + 1, K, originalString, numNonPeriodicFound, totalFound);

    alteredStringSoFar[nextIndex] = '0' + ('1' - alteredStringSoFar[nextIndex]);
    blah(alteredStringSoFar, nextIndex + 1, K, originalString, numNonPeriodicFound, totalFound);
    alteredStringSoFar[nextIndex] = '0' + ('1' - alteredStringSoFar[nextIndex]);
} 

uint64_t solveBruteForce(const string& binaryString, int N, int K)
{
    string alteredStringSoFar = binaryString;
    uint64_t numNonPeriodicFound = 0;
    uint64_t totalFound = 0;
    blah(alteredStringSoFar, 0, K, binaryString, numNonPeriodicFound, totalFound);
    cout << "totalFound: " << totalFound << endl;
    return numNonPeriodicFound;
}

uint64_t computeNumStringsWithUpToKChanges(int N, int K)
{
    vector<vector<uint64_t>> numOfLengthWithChanges(N + 1, vector<uint64_t>(K + 1, 0));
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
    uint64_t numStrings = 0;
    for (int numChanges = 0; numChanges <= K; numChanges++)
    {
        numStrings += numOfLengthWithChanges[N][numChanges];
    }
    cout << "total optimised: " <<  numStrings << endl;
    return numStrings;
}

uint64_t solveOptimised(const string& binaryString, int N, int K)
{
    const auto totalStringsMadeWithChanges = computeNumStringsWithUpToKChanges(N, K);
    uint64_t nonPeriodicStringsMade = 0;
    vector<bool> isPrime(N + 1, true);

    isPrime[1] = false;
    for (int factor = 2; factor <= N; factor++)
    {
        int mutiplied = 2 * factor;
        while (mutiplied <= N)
        {
            isPrime[mutiplied] = false;
            mutiplied += factor;
        }
    }

    vector<int> primesUpToN;
    for (int i = 1; i <= N; i++)
    {
        if (isPrime[i])
        {
            primesUpToN.push_back(i);
        }
    }

    for (const auto prime : primesUpToN)
    {
        if ((N % prime) != 0)
            continue;

        const auto blockSize = prime;
        const auto numBlocks = N / blockSize;

        vector<int64_t> nonPeriodicLastWithNumChanges(K + 1, 0);

        for (int posInBlock = 0; posInBlock < blockSize; posInBlock++)
        {
            vector<int64_t> nextNonPeriodicLastWithNumChanges(K + 1, 0);
            auto numChangesIfDontChange = 0;
            auto numChangesIfChange = 1;

            auto posInString = posInBlock + blockSize;
            while (posInString < N)
            {
                cout << "posInString: " << posInString << " N: " << N << " posInBlock: " << posInBlock << endl;
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

            cout << "blockSize: " << blockSize << " posInBlock: " << posInBlock << " numChangesIfChange: " << numChangesIfChange << " numChangesIfDontChange: " << numChangesIfDontChange << endl;
            if (posInBlock == 0)
            {
                if (numChangesIfDontChange <= K)
                    nextNonPeriodicLastWithNumChanges[numChangesIfDontChange] = 1;
                if (numChangesIfChange <= K)
                    nextNonPeriodicLastWithNumChanges[numChangesIfChange] = 1;
            }
            else
            {
                for (int numChanges = 0; numChanges <= K; numChanges++)
                {
                    if (numChanges - numChangesIfDontChange >= 0)
                        nextNonPeriodicLastWithNumChanges[numChanges] = nonPeriodicLastWithNumChanges[numChanges - numChangesIfDontChange];
                    if (numChanges - numChangesIfChange >= 0)
                        nextNonPeriodicLastWithNumChanges[numChanges] = nonPeriodicLastWithNumChanges[numChanges - numChangesIfChange];
                }
            }
            nonPeriodicLastWithNumChanges = nextNonPeriodicLastWithNumChanges;

        }
        for (int numChanges = 0; numChanges <= K; numChanges++)
        {
            nonPeriodicStringsMade += nonPeriodicLastWithNumChanges[numChanges];
        }

    }

    cout << "totalStringsMadeWithChanges: " << totalStringsMadeWithChanges << " nonPeriodicStringsMade: " << nonPeriodicStringsMade << endl;
    return totalStringsMadeWithChanges - nonPeriodicStringsMade;
}

int main(int argc, char* argv[])
{
    cout << "flipple: " << isPeriodic("011010011") << endl;
    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        int N;
        cin >> N;
        int K;
        cin >> K;
        string binaryString;
        cin >> binaryString;

        const auto solutionBruteForce = solveBruteForce(binaryString, N, K);
        cout << "solutionBruteForce: " << solutionBruteForce << endl;

        const auto solutionOptimised = solveOptimised(binaryString, N, K);
        cout << "solutionOptimised: " << solutionOptimised << endl;
    }


}
