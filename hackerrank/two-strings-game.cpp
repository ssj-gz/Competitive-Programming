// Simon St James (ssjgz) - 2019-05-25
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <stack>
#include <set>
#include <algorithm>
#include <cassert>

using namespace std;

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
                    // Use the fact that the last word in a transition has grundy
                    // number 0 if the nextState has grundy > 0 and 1 otherwise, 
                    // coupled with the fact that the grundy number for words in a 
                    // transition alternates between 0 and 1 so calculate 
                    // numWithGrundy0, numWithGrundy1 and grundyNumberAfterFirstLetter.
                    assert(!isOnExplicitState());
                    auto nextState = m_transition->nextState;
                    assert(nextState);
                    const int grundyNumberAtNextState = nextState->data.grundyNumber;
                    assert(grundyNumberAtNextState != -1);
                    m_transition->grundyNumberAfterFirstLetter = -1;
                    assert(m_posInTransition == 1);
                    const int numLettersOnTransition = remainderOfCurrentTransition().length() + 1;
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
                    {
                        // Again, use the fact that words along a transition alternate between
                        // a grundy number of 0 an a grundy number of 1.
                        return m_transition->grundyNumberAfterFirstLetter ^ ((m_posInTransition + 1) % 2);
                    }
                }
                bool isOnExplicitState() const
                {
                    return (m_transition == nullptr);
                }
                StateData& stateData()
                {
                    return m_state->data;
                }

                void sortTransitions()
                {
                    // Ensure that a DFS of the suffix automaton would generate all substrings in lexicographic order.
                    assert(isOnExplicitState());
                    sort(m_state->transitions.begin(), m_state->transitions.end(), [](const Transition& lhs, const Transition& rhs)
                            {
                            return lhs.firstLetter < rhs.firstLetter;
                            });
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

class GameState
{
    public:
        GameState()
        {
            isValid = false;
        }
        GameState(const string& aPrime, const string& bPrime)
            : aPrime(aPrime), bPrime(bPrime)
        {
        }
        string aPrime;
        string bPrime;
        bool isValid = true;
        static GameState invalid()
        {
            GameState invalidGameState;
            invalidGameState.isValid = false;
            return invalidGameState;
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

int grundyBlah(int grundyNumberAtNextState, int numLettersUntilNextState)
{
    cout << "grundyBlah: grundyNumberAtNextState: " << grundyNumberAtNextState << " numLettersUntilNextState: " << numLettersUntilNextState << endl;
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
    int64_t numSubstrings = 1; // Empty string.
    vector<int64_t> numWithGrundy = vector<int64_t>(2, 0); // Make room for grundy numbers 0 and 1 - for the others, we'll resize on-the-fly.

};
void initialiseGrundyInfo( Cursor state, SuffixTreeInfo& suffixTreeInfo)
{
    // Would be a very simple recursive algorithm, but would give stackoverflow on larger testcases :(

    // * Sorts the transitions for each state in lexicographic order;
    // * Updates wordLength for each state;
    // * Find grundyNumber for each state;
    // * Calculates grundy info for each transition (grundy number after following
    //   first letter; number of words with grundy numbers 0 or 1 on this transition;
    //   etc).
    //
    // All reasonably self-explanatory, I hope :)
    enum Phase { Initializing, ProcessingChildren, AfterRecurse };
    struct StackFrame
    {
        Cursor state;
        int wordLength = 0;
        Phase phase = Initializing;
        Cursor::NextLetterIterator nextLetterIterator;
        Cursor afterFollowingLetter;
        set<int> grundyNumbersAfterNextMove;
    };
    StackFrame initialStackFrame;
    initialStackFrame.state = state;
    stack<StackFrame> stackFrames;
    stackFrames.push(initialStackFrame);

    while (!stackFrames.empty())
    {
        StackFrame& stackFrame = stackFrames.top();
        auto& state = stackFrame.state;

        switch (stackFrame.phase)
        {
            case Initializing:
                assert(state.isOnExplicitState());
                assert(state.stateData().grundyNumber == -1);
                state.sortTransitions();

                state.stateData().wordLength = stackFrame.wordLength;
                stackFrame.nextLetterIterator = state.getNextLetterIterator();
                stackFrame.phase = ProcessingChildren;
                continue;
            case ProcessingChildren:
                if (stackFrame.nextLetterIterator.hasNext())
                {
                    stackFrame.afterFollowingLetter = stackFrame.nextLetterIterator.afterFollowingNextLetter();
                    Cursor nextState = stackFrame.afterFollowingLetter;
                    if (!stackFrame.afterFollowingLetter.isOnExplicitState())
                    {
                        const int numLettersUntilNextState = stackFrame.afterFollowingLetter.remainderOfCurrentTransition().length() + 1;

                        nextState.followToTransitionEnd();
                        StackFrame nextStackFrame;
                        nextStackFrame.state = nextState;
                        nextStackFrame.wordLength = stackFrame.wordLength + numLettersUntilNextState;
                        stackFrames.push(nextStackFrame);
                        stackFrame.phase = AfterRecurse;
                        continue;

                    }
                    else
                    {
                        suffixTreeInfo.numSubstrings++;
                        StackFrame nextStackFrame;
                        nextStackFrame.state = nextState;
                        nextStackFrame.wordLength = stackFrame.wordLength + 1;
                        stackFrames.push(nextStackFrame);
                        stackFrame.phase = AfterRecurse;
                        continue;

                    }
                }
                else
                {
                    // The grundyNumberForState is the mex of the grundy numbers after making each possible move.
                    int grundyNumberForState = 0;
                    while (stackFrame.grundyNumbersAfterNextMove.find(grundyNumberForState) != stackFrame.grundyNumbersAfterNextMove.end())
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
                    stackFrames.pop();
                    continue;
                }
                break;
            case AfterRecurse:
                    if (!stackFrame.afterFollowingLetter.isOnExplicitState())
                    {
                        const int numLettersUntilNextState = stackFrame.afterFollowingLetter.remainderOfCurrentTransition().length() + 1;
                        suffixTreeInfo.numSubstrings += numLettersUntilNextState;
                        stackFrame.afterFollowingLetter.calcGrundyInfoForTransition();
                        suffixTreeInfo.numWithGrundy[0] += stackFrame.afterFollowingLetter.numOnTransitionWithGrundyNumber(0);
                        suffixTreeInfo.numWithGrundy[1] += stackFrame.afterFollowingLetter.numOnTransitionWithGrundyNumber(1);
                        assert(numLettersUntilNextState > 0);
                        stackFrame.grundyNumbersAfterNextMove.insert(stackFrame.afterFollowingLetter.grundyNumber());
                    }
                    else
                    {
                        Cursor nextState = stackFrame.afterFollowingLetter;
                        stackFrame.grundyNumbersAfterNextMove.insert(nextState.grundyNumber());
                    }
                    stackFrame.nextLetterIterator++;
                    stackFrame.phase = ProcessingChildren;
                break;
        }
    }
}

string findNthWithoutGrundy(SuffixTreeBuilder& suffixTree, int unwantedGrundyNumber, int64_t N);

void findKthWinningGameState(Cursor aState, SuffixTreeBuilder& bSuffixTree, int64_t& K, const vector<int64_t>& numInBWithoutGrundy, GameState& result)
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
        result = GameState(aState.stringFollowed(), findNthWithoutGrundy(bSuffixTree, grundyForState, K));
        K = -1;
        return;
    }
    if (K == 0)
    {
        result = GameState(aState.stringFollowed(), findNthWithoutGrundy(bSuffixTree, grundyForState, numInBWithoutGrundy[grundyForState]));

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
                        result = GameState(onTransition.stringFollowed(), findNthWithoutGrundy(bSuffixTree, grundyNumberOnTransition, K));

                        K = -1;
                        return;
                    }
                    if (K == 0)
                    {
                        result = GameState(onTransition.stringFollowed(), findNthWithoutGrundy(bSuffixTree, grundyNumberOnTransition, numInBWithoutGrundy[grundyNumberOnTransition]));

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
        findKthWinningGameState(nextState, bSuffixTree, K, numInBWithoutGrundy, result);

        nextLetterIterator++;
    }
}

void findNthWithoutGrundy(Cursor state, int unwantedGrundyNumber, int64_t& N, string& result)
{
    // Would be a very simple recursive algorithm, but would give stackoverflow on larger testcases :(
    enum Phase { Initializing, ProcessingChildren, AfterRecurse };
    struct StackFrame
    {
        Cursor state;
        Phase phase = Initializing;
        Cursor::NextLetterIterator nextLetterIterator;
        Cursor afterFollowingLetter;
    };
    StackFrame initialStackFrame;
    initialStackFrame.state = state;
    stack<StackFrame> stackFrames;
    stackFrames.push(initialStackFrame);

    while (!stackFrames.empty())
    {
        StackFrame& stackFrame = stackFrames.top();
        auto& state = stackFrame.state;

        switch (stackFrame.phase)
        {
            case Initializing:
                assert(state.isOnExplicitState());
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

                stackFrame.nextLetterIterator = state.getNextLetterIterator();
                stackFrame.phase = ProcessingChildren;
                continue;
            case ProcessingChildren:
                {
                    if (stackFrame.nextLetterIterator.hasNext())
                    {
                        stackFrame.afterFollowingLetter = stackFrame.nextLetterIterator.afterFollowingNextLetter();
                        Cursor nextState = stackFrame.afterFollowingLetter;
                        if (!stackFrame.afterFollowingLetter.isOnExplicitState())
                        {
                            nextState.followToTransitionEnd();

                            const auto numWithGrundy0 = stackFrame.afterFollowingLetter.numOnTransitionWithGrundyNumber(0);
                            const auto numWithGrundy1 = stackFrame.afterFollowingLetter.numOnTransitionWithGrundyNumber(1);
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
                                // Walk along this transition letter-by-letter, as this is where we'll find the nth word
                                // without unwantedGrundyNumber.
                                // We'll only ever do this once per testcase, so it only contributes O(N) overall.
                                Cursor onTransition = stackFrame.nextLetterIterator.afterFollowingNextLetter();
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

                        }
                        StackFrame nextStackFrame;
                        nextStackFrame.state = nextState;
                        stackFrames.push(nextStackFrame);
                        stackFrame.phase = AfterRecurse;
                        continue;
                    }
                    else
                    {
                        stackFrames.pop();
                        continue;
                    }
                }
                break;
            case AfterRecurse:

                stackFrame.nextLetterIterator++;
                stackFrame.phase = ProcessingChildren;
                break;
        }
    }
}

string findNthWithoutGrundy(SuffixTreeBuilder& suffixTree, int unwantedGrundyNumber, int64_t N)
{
    string result = "-";
    findNthWithoutGrundy(suffixTree.rootCursor(), unwantedGrundyNumber, N, result);

    return result;
}

GameState findKthWinningGameState(SuffixTreeBuilder& aSuffixTree, SuffixTreeBuilder& bSuffixTree, int64_t K, const vector<int64_t>& numInBWithoutGrundy)
{
    GameState result = GameState::invalid(); // Guilty until proven innocent!
    findKthWinningGameState(aSuffixTree.rootCursor(), bSuffixTree, K, numInBWithoutGrundy, result);
    if (result.isValid)
    {
        assert(result.aPrime != "-" && result.bPrime != "-");
    }
    return result;
}

GameState findKthWinningGameState(const string& A, const string& B, int64_t K)
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
        assert(x >= 0);
    }
    return findKthWinningGameState(aSuffixTree, bSuffixTree, K, numInBWithoutGrundy);
}

