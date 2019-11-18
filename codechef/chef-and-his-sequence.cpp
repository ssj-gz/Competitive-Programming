// Simon St James (ssjgz) - 2019-11-18
// 
// Solution to: https://www.codechef.com/problems/CHEFSQ
//
#include <iostream>
#include <vector>

#include <cassert>

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

bool containsAsSubsequence(const vector<int>& sequence, const vector<int>& candidateSubsequence)
{
    int numOfCandidateSubsequenceFound = 0;
    for (const auto sequenceValue : sequence)
    {
        if (sequenceValue == candidateSubsequence[numOfCandidateSubsequenceFound])
        {
            numOfCandidateSubsequenceFound++;
            if (numOfCandidateSubsequenceFound == candidateSubsequence.size())
                return true;
        }
    }
    
    return false;
}

int main(int argc, char* argv[])
{
    // Trivial - the only difficult part is that the Problem Statement is 
    // so sloppy that it's hard to tell exactly what it wants
    // (it basically wants to know whether what I've called "candidateSubsequence" occurs
    // as a sub*sequence* of sequence - not a sub*string*, as stated in the Problem Statement
    // Sigh).
    //
    // This is easily-solved with the greedy algorithm above.
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();
        vector<int> sequence(N);
        for (auto& value : sequence)
            value = read<int>();

        const int M = read<int>();
        vector<int> candidateSubsequence(M);
        for (auto& value : candidateSubsequence)
            value = read<int>();

        cout << (containsAsSubsequence(sequence, candidateSubsequence) ? "Yes" : "No") << endl;
    }

    assert(cin);
}
