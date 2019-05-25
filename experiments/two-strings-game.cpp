// Simon St James (ssjgz)
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
//#define NDEBUG
#endif
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

using namespace std;

//#define VERY_VERBOSE
//#define PRINT_COMPUTER_MOVES

struct StateData
{
    int wordLength = -1;
    int grundyNumber = -1;
};

/**
 * Simple implementation of Ukkonen's algorithm:
 *  https://en.wikipedia.org/wiki/Ukkonen's_algorithm
 * for online construction of suffix trees.
 * @author Simon St James, Jan 2017.
 */
class SuffixTreeBuilder
{
    private:
        struct State;
        struct Substring
        {
            Substring()
                : Substring(-1, -1)
            {
            }
            Substring(int startIndex, int endIndex)
                : startIndex(startIndex), endIndex(endIndex)
            {
            }
            int length(int fullStringLength) const
            {
                const auto adjustedEndIndex = (endIndex == openTransitionEnd ? fullStringLength - 1: endIndex - 1);
                const auto length = adjustedEndIndex - startIndex + 2;
                assert(length >= 0);
                return length;
            }
            int startIndex = -1;
            int endIndex = -1;
        };
        struct Transition
        {
            Transition(State *nextState, const Substring& substringFollowed, const string& currentString)
                : nextState(nextState), substringFollowed(substringFollowed)
            {
                if (substringFollowed.startIndex >= 1)
                    firstLetter = currentString[substringFollowed.startIndex - 1];
            }
            int substringLength(int fullStringLength) const
            {
                return substringFollowed.length(fullStringLength);
            }

            State *nextState = nullptr;
            Substring substringFollowed;
            char firstLetter;
            // Grundy info for this transition.
            int64_t numWithGrundy0 = -1;
            int64_t numWithGrundy1 = -1;
            int grundyNumberAfterFirstLetter = -1;
        };
        struct State
        {
            vector<Transition> transitions;
            State* suffixLink = nullptr;
            State* parent = nullptr;
            int index = -1;
            bool isFinal = false;

            StateData data;
        };
    public:
        SuffixTreeBuilder()
        {
            m_states.reserve(1'000'000);

            m_root = createNewState();
            m_auxiliaryState = createNewState();

            for (int i = 0; i < alphabetSize; i++)
            {
                m_auxiliaryState->transitions.push_back(Transition(m_root, Substring(-(i + 1), -(i + 1)), m_currentString));
            }
            m_root->suffixLink = m_auxiliaryState;

            m_s = m_root;
            m_k = 1;
        }
        SuffixTreeBuilder(const SuffixTreeBuilder& other) = delete;
        void appendLetter(char letter)
        {
            m_currentString += letter;
            const auto updateResult = update(m_s, m_k, m_currentString.size());
            m_s = updateResult.first;
            m_k = updateResult.second;
            const auto canonizeResult = canonize(m_s, m_k, m_currentString.size());
            m_s = canonizeResult.first;
            m_k = canonizeResult.second;
        }
        void appendString(const string& stringToAppend)
        {
            for (auto letter : stringToAppend)
            {
                appendLetter(letter);
            }
        }
        int numStates() const
        {
            return m_states.size();
        }
        /**
         * Class used to navigate the suffix tree.  Can be invalidated by making changes to the tree!
         */
        class Cursor
        {
            public:
                Cursor() = default;
                Cursor(const Cursor& other) = default;
                bool operator==(const Cursor& other) const
                {
                    if (m_state != other.m_state)
                        return false;
                    if (m_transition != other.m_transition)
                        return false;
                    if (m_posInTransition != other.m_posInTransition)
                        return false;
                    return true;
                }
                bool operator!=(const Cursor& other) const
                {
                    return !(*this == other);
                }
                bool operator<(const Cursor& other) const
                {
                    if (m_state < other.m_state)
                        return true;
                    if (m_state > other.m_state)
                        return false;
                    if (m_transition < other.m_transition)
                        return true;
                    if (m_transition > other.m_transition)
                        return false;
                    if (m_posInTransition < other.m_posInTransition)
                        return true;
                    if (m_posInTransition > other.m_posInTransition)
                        return false;
                    return false;
                }
                void calcGrundyInfoForTransition()
                {
                    assert(!isOnExplicitState());
                    auto nextState = m_transition->nextState;
                    assert(nextState);
                    const int grundyNumberAtNextState = nextState->data.grundyNumber;
                    assert(grundyNumberAtNextState != -1);
                    m_transition->grundyNumberAfterFirstLetter = -1;
                    assert(m_posInTransition == 1);
                    const int numLettersOnTransition = remainderOfCurrentTransition().length() + 1;
                    cout << "calcGrundyInfoForTransition; posInTransition: " << m_posInTransition << " numLettersOnTransition: " << numLettersOnTransition << endl;
                    if (grundyNumberAtNextState > 0)
                    {
                        m_transition->numWithGrundy0 = numLettersOnTransition / 2;
                        m_transition->numWithGrundy1 = (numLettersOnTransition - 1) / 2;
                        m_transition->grundyNumberAfterFirstLetter = (numLettersOnTransition % 2);

                    }
                    else
                    {
                        m_transition->numWithGrundy1 = numLettersOnTransition / 2;
                        m_transition->numWithGrundy0 = (numLettersOnTransition - 1) / 2;
                        m_transition->grundyNumberAfterFirstLetter = 1 - (numLettersOnTransition % 2);
                    }
                }
                int64_t numOnTransitionWithGrundyNumber(int grundyNumber) const
                {
                    assert(grundyNumber == 0 || grundyNumber == 1);
                    if (grundyNumber == 0)
                        return m_transition->numWithGrundy0;
                    else
                        return m_transition->numWithGrundy1;
                }
                int64_t grundyNumber() const
                {
                    if (isOnExplicitState())
                        return m_state->data.grundyNumber;
                    else
                        return m_transition->grundyNumberAfterFirstLetter ^ ((m_posInTransition + 1) % 2);
                }
                bool isValid() const
                {
                    return m_isValid;
                }
                bool isOnExplicitState() const
                {
                    return (m_transition == nullptr);
                }
                bool isOnFinalState() const
                {
                    assert(isOnExplicitState());
                    return m_state->isFinal;
                }
                StateData& stateData()
                {
                    return m_state->data;
                }
                int stateId() const
                {
                    const int stateId = m_state->index;
                    return stateId;
                }
                int posInTransition() const
                {
                    assert(!isOnExplicitState());
                    return m_posInTransition;
                }
                vector<char> nextLetters() const
                { char nextLetters[27];
                    const int numNextLetters = this->nextLetters(nextLetters);
                    return vector<char>(nextLetters, nextLetters + numNextLetters);
                }
                int nextLetters(char* dest) const
                {
                    int numNextLetters = 0;
                    if (m_transition == nullptr)
                    {
                        for (const auto& transition : m_state->transitions)
                        {
                            const char letter = (*m_string)[transition.substringFollowed.startIndex - 1];
                            *dest = letter;
                            dest++;
                            numNextLetters++;
                        }
                    }
                    else
                    {
                        *dest = (*m_string)[m_transition->substringFollowed.startIndex + m_posInTransition - 1];
                        dest++;
                        numNextLetters++;
                    }
                    return numNextLetters;
                }

