// Simon St James (ssjgz) - 2017-12-03.  Framework for exploring "Optimal Play" Game Theory games, complete with example ("Move The Coins").
//#define BRUTE_FORCE
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <fstream>
#include <limits>
#include <algorithm>
#include <sys/time.h>
#include <cassert>
#include <cmath>

using namespace std;

const int64_t Mod = 1'000'000'007ULL;

class ModNum
{
    public:
        ModNum(int64_t n = 0)
            : m_n{n}
        {
            assert(n >= 0);
        }
        ModNum& operator+=(const ModNum& other)
        {
            m_n = (m_n + other.m_n) % Mod;
            return *this;
        }
        ModNum& operator++(int)
        {
            m_n = (m_n + 1) % Mod;
            return *this;
        }
        ModNum& operator*=(const ModNum& other)
        {
            m_n = (m_n * other.m_n) % Mod;
            return *this;
        }
        int64_t value() const { return m_n; };
    private:
        int64_t m_n;
};

ModNum operator+(const ModNum& lhs, const ModNum& rhs)
{
    ModNum result(lhs);
    result += rhs;
    return result;
}

ModNum operator*(const ModNum& lhs, const ModNum& rhs)
{
    ModNum result(lhs);
    result *= rhs;
    return result;
}

ostream& operator<<(ostream& os, const ModNum& toPrint)
{
    os << toPrint.value();
    return os;
}

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

string asBinary(int64_t n, int numDigits)
{
    string asBinary;
    if (n == 0)
        return "0";
    for (int i = 0; i < numDigits; i++)
    {
        if ((n & 1) == 1)
        {
            asBinary.push_back('1');
        }
        else
        {
            asBinary.push_back('0');
        }

        n >>= 1;
    }

    reverse(asBinary.begin(), asBinary.end());

    return asBinary;
}


//#define VERY_VERBOSE
//#define PRINT_COMPUTER_MOVES

enum PlayState { Player1Win, Player1Lose, Draw };
enum Player { Player1, Player2 };

const string player1Name = "Player 1";
const string player2Name = "Player 2";

enum PlayerType { CPU, Random, Human };

Player otherPlayer(Player player)
{
    if (player == Player1)
        return Player2;
    else
        return Player1;
}

PlayState winForPlayer(Player player)
{
    if (player == Player1)
        return Player1Win;
    else
        return Player1Lose;
}

PlayState loseForPlayer(Player player)
{
    if (player == Player1)
        return Player1Lose;
    else
        return Player1Win;
}

class GameState;
class Move;

vector<Move> movesFor(Player currentPlayer, const GameState& gameState);

class GameState
{
    public:
        vector<int64_t> thresholdForPile;
        vector<int64_t> numStonesInPile;
        bool hasWinningPlayerOverride(Player currentPlayer) const
        {
            // Assume that a GameState that has no moves that lead to a Lose for the
            // other player is a losing state (and vice-versa).
            return false;
        }
        PlayState winningPlayerOverride(Player currentPlayer) const
        {
            assert(false);
            return Player1Win;
        }
};

bool operator<(const GameState& lhs, const GameState& rhs)
{
    return lhs.numStonesInPile < rhs.numStonesInPile;
}

struct StackAndNumStones
{
    int stackIndex = -1;
    int numStonesToAdd = -1;
};

class Move
{
    public:
        vector<StackAndNumStones> numStonesToAddToStacks;
        static Move preferredMove(const vector<Move>& moves, PlayState moveOutcome, Player currentPlayer, const GameState& gameState)
        {
            assert(!moves.empty());
            // Pick the first one, arbitrarily - feel free to add your own preferences here :)
            return moves.front();
        }
};

ostream& operator<<(ostream& os, const GameState& gameState)
{
    for (const auto x : gameState.numStonesInPile)
    {
        os << x << " ";
    }
    return os;
}

ostream& operator<<(ostream& os, const Move& move)
{
    cout << "Choose " << move.numStonesToAddToStacks.size() << " stacks" << endl;
    for (const auto x : move.numStonesToAddToStacks)
    {
        os << "Take " << x.numStonesToAdd << " from stack: " << x.stackIndex << endl;
    }
    return os;
}

