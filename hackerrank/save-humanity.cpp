// Simon St James (ssjgz) 2017-07-18 16:47
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#define NO_VERIFY_UKKONEN
#endif
#include <vector>
#include <stack>
#include <iostream>
#include <algorithm>
#include <cassert>

using namespace std;
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
        };
        struct State
        {
            vector<Transition> transitions;
            State* suffixLink = nullptr;
            State* parent = nullptr;
            int index = -1;
            bool isFinal = false;
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
        void makeFinalStatesExplicitAndMarkThemAsFinal()
        {
            // Trick described in Ukkonen's paper.
            const char unusedLetter = '{';
            appendLetter(unusedLetter);

            // Remove the unused letter again!
            for (auto& state : m_states)
            {
                for (auto transitionIter = state.transitions.begin(); transitionIter != state.transitions.end(); )
                {
                    const Transition& transition = *transitionIter;
                    if (transition.substringFollowed.startIndex < 1)
                    {
                        transitionIter++;
                        continue;
                    }
                    const auto realEndIndex = (transition.substringFollowed.endIndex == openTransitionEnd ? static_cast<int>(m_currentString.size() - 1) : transition.substringFollowed.endIndex - 1);
                    const char lastCharInTransition = m_currentString[realEndIndex];
                    bool needToRemoveTransition = false;
                    if (lastCharInTransition == unusedLetter)
                    {
                        const bool transitionConsistsSolelyOfUnusedChar = (transition.substringFollowed.length(m_currentString.size()) == 1);
                        if (transitionConsistsSolelyOfUnusedChar)
                        {
                            needToRemoveTransition = true;
                            state.isFinal = true;
                        }
                        else
                        {
                            transition.nextState->isFinal = true;
                        }
                    }

                    if (needToRemoveTransition)
                        transitionIter = state.transitions.erase(transitionIter);
                    else
                        transitionIter++;
                }
            }

            m_currentString.pop_back(); // Remove the unusedLetter we just added.
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
                {
                    char nextLetters[27];
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
        static const int alphabetSize = 27; // Include the magic '{' for making final states explicit - assumes the input string has no '{''s, obviously!
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

vector<int> findLargestCommonPrefixForEachSuffix(const string& s, const string& t)
{
    SuffixTreeBuilder suffixTree;
    suffixTree.appendString(s);

    suffixTree.makeFinalStatesExplicitAndMarkThemAsFinal();

    vector<int> largestCommonPrefixForEachSuffix(s.size());

    // This would be a relatively straightforward recursive algorithm, but annoyingly, in some testcases, we need 10s of thousands of levels 
    // of recursion, leading to stack overflow :(
    // Simulate recursion ourselves using a stack.
    struct State
    {
        Cursor cursor;
        long numLettersFollowed = -1;
        long lengthOfPrefixOfTFollowed = -1;
        SuffixTreeBuilder::Cursor::NextLetterIterator nextLetterIterator;
        int indexIntoNextLetters = -1;
        bool haveRecursed = false;
    };

    // "State"s are essentially stack-frames, not to be be confused with "States" of the suffix tree!
    State initialState;
    initialState.cursor = suffixTree.rootCursor();
    initialState.numLettersFollowed = 0;
    initialState.lengthOfPrefixOfTFollowed = 0;
    stack<State> states;
    states.push(initialState);
    while (!states.empty())
    {
        State& currentState = states.top();
        const auto haveFollowedPrefixOfT = (currentState.lengthOfPrefixOfTFollowed == currentState.numLettersFollowed);
        if (currentState.cursor.isOnExplicitState())
        {
            if (currentState.indexIntoNextLetters == -1)
            {
                const auto isFinalState = currentState.cursor.isOnFinalState();
                currentState.nextLetterIterator = currentState.cursor.getNextLetterIterator();
                currentState.indexIntoNextLetters = 0;
                if (isFinalState && currentState.numLettersFollowed > 0)
                {
                    // Reached a final state; the string we've followed is a suffix of s.
                    // Update the result.
                    const long suffixBeginPos = s.length() - currentState.numLettersFollowed;
                    assert(suffixBeginPos >= 0 && suffixBeginPos < s.size());
                    largestCommonPrefixForEachSuffix[suffixBeginPos] = currentState.lengthOfPrefixOfTFollowed;
                }
            }
            if (!currentState.nextLetterIterator.hasNext())
            {
                // Done exploring this state.
                states.pop();
                continue;
            }
            const auto nextLetter = currentState.nextLetterIterator.nextLetter();
            const auto doesLetterContinueS = haveFollowedPrefixOfT && 
                                          (t[currentState.numLettersFollowed] == nextLetter);
            const auto newLengthOfPrefixOfTFollowed = (doesLetterContinueS ? currentState.lengthOfPrefixOfTFollowed + 1 : currentState.lengthOfPrefixOfTFollowed);
            auto cursorAfterLetter = currentState.nextLetterIterator.afterFollowingNextLetter();
            currentState.nextLetterIterator++;
            State newState;
            newState.cursor = cursorAfterLetter;
            newState.numLettersFollowed = currentState.numLettersFollowed + 1;
            newState.lengthOfPrefixOfTFollowed = newLengthOfPrefixOfTFollowed;
            states.push(newState);

            currentState.indexIntoNextLetters++;
            continue;
        }
        else
        {
            if (currentState.haveRecursed)
            {
                // Done exploring.
                states.pop();
                continue;
            }
            Cursor nextCursor(currentState.cursor);
            bool followedLetters = false;
            int newNumLettersFollowed = currentState.numLettersFollowed;
            int newLengthOfPrefixOfTFollowed = currentState.lengthOfPrefixOfTFollowed;
            if (haveFollowedPrefixOfT)
            {
                // Follow as many chars in the continuation of t on this transition as we can, updating newLengthOfPrefixOfTFollowed and newNumLettersFollowed
                // accordingly.
                while (currentState.lengthOfPrefixOfTFollowed < t.length() && nextCursor.canFollowLetter(t[currentState.lengthOfPrefixOfTFollowed]) && !nextCursor.isOnExplicitState())
                {
                    followedLetters = true;
                    nextCursor.followLetter(t[currentState.lengthOfPrefixOfTFollowed]);
                    newLengthOfPrefixOfTFollowed++;
                    newNumLettersFollowed++;
                }
            }
            if (!followedLetters)
            {
                const auto remainderOfTransition = nextCursor.remainderOfCurrentTransition();
                nextCursor.followNextLetters(remainderOfTransition.length());
                newNumLettersFollowed += remainderOfTransition.length();
                assert(nextCursor.isOnExplicitState());
            }
            State newState;
            newState.cursor = nextCursor;
            newState.numLettersFollowed = newNumLettersFollowed;
            newState.lengthOfPrefixOfTFollowed = newLengthOfPrefixOfTFollowed;
            states.push(newState);
            // Make a note that we've already explored this state.
            currentState.haveRecursed = true;
            continue;
        }
    }

    return largestCommonPrefixForEachSuffix;
}

struct InitialRepeat
{
    int repeatedLength = -1;
    int numRepeats = -1;
};

InitialRepeat computeLargestInitialRepeatedSubstring(const string& s)
{
    InitialRepeat largestInitialRepeated;
    largestInitialRepeated.repeatedLength = 1; // The initial character appears once.
    largestInitialRepeated.numRepeats = 1; 

    const vector<int> largestCommonPrefixForEachSuffix = findLargestCommonPrefixForEachSuffix(s, s);

    // Obviously ignore the first entry, as that is the longest commen prefix of s with s XD
    for (int i = 0; i < largestCommonPrefixForEachSuffix.size(); i++)
    {
        // The "isMatchWithOwnPrefix" check prohibits in e.g.
        //  "sausagexsausagesausagesausage"
        // the 3 sausages after the x from being classed as an initial prefix.
        const bool isMatchWithOwnPrefix = (i != 0 && i <= largestCommonPrefixForEachSuffix[i]);
        if (isMatchWithOwnPrefix)
        {
            const long repeatedStringLength = i;
            const long numRepetitions = largestCommonPrefixForEachSuffix[i] / repeatedStringLength + // Repetitions in the suffix.
                                        i / repeatedStringLength; // Repetitions before the suffix.
            if (numRepetitions > largestInitialRepeated.numRepeats)
            {
                largestInitialRepeated.numRepeats = numRepetitions;
                largestInitialRepeated.repeatedLength = repeatedStringLength;
            }
        }
    }

    return largestInitialRepeated;
}

void findOccurrencesOfWord(int fullStringLength, SuffixTreeBuilder::Cursor cursorAfterWord, int wordLength, vector<int>& destOccurences)
{
    struct State
    {
        Cursor cursor;
        int wordLength;

        vector<char> nextLetters;
        int indexIntoNextLetters = -1;

        bool haveRecursed = false;
    };
    // Just find the final states reachable from cursorAfterWord, and thus find the positions at which the word represented
    // by cursorAfterWord can be found in the original string.
    // Again, a recursive solution would be nice, but ...
    State initialState;
    initialState.cursor = cursorAfterWord;
    initialState.wordLength = wordLength;
    stack<State> states;
    states.push(initialState);
    while (!states.empty())
    {
        State& currentState = states.top();
        if (currentState.cursor.isOnExplicitState())
        {
            if (currentState.indexIntoNextLetters == -1)
            {
                currentState.nextLetters = currentState.cursor.nextLetters();
                currentState.indexIntoNextLetters = 0;
                if (currentState.cursor.isOnFinalState())
                {
                    destOccurences.push_back(fullStringLength - currentState.wordLength);
                }
            }
            if (currentState.indexIntoNextLetters == currentState.nextLetters.size())
            {
                // Done exploring this state.
                states.pop();
                continue;
            }
            const auto nextLetter = currentState.nextLetters[currentState.indexIntoNextLetters];
            auto cursorAfterLetter = currentState.cursor;
            cursorAfterLetter.followLetter(nextLetter);
            State newState;
            newState.cursor = cursorAfterLetter;
            newState.wordLength = currentState.wordLength + 1;
            states.push(newState);

            currentState.indexIntoNextLetters++;
            continue;
        }
        else
        {
            if (currentState.haveRecursed)
            {
                // Done exploring.
                states.pop();
                continue;
            }
            Cursor nextCursor(currentState.cursor);
            nextCursor.followToTransitionEnd();

            State newState;
            newState.cursor = nextCursor;
            newState.wordLength = currentState.wordLength + currentState.cursor.remainderOfCurrentTransition().length();
            states.push(newState);
            // Make a note that we've already explored this state.
            currentState.haveRecursed = true;
            continue;
        }
    }
}

SuffixTreeBuilder::Cursor jumpThroughRestOfRepeatedInitial(Cursor mismatchCursor, int numFollowedByMismatch, const string& v, const string&p,  const string& initialRepeatedString, const int numInitialRepeats, const vector<int>& numOfInitialSequenceInPatientSuffix)
{
    const int sizeOfInitialRepeatBlock = numInitialRepeats * initialRepeatedString.size();
    int numLettersUntilOutOfRepeatBlock = (sizeOfInitialRepeatBlock - numFollowedByMismatch);
    while (numLettersUntilOutOfRepeatBlock > 0 && mismatchCursor.isValid())
    {
        assert(mismatchCursor.stringFollowed().size() == numFollowedByMismatch);
        const int untilNextRepeatBegin = (initialRepeatedString.size() - (numFollowedByMismatch % initialRepeatedString.size())) % initialRepeatedString.size();
        assert(untilNextRepeatBegin >= 0);
        const int numStepsToBeginOfRepetitionOrEndOfRepeatedPrefix = min(untilNextRepeatBegin, numLettersUntilOutOfRepeatBlock);
        // Move the cursor forward so that it is at the beginning of the repeated sequence if we can e.g. if the original string is
        // repeatedrepeatedrepeatedrepeated
        //            x
        //                 y
        // and mismatchCursor is at x, then move it forward so that it is at y.
        for (int count = 0; count < numStepsToBeginOfRepetitionOrEndOfRepeatedPrefix; count++)
        {
            const char nextLetter = v[numFollowedByMismatch];
            if (!mismatchCursor.canFollowLetter(nextLetter))
            {
                mismatchCursor = SuffixTreeBuilder::invalidCursor();
                break;
            }
            else
            {
                mismatchCursor.followLetter(nextLetter);
                numLettersUntilOutOfRepeatBlock--;
                numFollowedByMismatch++;
            }
        }
        if (numLettersUntilOutOfRepeatBlock > 0 &&  mismatchCursor.isValid())
        {
            assert(mismatchCursor.stringFollowed().size() == numFollowedByMismatch);
            if (mismatchCursor.isOnExplicitState())
            {
                // Follow one character.
                const char nextVirusLetter = v[numFollowedByMismatch];
                if (mismatchCursor.canFollowLetter(nextVirusLetter))
                {
                    mismatchCursor.followLetter(nextVirusLetter);
                    numLettersUntilOutOfRepeatBlock--;
                    numFollowedByMismatch++;
                }
                else
                {
                    mismatchCursor = SuffixTreeBuilder::invalidCursor();
                }
            }
            else
            {
                // Attempt to "leap" up the transition (or determine that we can't) - if we didn't do this and just
                // tried to follow the repeating sequence letter-by-letter, it would be O(|v|^2).
                assert(numLettersUntilOutOfRepeatBlock >= 0);
                assert((numFollowedByMismatch % initialRepeatedString.size()) == 0);
                const auto substringRemainingOnTransition = mismatchCursor.remainderOfCurrentTransition();

                // See if we can follow the repeated sequence from here either until the end of the transition
                // or until we are free of the entire repeated prefix (whichever is sooner).
                const int numCharsToFollow = min(substringRemainingOnTransition.length(), numLettersUntilOutOfRepeatBlock);
                if (numOfInitialSequenceInPatientSuffix[substringRemainingOnTransition.startIndex()] < numCharsToFollow)
                {
                    mismatchCursor = SuffixTreeBuilder::invalidCursor();
                    break;
                }
                else
                {
                    numLettersUntilOutOfRepeatBlock -= numCharsToFollow;
                    numFollowedByMismatch += numCharsToFollow;
                    if (numCharsToFollow < substringRemainingOnTransition.length())
                    {
                        mismatchCursor.followLetters(p, substringRemainingOnTransition.startIndex(), numCharsToFollow);
                        assert(mismatchCursor.isValid());
                        assert(numLettersUntilOutOfRepeatBlock == 0);
                    }
                    else
                    {
                        mismatchCursor.followToTransitionEnd();
                    }
                    assert(numLettersUntilOutOfRepeatBlock >= 0);
                }
            }
        }
        assert(numLettersUntilOutOfRepeatBlock >= 0);
        assert(!mismatchCursor.isValid() || (mismatchCursor.stringFollowed().size() == numFollowedByMismatch));
    }

    return mismatchCursor; }

vector<int> findExactAndOneMismatchOccurences(const string& p, const string& v)
{
    vector<int> occurences;

    SuffixTreeBuilder patientSuffixTree;
    patientSuffixTree.appendString(p);
    patientSuffixTree.makeFinalStatesExplicitAndMarkThemAsFinal();

    SuffixTreeBuilder::Cursor exactMatch = patientSuffixTree.rootCursor();
    vector<SuffixTreeBuilder::Cursor> oneMismatches;

    const InitialRepeat largestInitialVirusRepeat = computeLargestInitialRepeatedSubstring(v);
    const string initialRepeatedString = v.substr(0, largestInitialVirusRepeat.repeatedLength);
    const int numInitialRepeats = largestInitialVirusRepeat.numRepeats;
    const int sizeOfInitialRepeatBlock = numInitialRepeats * initialRepeatedString.size();
    const auto numOfInitialSequenceInPatientSuffix = findLargestCommonPrefixForEachSuffix(p, v);

    // Handle the initial repetition, first: if v = (w ^^ n)x, then we handle the first
    // (w ^^ n) portion here, in linear time (probably O(|w||p|), more accurately).
    for (int i = 0; i < sizeOfInitialRepeatBlock; i++)
    {
        const auto virusLetter = v[i];
        if (exactMatch.isValid() && exactMatch.isOnExplicitState())
        {
            auto nextLetterIterator = exactMatch.getNextLetterIterator();
            while (nextLetterIterator.hasNext())
            {
                const auto mismatchLetter = nextLetterIterator.nextLetter();
                if (mismatchLetter != virusLetter)
                {
                    Cursor mismatchCursor = nextLetterIterator.afterFollowingNextLetter();
                    mismatchCursor = jumpThroughRestOfRepeatedInitial(mismatchCursor, i + 1, v, p, initialRepeatedString, numInitialRepeats, numOfInitialSequenceInPatientSuffix); 

                    if (mismatchCursor.isValid())
                        oneMismatches.push_back(mismatchCursor);
                }
                nextLetterIterator++;

            }
        }
        if (exactMatch.isValid())
        {
            if (exactMatch.canFollowLetter(virusLetter))
                exactMatch.followLetter(virusLetter);
            else
            {
                if (!exactMatch.isOnExplicitState())
                {
                    // Missing an exact match generates one mismatch.  It won't happen again!
                    Cursor mismatchCursor(exactMatch);
                    mismatchCursor.followNextLetter();
                    mismatchCursor = jumpThroughRestOfRepeatedInitial(mismatchCursor, i + 1, v, p, initialRepeatedString, numInitialRepeats, numOfInitialSequenceInPatientSuffix); 
                    if (mismatchCursor.isValid())
                        oneMismatches.push_back(mismatchCursor);
                }
                exactMatch = SuffixTreeBuilder::invalidCursor();
            }
        }

    }

    assert(!exactMatch.isValid() || (exactMatch.stringFollowed().length() == sizeOfInitialRepeatBlock));

    // Handle the remainder (the "x" bit in the (w ^^ n)x above).
    for (int i = sizeOfInitialRepeatBlock; i < v.size(); i++)
    {
        const auto virusLetter = v[i];
        vector<SuffixTreeBuilder::Cursor> nextOneMismatch;
        nextOneMismatch.reserve(oneMismatches.size() * 2);
        if (exactMatch.isValid())
        {
            if (exactMatch.isOnExplicitState())
            {
                auto nextLetterIterator = exactMatch.getNextLetterIterator();
                while (nextLetterIterator.hasNext())
                {
                    const auto mismatchLetter = nextLetterIterator.nextLetter();
                    if (mismatchLetter != virusLetter)
                    {
                        // Generate mismatches by not following the virusLetter.
                        const Cursor mismatchCursor = nextLetterIterator.afterFollowingNextLetter();
                        nextOneMismatch.push_back(mismatchCursor);
                    }
                    nextLetterIterator++;
                }
            }
            if (exactMatch.canFollowLetter(virusLetter))
            {
                exactMatch.followLetter(virusLetter);
            }
            else
            {
                if (!exactMatch.isOnExplicitState())
                {
                    // Missing an exact match generates one mismatch.  It won't happen again!
                    // The mismatch for if exactMatch is on an explicit state is done earlier.
                    Cursor mismatchCursor(exactMatch);
                    mismatchCursor.followNextLetter();
                    nextOneMismatch.push_back(mismatchCursor);
                }
                exactMatch = SuffixTreeBuilder::invalidCursor();
            }
        }

        for (const auto mismatchCursor : oneMismatches)
        {
            // Update our population of near misses.  If they miss again, they are not carried over.
            if (mismatchCursor.canFollowLetter(virusLetter))
            {
                Cursor nextMismatchCursor(mismatchCursor);
                nextMismatchCursor.followLetter(virusLetter);
                nextOneMismatch.push_back(nextMismatchCursor);
            }
        }

        oneMismatches = std::move(nextOneMismatch);
    }

    if (exactMatch.isValid())
    {
        findOccurrencesOfWord(p.size(), exactMatch, v.size(), occurences);
    }
    for (const auto mismatchCursor : oneMismatches)
    {
        findOccurrencesOfWord(p.size(), mismatchCursor, v.size(), occurences);
    }
    sort(occurences.begin(), occurences.end());

    return occurences;
}

void printOccurences(const vector<int>& occurences)
{
    if (occurences.empty())
    {
        cout << "No Match!";
    }
    else
    {
        for (const auto pos : occurences)
        {
            cout << pos << " ";
        }
    }
}

int main() 
{
    // Fairly tough one. First and foremost: like "Determining DNA Health", this seems like it
    // could be solved using means other than a Suffix Tree (there is no 'distinct substrings'
    // requirement in either that or this problem, which is usually what you want a Suffix Tree
    // for) - it's very KMP-ish - but, again like "Determining DNA Health", I thought I'd use
    // the Suffix Tree approach to see how well it worked :)
    // So - the absolute basic approach: form the suffix tree for P; form an "exactMatch" 
    // cursor as the root of the tree and add an empty set (oneMismatches) of other cursors.
    // For each successive letter in v (virusLetter) take exactMatch and first generate a new cursor (mismatchCursor)
    // by following each letter followable by exactMatch that is *not* virusLetter and adding it to oneMismatches, and then 
    // following, if possible, virusLetter from exactMatch - if it is not possible, exactMatch should be discarded
    // and we ignore it from here on.  Each mismatchCursor in the original oneMismatches should also be made to follow virusLetter,
    // and if it can't, then again it is discarded.
    // In this way, we maintain the invariant that exactMatch is the cursor obtained by following the current prefix
    // of v (or it is invalid if we can't) and oneMismatches consists of precisely* the set of cursors obtained
    // by following any string that differs from the current prefix of v by exactly one letter.  When all letters in
    // v have been processed in this form, we simply find the suffixes corresponding to exactMatch (if it is still valid)
    // and all remaining elements on oneMismatches by exploring from them to the final states - see findOccurrencesOfWord() 
    // for this step.
    // So far, so good, but we might object from an efficiency point of view about having to update all elements of oneMismatches
    // for each letter of V.  In general, this will be fine, intuitively because for each element in oneMismatches there must
    // be one string in P that differs from the current prefix of V by one character, and a moments' though tells us that
    // if the current prefix of V is quite large, then the number of possible occurrences of a (near)-match in P
    // is quite low, and will decrease with each successive letter of : minor variations of the string abcdefghijklmnopqrst 
    // won't have many chances to appear in a string of length 100, say! In general, we can reasonably expect there to 
    // be approx |P|/|prefix of V| elements in oneMismatches, and amortised over |V| we get about |P|.
    // But this only occurs when the occurences of prefixes of V in P can't overlap (as in abcdefghijklmnopqrst) - if they
    // can, then things go badly wrong.  Imagine V = (abc)^1000 and P = (abc)^1000dbc(abc)^1000 - then we'd see that
    // the a prefix V' of V can occur far more times than |P|/ |V'| times - in fact, if we analyse this 
    // example, we see that the population of oneMismatches grows with each successive letter of V, and they don't get killed off.
    // This blowup only happens because the prefixes of V can overlap.
    // In general, V of the form (w ^^ m)x can easily lead to O(|m^2|) calculations, which can easily be O(|V|^2).  Only V of this form
    // triggers worse than O(|P|) calculations, as far as I can tell.
    // So: how can we deal with this? Again, it all seems very KMP-ish, but I chose to detect this situation and extract the largest
    // m such that V = (w ^^ m)x using suffix arrays, and then handle the (w ^^ m) prefix of V in a special way that does not
    // lead to blowup.  The remaining x portion can be deal with using the original algorithm.
    // Detecting w,m in the first place is handled using a twist on findLargestCommonPrefixForEachSuffix() - V of this form has 
    // the property that the suffix (w^^m-1)x has a large overlap with V, and we can use this to easily determine w,m (see initialRepeatedString
    // and numInitialRepeats).  We call the (w^^m) prefix of V the "initial repeating block" and deal with it much the same
    // way as the original algorithm, but with a few shortcuts: when a mismatchCursor is introduced, we immediately "feed" it the 
    // remaining portion of the initial repeating block using some shortcuts: e.g. if w = abc, m = 100, and we've generated a mismatchCursor
    // after the prefix abcabc of V, we can feed mismatchCursor the remaining (abc)^98 letters of the using some shortcuts:
    // in essence, if mismatchCursor is partway along some transition in the suffix tree, then we know the substring of P that it can
    // follow.  If the substring of P does not consist of the repeating pattern abcabcabc until it reaches the end of the transition (or 
    // uses up the remaining 98 abcs) then we can kill mismatchCursor; otherwise, we can skip mismatchCursor straight to the end of the transition.
    // largestCommonPrefixForEachSuffix() gives us the information we need to decide this.
    // We can then keep skipping along transitions in this way spending only a small time on each transition.  It can be shown that
    // applying this algorithm to the initial repeating block never processes the same transition twice, and so, since
    // we spend a constant amount of time on each transition, dealing with the whole initial repeating block takes up O(|P|) rather
    // than O(m^2).
    // And that's about it!

    // * There's actually an additional edge-case the first time exactMatch can't follow virus letter and exactMatch is not
    // on an explicit state - a further mismatchCursor is generated in this one-off special case.
    long T;
    cin >> T;
    for (int t = 0; t < T; t++)
    {
        string P;
        cin >> P;
        string V;
        cin >> V;
        printOccurences(findExactAndOneMismatchOccurences(P, V));
        cout << endl;
    }
    return 0;
}