                bool canFollowLetter(char letter) const
                {
                    if (isOnExplicitState())
                    {
                        auto nextLetterIterator = getNextLetterIterator();
                        while (nextLetterIterator.hasNext())
                        {
                            if (nextLetterIterator.nextLetter() == letter)
                                return true;
                            nextLetterIterator++;
                        }
                        return false;
                    }
                    else
                    {
                        char nextLetter;
                        nextLetters(&nextLetter);
                        return nextLetter == letter;

                    }
                }

                void sortTransitions()
                {
                    assert(isOnExplicitState());
                    sort(m_state->transitions.begin(), m_state->transitions.end(), [](const Transition& lhs, const Transition& rhs)
                            {
                            return lhs.firstLetter < rhs.firstLetter;
                            });
                }

                void followLetter(char letter)
                {
                    const string& theString = *m_string;
                    if (m_transition == nullptr)
                    {
                        for (auto& transition : m_state->transitions)
                        {
                            if (-transition.substringFollowed.startIndex == (letter - 'a' + 1))
                            {
                                m_transition = &transition;
                                break;
                            }
                            else 
                            {
                                assert(theString[transition.substringFollowed.startIndex - 1] == transition.firstLetter);
                                if (transition.firstLetter == letter)
                                {
                                    m_transition = &transition;
                                    break;
                                }
                            }
                        }
                        assert(m_transition);
                        if (m_transition->substringLength(m_string->size()) == 1)
                        {
                            followToTransitionEnd();
                        }
                        else
                        {
                            m_posInTransition = 1; // We've just followed the 0th.
                        }
                    }
                    else
                    {
                        assert(m_posInTransition != -1);
                        const int transitionStringLength = m_transition->substringLength(m_string->size());
                        assert(m_posInTransition <= transitionStringLength);
                        m_posInTransition++;
                        if (m_posInTransition == transitionStringLength)
                        {
                            m_state = m_transition->nextState;
                            movedToExplicitState();
                        }

                    }
                }
                void followLetters(const string& letters, string::size_type startIndex = 0, string::size_type numLetters = string::npos)
                {
                    if (numLetters == string::npos)
                    {
                        numLetters = letters.size();
                    }
                    while (numLetters > 0)
                    {
                        if (m_transition)
                        {
                            const auto numLeftInTransition = m_transition->substringLength(m_string->size()) - m_posInTransition;
                            int numLettersConsumed = 0;
                            if (numLeftInTransition > numLetters)
                            {
                                m_posInTransition += numLetters;
                                numLettersConsumed = numLetters;
                            }
                            else
                            {
                                numLettersConsumed = numLeftInTransition;
                                followToTransitionEnd();
                            }
                            numLetters -= numLettersConsumed;
                            startIndex += numLettersConsumed;
                        }
                        else
                        {
                            const auto letterToFollow = letters[startIndex];
                            followLetter(letterToFollow);
                            numLetters -= 1;
                            startIndex += 1;
                        }
                    }
                }
                void followNextLetter()
                {
                    followNextLetters(1);
                }
                void followNextLetters(int numLetters)
                {
                    assert(m_transition);
                    assert(remainderOfCurrentTransition().length() >= numLetters);
                    m_posInTransition += numLetters;
                    const int transitionStringLength = m_transition->substringLength(m_string->size());
                    if (m_posInTransition == transitionStringLength)
                    {
                        m_state = m_transition->nextState;
                        movedToExplicitState();
                    }
                }
                class NextLetterIterator
                {
                    public:
                        NextLetterIterator()
                        {
                        }
                        bool hasNext()
                        {
                            return transitionIterator != endtransitionIterator;
                        }
                        char nextLetter()
                        {
                            return (*(cursor->m_string))[transitionIterator->substringFollowed.startIndex - 1];
                        }
                        NextLetterIterator operator++(int)
                        {
                            transitionIterator++;
                            return *this;
                        }
                        Cursor afterFollowingNextLetter()
                        {
                            Cursor afterCursor(*cursor);
                            afterCursor.enterTransitionAndSkipLetter(*transitionIterator);
                            return afterCursor;
                        }
                    private:
                        NextLetterIterator(vector<Transition>& transitions, const Cursor* cursor)
                            : transitionIterator(transitions.begin()),
                            endtransitionIterator(transitions.end()),
                            cursor(cursor)
                    {
                    };
                        friend class Cursor;
                        vector<Transition>::iterator transitionIterator;
                        vector<Transition>::iterator endtransitionIterator;
                        const Cursor* cursor = nullptr;
                };
                friend class NextLetterIterator;
                NextLetterIterator getNextLetterIterator() const
                {
                    assert(isOnExplicitState());
                    return NextLetterIterator(m_state->transitions, this);
                }
                class Substring
                {
                    public:
                        Substring()
                        {
                        }
                        Substring(const Substring& other) = default;
                        Substring(int startIndex, int endIndex)
                            : m_startIndex{startIndex}, m_endIndex{endIndex}
                        {
                        }
                        int startIndex() const {
                            return m_startIndex;
                        }
                        int endIndex() const
                        {
                            return m_endIndex;
                        }
                        int length() const
                        {
                            return m_endIndex - m_startIndex + 1;
                        }
                    private:
                        int m_startIndex = -1;
                        int m_endIndex = -1;
                };
                Substring remainderOfCurrentTransition()
                {
                    assert(!isOnExplicitState());
                    auto startIndex = m_transition->substringFollowed.startIndex - 1 + m_posInTransition;
                    auto endIndex = (m_transition->substringFollowed.endIndex == openTransitionEnd ? static_cast<int>(m_string->size() - 1) : m_transition->substringFollowed.endIndex - 1);

                    return {startIndex, endIndex};
                }
                void followToTransitionEnd()
                {
                    assert(m_transition);
                    m_state = m_transition->nextState;
                    movedToExplicitState();
                } 
                bool canMoveUp()
                {
                    return (m_state != m_root || m_transition);
                }
                char moveUp()
                {
                    assert(canMoveUp());
                    if (m_transition)
                    {
                        assert(m_posInTransition > 0);
                        const char charFollowed = (*m_string)[m_transition->substringFollowed.startIndex - 1 + m_posInTransition - 1];
                        if (m_posInTransition != 1)
                        {
                            m_posInTransition--;
                        }
                        else
                        {
                            movedToExplicitState();
                        }
                        return charFollowed;
                    }
                    else
                    {
                        Transition* transitionFromParent = findTransitionFromParent();
                        m_state = m_state->parent;
                        m_transition = transitionFromParent;
                        m_posInTransition = transitionFromParent->substringLength(m_string->size()) - 1;
                        const char charFollowed = (*m_string)[m_transition->substringFollowed.startIndex - 1 + m_posInTransition];
                        if (m_posInTransition == 0)
                        {
                            movedToExplicitState();
                        }
                        return charFollowed;
                    }
                }

