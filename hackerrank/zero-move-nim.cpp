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

    if (haveZeroMove && numInPile > 0)
    {
        // Use up the zero-move for this pile.
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
    // Interesting one :) It clearly reduces to finding the grundy number of a pile
    // with a given number of stones, which is very easy (see findGrundyNumber(...),
    // above), though note that this won't work as a full solution due to the large pile-sizes involved.  
    // From there, after generating the grundy numbers for the first 100
    // possible pile sizes, it's easy to pattern-match our way to the solution - we 
    // see that the rightmost digit cycles through the values in "cycle", below, while
    // the other digits are roughly the same as the number of stones in the pile, with a 
    // few tweaks.
    //
    // Not very satisfying, and hard to Logic our way to, but oh well :)
    //
    // UPDATE: The Editorial is simpler (doh), but still gives no logic behind it - I guess the
    // submitter pattern-matched his way to a solution, too :)
    int T;
    cin >> T;

    const vector<int> cycle = { 2, 1, 4, 3, 6, 5, 8, 7, 0, 9 };

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

        int xorSum = 0;
        for (int i = 0; i < n; i++)
        {
            int grundyNumber = ((numInPile[i] - 1) / 10) * 10 + (cycle[(numInPile[i] - 1) % 10]);
            if (numInPile[i] % 10 == 9)
            {
                grundyNumber += 10;
            }
            xorSum ^= grundyNumber;
        }

        if (xorSum == 0)
            cout << "L";
        else
            cout << "W";
        cout << endl;
    }


#if 0
    // Used for exploring the patterns in the grundy numbers.
    vector<vector<int>> grundyLookup(2, vector<int>(10000, -1));
    for (int i = 1; i < grundyLookup[0].size(); i++)
    {
        const auto grundyNumber = findGrundyNumber(i, true, grundyLookup);
        int floop = ((i - 1) / 10) * 10 + (cycle[(i - 1) % 10]);
        if (i % 10 == 9)
        {
            floop += 10;
        }
        cout << "i: " << i << " grundyNumber: " << grundyNumber << " floop: " << floop << endl;
        assert(grundyNumber == floop);
    }
#endif
}

