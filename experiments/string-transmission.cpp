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

void blah(string& alteredStringSoFar, int nextIndex, int K, const string& originalString, uint64_t& numFound)
{
    if (nextIndex == originalString.size())
    {
        if (numChanges(alteredStringSoFar, originalString) <= K && !isPeriodic(alteredStringSoFar))
        {
            numFound++;
        }
        return;
    }
    blah(alteredStringSoFar, nextIndex + 1, K, originalString, numFound);

    alteredStringSoFar[nextIndex] = '0' + ('1' - alteredStringSoFar[nextIndex]);
    blah(alteredStringSoFar, nextIndex + 1, K, originalString, numFound);
    alteredStringSoFar[nextIndex] = '0' + ('1' - alteredStringSoFar[nextIndex]);
} 

uint64_t solveBruteForce(const string& binaryString, int N, int K)
{
    string alteredStringSoFar = binaryString;
    uint64_t numFound = 0;
    blah(alteredStringSoFar, 0, K, binaryString, numFound);
    return numFound;
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
    }


}
