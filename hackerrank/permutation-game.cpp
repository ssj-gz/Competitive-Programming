// Simon St James (ssjgz) - 2017-12-03
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

bool isWinnerWithRemainingBitset(int remainingNumberBitset, const vector<int>& a, vector<char>& currentPlayerWinsForRemainingBitsetLookup)
{
    assert(remainingNumberBitset != 0);
    if (currentPlayerWinsForRemainingBitsetLookup[remainingNumberBitset] != -1)
        return currentPlayerWinsForRemainingBitsetLookup[remainingNumberBitset];

    auto isWin = false;

    const auto n = a.size();

    // Check if inOrder.
    const auto maxPowerOf2ForN = (1 << (n - 1));
    auto previousRemainingArrayElement = -1;
    auto inOrder = true;
    auto powerOf2 = maxPowerOf2ForN;
    for (auto i = 0; i < n; i++)
    {
        if ((remainingNumberBitset & powerOf2) != 0)
        {
            if (previousRemainingArrayElement != -1 && a[i] < previousRemainingArrayElement)
            {
                inOrder = false;
                break;
            }
            previousRemainingArrayElement = a[i];

        }
        powerOf2 >>= 1;
    }

    if (!inOrder)
    {
        // Perform each "move", where a move is removing an array element that still remains.
        powerOf2 = maxPowerOf2ForN;
        for (auto i = 0; i < n; i++)
        {
            if ((remainingNumberBitset & powerOf2) != 0)
            {
                const auto nextRemainingNumberBitset = remainingNumberBitset - (remainingNumberBitset & powerOf2);
                if (!isWinnerWithRemainingBitset(nextRemainingNumberBitset, a, currentPlayerWinsForRemainingBitsetLookup))
                {
                    // This move forces the other player to lose - a Win for us!
                    isWin =  true;
                }
            }
            powerOf2 >>= 1;
        }
    }

    currentPlayerWinsForRemainingBitsetLookup[remainingNumberBitset] = isWin;
    return isWin;
}


int main(int argc, char** argv)
{
    // Very easy one - just a brute force exploration with memo-isation using the classic "Can I Make A Move That Guarantees The Other Player Loses" algorithm.
    // Any state of the array can be represented by an n-bit number representing
    // the (original) indices of the array elements that have not yet been removed.
    int T;
    cin >> T;

    for (auto t = 0; t < T; t++)
    {
        int n;
        cin >> n;

        vector<int> a(n);

        vector<char> currentPlayerWinsForRemainingBitsetLookup(1 << n, -1);
        for (auto i = 0; i < n; i++)
        {
            cin >> a[i];
        }

        const auto allRemainingBitset = (1 << n) - 1;
        const auto aliceWins = isWinnerWithRemainingBitset(allRemainingBitset, a, currentPlayerWinsForRemainingBitsetLookup);
        if (aliceWins)
            cout << "Alice";
        else
            cout << "Bob";
        cout << endl;
    }
}

