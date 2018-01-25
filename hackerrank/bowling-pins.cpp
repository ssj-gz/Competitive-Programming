// Simon St James (ssjgz) 2018-01-25
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

int findGrundyNumber(const int numInPile, vector<int>& grundyNumberForPileSizeLookup)
{
    if (grundyNumberForPileSizeLookup[numInPile] != -1)
        return grundyNumberForPileSizeLookup[numInPile];

    vector<int> grundyNumbersForMoves;
    for (auto numStonesToTake = 1; numStonesToTake <= 2; numStonesToTake++)
    {
        for (auto positionToTakeFrom = 0; positionToTakeFrom + numStonesToTake <= numInPile; positionToTakeFrom++)
        {
            const auto numInSplitPile1 = positionToTakeFrom;
            const auto numInSplitPile2 = numInPile - numInSplitPile1 - numStonesToTake;
            const auto splitPile1GrundyNumber = findGrundyNumber(numInSplitPile1, grundyNumberForPileSizeLookup);
            const auto splitPile2GrundyNumber = findGrundyNumber(numInSplitPile2, grundyNumberForPileSizeLookup);
            // NB: this is still correct even if one or both of the piles is empty, as an empty pile has Grundy number 0,
            // and x xor 0 == x for all x.
            grundyNumbersForMoves.push_back(splitPile1GrundyNumber ^ splitPile2GrundyNumber);
        }
    }

    const auto grundyNumber = mex(grundyNumbersForMoves);

    grundyNumberForPileSizeLookup[numInPile] = grundyNumber;

    return grundyNumber;
}

int main()
{
    // A fairly easy one.  Firstly, let's translate this into a problem involving piles of stones:
    // a run of s standing bowling pins represents a pile of s stones e.g.
    //
    //  XXIIIXXXXIXXXXIIII
    //
    // corresponds to 3 piles of size 3, 1 and 4 stones respectively.
    //
    // The bowler may hit one pin or two adjacent pins (he gets to choice which) - thus, hitting x pins
    // corresponds to taking x stones from some location in some pile P, which will split the pile
    // into two piles (at least one of which can be empty) whose combined number of stones is equal to
    // the number of stones in the original P, minus x.  The x stones can be removed from any location in
    // the pile.  Thus, it's easy to enumerate all possible moves we can make on a pile of stones and then,
    // use the Sprague-Grundy Theorem to (recursively) find the Grundy number for any pile size.  It 
    // is then a simple matter, again by Sprague-Grundy, to find out who the winner is.
    int T;
    cin >> T;

    constexpr auto maxN = 300;
    vector<int> grundyNumberForPileSizeLookup(maxN + 1, -1);

    for (int t = 0; t < T; t++)
    {
        int n;
        cin >> n;

        string s;
        cin >> s;

        assert(s.size() == n);

        vector<int> numInEachPile;
        int numInCurrentPile = 0;
        // NB: note throughout here that adding a pile with 0 stones in it to numInEachPile makes
        // no difference to the result, as x xor 0 == x for any x.
        for (const auto letter : s)
        {
            if (letter == 'I')
                numInCurrentPile++;
            else if (letter == 'X')
            {
                numInEachPile.push_back(numInCurrentPile);
                numInCurrentPile = 0;
            }
            else
                assert(false);
        }
        numInEachPile.push_back(numInCurrentPile);

        int xorSum = 0;
        for (const auto numInPile : numInEachPile)
        {
            xorSum ^= findGrundyNumber(numInPile, grundyNumberForPileSizeLookup);
        }

        if (xorSum != 0)
            cout << "WIN";
        else
            cout << "LOSE";
        cout << endl;
    }
}
