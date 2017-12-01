// Simon St James (ssjgz) 2017-12-01
#include <iostream>
#include <array>
#include <map>
#include <cassert>

using namespace std;

int main()
{
    // Fairly easy one, though I did a brute-force verification as practice :)
    //
    // The trick is to notice that :
    //
    //  a) the numbers on the blocks are more-or-less irrelevant; only their value mod 3 is important; thus
    //  we can model the problem as three piles of blocks, where pile A has the blocks whose values mod 3 are 0; B has those whose value is 2; and C has 
    //  those whose value is 3; 
    //
    //  b) after each move, the number of blocks always blocks reduces by exactly 1 (easily verified); and
    //
    //  c) the person who can make the last move always wins.
    //
    // To see c), let's look at all the states the penultimate move can be in i.e. all the contents of the 3 piles when there are exactly two blocks remaining.
    //
    //  i) 2 0 0 ii) 0 2 0 iii) 0 0 2 iv) 1 1 0 v) 1 0 1 vi) 0 1 1
    //     A B C     A B C      A B C     A B C    A B C     A B C
    //
    // For i), ii) and iii), the only play is to take the two blocks and add one block to pile A (since the two blocks have the same value mod 3); pile A is the "0 mod 3"
    // pile, and so the player who makes this move wins i.e. the player who makes the final move wins.
    //
    // For iv),  we can take the two remaining blocks in either order: taking the one from A then the one from B means we need to add to pile C (0 - 1 is 2, mod 3);
    // taking the one from B then the one from A means we add to pile B (1 - 0 is 1, mod 3) - thus if the current player is Kitty, she makes the move that adds to B;
    // if current player is Katty, she makes the move that adds to C.  Either way, the player who makes the final move wins.
    //
    // v) and vi) are basically identical to iv) - we have two orders for picking the last two blocks, and the current player can ensure that the last
    // block is added to B or C and so should choose the one that makes them win.
    //
    // So in all cases, the current player i.e. the one who makes the final move is the winner.  Coupled with a), we see that, if n >= 2,
    // then Kitty makes the last move if and only if n is even i.e. for n >= 2, Kitty wins if and only if n is even.  She also wins if n == 1.
    
    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        int n;
        cin >> n;

        if (n == 1 || ((n % 2) == 0))
            cout << "Kitty";
        else
            cout << "Katty";
        cout << endl;
    }
}


