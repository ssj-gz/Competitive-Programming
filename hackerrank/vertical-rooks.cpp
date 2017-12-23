// Simon St James (ssjgz) 2017-12-02
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

int main()
{
    // Unsurprisingly, another bunch of concurrent Nim games in disguise (in fact, something more like 
    // Poker Nim, so you might want to glance through the explanation for that).
    //
    // So - it seems intuitively obvious that this is a set of n Nim games, where the number of stones
    // in pile p is equal to the distance (minus 1 - so adjacent rooks have distance 0) between the opposing rooks in column p.  More evidence 
    // for this comes when we consider what happens when all "piles" are "empty" i.e. in each row,
    // the opposing rooks are adjacent to each other - if it is our turn in such a state, then
    // there's little we can do: at best, we can move one of our rooks (the one in column p, say) 
    // back x paces for some x (and if we can't even do that, it's Game Over) - but then, the opposing 
    // player immediately counteracts this by moving his rooks in column p x paces towards ours, 
    // completely negating our move.  That is: if all "piles" are empty at the beginning of our turn,
    // then it is game over for us, just as it would be in a set of normal Nim games.
    //
    // A complication comes from the ability to walk our rook back away from the opposing rook, and in fact
    // this stops the game from being impartial - in column p, one player might be able to walk his rook
    // backwards by x steps (adding x to the number of stones in pile p) whereas the other player might only
    // be able to walk back by y steps (adding y to the number of stones in p), which is not impartial.
    // However, we can draw inspiration from what happens in the "all piles empty" case: if a player does 
    // walk back by x steps in pile p (adding x stones to the pile p), then the other player can immediately
    // counter by moving their piece in pile p forward by x steps (returning the number of stones in p
    // back to its original value), returning the board state back to how it originally was.  Thus, we see that
    // moving a piece backwards never gives any advantage.
    //
    // In a nutshell, then: if the first player is confronted by a board where the nim sum (i.e. the xor of the 
    // distances (each minus one)) is non-zero, then he simply plays according to the normal rules of Nim
    // i.e. moving his rooks so that the xor sum is 0, except whenever the other player has moved one of their 
    // rooks back by x paces, in which case he instead counteracts this by moving his corresponding rook forward
    // x paces.  Basically: if the initial xor sum is non-zero, he wins; else, he loses.
    int T;
    cin >> T;


    for (int t = 0; t < T; t++)
    {
        int n;
        cin >> n;

        vector<int> player1RookRowForColumn(n);
        for (int i = 0; i < n; i++)
        {
            cin >> player1RookRowForColumn[i];
        }
        vector<int> player2RookRowForColumn(n);
        for (int i = 0; i < n; i++)
        {
            cin >> player2RookRowForColumn[i];
        }

        int nimSum = 0;
        for (int i = 0; i < n; i++)
        {
            const int rookDistance = abs(player2RookRowForColumn[i] - player1RookRowForColumn[i]) - 1;
            nimSum ^= rookDistance;
        }
        // Player 2 goes first(!) so if the nimSum is initially 0, he loses i.e.
        // Player 1 wins.
        if (nimSum == 0)
            cout << "player-1";
        else
            cout << "player-2";

        cout << endl;
    }
    assert(cin);
}