                string stringFollowed() const
                {
                    Cursor copy(*this);
                    string stringFollowedReversed;
                    while (copy.canMoveUp())
                    {
                        stringFollowedReversed += copy.moveUp();
                    }
                    return string(stringFollowedReversed.rbegin(), stringFollowedReversed.rend());
                }

            private:
                Cursor(State* state, const string& str, State* root)
                    : m_state{state}, 
                    m_string{&str},
                    m_root{root},
                    m_isValid{true}
                {
                }

                void movedToExplicitState()
                {
                    assert(m_state);
                    m_transition = nullptr;
                    m_posInTransition = -1;
                }

                void enterTransitionAndSkipLetter(Transition& transition)
                {
                    m_transition = &transition;
                    if (m_transition->substringLength(m_string->size()) == 1)
                    {
                        followToTransitionEnd();
                    }
                    else
                    {
                        m_posInTransition = 1; // We've just followed the 0th.
                    }
                }

                Transition* findTransitionFromParent(State* state = nullptr)
                {
                    if (!state)
                        state = m_state;
                    Transition* transitionFromParent = nullptr;
                    for (Transition& transition : state->parent->transitions)
                    {
                        if (transition.nextState == state)
                        {
                            transitionFromParent = &transition;
                            break;
                        }
                    }
                    assert(transitionFromParent);
                    return transitionFromParent;
                }

                friend class SuffixTreeBuilder;
                State *m_state = nullptr;
                Transition *m_transition = nullptr;
                int m_posInTransition = -1;
                const string* m_string = nullptr;
                State *m_root = nullptr;
                bool m_isValid = false;
        };
        Cursor rootCursor() const
        {
            return Cursor(m_root, m_currentString, m_root);
        }
        static Cursor invalidCursor()
        {
            return Cursor();
        }
    private:
        static const int alphabetSize = 27; // Include the magic '{' for making final states explicit.
        static const int openTransitionEnd = numeric_limits<int>::max();

        string m_currentString;

        vector<State> m_states;
        State *m_root = nullptr;
        State *m_auxiliaryState = nullptr;

