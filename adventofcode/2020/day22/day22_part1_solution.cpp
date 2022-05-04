#include <iostream>
#include <deque>
#include <cassert>

using namespace std;

int64_t calcScore(const deque<int>& cards)
{
    int64_t score = 0;
    int64_t multiplier = cards.size();
    for (const auto card : cards)
    {
        score += multiplier * card;
        multiplier--;
    }
    return score;
}

int main()
{
    string line;
    int playerCardsBeingRead = -1;
    deque<int> player1Cards;
    deque<int> player2Cards;
    while (getline(cin, line))
    {
        if (line.empty())
        {
            playerCardsBeingRead = -1;
            continue;
        }

        if (line == "Player 1:")
        {
            assert(playerCardsBeingRead == -1);
            playerCardsBeingRead = 1;
            continue;
        }
        else if (line == "Player 2:")
        {
            assert(playerCardsBeingRead == -1);
            playerCardsBeingRead = 2;
            continue;
        }
        else
        {
            cout << "line: " << line << endl;
            int64_t card = stol(line);
            if (playerCardsBeingRead == 1)
                player1Cards.push_back(card);
            else if (playerCardsBeingRead == 2)
                player2Cards.push_back(card);
            else
                assert(false);
        }
    }
    cout << "Player 1 cards: " << endl;
    for (const auto x : player1Cards)
    {
        cout << " " << x <<endl;
    }
    cout << "Player 2 cards: " << endl;
    for (const auto x : player2Cards)
    {
        cout << " " << x <<endl;
    }

    while (!player1Cards.empty() && !player2Cards.empty())
    {
        const int player1DrawnCard = player1Cards.front();
        player1Cards.pop_front();
        const int player2DrawnCard = player2Cards.front();
        player2Cards.pop_front();

        if (player1DrawnCard > player2DrawnCard)
        {
            player1Cards.push_back(player1DrawnCard);
            player1Cards.push_back(player2DrawnCard);
        }
        else if (player2DrawnCard > player1DrawnCard)
        {
            player2Cards.push_back(player2DrawnCard);
            player2Cards.push_back(player1DrawnCard);
        }
        else
        {
            assert(false);
        }

        cout << "After round: " << endl;

        cout << "Player 1 cards: " << endl;
        for (const auto x : player1Cards)
        {
            cout << " " << x <<endl;
        }
        cout << "Player 2 cards: " << endl;
        for (const auto x : player2Cards)
        {
            cout << " " << x <<endl;
        }
    }
    if (!player1Cards.empty())
        cout << calcScore(player1Cards) << endl;
    else
        cout << calcScore(player2Cards) << endl;

}