int main(int argc, char** argv)
{
    // Fundamentally an easy one, though tricky to implement.
    //
    // Firstly - we should immediately recognise that we are playing two
    // copies of an impartial game concurrently, so The Sprague-Grundy
    // Theorem should immediately spring to mind.  The game that is being
    // played twice is a simple one: given a substring x of S (where S is either A or B, 
    // depending on which "copy" of the game we are playing), add a letter y
    // to x such that the resulting string xy is still in S - if we can't do
    // this, then we lose.
    //
    // All this talk of substrings and choosing letters should call forth Suffix Automata:
    // if we follow x in the Suffix Automaton representing S, then we can
    // immediately find all possible "moves" (the letters y) for x: if x leads to
    // an explicit state, then the set of moves is precisely the set of y such
    // that y is the first letter in a transition leading from that state;
    // if x does not lead to an explicit state, then x leads us partway along a transition,
    // and the sole possible y is simply the next letter on that transition.
    //
    // If following x leads to an explicit state with no transitions leading from it
    // (a leaf state), then this is a losing position for the current player,
    // and so its Grundy number would be 0.
    //
    // If following x does not lead to an explicit state, then we are partway along a transition, t.
    // If following the next letter in t leads us to an explicit state S', and we know the Grundy
    // Number for this state, then grundy(x) is mex({grundy(S')} by Sprague-Grundy, and
    // so is equal to 0 if grundy(S') > 0, and 1 if grundy(S') == 0.
    // Let x' be x with its final letter (y', say) removed.  If x' is still on t, then y' is the
    // only possible move from x' (and forms x'y' = x), and so grundy(x') = mex(grundy(x)).
    // Again, this is 0 if grundy(x) > 0, and 1 if grundy(x) == 0.
    //
    // Thus, we see that if x is on a transition, then the value of grundy(x) is either 0 or 1.
    // Further, the grundy number of a word leading to a transition *alternates* between 0 and 1
    // as we work our way backwards along a transition.
    //
    // If following x leads to an explicit state with at least one transition/ allowed move, then let
    // y_1, y_2, ... y_p be all allowed moves: then grundy(x) = mex({grundy(xy_1), grundy(xy_2), ... , grundy(xy_p)}).
    //
    // Putting all this together, we can work out the grundy number for all explicit states in the 
    // Suffix Automaton by starting with the leaf states (grundy == 0) and using the fact
    // that the grundy number of a word that leads along a transition alternates between 0 and 1
    // (with the last word in the transition having grundy 0 if the explicit state that follows
    // it has grundy > 0, and 1 otherwise) and the fact that the grundy number of an explicit
    // state is the mex of all moves from that state.  This can be easily accomplished by a DFS,
    // and initialiseGrundyInfo performs this calculation of grundy numbers for explicit states
    // (plus a whole bunch of other tasks!).  So given a substring x of S, we can easily compute the grundy
    // number of x: call it grundy(x, S).
    //
    // So: given a state (A', B') in the "full" game (where A' is a substring of A; B' is a substring of B),
    // then grundy((A', B')) = grundy(A', A) ^ grundy(B', B) by the Sprague Grundy Theorem.
    //
    // So: are we closer to solving the problem? A little: we can now enumerate all pairs (A', B') in lexicographic
    // order, and reasonably efficiently compute which of these are winning states for the first player
    // (these are the ones for which grundy((A', B')) != 0), and then pick the Kth one.  Of course, this is
    // (at least!) O(N ^ 4), so that's no good, but there are efficiency-improving shortcuts!
    //
    // We can generate all possible substrings A' of A in lexicographic order simply by doing a letter-by-letter
    // DFS on the suffix automaton representing A where we follow transitions from an explicit state
    // in alphabetical order of the first letter of that transition (initialiseGrundyInfo is responsible
    // for sorting the transitions in this order via sortTransitions).
    // 
    // Let x be the first string we've generated in this way (it will always be the empty string, but let's
    // give it a name, anyway!).  We can immediately compute grundy(x) in O(1) if we've run initialiseGrundyInfo. 
    // Can we deduce whether the Kth winning state begins with x? Yes: it will begin with x if and only if
    // there are at most K words y in B such that grundy(x) ^ grundy(y) != 0: or in other words, at most
    // K words y in B such that grundy(y) != grundy(x).  If we precompute the number of words in B whose grundy
    // is different from a given value (numInBWithoutGrundy[grundy(x)]; computed by initialiseGrundyInfo), 
    // then we can decide this very easily in O(1).
    //
    // If numInBWithoutGrundy[grundy(x)] <= the current value of K, then the result we want is the state (x, <Kth y (substring of B)
    // in lexicographic order with grundy(y) != grundy(x)>).  This can be computed easily (see findNthWithoutGrundy)
    // and we'll discuss how to find it a bit later.  Otherwise, x is *not* the first element in the state we want:
    // let's subtract numInBWithoutGrundy[grundy(x)] from K (essentially "skipping down the list" of winning states beginning
    // where the first element of the state pair is x), and continue with the next substring of A after X in 
    // lexicographic order; call it x'.  We repeat the procedure: if numInBWithoutGrundy[grundy(x')] <= the current value of K,
    // then the answer begins with x' and we can find the other element using findNthWithoutGrundy; otherwise, skip down
    // numInBWithoutGrundy[grundy(x')] in the list (i.e subtract this value from K), find the next substring of A in lexicographic order
    // after after x' (by continuing our letter-by-letter DFS of the suffix automaton representing A); etc.
    //
    // We'll see shortly that the complexity of findNthWithoutGrundy is O(N), so so far this procedure is O(N^2), dominated
    // by the cost of generating all substrings of A in lexicographic order via the letter-by-letter DFS of A.
    // Can we do better? Yes - as is often the case, we can take a shortcut by only considering explicit states (of which there
    // are O(N)).
    //
    // Let's say we've done our DFS, and have ended up following the first letter of some transition, t, whose endpoint is
    // some explicit state, S.  Do we really have to crawl along t letter-by-letter? Not necessarily: we can immediately
    // compute the result of performing the above procedure for each point along t by noting that the grundy number
    // of any word that leads us partway along t will be 0 or 1, and that we can instantly compute how many will have
    // a grundy number of each of these values by recalling that the grundy numbers along t alternate between 0 and 1, with the
    // parity depending on the distance along t and the grundy number of S.  Assume that are numWithGrundy0 and numWithGrundy1
    // along t, respectively: then if we did the letter-by-letter procedure above, our K would decrease by
    //
    //    numWithGrundy0 * numInBWithoutGrundy[0]  + numWithGrundy1 * numWithGrundy1.
    //
    // (numWithGrundy0 and numWithGrundy1 are computed by initialiseGrundyInfo via calcGrundyInfoForTransition). 
    //
    // If after this decrease our K >= 0, then we can skip crawling along t and jump straight to S.  Otherwise - and this will only 
    // happen once per testcase - we must revert to the letter-by-letter crawl we originally performed, contributing an extra O(N).
    //
    // That's almost everything, except for the computation of findNthWithoutGrundy for some K and some grundy value g, which we perform once per testcase.  
    // We could this with an O(N^2) letter-by-letter DFS of the suffix automaton representing B (with transitions sorted by
    // their first letter), continuing until we've generated K substrings of B whose grundy number is other than g, but
    // again there is a better way where we can skip (all but at most one) transitions: if we've just embarked on a transition t,
    // then if numWithGrundy0 and numWithGrundy1 are again the number of words that end in t whose grundy numbers are 0 or 1, respectively,
    // then following t letter-by-letter would find:
    //
    //    numWithGrundy0 if g != 0 plus
    //    numWithGrundy1 if g != 1
    //
    // words with grundy other than g i.e. K would decrease by this number if we followed this transition letter-by-letter.
    // If this would bring K to less than 0, then the Kth word with grundy other than g lies on t, so we crawl along t
    // letter-by-letter until we find this word (again, this contributes O(N) to the run-time, but we only have to do it at most
    // once per testcase); otherwise, we subtract this number from K and continue our DFS which, since we are skipping over
    // transitions in O(1), takes only a total of O(N).
    //
    // And that's it - the whole procedure is O(N).  There are some annoyances with the implementation - for example, the
    // depth of recursion in our DFS of the suffix automata can trigger a stack overflow, so initialiseGrundyInfo and findNthWithoutGrundy
    // had to be re-worked to be non-recursive, plus other minor annoyances.  Hopefully the rest of the code is self-explanatory!

    int N;
    cin >> N;
    int M;
    cin >> M;
    int64_t K;
    cin >> K;

    string A;
    string B;

    cin >> A;
    cin >> B;

    const auto result = findKthWinningGameState(A, B, K);
    if (result.isValid)
    {
        cout << result.aPrime << endl << result.bPrime << endl;
    }
    else
    {
        cout << "no solution" << endl;
    }
}



