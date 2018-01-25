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
    for (int numStonesToTake = 1; numStonesToTake <= 2; numStonesToTake++)
    {
        for (int positionToTakeFrom = 0; positionToTakeFrom + numStonesToTake <= numInPile; positionToTakeFrom++)
        {
            const int numInSplitPile1 = positionToTakeFrom;
            const int numInSplitPile2 = numInPile - numInSplitPile1 - numStonesToTake;
            grundyNumbersForMoves.push_back(findGrundyNumber(numInSplitPile1, grundyNumberForPileSizeLookup) ^ findGrundyNumber(numInSplitPile2, grundyNumberForPileSizeLookup));
        }
    }

    const auto grundyNumber = mex(grundyNumbersForMoves);

    grundyNumberForPileSizeLookup[numInPile] = grundyNumber;

    return grundyNumber;
}

int main()
{
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
        if (numInCurrentPile != 0)
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
