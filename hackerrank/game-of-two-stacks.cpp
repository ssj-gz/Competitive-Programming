// Simon St James (ssjgz) - 2019-07-06
#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>

#include <cassert>

using namespace std;

int main()
{
    // Trivially easy.  Note first of all that the order we choose elements from
    // A and B is irrelevant: only the number we chose from A and the number
    // we choose from B affect the score and the number of moves.
    // 
    // We then use a two-pointer-esque kind of technique, where we initially 
    // choose all of B, then winnow it back until the sum is <= limit.
    //
    // Then choose the next element of A, then, again, winnow the number
    // chosen from B down (if necessary) until the sum is <= limit again.
    //
    // Since there are no negative elements, the number we choose from B
    // is non-increasing as the number we choose from A increases, so
    // this "winnowing", across all elements of A, contributes no more than
    // O(m) to the run-time (this would not be the case if there were negative
    // elements, though - that would be much harder!).
    //
    // There is a bit of hackage where we prepend a "0" to the beginning of A:
    // this just makes the case where we pick no elements from A slightly less clunky,
    // code-wise.
    //
    // And that's basically it!
    //
    // Slightly surprised by the low completion rate for this one (39.66% at the time
    // of writing).

    auto readInt = [](){ int x; cin >> x; assert(cin); return x; };
    const int numGames = readInt();;

    for (int g = 0; g < numGames; g++)
    {
        const int n = readInt();
        const int m = readInt();
        const int limit = readInt();

        vector<int> a(n);
        for (int i = 0; i < n; i++)
        {
            cin >> a[i];
        }
        vector<int> b(m);
        for (int i = 0; i < m; i++)
        {
            cin >> b[i];
        }

        // Take every element in B, initially.
        int64_t sumTakenFromB = 0;
        stack<int> takenFromBStack;
        for (const auto bElement : b)
        {
            sumTakenFromB += bElement;
            takenFromBStack.push(bElement);
        }


        // A bit of a hack to make the case where we choose no elements
        // from A a little less clunky.
        a.insert(a.begin(), 0);
        int64_t sumTakenFromA = 0;
        int numTakenFromA = 0;

        int largestNumMoves = 0;
        for (const auto aElement : a)
        {
            sumTakenFromA += aElement;
            while (!takenFromBStack.empty() && (sumTakenFromA + sumTakenFromB > limit))
            {
                // Winnow the amount taken from B down so we are below the limit.
                sumTakenFromB -= takenFromBStack.top();
                takenFromBStack.pop();
            }
            if (sumTakenFromA + sumTakenFromB <= limit)
            {
                // This is a valid sequence of moves: update largestNumMoves.
                const int numTakenFromB = takenFromBStack.size();
                const int numMoves = numTakenFromA + numTakenFromB;
                largestNumMoves = max(largestNumMoves, numMoves);
            }
            numTakenFromA++;
        }
        cout << largestNumMoves << endl;
    }

    
}