        // "Persistent" versions of s & k from Algorithm 2 in Ukkonen's paper!
        State *m_s; 
        int m_k;

        std::pair<State*, int> update(State* s, int k, int i)
        {
            State* oldr = m_root;
            const auto testAndSplitResult = testAndSplit(s, k, i - 1, t(i));
            auto isEndPoint = testAndSplitResult.first;
            auto r = testAndSplitResult.second;
            while (!isEndPoint)
            {
                auto rPrime = createNewState(r);
                r->transitions.push_back(Transition(rPrime, Substring(i, openTransitionEnd), m_currentString));
                if (oldr != m_root)
                {
                    oldr->suffixLink = r;
                }
                oldr = r;
                const auto canonizeResult = canonize(s->suffixLink, k, i - 1);
                s = canonizeResult.first;
                k = canonizeResult.second;

                const auto testAndSplitResult = testAndSplit(s, k, i - 1, t(i));
                isEndPoint = testAndSplitResult.first;
                r = testAndSplitResult.second;
            }

            if (oldr != m_root)
            {
                oldr->suffixLink = s;
            }
            return {s, k};


        }
        pair<bool, State*> testAndSplit(State* s, int k, int p, int letterIndex)
        {
            assert(s);
            if (k <= p)
            {
                const auto tkTransitionIter = findTransitionIter(s, t(k));
                auto sPrime = tkTransitionIter->nextState;
                auto kPrime = tkTransitionIter->substringFollowed.startIndex;
                auto pPrime = tkTransitionIter->substringFollowed.endIndex;
                if (letterIndex == t(kPrime + p - k + 1))
                    return {true, s};
                else
                {
                    s->transitions.erase(tkTransitionIter);
                    auto r = createNewState(s);
                    s->transitions.push_back(Transition(r, Substring(kPrime, kPrime + p - k), m_currentString));
                    r->transitions.push_back(Transition(sPrime, Substring(kPrime + p - k + 1, pPrime), m_currentString));
                    sPrime->parent = r;
                    return {false, r};
                }
            }
            else
            {
                auto tTransitionIter = findTransitionIter(s, letterIndex, false);
                if (tTransitionIter == s->transitions.end())
                    return {false, s};
                else
                    return {true, s};
            }
        }
        std::pair<State*, int> canonize(State* s, int k, int p)
        {
            assert(s);
            if (p < k)
                return {s, k};
            else
            {
                auto tkTransitionIter = findTransitionIter(s, t(k));
                auto sPrime = tkTransitionIter->nextState;
                auto kPrime = tkTransitionIter->substringFollowed.startIndex;
                auto pPrime = tkTransitionIter->substringFollowed.endIndex;
                while (pPrime - kPrime <= p - k)
                {
                    k = k + pPrime - kPrime + 1;
                    s = sPrime;
                    if (k <= p)
                    {
                        tkTransitionIter = findTransitionIter(s, t(k));
                        sPrime = tkTransitionIter->nextState;
                        kPrime = tkTransitionIter->substringFollowed.startIndex;
                        pPrime = tkTransitionIter->substringFollowed.endIndex;
                    }
                }
            }
            return {s, k};
        }
        State *createNewState(State* parent = nullptr)
        {
            m_states.push_back(State());
            State *newState = &(m_states.back());
            newState->parent = parent;
            newState->index = m_states.size() - 1;
            return newState;
        }
        decltype(State::transitions.begin()) findTransitionIter(State* state, int letterIndex, bool assertFound = true)
        {
            for (auto transitionIter = state->transitions.begin(); transitionIter != state->transitions.end(); transitionIter++)
            {
                if (transitionIter->substringFollowed.startIndex >= 0 && t(transitionIter->substringFollowed.startIndex) == letterIndex)
                    return transitionIter;

                if (transitionIter->substringFollowed.startIndex == -letterIndex)
                    return transitionIter;
            }
            if (assertFound)
                assert(false);
            return state->transitions.end();
        };
        int t(int i)
        {
            // Ukkonen's algorithm uses 1-indexed strings throughout and alphabet throughout; adjust for this.
            return m_currentString[i - 1] - 'a' + 1;
        }
};

using Cursor = SuffixTreeBuilder::Cursor;



string A;
string B;

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
        string aPrime;
        string bPrime;
        bool isValid = true;
        bool isKnown = true;
        static GameState invalid()
        {
            GameState invalidGameState;
            invalidGameState.isValid = false;
            return invalidGameState;
        }
        static GameState unknown()
        {
            GameState unknownGameState;
            unknownGameState.isKnown = false;
            return unknownGameState;
        }
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
        bool operator==(const GameState& other) const
        {
            return (aPrime == other.aPrime) && (bPrime == other.bPrime); 
        }
};

bool operator<(const GameState& lhs, const GameState& rhs)
{
    if (lhs.aPrime != rhs.aPrime)
    {
        return lhs.aPrime < rhs.aPrime;
    }
    return lhs.bPrime < rhs.bPrime;
}

class Move
{
    public:
        bool addToA = false;
        char letterAdded = '\0';
        static Move preferredMove(const vector<Move>& moves, PlayState moveOutcome, Player currentPlayer, const GameState& gameState)
        {
            assert(!moves.empty());
            // Pick the first one, arbitrarily - feel free to add your own preferences here :)
            return moves.front();
        }
};

