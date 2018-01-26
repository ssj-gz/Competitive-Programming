// Simon St James (ssjgz) - 2018-01-26
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

int mex(const vector<int64_t>& numbers)
{
    if (numbers.empty())
        return 0;
    auto numbersSorted = numbers;
    sort(numbersSorted.begin(), numbersSorted.end());

    int64_t mex = 0;
    for (const auto number : numbersSorted)
    {
        if (number == mex)
            mex++;
        else if (number > mex)
            break;
    }

    return mex;
}


int64_t xorPower(int64_t base, int64_t exponent)
{
    if ((exponent % 2) == 0)
        return 0;
    return base;
}

int64_t findGrundyNumber(const int64_t pileSize, const vector<int64_t>& s, map<int64_t, int64_t>& grundyForPileSizeLookup)
{
    if (grundyForPileSizeLookup.find(pileSize) != grundyForPileSizeLookup.end())
        return grundyForPileSizeLookup[pileSize];

    vector<int64_t> grundyNumbersForMoves;
    for (const auto numSplitPiles : s)
    {
        if ((pileSize % numSplitPiles) != 0)
            continue;
        const auto splitPileSize = pileSize / numSplitPiles;
        const auto grundyNumberForMove = xorPower(findGrundyNumber(splitPileSize, s, grundyForPileSizeLookup), numSplitPiles);

        grundyNumbersForMoves.push_back(grundyNumberForMove);
    }

    const auto grundyNumberForPileSize = mex(grundyNumbersForMoves);

    grundyForPileSizeLookup[pileSize] = grundyNumberForPileSize;

    return grundyNumberForPileSize;
}

int main()
{
    // Very basic Sprague-Grundy example: the only complication is the potentially very large a) number of stones in any pile
    // and b) number of piles - using a map for the former and the fact that x ^ x == 0 (see xorPower) for the latter
    // deals with this very easily.
    int64_t n, m;
    cin >> n >> m;

    vector<int64_t> s(m);

    for (int i = 0; i < m; i++)
    {
        cin >> s[i];
    }

    map<int64_t, int64_t> grundyForPileSizeLookup;
    const auto grundyNumber = findGrundyNumber(n, s, grundyForPileSizeLookup);

    if (grundyNumber == 0)
        cout << "Second";
    else
        cout << "First";
    cout << endl;
}
