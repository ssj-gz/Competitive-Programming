#include <iostream>

using namespace std;

int main()
{
    const string playerStartPosPrefix = "Player X starting position: ";
    string player1StartPosLine;
    getline(cin, player1StartPosLine);
    const int player1StartPos = stoi(player1StartPosLine.substr(playerStartPosPrefix.size())) - 1;
    

    string player2StartPosLine;
    getline(cin, player2StartPosLine);
    const int player2StartPos = stoi(player2StartPosLine.substr(playerStartPosPrefix.size())) - 1;

    cout << "player1StartPos: " << player1StartPos << endl;
    cout << "player2StartPos: " << player2StartPos << endl;

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
}

