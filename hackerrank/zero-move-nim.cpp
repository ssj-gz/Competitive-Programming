// Simon St James (ssjgz) - 2018-01-25
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;


int mex(const vector<int>& numbers)
{
    if (numbers.empty())
        return 0;
    auto numbersSorted = numbers;
    sort(numbersSorted.begin(), numbersSorted.end());

    int mex = 0;
    for (const auto number : numbersSorted)
    {
        if (number == mex)
            mex++;
        else if (number > mex)
            break;
    }

    return mex;
}


int findGrundyNumber(const int numInPile, bool haveZeroMove, vector<vector<int>>& grundyLookup)
{
    if (grundyLookup[haveZeroMove][numInPile] != -1)
        return grundyLookup[haveZeroMove][numInPile];

    vector<int> moveGrundies;

    if (haveZeroMove)
    {
        // Blow zero-move; the result is a "normal" game of Nim with numInPile in the pile, 
        // which has grundy number numInPile.
        moveGrundies.push_back(findGrundyNumber(numInPile, false, grundyLookup));
    }

    // Take some stones.
    for (int stonesToTake = 1; stonesToTake <= numInPile; stonesToTake++)
    {
        const int numRemaining = numInPile - stonesToTake;
        moveGrundies.push_back(findGrundyNumber(numRemaining, haveZeroMove, grundyLookup));
    }

    const auto grundyNumber = mex(moveGrundies);

    grundyLookup[haveZeroMove][numInPile] = grundyNumber;

    return grundyNumber;
}

int main()
{
    cout << ((51 ^ 14) ^ 35) << endl;
    int T;
    cin >> T;

    const vector<int> cycle = { 1, 0, 3, 2, 5, 4, 7, 6, 9, 8 };

    for (int t = 0; t < T; t++)
    {
        int n;
        cin >> n;

        vector<int> numInPile(n);

        int maxInPile = 0;
        for (int i = 0; i < n; i++)
        {
            cin >> numInPile[i];
            maxInPile = max(maxInPile, numInPile[i]);
        }

        vector<vector<int>> grundyLookup(2, vector<int>(maxInPile + 1, -1));

        int xorSum = 0;
        for (int i = 0; i < n; i++)
        {
            const auto grundyNumber = (numInPile[i] / 10) * 10 + cycle[numInPile[i] % 10];
            const auto grundyNumberDebug = findGrundyNumber(numInPile[i], true, grundyLookup);
            cout << " numInPile: " << numInPile[i] << " grundyNumber: " << grundyNumber << " grundyNumberDebug: " << grundyNumberDebug << endl;
            //assert(grundyNumber == findGrundyNumber(numInPile[i], false, grundyLookup));
            xorSum ^= grundyNumberDebug;
        }

        cout << "t: " << t << " n: " << n << " xorSum: " << xorSum << endl;

        if (xorSum == 0)
            cout << "L";
        else
            cout << "W";
        cout << endl;
    }


#if 0
    vector<int> grundyLookup(100000, -1);
    for (int i = 0; i < grundyLookup.size(); i++)
    {
        const auto grundyNumber = findGrundyNumber(i, grundyLookup);
        const auto floop = (i / 10) * 10 + cycle[i % 10];
        cout << "i: " << i << " grundyNumber: " << grundyNumber << " floop: " << floop << endl;
        assert(grundyNumber == floop);
        //cout << " grundyNumber: " << grundyNumber << endl;
    }
#endif
}

