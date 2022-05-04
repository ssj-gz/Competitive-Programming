#include <iostream>
#include <deque>
#include <vector>
#include <set>
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

constexpr int player1Index = 0;
constexpr int player2Index = 1;
constexpr int numPlayers = 2;
constexpr int otherPlayer(int playerIndex)
{
    return 1 - playerIndex;
}

struct State
{
    State(const deque<int>& player1Cards, const deque<int>& player2Cards)
    {
        playerCards[player1Index] = player1Cards;
        playerCards[player2Index] = player2Cards;
    }
    deque<int> playerCards[numPlayers];
    // Add spaceship operator so that we can place States in e.g. std::sets.
    // The default implementation suffices for insertion and checking containment.
    auto operator<=>(const State& other) const = default;
};

class RecursiveCombatGame
{
    public:
        RecursiveCombatGame(const deque<int>& player1Cards, const deque<int>& player2Cards)
            : m_currentState{player1Cards, player2Cards}
        {
            static int nextGameId = 1;
            m_gameId = nextGameId;
            nextGameId++;
        }
        void printDeck(int playerIndex)
        {
            const auto& cards = m_currentState.playerCards[playerIndex];
            const int numCards = static_cast<int>(cards.size());
            for (int cardIndex = 0; cardIndex < numCards; cardIndex++ )
            {
                cout << cards[cardIndex];
                if (cardIndex != numCards - 1)
                    cout << ", ";
            }
        }
        int winningPlayerIndex()
        {
            cout << endl << "=== Game " << m_gameId << " ===" << endl << endl;;
            int roundNum = 0;
            while (true)
            {
                roundNum++;

                auto printGameWinner = [roundNum, gameId = m_gameId](const int playerIndex)
                {
                    cout << "The winner of game " << gameId << " is player "<<  (playerIndex + 1) << "!" << endl << endl;
                };

                // If we've already seen this state, Player 1 wins.
                if (m_seenStates.contains(m_currentState))
                {
                    const int winningPlayerIndex = player1Index;
                    printGameWinner(winningPlayerIndex);
                    return winningPlayerIndex;
                }
                m_seenStates.insert(m_currentState);

                // If a player has no cards, they lose.
                for (auto playerIndex : {player1Index, player2Index})
                {
                    if (m_currentState.playerCards[playerIndex].empty())
                    {
                        const int winningPlayerIndex = otherPlayer(playerIndex);
                        printGameWinner(winningPlayerIndex);
                        return winningPlayerIndex;
                    }
                }

                cout << "-- Round " << roundNum << " (Game " << m_gameId << ") --" << endl;

                for (auto playerIndex : {player1Index, player2Index})
                {
                    cout << "Player " << (playerIndex + 1) << "'s deck: ";
                    printDeck(playerIndex);
                    cout << endl;
                }


                vector<int> playedCards(numPlayers);
                int highestCard = -1;
                int highestCardPlayerIndex = -1;
                bool needToRecurse = true;
                for (auto playerIndex : {player1Index, player2Index})
                {
                    auto& playerCards = m_currentState.playerCards[playerIndex];
                    const int cardToPlay = playerCards.front();
                    cout << "Player " << (playerIndex + 1) << " plays: " << cardToPlay << endl;
                    playedCards[playerIndex] = cardToPlay;
                    playerCards.pop_front();

                    if (cardToPlay > highestCard)
                    {
                        highestCard = cardToPlay;
                        highestCardPlayerIndex = playerIndex;
                    }
                    if (static_cast<int>(playerCards.size()) < cardToPlay)
                    {
                        needToRecurse = false;
                    }
                }

                int winningPlayerIndex = -1;
                if (needToRecurse)
                {
                    deque<int> stateForSubgame[numPlayers]; 
                    for (auto playerIndex : {player1Index, player2Index})
                    {
                        auto& playerCards = m_currentState.playerCards[playerIndex];
                        stateForSubgame[playerIndex].assign( playerCards.begin(), playerCards.begin() + playedCards[playerIndex]);
                    }
                    cout << "Playing a sub-game to determine the winner..." << endl;
                    RecursiveCombatGame subGame{stateForSubgame[player1Index], stateForSubgame[player2Index]};
                    winningPlayerIndex = subGame.winningPlayerIndex();
                    cout << "...anyway, back to game " << m_gameId << "." << endl;
                }
                else
                {
                    winningPlayerIndex = highestCardPlayerIndex;
                }

                cout << "Player " << (winningPlayerIndex + 1) << " wins round " << roundNum << " of game " << m_gameId << "!" << endl << endl;

                assert(winningPlayerIndex != -1);
                m_currentState.playerCards[winningPlayerIndex].push_back(playedCards[winningPlayerIndex]);
                m_currentState.playerCards[winningPlayerIndex].push_back(playedCards[otherPlayer(winningPlayerIndex)]);

            }
            assert(false && "Unreachable");
            return -1;

        }

        deque<int> playerCards(int playerIndex)
        {
            return m_currentState.playerCards[playerIndex];
        }
    private:
        State m_currentState;
        set<State> m_seenStates;

        int m_gameId = -1;
};

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
            int card = static_cast<int>(stol(line));
            if (playerCardsBeingRead == 1)
                player1Cards.push_back(card);
            else if (playerCardsBeingRead == 2)
                player2Cards.push_back(card);
            else
                assert(false);
        }
    }

    RecursiveCombatGame mainGame{player1Cards, player2Cards};
    const int winningPlayerIndex = mainGame.winningPlayerIndex();
    cout << "Score: " << calcScore(mainGame.playerCards(winningPlayerIndex)) << endl;

}