ostream& operator<<(ostream& os, const GameState& gameState)
{
    cout << "A': " << gameState.aPrime << " B': " << gameState.bPrime;
    return os;
}

ostream& operator<<(ostream& os, const Move& move)
{
    cout << "Adding " << move.letterAdded << " to " << (move.addToA ? "A'" : "B'") << endl;
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
    for (char letterToAdd = 'a'; letterToAdd <= 'z'; letterToAdd++)
    {
        if (A.find(gameState.aPrime + letterToAdd) != string::npos)
        {
            Move addToA;
            addToA.addToA = true;
            addToA.letterAdded = letterToAdd;
            moves.push_back(addToA);
        }
        if (B.find(gameState.bPrime + letterToAdd) != string::npos)
        {
            Move addToB;
            addToB.addToA = false;
            addToB.letterAdded = letterToAdd;
            moves.push_back(addToB);
        }
    }

    return moves;
}

GameState gameStateAfterMove(const GameState& gameState, Player currentPlayer, const Move& move)
{
    GameState nextGameState = gameState;
    if (move.addToA)
    {
        nextGameState.aPrime.push_back(move.letterAdded);
    }
    else
    {
        nextGameState.bPrime.push_back(move.letterAdded);
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
                cout << "Computer (" << currentPlayer << ") played move: " << chosenMove << " and thinks it will " << (playState == winForPlayer(currentPlayer) ? "Win" : (playState == Draw ? "Draw" : "Lose")) << ". Game state now: " <<  gameStateAfterMove(gameState, currentPlayer, chosenMove) << endl;
#endif
                updatePlayStateFromMove(chosenMove, false);
            }
        }
    }

#ifdef VERY_VERBOSE
    cout << "At game state: " << gameState << " with player " << currentPlayer << ", the player " << (playState == winForPlayer(currentPlayer) ? "Wins" : playState == winForPlayer(otherPlayer(currentPlayer)) ? "Loses" : "Draws") << endl;

#endif
    playStateForLookup[{gameState, currentPlayer}] = playState;

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

int grundyBlah(int grundyNumberAtNextState, int numLettersUntilNextState)
{
    if (grundyNumberAtNextState > 0 && ((numLettersUntilNextState % 2) == 1))
    {
        return 0;
    }
    if (grundyNumberAtNextState > 0 && ((numLettersUntilNextState % 2) == 0))
    {
        return 1;
    }
    if (grundyNumberAtNextState == 0 && ((numLettersUntilNextState % 2) == 1))
    {
        return 1;
    }
    if (grundyNumberAtNextState == 0 && ((numLettersUntilNextState % 2) == 0))
    {
        return 0;
    }
    assert(false);
    return -1;
}

struct SuffixTreeInfo
{
    int maxGrundy = 1; // Doesn't have to precise - we just want a good upper-bound.
    int numSubstrings = 1; // Empty string.
    vector<int64_t> numWithGrundy = vector<int64_t>(2, 0); // Make room for grundy numbers 0 and 1 - for the others, we'll resize on-the-fly.

};
int initialiseGrundyInfo( Cursor state, SuffixTreeInfo& suffixTreeInfo, int wordLength = 0)
{
    assert(state.isOnExplicitState());
    assert(state.stateData().grundyNumber == -1);
    state.sortTransitions();

    state.stateData().wordLength = wordLength;
    set<int> grundyNumbersAfterNextMove;
    auto nextLetterIterator = state.getNextLetterIterator();
    while (nextLetterIterator.hasNext())
    {
        Cursor afterFollowingLetter = nextLetterIterator.afterFollowingNextLetter();
        Cursor nextState = afterFollowingLetter;
        if (!afterFollowingLetter.isOnExplicitState())
        {
            const int numLettersUntilNextState = afterFollowingLetter.remainderOfCurrentTransition().length() + 1;

            nextState.followToTransitionEnd();
            initialiseGrundyInfo(nextState, suffixTreeInfo, wordLength + numLettersUntilNextState);

            suffixTreeInfo.numSubstrings += numLettersUntilNextState;
            afterFollowingLetter.calcGrundyInfoForTransition();
            suffixTreeInfo.numWithGrundy[0] += afterFollowingLetter.numOnTransitionWithGrundyNumber(0);
            suffixTreeInfo.numWithGrundy[1] += afterFollowingLetter.numOnTransitionWithGrundyNumber(1);
            assert(numLettersUntilNextState > 0);
            grundyNumbersAfterNextMove.insert(afterFollowingLetter.grundyNumber());
        }
        else
        {
            suffixTreeInfo.numSubstrings++;
            initialiseGrundyInfo(afterFollowingLetter, suffixTreeInfo, wordLength + 1);
            grundyNumbersAfterNextMove.insert(nextState.grundyNumber());
        }

        nextLetterIterator++;
    }
    int grundyNumberForState = 0;
    while (grundyNumbersAfterNextMove.find(grundyNumberForState) != grundyNumbersAfterNextMove.end())
    {
        grundyNumberForState++;
    }
    state.stateData().grundyNumber = grundyNumberForState;

    if (grundyNumberForState > suffixTreeInfo.maxGrundy)
    {
        suffixTreeInfo.maxGrundy = grundyNumberForState;
    }

    if (suffixTreeInfo.numWithGrundy.size() < grundyNumberForState + 1)
    {
        suffixTreeInfo.numWithGrundy.resize(grundyNumberForState + 1);
    }
    suffixTreeInfo.numWithGrundy[grundyNumberForState]++;

    return grundyNumberForState;
}

