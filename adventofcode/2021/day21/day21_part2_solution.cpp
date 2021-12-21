#include <iostream>
#include <vector>
#include <deque>
#include <map>
#include <set>
#include <algorithm>
#include <cassert>

using namespace std;

constexpr auto winningScore = 21;
const int largestSquareNum = 10;

struct GameState
{
    int playerScores[2] = {-1,-1};
    int playerPositions[2] = {-1, -1}; // 1 ... 10.
    int currentPlayerTurn = -1;
    bool isValid() const
    {
        for (const auto score : playerScores)
        {
            if (score < 0)
                return false;
        }
        assert(currentPlayerTurn == 0 || currentPlayerTurn == 1);
        for (const auto playerPosition : playerPositions)
        {
            assert(1 <= playerPosition && playerPosition <= largestSquareNum);
        }


        for (const auto playerId : {0, 1})
        {
            if (playerScores[playerId] >= winningScore)
            {
                const int previousPlayer = 1 - playerId;
                const int previousPlayerScore = playerScores[previousPlayer] - playerPositions[previousPlayer];
                if (previousPlayerScore >= winningScore)
                    return false;

            }
        }
        return true;
    }
    bool isEnd() const
    {
        for (const auto score : playerScores)
        {
            if (score >= winningScore)
                return true;
        }
        return false;

    }
    bool operator<(const GameState& other) const
    {
        if (currentPlayerTurn != other.currentPlayerTurn)
            return currentPlayerTurn < other.currentPlayerTurn;
        for (const auto playerId : {0, 1})
        {
            if (playerScores[playerId] != other.playerScores[playerId])
                return playerScores[playerId] < other.playerScores[playerId];
            if (playerPositions[playerId] != other.playerPositions[playerId])
                return playerPositions[playerId] < other.playerPositions[playerId];
        }

        return false;
    }
};

ostream& operator<<(ostream& os, const GameState& gameState)
{
    os << "Gamestate: currentPlayerTurn: " << gameState.currentPlayerTurn << " scores: {" << gameState.playerScores[0] << ", " << gameState.playerScores[1] << "} positions: {" << gameState.playerPositions[0] << ", " << gameState.playerPositions[1] << "}";
    return os;
}

int newPos(int pos, int diceRoll)
{
    if (diceRoll == 0)
        return pos;
    pos += diceRoll;
    while (pos > largestSquareNum)
        pos -= largestSquareNum;
    while (pos < 1)
        pos += largestSquareNum;
    assert(1 <= pos && pos <= largestSquareNum);
    return pos;
}

int64_t numUniversesReachingGameState(const GameState& gameState, map<GameState, int64_t>& numUniversesReachingGameStateLookup, map<GameState, vector<GameState>>& predecessorStates)
{
    if (numUniversesReachingGameStateLookup.contains(gameState))
        return numUniversesReachingGameStateLookup[gameState];

    cout << "numUniversesReachingGameState: " << gameState << endl;

    assert(gameState.isValid());
    int64_t result = 0;

    for (const auto& predecessor : predecessorStates[gameState])
    {
        result += numUniversesReachingGameState(predecessor, numUniversesReachingGameStateLookup, predecessorStates);
    }

    numUniversesReachingGameStateLookup[gameState] = result;
    cout << "numUniversesReachingGameState: " << gameState << " result: " << result << endl;
    return result;
#if 0
    if (!gameState.isValid())
        return 0;

    const int previousPlayer = 1 - gameState.currentPlayerTurn;
    GameState previousGameState(gameState);
    previousGameState.currentPlayerTurn = previousPlayer;
    const int scorePreviousPlayerGained = gameState.playerPositions[previousPlayer];
    previousGameState.playerScores[previousPlayer] -= scorePreviousPlayerGained;
    for (int prevDice1 : {1, 2, 3})
    {
        for (int prevDice2 : {1, 2, 3})
        {
            for (int prevDice3 : {1, 2, 3})
            {
                previousGameState.playerPositions[previousPlayer] = newPos(gameState.playerPositions[previousPlayer], -(prevDice1 + prevDice2 + prevDice3));
                result += numUniversesReachingGameState(previousGameState, numUniversesReachingGameStateLookup);
            }
        }
    }

    numUniversesReachingGameStateLookup[gameState] = result;
    cout << "numUniversesReachingGameState: " << gameState << " result: " << result << endl;
    return result;
#endif

}


