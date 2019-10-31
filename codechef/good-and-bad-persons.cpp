// Simon St James (ssjgz) - 2019-10-31
// 
// Solution to: https://www.codechef.com/problems/GOODBAD
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

string findPossibleSenders(int N, const string& s, int K)
{
    int numLowerCase = 0;
    int numUpperCase = 0;

    for (const auto letter : s)
    {
        if (islower(letter))
            numLowerCase++;
        else
            numUpperCase++;
    }

    const bool couldBeChef = numUpperCase <= K;
    const bool couldBeBrother = numLowerCase <= K;

    if (couldBeChef && couldBeBrother)
        return "both";
    if (couldBeChef)
        return "chef";;
    if (couldBeBrother)
        return "brother";
    
    return "none";
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();
        const int K = read<int>();
        const string s = read<string>();

        cout << findPossibleSenders(N, s, K) << endl;
    }

    assert(cin);
}
