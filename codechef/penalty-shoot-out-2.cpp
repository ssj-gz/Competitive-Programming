// Simon St James (ssjgz) - 2021-04-25
// 
// Solution to: https://www.codechef.com/problems/PSHOT/
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


int findFirstShotWhereResultIsKnown(int numShots, const string& shotResults)
{
    int maxTotalA = numShots;
    int maxTotalB = numShots;
    int minTotalA = 0;
    int minTotalB = 0;
    for (int shotIndex = 0; shotIndex < numShots * 2; shotIndex++)
    {
        const bool teamATookShot = ((shotIndex % 2) == 0);
        if (shotResults[shotIndex] == '1')
        {
            if (teamATookShot)
            {
                minTotalA++;
            }
            else
            {
                minTotalB++;
            }
        }
        else
        {
            if (teamATookShot)
            {
                maxTotalA--;
            }
            else
            {
                maxTotalB--;
            }
        }

        if ((minTotalA > maxTotalB) || (minTotalB > maxTotalA))
            return shotIndex + 1;
    }
    // Draw.
    return 2 * numShots;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);

    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int numShots = read<int>();
        const string shotResults = read<string>();

        cout << findFirstShotWhereResultIsKnown(numShots, shotResults) << endl;
    }

    assert(cin);
}
