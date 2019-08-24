// Simon St James (ssjgz) - 2019-08-03
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>

#include <cassert>

#include <sys/time.h>

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

bool solveBruteForce(const string& cardsState)
{
    const auto numCards = cardsState.size();
    set<string> visitedCardStates = { cardsState };
    vector<string> cardStatesToExplore = { cardsState };

    while (!cardStatesToExplore.empty())
    {
        vector<string> nextCardStatesToExplore;

        for (const auto& cardState : cardStatesToExplore)
        {
            if (cardState == string(numCards, '.'))
                return true;

            for (auto i = 0; i < numCards; i++)
            {
                if (cardState[i] == '1')
                {
                    string nextCardState = cardState;
                    nextCardState[i] = '.';
                    if (i - 1 >= 0 && nextCardState[i - 1] != '.')
                        nextCardState[i - 1] = '0' + ('1' - nextCardState[i - 1]);
                    if (i + 1 < numCards && nextCardState[i + 1] != '.')
                        nextCardState[i + 1] = '0' + ('1' - nextCardState[i + 1]);


                    if (visitedCardStates.find(nextCardState) == visitedCardStates.end())
                    {
                        visitedCardStates.insert(nextCardState);
                        nextCardStatesToExplore.push_back(nextCardState);
                    }
                }
            }
        }

        cardStatesToExplore = nextCardStatesToExplore;
    }
    return false;
}

bool calcCanChefWin(const string& cardsStateOriginal)
{
    string cardState = cardsStateOriginal;
    const char removedCard = '.';
    while (true)
    {
        cout << "Current card state: " << cardState << endl; 
        vector<int> positionsOfOddFaceUp;
        int numFaceUpSeen = 0;
        for (int i = 0; i < cardState.size(); i++)
        {
            if (cardState[i] == removedCard)
            {
                // New sub-game - reset count.
                numFaceUpSeen = 0;
            }
            if (cardState[i] == '1')
            {
                numFaceUpSeen++;
                if ((numFaceUpSeen % 2) == 1)
                {
                    positionsOfOddFaceUp.push_back(i);
                }
            }
        }
        if (positionsOfOddFaceUp.empty())
        {
            const auto numCards = std::count_if(cardState.begin(), cardState.end(), [](const auto card) { return card != removedCard; });
            if (numCards > 0)
                return false;
            else
                return true;
        }
        const int moveToMake = positionsOfOddFaceUp[rand() % positionsOfOddFaceUp.size()];
        cout << "Choosing card:     " << string(moveToMake + 1, ' ') << "^" << endl;
        cout << "moveToMake: " << moveToMake << endl;
        cardState[moveToMake] = removedCard;
        if (moveToMake != 0 && cardState[moveToMake - 1] != removedCard)
        {
            cardState[moveToMake - 1] = '0' + ('1' - cardState[moveToMake - 1]);
        }
        if (moveToMake != cardState.size() - 1 && cardState[moveToMake + 1] != removedCard)
        {
            cardState[moveToMake + 1] = '0' + ('1' - cardState[moveToMake + 1]);
        }
    }
}

