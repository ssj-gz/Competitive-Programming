// KingFlerple, 2021-12-21, Advent of Code 2021 Day 21: "Dirac Dice", Part 2.
#include <iostream>
#include <vector>
#include <deque>
#include <map>
#include <set>
#include <algorithm>
#include <cassert>

using namespace std;

constexpr auto winningScore = 21;
constexpr auto largestSquareNum = 10;

struct GameState
{
    int playerScores[2] = {-1,-1};
    int playerPositions[2] = {-1, -1}; // 1 ... 10.
    int currentPlayerTurn = -1; // {0, 1}

    // Impose some arbitrary ordering for use in set/ maps.
    auto operator<=>(const GameState& other) const = default;
};

ostream& operator<<(ostream& os, const GameState& gameState)
{
    os << "Gamestate: currentPlayerTurn: " << gameState.currentPlayerTurn << " scores: {" << gameState.playerScores[0] << ", " << gameState.playerScores[1] << "} positions: {" << gameState.playerPositions[0] << ", " << gameState.playerPositions[1] << "}";
    return os;
}

int newPos(int pos, int diceRoll)
{
    assert(diceRoll > 0);
    pos += diceRoll;
    while (pos > largestSquareNum)
        pos -= largestSquareNum;
    return pos;
}

int64_t numUniversesReachingGameState(const GameState& gameState, map<GameState, int64_t>& numUniversesReachingGameStateLookup, map<GameState, vector<GameState>>& predecessorStates)
{
    if (numUniversesReachingGameStateLookup.contains(gameState))
        return numUniversesReachingGameStateLookup[gameState];

    int64_t result = 0;

    for (const auto& predecessor : predecessorStates[gameState])
    {
        result += numUniversesReachingGameState(predecessor, numUniversesReachingGameStateLookup, predecessorStates);
    }

    numUniversesReachingGameStateLookup[gameState] = result;
    cout << "numUniversesReachingGameState: " << gameState << " result: " << result << endl;
    return result;
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

    // Build the list of "predecessor states" for all reachable states.
    // For each state S, this is a list of states P that can reach S by having P's current player take a turn.
    // Such P will occur in predecessorStates[S] as many times as there are turns from P that reach S: could be optimised 
    // so that there is a _count_ of P instead of just duplicating P, but oh well :)
    // S and P will alway be states that are reachable from the initialGameState via valid turns.
    map<GameState, vector<GameState>> predecessorStates;
    set<GameState> seenStates = { initialGameState };
    deque<GameState> toProcess = { initialGameState };
    while (!toProcess.empty())
    {
        const auto gameState = toProcess.front();
        toProcess.pop_front();
        const int numWinningPlayers = static_cast<int>(count_if(begin(gameState.playerScores), end(gameState.playerScores), [](const auto score) { return score >= winningScore; }));
        assert(numWinningPlayers == 0 || numWinningPlayers == 1);
        if (numWinningPlayers == 1)
            continue;
        // Take all possible turns from gameState.
        // Again, could be optimised (many dice outcomes are "redundant" i.e. have the same diceSum), but don't care :p
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

    // Now do the dynamic programming step to compute, for all valid states S, the number of universes in which S is reached.
    // While we are are it, decide if each such S is a Win for either player, and update numUniversesInWhichPlayerWins accordingly.
    int64_t numUniversesInWhichPlayerWins[2] = {0,0};
    for (const auto& [state, predecessors] : predecessorStates)
    {
        for (int playerId : {0, 1})
        {
            if (state.playerScores[playerId] >= winningScore)
            {
                numUniversesInWhichPlayerWins[playerId] += numUniversesReachingGameState(state, numUniversesReachingGameStateLookup, predecessorStates);
            }
        }
    }
    for (int playerId : {0, 1})
    {
        cout << "player:" << playerId << " numUniversesInWhichPlayerWins: " << numUniversesInWhichPlayerWins[playerId] << endl;
    }
    cout << *max_element(begin(numUniversesInWhichPlayerWins), end(numUniversesInWhichPlayerWins)) << endl;
}

