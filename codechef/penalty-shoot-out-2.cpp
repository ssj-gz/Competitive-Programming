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
    int finalAIfWinRest = numShots;
    int finalBIfWinRest = numShots;
    int finalAIfLoseRest = 0;
    int finalBIfLoseRest = 0;
    int numShotsTaken = 0;
    bool isTeamATurn = true;
    for (const auto shotResult : shotResults)
    {
        if (shotResult == '1')
        {
            if (isTeamATurn)
            {
                finalAIfLoseRest++;
            }
            else
            {
                finalBIfLoseRest++;
            }
        }
        else
        {
            if (isTeamATurn)
            {
                finalAIfWinRest--;
            }
            else
            {
                finalBIfWinRest--;
            }
        }
        numShotsTaken++;
        isTeamATurn = !isTeamATurn;

        if ((finalAIfLoseRest > finalBIfWinRest) || (finalBIfLoseRest > finalAIfWinRest))
            break;

    }
    return numShotsTaken;
}

int main()
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