int main(int argc, char* argv[])
{
    if (argc == 2 && argv[1] == string("--test"))
    {
        // Generate a set up cards with an odd number facing up.
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int numCards = rand() % 20 + 1;
        int numFaceUp = rand() % numCards;
        if (numFaceUp % 2 == 0)
        {
            if (numFaceUp != 0)
                numFaceUp--;
            else
                numFaceUp++;
        }
        string cards;
        for (int i = 0; i < numCards; i++)
        {
            if (i < numFaceUp)
                cards.push_back('1');
            else
                cards.push_back('0');
        }
        random_shuffle(cards.begin(), cards.end());
        cout << 1 << endl;
        cout << cards << endl;
        return 0;

    }
    // Very easy solution, though quite tricky to "logic" your way to it - 
    // in the end, I didn't bother and just used the brute force solution (solveBruteForce)
    // with a random testcase generator and pattern-matched my way to a solution :)
    //
    // TL;DR - the game is winnable if and only if the number of cards initially face up is 
    // odd.
    //
    // Why is this? Well ...
    //
    // Lemma
    //
    // Making a move splits the (sub-)game into two independent subgames, and we can win after making this
    // move if and only if *both* the subgames are winnable.
    //
    // Proof
    //
    // No formal proof - simply observe:
    //
    // Initial (sub-)game state (B_M == 1):
    //
    //    B_1 B_2 ... B_(M-1) B_M B_(M+1) ... B_N
    //
    // Remove B_M; flip B_(M-1) and B_(M+1) ("~" denotes bitwise negation i.e. ~0 == 1; ~1 == 0):
    //
    //    B_1 B_2 ... ~B_(M-1)    ~B_(M+1) ... B_N
    //    |      sub-game 1      |    |  sub-game 2  |
    //
    // If M == 1 or M == N, we have instead:
    //
    //                                  ~B_(M+1) ... B_N
    //    | sub-game 1 (empty) |    |  sub-game 2  |
    //
    // and
    //
    //    B_1 B_2 ... ~B_(M-1)    
    //    |      sub-game 1      |   | sub-game 2 (empty) |
    //
    // respectively.  We treat an empty subgame as trivially winnable because - well, it's already been
    // won as there are no cards in it :)
    //
    // Note that in all cases, the "blank" obtained by removing B_M acts as a "break" that stops any
    // move in sub-game 1 interfering with sub-game 2 (and vice versa) so sub-games 1 and 2 are
    // indeed independent.  It is obvious that clearing all remaining cards is equivalent to winning
    // *both* sub-game 1 and sub-game 2.
    //
    // QED
    //
    // The Lemma can be applied recursively (so we have sub-games of a sub-game of a sub-game etc).
    //
    // Theorem
    //
    // A (sub-)game can be won if and only if the number of face-up cards is odd.
    //
    // Proof
    //
    // Proof is by induction on the size of the (sub-)game.
    //
    // (=>) A sub-game can be won implies that the number of face-up cards is odd.
    //
    // The contra-positive is 
    //
    //    If a sub-game has an even number of cards face up, it cannot be won.
    //
    // Proof for N == 1 is immediate: such a sub-game would have its only card facing downwards,
    // and this game obviously cannot be won - there's no moves remaining, so we cannot remove
    // the remaining card.
    //
    // Let B_1 ... B_N (N > 1) be the initial game state with an even number of the B_i's facing up.
    //
    // Let M be the index of *any* card facing upwards i.e. B_M = 1.
    //
    // There are three possibilities:
    //
    //    i) M == 1
    //    ii) M == N
    //    iii) 1 < M < N
    //
    // For case i) - as noted, making the move B_M will split the game into an (empty) sub-game 1 to the
    // left of B_M (which is trivially winnable), and a sub-game 2 to the right of B_M which looks like:
    //
    //    ~B_(M+1) B_(M+2) ... B_N
    //
    // Now, the number of cards facing up in B_(M+1) B_(M+2) ... B_N must be odd (as the number of cards
    // in B_M B_(M+1) B_(M+2) ... B_N was assumed to be even (remember, M == 1 here)).   Thus, in the
    // sub-game 2, which flips B_(M+1) from 0 to 1 or vice versa, the number of cards facing up must 
    // then be even i.e. sub-game 2 == ~B_(M+1) B_(M+2) ... B_N has an even number of cards facing up.
    // Thus, by induction hypothesis, sub-game 2 cannot be won, and so by the Lemma making the move M == 1
    // is a losing move.
    //
    // For case ii), the reasoning is almost identical; removing the card B_M for M == N will split into
    // two sub-games; sub-game 1 == B_1 B_2 ... ~B_(M-1) to the left of M and the empty, trivially winnable 
    // sub-game 2 to the right of M.  Again, B_1 B_2 ... B_(M-1) must have had an odd number of cards
    // facing up, so B_1 B_2 ... ~B_(M-1) must have an even number, and so again by induction hypothesis and 
    // the Lemma, the move M == N is a losing move.
    //
    // For case iii), the reasoning is, again, almost identical.  Removing the card B_M for 1 < M < N gives two
    // sub-games:
    //
    //    B_1 B_2 ... ~B_(M-1)
    //
    // and 
    //
    //    ~B_(M+1) B_(M+2) ... B_N
    //
    // Now, the number of cards facing up in B_1 B_2 ... B_(M-1) plus the number of cards facing up in
    // B_(M+1) B_(M+2) ... B_N must have been odd (as the number of cards in 
    // B_1 B_2 ... B_(M-1) B_M B_(M+1) ... B_N is even, by assumption), so the number of cards facing up in
    // B_1 B_2 ... ~B_(M-1) plus the number of cards facing up in ~B_(M+1) B_(M+2) ... B_N must also 
    // be even (we've flipped a card in the left sub-game *and* flipped a card in the right sub-game, which
    // "cancel out").  Thus, at least one of sub-game 1 and sub-game 2 must have an *even* number of cards
    // facing up (they can't both be odd, as an odd number plus any other odd number is always even).
    // Thus, from the induction hypothesis and the Lemma, B == M is a losing move.
    //
    // Thus, if the number of cards facing up is even, all possible moves are losing moves, and so the 
    // game cannot be won; hence result.
    //
    // (<=) If the number of face up cards is odd, then the game can be won.
    //
    // Proof is again by induction on the number of cards, N.  The N == 1 case is trivial: the (sub-)game
    // consists of a single card, and that card is facing up.  Removing it clears all remaining cards
    // and wins the game.
    //
    // Assume N > 1, and that an odd number of B_1 B_2 ... B_N are 1 i.e. an odd number of the B_i's are 1.
    // Then at least one of the B_i's is 1.
    //
    // Let M be the smallest index such that B_M is 1 (there is at least one such M as mentioned above).
    //
    // If M == 1, then making the move M gives us an empty (and so, trivially winnable) sub-game 1 to the
    // left, and a sub-game 2 ~B_(M+1) B_(M+2) ... B_N to the right.  The number of cards facing up in
    // B_(M+1) B_(M+2) ... B_N must have been even, so the of cards facing up in subgame 2 must be odd
    // as we flip B_(M+1) over.  By induction hypothesis and the Lemma, this move is a winning move.
    //
    // If M == N, then similarly we get two sub-games - sub-game 2 to the right is trivially winnable, and
    // so, using the same logic as the M == 1 case, is sub-game 1 == B_1 B_2 ... ~B_(M-1), so this is also a winning move for
    // the same reason.
    //
    // If 1 < M < N (the sole remaining case), then we split into two subgames - sub-game 1 to the left of M:
    //
    //    B_1 B_2 ... ~B_(M-1)
    //
    // and sub-game 2 to the right of M:
    //
    //    ~B_(M+1) B_(M+2) ... B_N
    //
    // By choice of M, the number of cards facing up in B_1 B_2 ... B_(M-1) is 0, so the number of cards
    // facing up in sub-game 1 == B_1 B_2 ... ~B_(M-1) is 1.
    //
    // The number of cards facing up in B_(M+1) B_(M+2) ... B_N must be even (as the number of cards facing up
    // in B_1 B_2 ... B_(M-1) ( == 0) plus 1 for B_M plus number facing up in B_(M+1) B_(M+2) ... B_N is odd
    // by assumption), so the number facing up in sub-game 2 == ~B_(M+1) B_(M+2) ... B_N must be odd.
    //
    // So both sub-game 1 and sub-game 2 have an odd number of cards; therefore, by induction hypothesis and the Lemma,
    // B == M is a winning move.
    //
    // QED
    //
    // We don't use the following Corollary, but IMO it's interesting enough to note, nonetheless :)
    //
    // Corollary
    //
    // A game is winnable if and only if the following strategy clears all cards:
    // 
    //   On each move, find the card with the smallest index such that B_M == 1 (if there is one) and choose
    //   that card for our move.
    //
    // Proof
    //
    // We proved that this strategy works when there is an odd number of cards in the (<=) part of the theorem.
    // If there is an even number of cards, then this strategy clearly won't work, as the Theorem states that
    // there is no winning strategy in that case.
    //
    // QED
    //
    // And that's it - a very long write-up for a solution that is basically one line long XD
    const auto T = read<int>();
    for (auto t = 0; t < T; t++)
    {
        const string cardsState = read<string>();

        const auto canChefWin = calcCanChefWin(cardsState);
        cout << "status:" << (canChefWin ? "WIN" : "LOSE") << endl;
    }
}