ostream& operator<<(ostream& os, Player player)
{
    if (player == Player1)
        os << player1Name;
    else
        os << player2Name;

    return os;
}

ostream& operator<<(ostream& os, PlayState playState)
{
    if (playState == Player1Win)
        os << Player1 << " wins";
    else if (playState == Player1Lose)
        os << Player2 << " wins";
    else
        os << "Draw";

    return os;
}


map<pair<GameState, Player>, PlayState> playStateForLookup;

vector<Move> movesFor(Player currentPlayer, const GameState& gameState)
{
    vector<Move> moves;

    // Choose one stack.
    for (int stackIndex = 0; stackIndex < gameState.numStonesInPile.size(); stackIndex++)
    {
        for (int numStonesToAdd = 1; numStonesToAdd + gameState.numStonesInPile[stackIndex] <= gameState.thresholdForPile[stackIndex]; numStonesToAdd++)
        {
            Move move;
            if (gameState.thresholdForPile[stackIndex] % (numStonesToAdd + gameState.numStonesInPile[stackIndex]) == 0)
            {
                move.numStonesToAddToStacks.push_back({stackIndex, numStonesToAdd});
                moves.push_back(move);
            }
        }
    }
    // Choose two stacks.
    for (int stack1Index = 0; stack1Index < gameState.numStonesInPile.size(); stack1Index++)
    {
        if (gameState.numStonesInPile[stack1Index] == 0)
        {
            continue;
        }
        for (int stack2Index = stack1Index + 1; stack2Index < gameState.numStonesInPile.size(); stack2Index++)
        {
            if (gameState.numStonesInPile[stack2Index] == 0)
            {
                continue;
            }

            for (int numStonesToAdd1 = 1; numStonesToAdd1 + gameState.numStonesInPile[stack1Index] <= gameState.thresholdForPile[stack1Index]; numStonesToAdd1++)
            {
                for (int numStonesToAdd2 = 1; numStonesToAdd2 + gameState.numStonesInPile[stack2Index] <= gameState.thresholdForPile[stack2Index]; numStonesToAdd2++)
                {
                    if (gameState.thresholdForPile[stack1Index] % (numStonesToAdd1 + gameState.numStonesInPile[stack1Index]) == 0 &&
                        gameState.thresholdForPile[stack2Index] % (numStonesToAdd2 + gameState.numStonesInPile[stack2Index]) == 0)
                    {
                        Move move;
                        move.numStonesToAddToStacks.push_back({stack1Index, numStonesToAdd1});
                        move.numStonesToAddToStacks.push_back({stack2Index, numStonesToAdd2});
                        moves.push_back(move);
                    }
                }
            }
        }
    }

    return moves;
}

GameState gameStateAfterMove(const GameState& gameState, Player currentPlayer, const Move& move)
{
    GameState nextGameState;
    nextGameState.thresholdForPile = gameState.thresholdForPile;
    nextGameState.numStonesInPile = gameState.numStonesInPile;
    assert(move.numStonesToAddToStacks.size() == 1 || move.numStonesToAddToStacks.size() == 2);

    for (const auto x : move.numStonesToAddToStacks)
    {
        assert(x.stackIndex >= 0 && x.stackIndex < nextGameState.numStonesInPile.size());
        nextGameState.numStonesInPile[x.stackIndex] += x.numStonesToAdd;
        assert(nextGameState.numStonesInPile[x.stackIndex] <= nextGameState.thresholdForPile[x.stackIndex]);
    }

    return nextGameState;
}

