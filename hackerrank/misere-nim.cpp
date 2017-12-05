// Simon St James (ssjgz) - 2017-12-05
#include <iostream>

using namespace std;

int main()
{
    // Ugh - took me ages, as it's not very inituitive in some sense (I would have thought
    // that the completely different goal would lead to a completely different strategy
    // from the very first move, but apparently not!).
    //
    // Anyway, it turns out that this is just Nim as usual, with a small twist - let's look
    // at what and why.
    //
    // Firstly: if there are no piles with more than one stone i.e. if all piles have either
    // one stone or zero, then the game becomes essentially deterministic.  If the total 
    // number of stones is odd, then the First player will be forced to pick up the 
    // last stone (and so, lose); else, then Second.
    //
    // So obviously, if the initial configuration *does* have a pile with more than one stone,
    // the move that transforms the game from a state S where at least one pile has more than one
    // stone to a state S' where no pile has more than one stone is very important.  In such an S,
    // there is obviously precisely one pile that has more than one stone (can't be two, as 
    // then one move couldn't move us to a state where none do!); call this pile P.  Since it has
    // at least two stones, we have at least two moves that can take us to such an S' - we
    // could remove all stones in P, or all but one stones in P.  But the number of remaining stones
    // after making one of these moves will have different parity deciding on which of the two moves
    // we choose, and as we saw earlier, in a state where no pile has more than one stone, the parity
    // decides the winner.  Thus, the person who can make a move at such a state S decides the fate
    // of the game.
    //
    // So far, so straightforward; here's the bit that took me a while to realise XD The xor sum of 
    // the piles at such an S must be non-zero (the bit representation of the number of stones in pile P
    // in S must have a bit higher than 1 = 2^0 since the number of stones in P is > 0, and no other
    // pile can have this higher bit - therefore, the xor's of all the pile sizes is at least the value
    // of this bit in the binary representation of the size of P) - thus, if the first player
    // simply plays up until the state S using the "normal" Winning strategy of Nim - ensuring that
    // the other player always ends up with a 0 pile-size xor sum - then he will always be the
    // one to make the deciding move at state S, and so will be the winner! Of course, if the First player
    // can't play this strategy - because the initial xor sum is 0 - then the second player will end up
    // reaching S and making the deciding move.
    //
    // So in a nutshell: if there are no piles with size greater than one, then the game is deterministic
    // and the First player wins if and only if the total number of stones is even; otherwise,
    // if the initial xor sum is non-zero, the First player simply plays normal winning-strategy Nim
    // up until we can make a move that makes the number of stones in each pile at most one, and then
    // takes either all stones from the single pile of size > 1 or none of them - whichever makes him win.
    // If the initial xor sum is 0, then it is the second player who can make the deciding move at this critical
    // state.

    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        int numPiles;
        cin >> numPiles;

        int numPilesOfSizeOne = 0;
        int numPilesOfSizeGreaterThanOne = 0;
        int pilesNimSum = 0;
        for (int i = 0; i < numPiles; i++)
        {
            int numInPile;
            cin >> numInPile;

            if (numInPile == 1)
                numPilesOfSizeOne++;
            else if (numInPile > 1)
                numPilesOfSizeGreaterThanOne++;

            pilesNimSum ^= numInPile;
        }

        if (numPilesOfSizeGreaterThanOne == 0)
        {
            // Every pile has size 0 or 1; the game is 
            // deterministic from here, and the winner is
            // First if the total number of stones is even, else
            // Second.
            const int numStones = numPilesOfSizeOne * 1;

            if ((numStones % 2) == 0)
                cout << "First";
            else
                cout << "Second";
        }
        else
        {
            // Play Nim as usual until we reach the crucial state, then pick one of the two
            // moves that makes us win.
            // The First player reaches this crucial state first if and only if the initial
            // pilesNimSum is non 0.
            if (pilesNimSum == 0)
                cout << "Second";
            else
                cout << "First";
        }
        cout << endl;
    }
}
