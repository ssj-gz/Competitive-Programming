// Simon St James (ssjgz) 2018-02-04
#include <iostream>
#include <vector>

using namespace std;

int main()
{
    // Very easy one; just do an initial sweep through the array, making note, for each value,
    // of the highest value seen so far (excluding current value) and the index of that
    // value, and store in highestElementToLeft.
    // Then it's trivial to find the maximum value remaining in the array and its index,
    // and trivial to use this to simulate the game, all in O(N).
    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        int n;        
        cin >> n;

        vector<int> A(n);
        for (int i = 0; i < n; i++)
        {
            cin >> A[i];
        }

        struct HighestElement
        {
            int value = -1;
            int index = -1;
        };

        vector<HighestElement> highestElementToLeft(n);
        HighestElement currentHighestElement = {-1, -1};
        for (int i = 0; i < n; i++)
        {
            highestElementToLeft[i] = currentHighestElement;
            if (A[i] > currentHighestElement.value)
            {
                currentHighestElement.value = A[i];
                currentHighestElement.index = i;
            }
        }

        // Simulate the game.
        int lastRemainingIndex = n - 1;
        bool currentPlayerIsBob = true;
        while (lastRemainingIndex >= 0)
        {
            currentPlayerIsBob = !currentPlayerIsBob;
            const int indexToRemove = highestElementToLeft[lastRemainingIndex].index;
            lastRemainingIndex = indexToRemove - 1;
        }

        // The current player could not make a move.
        if (currentPlayerIsBob)
        {
            cout << "ANDY";
        }
        else
            cout << "BOB";

        cout << endl;
    }
}
