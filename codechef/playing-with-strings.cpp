// Simon St James (ssjgz) - 2019-10-31
// 
// Solution to: https://www.codechef.com/problems/PLAYSTR
//
#include <iostream>
#include <algorithm>

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

bool canConvertByAdjacentSwaps(const string& sourceBinaryString, const string& targetBinaryString)
{
    // Can transform one into the other if and only if they have the same number of 0's and 1's
    // (or equivalently, since sourceBinaryString and targetBinaryString are the same length,
    // the same number of 0's).
    assert(sourceBinaryString.length() == targetBinaryString.length());
    return count(sourceBinaryString.begin(), sourceBinaryString.end(), '0') == 
           count(targetBinaryString.begin(), targetBinaryString.end(), '0');
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        read<int>(); // N (unused)

        const auto sourceBinaryString = read<string>();
        const auto targetBinaryString = read<string>();

        cout << (canConvertByAdjacentSwaps(sourceBinaryString, targetBinaryString) ? "YES" : "NO") << endl;
    }

    assert(cin);
}
