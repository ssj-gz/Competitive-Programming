#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

int main() {
    int T;
    cin >> T;
    for (int i = 0; i < T; i++)
    {
        // Took me a while - surprised everyone else found it so easy!
        // Firstly, note that the whole "divisibility" thing is a red-herring.
        // Let X = p0^i0 * p1^i1 * ... pk^ik (each pj prime, ij > 0) be the prime
        // decomposition of X; then note that if Y evenly divides X, then Y must
        // be of the form p0^j0 * p1^j1 * ... * pk^jk, where each of the jl's <= il,
        // and the result of X / Y is p0^(i0 - j0) * p1^(i1 - j1) * ... * pk^(ik - jk).
        // The upshot of this is that we can represent a tower by (i0 + i1 + ... + ik)
        // stones, and reducing a tower at any stage simply takes away some amount of these
        // stones.  Note that if M == 1, then the number of "stones" is 0, and player 1
        // loses as they can't even make an initial move.
        // Anyway: so we have a series of N towers, each with some number S of stones (where
        // S is determined by M's prime decomposition, as above.)
        // Now: if N is even, then player 2 has an unbeatable strategy: pair the towers so
        // that each tower has a "twin".  If player 1 remove s stones from a tower t, player 
        // 2 should then remove s stones from t's twin, maintaining the invariant that 
        // after player 2's turn, each tower has the same number of stones as its twin.
        // It follows then that player 2 can *always* make a move after player 1, and since
        // the sum total of stones eventually reaches 0, it must be player 1 who is faced
        // with 0 stones and so, loses.
        // If, however, N is odd, then player 1 can "turn the tables" on player 2 by 
        // removing all stones from one of the towers: then it is player 2 that is faced
        // with an even-number of towers of the same height and so faces the unbeatable
        // strategy (and inevitable defeat) described above.
        // So: if M == 1, player 1 loses; otherwise, if N is odd, player 1 wins and if N is even,
        // player 2 wins.

        long N, M;
        cin >> N >> M;
        if (M == 1)
            cout << 2;
        else if ((N % 2) == 1)
            cout << 1;
        else if ((N % 2) == 0)
            cout << 2;

        cout << endl;

    }
    return 0;
}
