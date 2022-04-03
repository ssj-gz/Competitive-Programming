// Simon St James (ssjgz) - 2022-03-31
//
// Solution to: https://www.codechef.com/MARCH221D/problems/CHFCLASS
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

int main()
{
    ios::sync_with_stdio(false);

    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int numDaysInMonth = read<int>();
        const int numSaturdays = (numDaysInMonth + 1) / 7;
        cout << numSaturdays << endl;
    }

    assert(cin);
}

