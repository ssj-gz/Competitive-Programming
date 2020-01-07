// Simon St James (ssjgz) - 2019-08-22
#include <iostream>
#include <vector>
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

//#define DIAGNOSTICS

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);

    const int numHouses = 100;
    const int T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int M = read<int>();
        const int x = read<int>();
        const int y = read<int>();

        vector<int> copLocations(M);
        for (auto& copLocation : copLocations)
            copLocation = read<int>() - 1;

        const int houseSearchSpan = x * y;

        vector<bool> isHouseSafe(numHouses, true);

        for (const auto& copLocation : copLocations)
        {
            for (int i = copLocation - houseSearchSpan; i <= copLocation + houseSearchSpan; i++)
            {
                if (0 <= i && i < isHouseSafe.size())
                    isHouseSafe[i] = false;
            }
        }

        int numSafeHouses = 0;
        for (const auto& safe : isHouseSafe)
        {
            if (safe)
                numSafeHouses++;
        }
#ifdef DIAGNOSTICS
        for (const auto isSafe : isHouseSafe)
        {
            cout << (isSafe ? "." : "X");
        }
        cout << endl;
#endif

        cout << numSafeHouses << endl;
    }

    assert(cin);
}


