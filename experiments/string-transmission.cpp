#include <iostream>
#include <vector>
#include <cassert>

#include <sys/time.h>

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

int period(const string& a)
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
            return length;
    }
    return -1;
}

bool isPeriodic(const string& a)
{
    return period(a) != -1;
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
        {
            totalFound++;
            if (isPeriodic(alteredStringSoFar))
            {
                cout << "Made periodic string " << alteredStringSoFar << "(period " << period(alteredStringSoFar) << ") with " << numChanges(alteredStringSoFar, originalString) << " changes" << endl;
            }
        }
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

    for (int i = 1; i <= N; i++)
    {
        cout << "Factors of " << i << ": " << endl;
        for (const auto x : factorsOf[i])
        {
            cout  << " " << x << endl;
        }
    }

    vector<uint64_t> periodicStringsMadeBy(N + 1);

    //cout << "blockSizes.size(): " << blockSizes.size() << endl;
    //if (std::find(blockSizes.begin(), blockSizes.end(), 1) == blockSizes.end())
        //blockSizes.insert(blockSizes.begin(), 1);

    //int numPeriodicWithBlocksizeOne = 0;

    for (const auto blockSize : blockSizes)
    {
        if ((N % blockSize) != 0)
            continue;

        cout << "blockSize: " << blockSize << endl;
        const auto numBlocks = N / blockSize;

        vector<int64_t> periodicLastWithNumChanges(K + 1, 0);

        for (int posInBlock = 0; posInBlock < blockSize; posInBlock++)
        {
            vector<int64_t> nextPeriodicLastWithNumChanges(K + 1, 0);
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
            cout << " after posInBlock: " << posInBlock << " periodicLastWithNumChanges: " << endl;
            for (int numChanges = 0; numChanges <= K; numChanges++)
            {
                cout << " periodicLastWithNumChanges[" << numChanges << " ]: " << periodicLastWithNumChanges[numChanges] << endl;
            }

        }
        uint64_t periodicStringsMadeWithBlocksize = 0;
        for (int numChanges = 0; numChanges <= K; numChanges++)
        {
            periodicStringsMadeWithBlocksize += periodicLastWithNumChanges[numChanges];
        }
        cout << " periodicStringsMadeWithBlocksize: " << blockSize << " = "  << periodicStringsMadeWithBlocksize << endl;
        //periodicStringsMade += periodicStringsMadeWithBlocksize;

        //if (blockSize == 1)
            ////numPeriodicWithBlocksizeOne = periodicStringsMade;
        //else
            //periodicStringsMade -= numPeriodicWithBlocksizeOne;

        //cout << " periodicStringsMade: " << periodicStringsMade << endl;

        //cout << "numPeriodicWithBlocksizeOne: " << numPeriodicWithBlocksizeOne << endl;

        periodicStringsMadeBy[blockSize] = periodicStringsMadeWithBlocksize;
    }

    uint64_t periodicStringsMade = 0;
    for (const auto blockSize : blockSizes)
    {
        periodicStringsMade += periodicStringsMadeBy[blockSize];
        for (const auto factorOfBlockSize : factorsOf[blockSize])
        {
            if (factorOfBlockSize != blockSize)
            {
                periodicStringsMade -= periodicStringsMadeBy[factorOfBlockSize];
                cout << "subtracted  " << periodicStringsMadeBy[factorOfBlockSize] << " from periodicStringsMade" << endl;
            }
        }
    }
    cout << "totalStringsMadeWithChanges: " << totalStringsMadeWithChanges << " periodicStringsMade: " << periodicStringsMade << endl;
    return totalStringsMadeWithChanges - periodicStringsMade;
}

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int N = rand() % 15 + 2;
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
    cout << "flipple: " << isPeriodic("011010011") << endl;
    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        cout << "t: " << t << endl;
        int N;
        cin >> N;
        int K;
        cin >> K;
        string binaryString;
        cin >> binaryString;

        const auto solutionBruteForce = solveBruteForce(binaryString, N, K);

        const auto solutionOptimised = solveOptimised(binaryString, N, K);
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
        cout << "solutionOptimised: " << solutionOptimised << endl;
        
        assert(solutionOptimised == solutionBruteForce);
    }


}
