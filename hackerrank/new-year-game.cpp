// Simon St James (ssjgz) - 2018-01-12.
#include <iostream>
#include <vector>
#include <map>
#include <array>
#include <sstream>
#include <fstream>
#include <limits>
#include <cassert>

using namespace std;

int main(int argc, char** argv)
{
    // Fairly easy one.  We're interested only in the difference of scores mod 3, which suggests that we don't
    // really care about the full values in the array, merely their value mod 3.  Thus, we can boil down the
    // array at any state in the game to just the number of values that are 0 mod 3, 1 mod 3, and 2 mod 3.  Let N(x) be the number
    // of values in the array that have value x mod 3 for x = 0, 1, 2.
    //
    // If scoreDiff is Balsa's score minus Koca's score, mod 3, then Balsa taking a value v from the array increases scoreDiff by 
    // v (mod 3), and Koca taking a value v' from the array *decreases* scoreDiff by v' (again, all mod 3).  Note that if Balsa takes 
    // a value which is x mod 3 from the array and Koca then takes a value which has the same value x mod 3, then scoreDiff is unchanged;
    // this hints that mirroring might form part of the optimal strategy and, further, that the winner might depend only on the parity of 
    // each N(x) for each x = 0, 1, 2.  In fact, as we'll see, this is indeed the case.
    //
    // Let's switch to looking at a near-end stage of the game - where each N(x) is either 0 or 1 (parity!)
    //
    // There are eight possibilities: each triple below represents (N(0), N(1), N(2)).
    //
    //  1) (0, 0, 0)
    //     No player can move, so scoreDiff remains at 0; Koca wins.
    //  2) (0, 0, 1)
    //     Balsa must take the sole value which is 2 mod 3, making the scoreDiff 2 mod 3.  Koca cannot make any moves, so the final scoreDiff is 2: Balsa wins.
    //  3) (0, 1, 0)
    //     Using identical reasoning to 2), the final scoreDiff is 1, so Balsa wins.
    //  4) (0, 1, 1) 
    //     Two sub-possibilities:
    //      
    //     a) Balsa takes the value which is 2 mod 3 (making scoreDiff 2); Koca then must take the sole remaining value which decreases scoreDiff to 1.  No
    //        further moves are possible -> Balsa wins.
    //     b) Similar to a) - final scoreDiff is 2, so Balsa wins.
    //  5) (1, 0, 0)
    //     Balsa must take the sole value which is 0 mod 3, leaving scoreDiff as 0.  No further moves are possible, so final scoreDiff is 0 -> Koca wins.
    //  6 - 8): (1, 0, 1), (1, 1, 0), (1, 1, 1) - exercise for the reader ;) Balsa wins each of these.
    //
    // So we see that in this simple game, Koca wins if and only if N(1) == N(2) == 0 (the value of N(0) makes no difference).
    //
    // It's now fairly easy to see that Balsa has a winning strategy for (N(0), N(1), N(2)) if and only if it Balsa has a winning strategy for (N(0) % 2, N(1) % 2, N(2) % 2).
    // Let N'(x) be N(x) % 2.  Assume that Balsa has a winning strategy for (N'(0), N'(1), N'(2)).  Note that N(x) is odd if and only if N'(x) is 1.  Let Balsa use the following
    // strategy:
    //
    //  i) Pick the x such that x would be the winning first move for (N'(0), N'(1), N'(2)) (we know this exists, by assumption) - then we must have had N'(x) = 1.  Set N'(x) to 0.
    //  ii) Let Koca make his move, and call it x'. 
    //  iii) There are two situations that Balsa must respond to:
    //   a) N'(x') was equal to 1.  In this case, play whatever move we would respond to if we had played x then x' in the original (N'(0), N'(1), N'(2)) game and set N'(x') to 0.
    //   b) N'(x') was equal to 0, in which case N(x) was even when Koca made the move, and is now odd; have Balsa play the same move, x, essentially undoing Koca's change to scoreDiff.
    //   Since N(x) is odd when it is Balsa's turn, it is always possible for Balsa to play this move x.
    //  
    // It's easy to see that repeated application of this strategy (i), then repeat ii) and iii) until the game ends) will be a win for Balsa - we are essentially playing the 
    // reduced (N(0) % 2, N(1) % 2, N(2) % 2) game (which we know Balsa can win), except that when Koca plays a move x which is not possible in (N'(0), N'(1), N'(2)), we immediately
    // nullify it by playing the same move x (mirroring). So if there is a winning strategy for Balsa with (N(0) % 2, N(1) % 2, N(2) % 2), then there is a winning strategy for (N(0), N(1), N(2)).  
    // We can apply the same reasoning to form a winning strategy for Koca for (N(0), N(1), N(2)) if there is a winning strategy for Koca for (N(0) % 2, N(1) % 2, N(2) % 2).  Since if Balsa does not have a winning
    // strategy then Koca does, we see that both sides of the implication hold i.e. if  there is no winning strategy for Balsa for (N(0), N(1), N(2)), then there is no winning strategy
    // for Balsa with (N(0) % 2, N(1) % 2, N(2) % 2) i.e. the winner is completely determined by (N(0) % 2, N(1) % 2, N(2) % 2), in which only two configurations (1) and 5)) are wins 
    // for Koca.
    int T;
    cin >> T;
    for (int t = 0; t < T; t++)
    {
        int n;
        cin >> n;

        vector<int> a(n);
        for (int i = 0; i < n; i++)
        {
            cin >> a[i];
        }

        int numWithMod3[3] = {};
        for (const auto x : a)
        {
            numWithMod3[x % 3]++;
        }
        
        if (numWithMod3[1] % 2 == 0 && numWithMod3[2] % 2 == 0)
        {
            cout << "Koca";
        }
        else
        {
            cout << "Balsa";
        }
        cout << endl;
    }
}

