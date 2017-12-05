// Simon St James (ssjgz) - 2017-12-05
#include <iostream>

using namespace std;

int main()
{
    // Another one that took me an embarrassingly long time!
    // Ok, so imagine that the initial nim sum was non-zero - then
    // the First player, if this were a normal game of Nim, would 
    // never need to use the "add more chips" option *unless* the
    // Second (losing) player did.  So the player who adds chips first
    // is *always* the losing player i.e. the one who is confronting
    // with a xor sum of 0.
    //
    // So imagine the losing player adds x chips to pile P.  Now the winning player,
    // on his next move, can simply remove the chips that the losing player
    // added i.e. remove x chips from pile P, restoring the board to the state
    // before the losing player made his desperate move - i.e. restoring the board
    // state to the state that was a losing state for the losing player!
    //
    // In other words, the first player to add chips will always be the player who,
    // according to the usual Nim evaluation, is losing (has a board state of 0).
    // But if he decides to add chips, the winning player will just remove them
    // again, and so the losing player still ends up losing.  So there is no
    // point in ever adding chips, and so the game is equivalent to the "normal"
    // game of Nim.
    int T;
    cin >> T;

    for (int i = 0; i < T; i++)
    {
        int n, k;
        cin >> n >> k;

        int pilesNimSum = 0;
        for (int i = 0; i < n; i++)
        {
            int pileSize;
            cin >> pileSize;

            pilesNimSum ^= pileSize;
        }

        // Evaluate the winner according to the normal game of Nim, to which Poker Nim is equivalent.
        if (pilesNimSum == 0)
            cout << "Second";
        else
            cout << "First";
        cout << endl;
    }
}