int main()
{
    const string playerStartPosPrefix = "Player X starting position: ";
    string player1StartPosLine;
    getline(cin, player1StartPosLine);
    const int player1StartPos = stoi(player1StartPosLine.substr(playerStartPosPrefix.size()));


    string player2StartPosLine;
    getline(cin, player2StartPosLine);
    const int player2StartPos = stoi(player2StartPosLine.substr(playerStartPosPrefix.size()));

    cout << "player1StartPos: " << player1StartPos << endl;
    cout << "player2StartPos: " << player2StartPos << endl;

    map<GameState, int64_t> numUniversesReachingGameStateLookup;
    GameState initialGameState = {
        {0, 0},
        {player1StartPos, player2StartPos},
        0
    };
    numUniversesReachingGameStateLookup[initialGameState] = 1;


    map<GameState, vector<GameState>> predecessorStates;
    set<GameState> seenStates = { initialGameState };
    deque<GameState> toProcess = { initialGameState };
    while (!toProcess.empty())
    {
        const auto gameState = toProcess.front();
        toProcess.pop_front();
        const int numWinningPlayers = count_if(begin(gameState.playerScores), end(gameState.playerScores), [](const auto score) { return score >= winningScore; });
        assert(numWinningPlayers == 0 || numWinningPlayers == 1);
        if (numWinningPlayers == 1)
            continue;
        for (int valDice1 : {1, 2, 3})
        {
            for (int valDice2 : {1, 2, 3})
            {
                for (int valDice3 : {1, 2, 3})
                {
                    const int diceSum = valDice1 + valDice2 + valDice3;
                    GameState nextState = gameState;
                    nextState.currentPlayerTurn = 1 - nextState.currentPlayerTurn;
                    nextState.playerPositions[gameState.currentPlayerTurn] = newPos(gameState.playerPositions[gameState.currentPlayerTurn], diceSum);
                    nextState.playerScores[gameState.currentPlayerTurn] += nextState.playerPositions[gameState.currentPlayerTurn];
                    predecessorStates[nextState].push_back(gameState);

                    if (!seenStates.contains(nextState))
                    {
                        toProcess.push_back(nextState);
                        seenStates.insert(nextState);
                    }
                }
            }
        }
    }
    for (const auto& [blah, predecessors] : predecessorStates)
    {
        cout << "state: " << blah << " predecessors (includes duplicate states): " << predecessors.size() << endl;
    }

    {
        GameState nose(initialGameState);
        nose.currentPlayerTurn = 1;
        nose.playerScores[0] = 10;
        nose.playerPositions[0] = 10;
        cout << "glarp:" << nose << ":" << numUniversesReachingGameState(nose, numUniversesReachingGameStateLookup, predecessorStates) << endl;
        //return 0;
    }

    int64_t numUniversesInWhichPlayerWins[2] = {0,0};
    for (const auto& [blah, predecessors] : predecessorStates)
    {
        assert(blah.isValid());
        for (int currentPlayerTurn : {0, 1})
        {
            if (blah.playerScores[currentPlayerTurn] >= winningScore)
            {
                numUniversesInWhichPlayerWins[currentPlayerTurn] += numUniversesReachingGameState(blah, numUniversesReachingGameStateLookup, predecessorStates);
            }
        }
    }
        for (int currentPlayerTurn : {0, 1})
        {
            cout << "player:" << currentPlayerTurn << " numUniversesInWhichPlayerWins: " << numUniversesInWhichPlayerWins[currentPlayerTurn] << endl;
        }
        cout << *max_element(begin(numUniversesInWhichPlayerWins), end(numUniversesInWhichPlayerWins)) << endl;

#if 0
    int64_t result = 0;
    for (int player1Score = 0; player1Score <= winningScore; player1Score++)
    {
        for (int player2Score = 0; player2Score <= winningScore; player2Score++)
        {
            for (int player1Position = 1; player1Position <= largestSquareNum; player1Position++)
            {
                for (int player2Position = 1; player2Position <= largestSquareNum; player2Position++)
                {
                    for (int currentPlayerTurn : {0, 1})
                    {
                        GameState blah = {
                            {player1Score, player2Score},
                            {player1Position, player2Position},
                            currentPlayerTurn
                        };
                        const int64_t numWaysToReachState = numUniversesReachingGameState(blah, numUniversesReachingGameStateLookup);
                        if (blah.isValid() && blah.playerScores[0] >= winningScore)
                        {
                            result += numWaysToReachState;
                        }
                    }
                }
            }
        }
    }
    cout << "result: " << result << endl;
#endif




#if 0
    int64_t playerScores[2] = {0,0};
    int64_t playerPositions[2] = {player1StartPos, player2StartPos};
    int64_t numTurnsPlayed = 0;
    int64_t nextDiceRoll = 1;

    bool gameEnded = false;
    while (!gameEnded)
    {
        for (int playerId : {0, 1})
        {
            const int64_t diceSum = nextDiceRoll + (nextDiceRoll + 1) + (nextDiceRoll + 2);
            nextDiceRoll += 3;
            numTurnsPlayed++;

            playerPositions[playerId] = (playerPositions[playerId] + diceSum) % 10;
            playerScores[playerId] += (playerPositions[playerId] + 1);

            cout << "Player " << playerId << " rolled: " << diceSum << " moving to " << (playerPositions[playerId] + 1) << " ending up with score: " << playerScores[playerId] << endl;

            if (playerScores[playerId] >= 1000)
            {
                const int64_t losingPlayerScore = playerScores[1 - playerId];
                cout << "Losing player score: " << playerScores[1 - playerId] << endl;
                cout << "numTurnsPlayed: " << numTurnsPlayed << endl;
                const int64_t result = (3 * numTurnsPlayed) * losingPlayerScore;
                cout << "result: " << result << endl;
                gameEnded = true;
                break;
            }
        }
    }
#endif
}