int findGrundyNumberForString(const string& s, SuffixTreeBuilder& suffixTree)
{
    Cursor wordCursor = suffixTree.rootCursor();
    wordCursor.followLetters(s);
    if (wordCursor.isOnExplicitState())
    {
        return wordCursor.stateData().grundyNumber;
    }
    else
    {
        wordCursor.followToTransitionEnd();
        const int grundyNumberAtNextState = wordCursor.stateData().grundyNumber;
        const int numLettersUntilNextState = wordCursor.stateData().wordLength - s.size();
        //cout << "findGrundyNumberForString: " << s << " not explicit - next state: " << wordCursor.stringFollowed() << " grundy: " << grundyNumberAtNextState << endl;
        return grundyBlah(grundyNumberAtNextState, numLettersUntilNextState);
    }
}

string findNthWithoutGrundy(SuffixTreeBuilder& suffixTree, int unwantedGrundyNumber, int64_t N);

#if 1
void findKthOptimised(Cursor aState, SuffixTreeBuilder& bSuffixTree, int64_t& K, const vector<int64_t>& numInBWithoutGrundy, GameState& result)
{
    if (K < 0)
        return;
    assert(aState.isOnExplicitState());
    const auto grundyForState = aState.stateData().grundyNumber;
    if (K - numInBWithoutGrundy[grundyForState] >= 0)
    {
        K -= numInBWithoutGrundy[grundyForState];
    }
    else
    {
        result.aPrime = aState.stringFollowed();
        result.bPrime = findNthWithoutGrundy(bSuffixTree, grundyForState, K);
        assert(result.bPrime != "-");
        result.isValid = true;
        K = -1;
        return;
    }
    if (K == 0)
    {
        result.aPrime = aState.stringFollowed();
        result.bPrime = findNthWithoutGrundy(bSuffixTree, grundyForState, numInBWithoutGrundy[grundyForState]);
        assert(result.bPrime != "-");
        result.isValid = true;

        K = -1;
        return;
    }
    auto nextLetterIterator = aState.getNextLetterIterator();
    while (nextLetterIterator.hasNext())
    {
        if (K < 0)
            return;
        Cursor afterFollowingLetter = nextLetterIterator.afterFollowingNextLetter();
        Cursor nextState = afterFollowingLetter;

        if (!afterFollowingLetter.isOnExplicitState())
        {
            const auto numWithGrundy0 = afterFollowingLetter.numOnTransitionWithGrundyNumber(0);
            const auto numWithGrundy1 = afterFollowingLetter.numOnTransitionWithGrundyNumber(1);
            const auto kAfterFollowingTransition = K - (numWithGrundy0 * numInBWithoutGrundy[0] + numWithGrundy1 * numInBWithoutGrundy[1]);
            bool answerIsOnThisTransition = (kAfterFollowingTransition <= 0 );
            if (answerIsOnThisTransition)
            {
                Cursor onTransition = afterFollowingLetter;
                while (true)
                {
                    const auto grundyNumberOnTransition = onTransition.grundyNumber();
                    if (K >= numInBWithoutGrundy[grundyNumberOnTransition])
                    {
                        K -= numInBWithoutGrundy[grundyNumberOnTransition];
                    }
                    else
                    {
                        result.aPrime = onTransition.stringFollowed();
                        result.bPrime = findNthWithoutGrundy(bSuffixTree, grundyNumberOnTransition, K);
                        assert(result.bPrime != "-");
                        result.isValid = true;

                        K = -1;
                        return;
                    }
                    if (K == 0)
                    {
                        result.aPrime = onTransition.stringFollowed();
                        result.bPrime = findNthWithoutGrundy(bSuffixTree, grundyNumberOnTransition, numInBWithoutGrundy[grundyNumberOnTransition]);
                        assert(result.bPrime != "-");
                        result.isValid = true;

                        K = -1;
                        return;
                    }
                    onTransition.followNextLetter();
                }

            }
            else
            {
                K = kAfterFollowingTransition;
            }

            nextState.followToTransitionEnd();
        }
        findKthOptimised(nextState, bSuffixTree, K, numInBWithoutGrundy, result);

        nextLetterIterator++;
    }
}
#endif

void findNthWithoutGrundy(Cursor state, int unwantedGrundyNumber, int64_t& N, string& result)
{
    assert(state.isOnExplicitState());
    auto nextLetterIterator = state.getNextLetterIterator();
    if (state.stateData().grundyNumber != unwantedGrundyNumber && N > 0)
    {
        N--;
    }
    if (N == 0)
    {
        result = state.stringFollowed();
        N = -1;
        return;
    }
    while (nextLetterIterator.hasNext())
    {
        if (N == -1)
            return;
        Cursor afterFollowingLetter = nextLetterIterator.afterFollowingNextLetter();
        Cursor nextState = afterFollowingLetter;
        if (!afterFollowingLetter.isOnExplicitState())
        {
            const auto numWithGrundy0 = afterFollowingLetter.numOnTransitionWithGrundyNumber(0);
            const auto numWithGrundy1 = afterFollowingLetter.numOnTransitionWithGrundyNumber(1);
            bool answerIsOnThisTransition = false;
            int64_t nAfterFollowingTransition = N;
            if (unwantedGrundyNumber != 0 && numWithGrundy0 > 0)
            {
                nAfterFollowingTransition -= numWithGrundy0;
            }
            if (unwantedGrundyNumber != 1 && numWithGrundy1 > 0)
            {
                nAfterFollowingTransition -= numWithGrundy1;
            }
            if (nAfterFollowingTransition > 0)
            {
                N = nAfterFollowingTransition;
            }
            else
            {
                answerIsOnThisTransition = true;
            }
            if (answerIsOnThisTransition)
            {
                Cursor onTransition = nextLetterIterator.afterFollowingNextLetter();
                while (true)
                {
                    int grundyNumber = onTransition.grundyNumber();

                    if (grundyNumber != unwantedGrundyNumber)
                    {
                        N--;
                    }
                    if (N == 0)
                    {
                        result = onTransition.stringFollowed();
                        N = -1;
                        return;
                    }
                    onTransition.followNextLetter();
                }
            }
            nextState.followToTransitionEnd();
        }
        findNthWithoutGrundy(nextState, unwantedGrundyNumber, N, result);

        nextLetterIterator++;
    }
}

