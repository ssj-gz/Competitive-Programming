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
    int finalAIfAWinRest = numShots;
    int finalBIfBWinRest = numShots;
    int finalAIfALoseRest = 0;
    int finalBIfBLoseRest = 0;
    int numShotsTaken = 0;
    bool isTeamATurn = true;
    for (const auto shotResult : shotResults)
    {
        if (shotResult == '1')
        {
            if (isTeamATurn)
            {
                finalAIfALoseRest++;
            }
            else
            {
                finalBIfBLoseRest++;
            }
        }
        else
        {
            if (isTeamATurn)
            {
                finalAIfAWinRest--;
            }
            else
            {
                finalBIfBWinRest--;
            }
        }
        numShotsTaken++;
        isTeamATurn = !isTeamATurn;

#ifdef DIAGNOSTICS
        cout << "numShotsTaken: " << numShotsTaken << "; " << (numShots * 2 - numShotsTaken) << " remaining.  " << endl;
        cout << " finalAIfALoseRest: " << finalAIfALoseRest << " finalBIfBWinRest: " << finalBIfBWinRest;
        if (finalAIfALoseRest <= finalBIfBWinRest)
            cout << " ... no conclusion can be drawn";
        else
            cout << " ... A guaranteed to win, even if A loses all remaining games!";
        cout << endl;
        cout << " finalBIfBLoseRest: " << finalBIfBLoseRest << " finalAIfAWinRest: " << finalAIfAWinRest;
        if (finalBIfBLoseRest <= finalAIfAWinRest)
            cout << " ... no conclusion can be drawn";
        else
            cout << " ... B guaranteed to win, even if B loses all remaining games!";
        cout << endl;
#endif

        if ((finalAIfALoseRest > finalBIfBWinRest) || (finalBIfBLoseRest > finalAIfAWinRest))
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
