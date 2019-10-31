// Simon St James (ssjgz) - 2019-10-31
// 
// Solution to: https://www.codechef.com/problems/GOODBAD
//
#include <iostream>

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

string findPossibleSenders(const string& message, int numFlipsAllowed)
{
    int numLowerCase = 0;
    int numUpperCase = 0;

    for (const auto letter : message)
    {
        if (islower(letter))
            numLowerCase++;
        else
            numUpperCase++;
    }

    const bool couldBeChef = (numUpperCase <= numFlipsAllowed);
    const bool couldBeBrother = (numLowerCase <= numFlipsAllowed);

    if (couldBeChef && couldBeBrother)
        return "both";
    else if (couldBeChef)
        return "chef";
    else if (couldBeBrother)
        return "brother";
    
    return "none";
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        read<int>(); // N (unused).
        const int numFlipsAllowed = read<int>();
        const string message = read<string>();

        cout << findPossibleSenders(message, numFlipsAllowed) << endl;
    }

    assert(cin);
}