string findNthWithoutGrundy(SuffixTreeBuilder& suffixTree, int unwantedGrundyNumber, int64_t N)
{
    string result = "-";
    findNthWithoutGrundy(suffixTree.rootCursor(), unwantedGrundyNumber, N, result);

    return result;
}


GameState findKthOptimised(SuffixTreeBuilder& aSuffixTree, SuffixTreeBuilder& bSuffixTree, int64_t K, const vector<int64_t>& numInBWithoutGrundy)
{
    GameState result;
    result.aPrime = "-";
    result.bPrime = "-";
    result.isValid = false;
    findKthOptimised(aSuffixTree.rootCursor(), bSuffixTree, K, numInBWithoutGrundy, result);
    if (result.isValid)
    {
        assert(result.aPrime != "-" && result.bPrime != "-");
    }
    return result;
}

vector<string> orderedSubstringsOf(const string& s)
{
    vector<string> substrings;
    for (int start = 0; start < s.size(); start++)
    {
        for (int length = 0; start + length <= s.size(); length++)
        {
            substrings.push_back(s.substr(start, length));
        }
    }
    sort(substrings.begin(), substrings.end());
    substrings.erase(std::unique(substrings.begin(), substrings.end()), substrings.end());
    return substrings;
};


#ifdef BRUTE_FORCE
vector<GameState> 
#else
GameState
#endif
solveOptimised(const string& A, const string& B, int64_t K)
{
    SuffixTreeBuilder aSuffixTree;
    aSuffixTree.appendString(A);
    SuffixTreeInfo aSuffixTreeInfo;
    initialiseGrundyInfo(aSuffixTree.rootCursor(), aSuffixTreeInfo);

    SuffixTreeBuilder bSuffixTree;
    bSuffixTree.appendString(B);
    SuffixTreeInfo bSuffixTreeInfo;
    initialiseGrundyInfo(bSuffixTree.rootCursor(), bSuffixTreeInfo);

    const auto maxGrundy = max(aSuffixTreeInfo.maxGrundy, bSuffixTreeInfo.maxGrundy);
    bSuffixTreeInfo.numWithGrundy.resize(maxGrundy + 1);
    const auto numInBWithGrundy = bSuffixTreeInfo.numWithGrundy;
    const auto numSubstringsOfB = bSuffixTreeInfo.numSubstrings;
    auto numInBWithoutGrundy = numInBWithGrundy;
    for (auto& x : numInBWithoutGrundy)
    {
        x = numSubstringsOfB - x;
    }
#ifdef BRUTE_FORCE
    vector<int64_t> dbgNumInBWithoutGrundy(numInBWithoutGrundy.size());

    const auto substringsOfB = orderedSubstringsOf(B);
    for (const auto& substringOfB : substringsOfB)
    {
        cout << "substringOfB: " << substringOfB << " grundy num: " << findGrundyNumberForString(substringOfB, bSuffixTree) << endl;
        const auto grundyForSubstring = findGrundyNumberForString(substringOfB, bSuffixTree);
        for (int i = 0; i < dbgNumInBWithoutGrundy.size(); i++)
        {
            if (i != grundyForSubstring)
            {
                dbgNumInBWithoutGrundy[i]++;
            }
        }
    }
    assert(dbgNumInBWithoutGrundy == numInBWithoutGrundy);
    cout << "dbgNumInBWithoutGrundy.size():" << dbgNumInBWithoutGrundy.size() << endl;
    for(int i = 0; i < numInBWithoutGrundy.size(); i++)
    {
        cout << "numInBWithoutGrundy[" << i << "] = " << numInBWithoutGrundy[i] << endl;

    }
    for (int unwantedGrundyNumber = 0; unwantedGrundyNumber < numInBWithoutGrundy.size(); unwantedGrundyNumber++)
    {
        int substringWithoutUnwantedNum = 1;
        for (const auto& substringOfB : substringsOfB)
        {
            const auto grundyForSubstring = findGrundyNumberForString(substringOfB, bSuffixTree);
            if (grundyForSubstring != unwantedGrundyNumber)
            {
                const auto nthWithoutGrundy = findNthWithoutGrundy(bSuffixTree, unwantedGrundyNumber, substringWithoutUnwantedNum);
                cout << substringWithoutUnwantedNum << "th substring without grundy: " << unwantedGrundyNumber << " : " << substringOfB << " opt: " << nthWithoutGrundy << endl;
                assert(substringOfB == nthWithoutGrundy);
                substringWithoutUnwantedNum++;
            }

        }

    }
    cout << "substringOfB.size(): " << substringsOfB.size() << " opt: " << numSubstringsOfB << endl;;
    assert(substringsOfB.size() == numSubstringsOfB);

    vector<GameState> results;
    int64_t dbgK = 1;
    while (true)
    {
        const auto kthOptimised = findKthOptimised(aSuffixTree, bSuffixTree, dbgK, numInBWithoutGrundy);
        cout << "bloop dbgK: " << dbgK << " kthOptimised: " << kthOptimised << " valid? " << kthOptimised.isValid << endl;
        if (!kthOptimised.isValid)
        {
            break;
        }
        results.push_back(kthOptimised);
        dbgK++;
    }

    return results;
#else
    const auto result = findKthOptimised(aSuffixTree, bSuffixTree, K, numInBWithoutGrundy);
    return result;
#endif
}