PlayState findWinnerAux(Player currentPlayer, const GameState& gameState, PlayerType player1Type, PlayerType player2Type, bool isBruteForceMoveSearch)
{
    const auto playThisMoveAsType = (currentPlayer == Player1 ? player1Type : player2Type);
    const bool playThisMoveInteractively = !isBruteForceMoveSearch && (playThisMoveAsType == Human);
    const bool playThisMoveRandomly = !isBruteForceMoveSearch && (playThisMoveAsType == Random);
    if (isBruteForceMoveSearch && playStateForLookup.find({gameState, currentPlayer}) != playStateForLookup.end())
    {
        // Don't use the cache if we're interactive/ random: it will know all losing states from earlier dry-runs,
        // and if the human player is in a losing state, won't give them a chance to make a move!
        return playStateForLookup[{gameState, currentPlayer}];
    }

    // Assume a loss by default.
    PlayState playState = loseForPlayer(currentPlayer);

    vector<Move> winningMovesForCurrentPlayer;
    vector<Move> losingMovesForCurrentPlayer;
    vector<Move> drawingMovesForCurrentPlayer;


    auto updatePlayStateFromMove = [&playState, &gameState, currentPlayer, player1Type, player2Type, &winningMovesForCurrentPlayer, &drawingMovesForCurrentPlayer, &losingMovesForCurrentPlayer]
        (const Move& move, bool isBruteForceMoveSearch)
    {
        const auto newGameState = gameStateAfterMove(gameState, currentPlayer, move);
        const auto result = findWinnerAux(otherPlayer(currentPlayer), newGameState, player1Type, player2Type, isBruteForceMoveSearch);
        if (result == winForPlayer(currentPlayer))
        {
            winningMovesForCurrentPlayer.push_back(move);
        }
        else if (result == Draw)
        {
            drawingMovesForCurrentPlayer.push_back(move);
        }
        else
        {
            losingMovesForCurrentPlayer.push_back(move);
        }
        if (result == winForPlayer(currentPlayer))
        {
            playState = winForPlayer(currentPlayer);
        }
        else if (playState != winForPlayer(currentPlayer) && result == Draw)
        {
            playState = Draw;
        }

    };

    auto readInt = [](const string& message)
    {
        while (true)
        {
            cout << message << endl;
            int value;
            cin >> value;

            if (!cin)
            {
                cout << "Invalid input; please try again" << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            else
                return value;
        }
    };

    if (gameState.hasWinningPlayerOverride(currentPlayer))
    {
        playState = gameState.winningPlayerOverride(currentPlayer);
                
    }
    else
    {
        const vector<Move> availableMoves = movesFor(currentPlayer, gameState);

        if (playThisMoveInteractively)
        {
            if (!availableMoves.empty())
            {
                stringstream messagePromptStream;
                messagePromptStream << "Player " << currentPlayer << ", it is your move; game state is " << gameState << endl;
                for (int i = 0; i < availableMoves.size(); i++)
                {
                    messagePromptStream << i << ":  " << availableMoves[i] << endl;
                }
                int moveIndex = -1;
                while (true)
                {
                    moveIndex = readInt(messagePromptStream.str());
                    if (moveIndex < 0 || moveIndex >= availableMoves.size())
                    {
                        cout << "Invalid input" << endl;
                    }
                    else
                        break;
                }
                const auto chosenMove = availableMoves[moveIndex];
                cout << "You chose move " << chosenMove << " game state is now: " <<  gameStateAfterMove(gameState, currentPlayer, chosenMove) << endl;
                updatePlayStateFromMove(chosenMove, false);
            }
            else
            {
                cout << "Player " << currentPlayer << " - you have no moves available.  You lose; good-day, Sir!" << endl;
            }
        }
        else if (playThisMoveRandomly)
        {
            if (!availableMoves.empty())
            {
                const int moveIndex = rand() % availableMoves.size();
                const auto chosenMove = availableMoves[moveIndex];
                cout << "Randomly chose move " << chosenMove << " game state is now: " <<  gameStateAfterMove(gameState, currentPlayer, chosenMove) << endl;
                updatePlayStateFromMove(chosenMove, false);
            }
        }
        else
        {
            if (!isBruteForceMoveSearch)
            {
#ifdef PRINT_COMPUTER_MOVES
                cout << "Computer's turn; thinking ..." << endl;
#endif
            }
            for (const auto& move : availableMoves)
            {
                const auto oldPlayState = playState;
                updatePlayStateFromMove(move, true);

#ifdef VERY_VERBOSE
                    cout << "The move " << move << " from state: " << gameState << " is a " << (playState == winForPlayer(currentPlayer) ? "Win" : (playState == Draw ? "Draw" : "Lose")) << " for player " << currentPlayer << endl;
#endif
                                                  
            }

            if (!isBruteForceMoveSearch && !availableMoves.empty())
            {
                Move chosenMove;
                if (playState == winForPlayer(currentPlayer))
                {
                    assert(!winningMovesForCurrentPlayer.empty());
                    chosenMove = Move::preferredMove(winningMovesForCurrentPlayer, playState, currentPlayer, gameState);
                }
                else if (playState == Draw)
                {
                    assert(!drawingMovesForCurrentPlayer.empty());
                    chosenMove = Move::preferredMove(drawingMovesForCurrentPlayer, playState, currentPlayer, gameState);
                }
                else
                {
                    assert(!losingMovesForCurrentPlayer.empty());
                    chosenMove = Move::preferredMove(losingMovesForCurrentPlayer, playState, currentPlayer, gameState);
                }
#ifdef PRINT_COMPUTER_MOVES
                cout << "Computer (" << currentPlayer << ") played move: " << chosenMove << " and thinks it will " << (playState == winForPlayer(currentPlayer) ? "Win" : (playState == Draw ? "Draw" : "Lose")) << ". Game state now: " <<  gameStateAfterMove(gameState, currentPlayer, chosenMove) << "(" << winningMovesForCurrentPlayer.size() << " winning moves)" << endl;
#endif
                updatePlayStateFromMove(chosenMove, false);
            }
        }
    }

#ifdef VERY_VERBOSE
    cout << "At game state: " << gameState << " with player " << currentPlayer << ", the player " << (playState == winForPlayer(currentPlayer) ? "Wins" : playState == winForPlayer(otherPlayer(currentPlayer)) ? "Loses" : "Draws") << endl;

#endif
    playStateForLookup[{gameState, currentPlayer}] = playState;
    if (playStateForLookup.size() % 1000 == 0)
    {
        cerr << "playStateForLookup size: " << playStateForLookup.size() << endl;
    }

    return playState;
}

PlayState findWinner(Player firstPlayer, const GameState& initialGameState, PlayerType player1Type = CPU, PlayerType player2Type = CPU)
{
    cout << "Initial game state: " << initialGameState << endl;
    playStateForLookup.clear();
    const auto result = findWinnerAux(firstPlayer, initialGameState, player1Type, player2Type, false);

    if (player1Type == Human || player2Type == Human)
    {
        const auto interactivePlayer = (player1Type == Human ? Player1 : Player2);
        cout << "Result of interactive game: you played as " << interactivePlayer << "; " << firstPlayer << " took the first turn; " << result << endl;
    }

    return result;
}

int numFactors(int64_t number, const vector<int>& primesUpToRootMaxN)
{
    vector<std::pair<int64_t, int64_t>> primeFactorisation;
    for (const auto prime : primesUpToRootMaxN)
    {
        const auto oldNumber = number;
        int numOfPrime = 0;
        while (number % prime == 0)
        {
            numOfPrime++;
            number = number / prime;
        }
        if (numOfPrime > 0)
        {
            primeFactorisation.push_back({prime, numOfPrime});
        }
        if (prime * prime > oldNumber)
            break;
    }
    if (number > 1)
    {
        // If we ever exit the loop above with number != 1, number must
        // be prime.
        primeFactorisation.push_back({number, 1});
    }

    int numFactors = 1;
    for (const auto primeFactor : primeFactorisation)
    {
        numFactors *= (primeFactor.second + 1);
    }
    return numFactors;
}

constexpr auto maxX = 1'000'000'000UL;

ModNum solveBruteForce(const vector<int64_t>& thresholds, int64_t numPeople)
{
    const int numThresholds = thresholds.size();

    vector<int> thresholdIndexForPerson(numPeople, 0);
    ModNum numInitialWinStates = 0;
    int totalInitialStates = 1;
    for (int i = 0; i < numPeople; i++)
    {
        totalInitialStates *= numThresholds;
    }
    int numInitialStatesDone = 0;
    cerr << "totalInitialStates: " << totalInitialStates << endl;
    while (true)
    {
        playStateForLookup.clear();
        GameState initialState;
        initialState.numStonesInPile = vector<int64_t>(numPeople, 1);
        for (const auto thresholdIndex : thresholdIndexForPerson)
        {
            initialState.thresholdForPile.push_back(thresholds[thresholdIndex]);
        }
        const auto result = findWinner(Player1, initialState, CPU, CPU);
        cout << "With thresholds: ";
        for (const auto x : initialState.thresholdForPile)
        {
            cout << x << " ";
        }
        if (result == Player1Win)
        {
            cout << " Irshad wins";
            numInitialWinStates++;
        }
        else
        {
            cout << " Irshad loses";
        }
        cout << endl;

        int index = 0;
        while (index < numPeople && thresholdIndexForPerson[index] == numThresholds - 1)
        {
            thresholdIndexForPerson[index] = 0;
            index++;
        }
        if (index == numPeople)
            break;

        thresholdIndexForPerson[index]++;

        numInitialStatesDone++;
        if (numInitialStatesDone % 100 == 0)
        {
            cerr << "Done " << numInitialStatesDone << " initial states out of " << totalInitialStates << endl;
        }
    }


    return numInitialWinStates;
}

ModNum solveOptimised1(const vector<int64_t>& thresholds, int64_t numPeople, const vector<int>& primesUpToRootMaxN)
{
    const int numThresholds = thresholds.size();
    vector<int> numStonesInNimPile(numThresholds);
    for (int i = 0; i < numThresholds; i++)
    {
        numStonesInNimPileForThreshold[i] = numFactors(thresholds[i], primesUpToRootMaxN) - 1;
    }
    auto areBitsDivisibleBy3 = [](const vector<int>& numStonesInPile)
    {
        const int maxBits = 20;
        vector<int> numOfBits(maxBits + 1, 0);
        for (const auto x : numStonesInPile)
        {
            const auto bin = asBinary(x, maxBits);
            for (int i = maxBits - 1; i >= 0; i--)
            {
                if (bin[i] == '1')
                    numOfBits[i]++;
            }
        }
        for (const auto x : numOfBits)
        {
            if ((x % 3) != 0)
                return false;
        }
        return true;
    };
    vector<int> thresholdIndexForPerson(numPeople, 0);
    ModNum numInitialWinStates = 0;
    int totalInitialStates = 1;
    for (int i = 0; i < numPeople; i++)
    {
        totalInitialStates *= numThresholds;
    }
    int numInitialStatesDone = 0;
    cerr << "totalInitialStates: " << totalInitialStates << endl;
    while (true)
    {
        playStateForLookup.clear();
        vector<int> numStonesInPile;
        for (const auto thresholdIndex : thresholdIndexForPerson)
        {
            numStonesInPile.push_back(numStonesInNimPileForThreshold[thresholdIndex]);
        }
        if (!areBitsDivisibleBy3(numStonesInPile))
        {
            cout << " Irshad wins";
            numInitialWinStates++;
        }
        else
        {
            cout << " Irshad loses";
        }
        cout << endl;

        int index = 0;
        while (index < numPeople && thresholdIndexForPerson[index] == numThresholds - 1)
        {
            thresholdIndexForPerson[index] = 0;
            index++;
        }
        if (index == numPeople)
            break;

        thresholdIndexForPerson[index]++;

        numInitialStatesDone++;
        if (numInitialStatesDone % 100 == 0)
        {
            cerr << "Done " << numInitialStatesDone << " initial states out of " << totalInitialStates << endl;
        }
    }
    return numInitialWinStates;
}


int main(int argc, char** argv)
{
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time; 
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int T = 1;
        cout << T << endl;
        for (int t = 0; t < T; t++)
        {
            const int N = 1 + rand() % 5;
            const int K = 1 + rand() % 5;
            const int maxX = 24;

            cout << N << " " << K << endl;
            for (int i = 0; i < N; i++)
            {
                cout << (1 + rand() % maxX) << endl;
            }
        }
        return 0;
    }

    const int rootMaxX = sqrt(maxX);
    vector<char> isPrime(1'000'000 + 1, true);

    // Sieve of Eratosthenes.  To help with factorising numbers, compute a lookup table for
    // primes up to 1'000'000, but only store a list of primes for up to rootMaxX.
    vector<int> primesUpToRootMaxN;
    for (int64_t factor = 2; factor <= 1'000'000; factor++)
    {
        const bool isFactorPrime = isPrime[factor];
        assert(factor < isPrime.size());
        if (isFactorPrime)
        {
            if (factor <= rootMaxX)
            {
                primesUpToRootMaxN.push_back(factor);
            }
        }
        for (int64_t multiple = factor * factor; multiple < isPrime.size(); multiple += factor)
        {
            if (!isPrime[multiple] && !isFactorPrime)
            {
                // This multiple has already been marked, and since factor is not prime,
                // all subsequent multiples will already have been marked (by any of the
                // prime factors of factor!), so we can stop here.
                break;
            }
            isPrime[multiple] = false;
        }
    }


    auto areBitsDivisibleBy3 = [](const vector<int64_t>& numStonesInPile)
    {
        const int maxBits = 20;
        vector<int> numOfBits(maxBits + 1, 0);
        for (const auto x : numStonesInPile)
        {
            const auto bin = asBinary(x, maxBits);
            for (int i = maxBits - 1; i >= 0; i--)
            {
                if (bin[i] == '1')
                    numOfBits[i]++;
            }
        }
        for (const auto x : numOfBits)
        {
            if ((x % 3) != 0)
                return false;
        }
        return true;
    };

    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        int numThresholds;
        cin >> numThresholds;

        int64_t numPeople;
        cin >> numPeople;

        vector<int64_t> thresholds(numThresholds);
        for (auto& threshold : thresholds)
        {
            cin >> threshold;
            //cout << "threshold: " << threshold << endl;
        }
#ifdef BRUTE_FORCE
        const auto solutionBruteForce = solveBruteForce(thresholds, numPeople);
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
        const auto solutionOptimised1 = solveOptimised1(thresholds, numPeople, primesUpToRootMaxN);
        cout << "solutionOptimised1: " << solutionOptimised1 << endl;
#else
        const auto solutionOptimised1 = solveOptimised1(thresholds, numPeople, primesUpToRootMaxN);
        cout << "solutionOptimised1: " << solutionOptimised1 << endl;
#endif

    }
    assert(cin);

