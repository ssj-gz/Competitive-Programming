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
uint64_t solveOptimised(const string& binaryString, int N, int K)
{
    uint64_t result = 0;
    vector<vector<uint64_t>> numOfLengthWithChanges(N + 1, vector<uint64_t>(K + 1, 0));
    numOfLengthWithChanges[1][0] = 1;
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
    uint64_t total = 0;
    for (int numChanges = 0; numChanges <= K; numChanges++)
    {
        total += numOfLengthWithChanges[N][numChanges];
    }
    cout << "total optimised: " <<  total << endl;
    cout << "GLoop: " << numOfLengthWithChanges[N][K] << endl;
    return result;
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