int main(int argc, char** argv)
{
    struct timeval time; 
    gettimeofday(&time,NULL);
    srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

    if (argc == 2)
    {
        const int lengthA = (rand() % 15) + 1;
        const int lengthB = (rand() % 15) + 1;
        //const int64_t lengthA = (rand() % 300'000) + 1;
        //const int64_t lengthB = (rand() % 300'000) + 1;
        const int64_t K = rand() % (lengthA * lengthB) + 1;
        cout << lengthA << " " << lengthB << " " << K << endl;
        const int maxLetterA = rand() % 26 + 1;
        for (int i = 0; i < lengthA; i++)
        {
            cout << static_cast<char>('a' + rand() % maxLetterA);
        }
        cout << endl;
        const int maxLetterB = rand() % 26 + 1;
        for (int i = 0; i < lengthB; i++)
        {
            cout << static_cast<char>('a' + rand() % maxLetterB);
        }
        cout << endl;
        return 0;
    }

    int N;
    cin >> N;
    int M;
    cin >> M;
    int64_t K;
    cin >> K;

    cin >> A;
    cin >> B;

#ifdef BRUTE_FORCE
    SuffixTreeBuilder aSuffixTree;
    aSuffixTree.appendString(A);
    cout << "initialiseGrundyInfo A" << endl;
    SuffixTreeInfo aSuffixTreeInfo;
    initialiseGrundyInfo(aSuffixTree.rootCursor(), aSuffixTreeInfo);

    SuffixTreeBuilder bSuffixTree;
    bSuffixTree.appendString(B);
    cout << "initialiseGrundyInfo B" << endl;
    SuffixTreeInfo bSuffixTreeInfo;
    initialiseGrundyInfo(bSuffixTree.rootCursor(), bSuffixTreeInfo);

    set<GameState> allGameStates;
    for (const auto& aSubstring : orderedSubstringsOf(A))
    {
        for (const auto& bSubstring : orderedSubstringsOf(B))
        {
            GameState gameState;
            gameState.aPrime = aSubstring;
            gameState.bPrime = bSubstring;
            allGameStates.insert(gameState);
        }
    }

    vector<GameState> firstPlayerWinsStates;
    for (const auto& gameState : allGameStates)
    {
        const auto grundyNumber = findGrundyNumberForString(gameState.aPrime, aSuffixTree) ^ findGrundyNumberForString(gameState.bPrime, bSuffixTree);
        cout << "grundyNumber for A' " << gameState.aPrime << " : " << findGrundyNumberForString(gameState.aPrime, aSuffixTree) << endl;
        cout << "grundyNumber for B' " << gameState.bPrime << " : " << findGrundyNumberForString(gameState.bPrime, bSuffixTree) << endl;
        if (findWinner(Player1, gameState) == Player1Win)
        {
            firstPlayerWinsStates.push_back(gameState);
            cout << "First player wins for game state: " << gameState << endl;
            assert(grundyNumber != 0);
        }
        else
        {
            cout << "Second player wins for game state: " << gameState << endl;
            assert(grundyNumber == 0);
        }
    }

    cout << "brute force # winning: " << firstPlayerWinsStates.size() << endl;
    const auto optimisedResults = solveOptimised(A, B, K);
    cout << "brute force # winning: " << firstPlayerWinsStates.size() << " optimised # winning: " << optimisedResults.size() << endl;
    assert(firstPlayerWinsStates.size() == optimisedResults.size());
    int numSubstrings = 0;
    int numOptKnown = 0;
    for (int i = 0; i < firstPlayerWinsStates.size(); i++)
    {
        cout << i << "th win state: " << firstPlayerWinsStates[i] << " opt: ";
        if (optimisedResults[i].isKnown)
            cout << optimisedResults[i];
        else
            cout << "unknown";
        cout << endl;
        if (optimisedResults[i].isKnown)
        {
            assert(optimisedResults[i] == firstPlayerWinsStates[i]);
            numOptKnown++;
        }
        numSubstrings++;
    }
    cout << "numSubstrings: " << numSubstrings << " numOptKnown: " << numOptKnown << endl;
    if (K >= firstPlayerWinsStates.size())
    {
        cout << "no solution" << endl;
    }
    else
    {
        cout << "result: " << firstPlayerWinsStates[K - 1] << endl;
    }
#else
    const auto result = solveOptimised(A, B, K);
    if (result.isValid)
    {
        cout << result.aPrime << endl << result.bPrime << endl;
    }
    else
    {
        cout << "no solution" << endl;
    }
#endif
}