#if 0
    set<GameState> gameStates;
    for (const auto& blah : playStateForLookup)
    {
        GameState blee = blah.first.first;
        sort(blee.numStonesInPile.begin(), blee.numStonesInPile.end());
        gameStates.insert(blee);
    }
    cout << "Num distinct game states: " << gameStates.size() << endl;

    for (const auto& gameState : gameStates)
    {
        auto printIsWinner = [&areBitsDivisibleBy3](const auto& gameState, const auto player)
        {
            if (playStateForLookup.find({gameState, player}) == playStateForLookup.end())
                return false;
            
            const auto result = playStateForLookup[{gameState, player}];
            const bool isWinForPlayer = (result == winForPlayer(player));
            //if (isWinForPlayer)
                //return true;
            int numNonEmpty = 0;
            for (const auto x : gameState.numStonesInPile)
            {
                if (x != 0)
                    numNonEmpty++;
            }
            cout << "Game state: " << gameState << " with " << numNonEmpty << " piles is a " << (isWinForPlayer ? "win" : "loss") << " for current player; areBitsDivisibleBy3: " << areBitsDivisibleBy3(gameState.numStonesInPile) << endl;
            assert(isWinForPlayer == !areBitsDivisibleBy3(gameState.numStonesInPile));
            //for (const auto x : gameState.numStonesInPile)
            //{
                //cout << asBinary(x, 5) << endl;
            //}

            return true;
        };

        if (printIsWinner(gameState, Player1))
            continue;
        const bool blah = printIsWinner(gameState, Player2);
        assert(blah);
    }
#endif

}

