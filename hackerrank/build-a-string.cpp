// Simon St James (ssjgz) 2017-7-21 10:47
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <cassert>

using namespace std;

struct StateData
{
    int leastEndIndexForWord = -1;
    int wordLength = -1;
    bool wordEndsBefore(int pos)
    {
        return leastEndIndexForWord < pos;
    }
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

int findLeastEndpointsForWordsLeadingToExplicitStates(Cursor cursor, const int fullStringLength, int wordLengthFollowed)
{
    if (cursor.isOnExplicitState())
    {
        cursor.stateData().wordLength = wordLengthFollowed;
        int maxDistanceToFinalState = 0;
        auto nextLetterIterator = cursor.getNextLetterIterator();
        while (nextLetterIterator.hasNext())
        {
            const int nextWordLengthFollowed = wordLengthFollowed + 1;
            const Cursor nextCursor = nextLetterIterator.afterFollowingNextLetter();

            const int maxDistanceToFinalStateFromSuccessor = 1 + findLeastEndpointsForWordsLeadingToExplicitStates(nextCursor, fullStringLength, nextWordLengthFollowed);
            maxDistanceToFinalState = max(maxDistanceToFinalState, maxDistanceToFinalStateFromSuccessor);
            nextLetterIterator++;
        }
        const int indexOfEarliestOccurenceOfWord = fullStringLength - (wordLengthFollowed + maxDistanceToFinalState);
        cursor.stateData().leastEndIndexForWord = indexOfEarliestOccurenceOfWord + wordLengthFollowed - 1;
        return maxDistanceToFinalState;
    }
    else
    {
        const int numLettersOnRemainderOfTransition = cursor.remainderOfCurrentTransition().length();
        const int nextWordLengthFollowed = wordLengthFollowed + numLettersOnRemainderOfTransition;
        Cursor nextCursor(cursor);
        nextCursor.followToTransitionEnd();
        return numLettersOnRemainderOfTransition + findLeastEndpointsForWordsLeadingToExplicitStates(nextCursor, fullStringLength, nextWordLengthFollowed);
    }
}

struct Ancestor
{
    Ancestor(const Cursor& explicitState, char letterFollowed)
        : explicitState{explicitState}, letterFollowed{letterFollowed}
    {
    }
    Cursor explicitState;
    char letterFollowed;
};

Cursor findLongestSubstringOfPrefixThatIsPrefixOfSuffixAlongTransitionBetweenAncestors(const Ancestor& ancestor, int suffixBeginPos)
{
    Cursor explicitAncestor = ancestor.explicitState;
    Cursor currentBestCursor = explicitAncestor;
    if (currentBestCursor.stateData().wordEndsBefore(suffixBeginPos - 1))
    {
        // Room for improvement - try creeping (in O(1)) along the transition between this ancestor and the one following it.
        assert(explicitAncestor.isOnExplicitState());
        Cursor nextExplicitAncestor = currentBestCursor;
        nextExplicitAncestor.followLetter(ancestor.letterFollowed);
        int numLettersBetweenExplicitAncestors = 1;
        if (!nextExplicitAncestor.isOnExplicitState())
        {
            numLettersBetweenExplicitAncestors += nextExplicitAncestor.remainderOfCurrentTransition().length();
            nextExplicitAncestor.followToTransitionEnd();
        }
        assert(nextExplicitAncestor.isOnExplicitState());
        assert(!nextExplicitAncestor.stateData().wordEndsBefore(suffixBeginPos));
        const int leastEndPointAtNextExplicitAncestor = nextExplicitAncestor.stateData().leastEndIndexForWord;;
        // If w is a word that leads partway along the transition between explicitAncestor and nextExplicitAncestor,
        // and x is the word that leads to nextExplicitAncestor, then x = wu, and the least endpoint of w
        // is the least endpoint of x - |u|.
        // From this, we can work out the maximum number of letters on the transition between explicitAncestor and nextExplicitAncestor
        // that would still give us a word whose least endpoint < suffixBeginPos.
        const int shuffleAlongTransitionBy = suffixBeginPos - 1 - (leastEndPointAtNextExplicitAncestor - numLettersBetweenExplicitAncestors);
        if (shuffleAlongTransitionBy > 0 && shuffleAlongTransitionBy < numLettersBetweenExplicitAncestors)
        {
            currentBestCursor.followLetter(ancestor.letterFollowed);
            currentBestCursor.followNextLetters(shuffleAlongTransitionBy - 1);
        }
    }

    return currentBestCursor;
}

void findLongestSubstringOfPrefixThatIsPrefixOfSuffixAux(Cursor cursor, const int fullStringLength, int wordLengthFollowed, vector<Ancestor>& explicitCursorAncestors, vector<int>& longestSubstringOfPrefixThatIsPrefixOfSuffix)
{
    if (cursor.isOnExplicitState())
    {
        if (cursor.isOnFinalState())
        {
            const int suffixBeginPos = fullStringLength - wordLengthFollowed;
            Cursor currentBestCursor;
            if (cursor.stateData().wordEndsBefore(suffixBeginPos))
            {
                currentBestCursor = cursor;
            }
            else
            {
                // Binary search among Ancestors to find the explicit state that has the longest word that ends before
                // suffixBeginPos.
                auto ancestorIterHigh = explicitCursorAncestors.end() - 1;
                auto ancestorIterLow = explicitCursorAncestors.begin();
                auto latestAncestorThatEndsBeforeSuffixIter = ancestorIterHigh;
                while (ancestorIterHigh >= ancestorIterLow)
                {
                    const auto ancestorIterMed = ancestorIterLow + (ancestorIterHigh - ancestorIterLow) / 2;
                    const bool endsBeforeSuffix = ancestorIterMed->explicitState.stateData().wordEndsBefore(suffixBeginPos);
                    if (endsBeforeSuffix)
                    {
                        ancestorIterLow = ancestorIterMed;
                    }
                    else
                    {
                        // Later ancestors than this will also not end before suffix, so can be ruled out.
                        ancestorIterHigh = ancestorIterMed;
                    }
                    if (ancestorIterHigh - ancestorIterLow <= 1)
                    {
                        // The solution can be either of the low or the high when they are next to each other.
                        if (ancestorIterHigh->explicitState.stateData().wordEndsBefore(suffixBeginPos))
                        {
                            latestAncestorThatEndsBeforeSuffixIter = ancestorIterHigh;
                            break;
                        }
                        if (ancestorIterLow->explicitState.stateData().wordEndsBefore(suffixBeginPos))
                        {
                            latestAncestorThatEndsBeforeSuffixIter = ancestorIterLow;
                            break;
                        }
                    }
                }
                assert(latestAncestorThatEndsBeforeSuffixIter->explicitState.stateData().wordEndsBefore(suffixBeginPos));
                assert(latestAncestorThatEndsBeforeSuffixIter == explicitCursorAncestors.end() - 1 || !((latestAncestorThatEndsBeforeSuffixIter + 1)->explicitState.stateData().wordEndsBefore(suffixBeginPos)));
                // Refine our solution: the longest word ending before suffixBeginPos may lie on the transition between
                // latestAncestorThatEndsBeforeSuffixIter and its successor Ancestor.
                currentBestCursor = findLongestSubstringOfPrefixThatIsPrefixOfSuffixAlongTransitionBetweenAncestors(*latestAncestorThatEndsBeforeSuffixIter, suffixBeginPos);
            }
            assert(currentBestCursor.isValid());
            const int longestPrefixLength = currentBestCursor.stateData().wordLength + (currentBestCursor.isOnExplicitState() ? 0 : currentBestCursor.posInTransition());
            assert(longestPrefixLength == currentBestCursor.stringFollowed().length());
            assert(suffixBeginPos >= 0 && suffixBeginPos < longestSubstringOfPrefixThatIsPrefixOfSuffix.size());
            longestSubstringOfPrefixThatIsPrefixOfSuffix[suffixBeginPos] = longestPrefixLength;
        }
        auto nextLetterIterator = cursor.getNextLetterIterator();
        while (nextLetterIterator.hasNext())
        {
            // Recurse.
            const int nextWordLengthFollowed = wordLengthFollowed + 1;
            const Cursor nextCursor = nextLetterIterator.afterFollowingNextLetter();
            const char letterFollowed = nextLetterIterator.nextLetter();

            explicitCursorAncestors.push_back(Ancestor(cursor, letterFollowed));
            findLongestSubstringOfPrefixThatIsPrefixOfSuffixAux(nextCursor, fullStringLength, nextWordLengthFollowed, explicitCursorAncestors, longestSubstringOfPrefixThatIsPrefixOfSuffix);
            explicitCursorAncestors.pop_back();

            nextLetterIterator++;
        }
    }
    else
    {
        // Recurse.
        const int numLettersOnRemainderOfTransition = cursor.remainderOfCurrentTransition().length();
        const int nextWordLengthFollowed = wordLengthFollowed + numLettersOnRemainderOfTransition;
        Cursor nextCursor(cursor);
        nextCursor.followToTransitionEnd();
        findLongestSubstringOfPrefixThatIsPrefixOfSuffixAux(nextCursor, fullStringLength, nextWordLengthFollowed, explicitCursorAncestors, longestSubstringOfPrefixThatIsPrefixOfSuffix);
    }
}


// Returns a vector where, for index i, the value is the largest substring of PRE which is also a prefix of SUF, where PRE is the prefix of s
// ending before i, and SUF is the suffix of s beginning at i.  Takes O(|S| log |S|).
vector<int> findLongestCommonPrefixBetweenPrefixAndSuffix(const string& s)
{
    SuffixTreeBuilder suffixTree;
    suffixTree.appendString(s);
    suffixTree.makeFinalStatesExplicitAndMarkThemAsFinal();

    findLeastEndpointsForWordsLeadingToExplicitStates(suffixTree.rootCursor(), s.length(), 0);

    vector<Ancestor> explicitCursorAncestors;
    vector<int> longestSubstringOfPrefixThatIsPrefixOfSuffix(s.size() + 1); // Can have 0-length suffix all the way up to full length of s.
    findLongestSubstringOfPrefixThatIsPrefixOfSuffixAux(suffixTree.rootCursor(), s.length(), 0, explicitCursorAncestors, longestSubstringOfPrefixThatIsPrefixOfSuffix);

    return longestSubstringOfPrefixThatIsPrefixOfSuffix;
}

int64_t findMinCost(const vector<int>& longestSubstringOfPrefixThatIsPrefixOfSuffix, int N, int A, int B)
{
    // Simple recurrence relation.  The optimal strategy is, if we use the substring at all, to use the 
    // longest possible such.
    vector<int64_t> minCostToFormLastN(N + 1, -1);
    minCostToFormLastN[0] = 0;
    for (int numLastCharsToBuild = 1; numLastCharsToBuild <= N; numLastCharsToBuild++)
    {
        const int suffixBeginPos = N - numLastCharsToBuild;
        int64_t minCostToBuildLastChars = A + minCostToFormLastN[numLastCharsToBuild - 1];
        const int maxExistingSubstringLength = longestSubstringOfPrefixThatIsPrefixOfSuffix[suffixBeginPos];
        const int maxExistingSubstringUsableLength = min(maxExistingSubstringLength, numLastCharsToBuild);
        if (maxExistingSubstringUsableLength > 0)
        {
            minCostToBuildLastChars = min(minCostToBuildLastChars, B + minCostToFormLastN[numLastCharsToBuild - maxExistingSubstringUsableLength]);
        }
        minCostToFormLastN[numLastCharsToBuild] = minCostToBuildLastChars;
    }
    return minCostToFormLastN[N];
}

int main()
{
    // Liked this one a great deal - nikasvanidze always comes up with the best problems!
    // So: first, let's look at the problem of building the string.  This looks like a Dynamic Programming
    // problem, so what does the recurrence relation look like?
    // At each prefix p' of s (yielding a suffix s' beginning at |p'| + 1 = suffixBeginPos) we can either:
    //   - Pick a letter from the "not in prefix" pile, costing A and giving us a prefix of length |p'| + 1, or
    //   - Pick up to M letters from the "in prefix" pile, costing B and giving us a prefix of length |p'| + M,
    // where M is the length of the longest common prefix of s' and and a substring of p' (note that if there is an x 
    // of length M which is in p' and which is a suffix of s', then there will be one of length M - 1, M - 2 etc - just 
    // strip successive letters from the end of x).
    // So if minCostToFormLastN[k] tells us the min cost to form the last k letters of s, our recurrence
    // relation looks like:
    //
    //   minCostToFormLastN[k] = min(A + minCostToFormLastN[k - 1], B + min i = 1 ... M { minCostToFormLastN[k - i]})
    // 
    // This alone would take O(|S|^2) to evaluate, so let's see if we can do better.
    // It seems intuitive that if we elect to pick a string from the "in prefix" pile, we should always pick the absolute
    // longest, and in fact this is all that is needed in an optimal strategy.  To see this, assume we have
    // some optimal sequence which, at some point, chooses a substring from the "in prefix" pile which is not
    // the longest one it could have used.  Let p' be the prefix formed just before it makes this choice, and s'
    // the suffix, and C be the cost so far to build the p' part.  Let x be the substring of p' chosen.  From the assumption, 
    // there is a substring longer than x by 1 letter in p that is a prefix of s' - let x' = xu be this prefix (so 'u' is the extra letter).
    // (x' must share its first |x| letters with x, as there are both prefixes of s').
    // There are two possibilities for the choice after x in the original sequence:
    //  1) It was a letter v from the "not in prefix" sequence.
    // In this case, we originally built the first |p'| + |x| + 1 letters of s at a cost of C + B + A.
    // Now, imagine we had chosen xu instead of x: then we would have formed the first |p'| + |xu| = |p'| + |x| + 1
    // letters of s at the cost of C + B, which is strictly less than the cost using x then v.  Since the remainder
    // of the sequence is unchanged (and so, costs the same), we see that the original sequence was not optimal,
    // a contradiction.
    //  2) It was a substring y from the "in prefix" sequence.  Let y = vy', where v is the first letter of y and y'
    // is the remainder (which can be empty, if |y'| == 1).  By definition, y is a substring of p'.x.  Then our original sequence built the first
    // |p'| + |x| + |y| letters at a cost of C + A + A.  Let's see what happens if we had chosen xu.
    // Then we must have had u = v.  Note that since y is a substring of p'.x, y' must be a substring of p'.x and so of p'.xu.
    // Thus, after choosing xu, we can choose y'.
    // With this choice, we build the first |p'| + |xu| + |y'| = |p'| + |x| + |uy'| = |p'| + |x| + |y| letters at a cost of
    // C + A + A i.e. the full sequence costs exactly the same.
    // So we see that in either case, if we use less of a substring of the prefix that we could have, we can always get a new
    // sequence of identical cost by lengthening the prefix used by 1.
    // Thus, in our recurrence relation above, we need only consider the largest substring of the prefix i.e.
    //
    //   minCostToFormLastN[k] = min(A + minCostToFormLastN[k - 1], B + minCostToFormLastN[k - m])
    // 
    // If we can decide M for each k, then this is easily evaluated in O(|S|), as is done in findMinCost().
    // So: given a suffix of s, s', beginning at suffixBeginPos and a corresponding prefix p' (p' + s' = s), we need only
    // find the length of the substring of p' that has the longest common prefix with s'.  Now it's time to bust out the old
    // SuffixTreeBuilder :)
    // So: recall a few facts about suffix trees.  Reading a word w can lead us either to an explicit state or to partway along a transition
    // between explicit states.  If reading a word w reaches a state marked as final, then w is the suffix of s beginning
    // at |s| - |w|.  If w' leads to a state S', and w leads to a state S, then w' is a prefix of w if and only if w passes through S'.
    // Let's also look at the problem we want to solve: For a suffix s' beginning at suffixBeginPos and corresponding prefix p', we want the 
    // largest substring of p' that is a prefix of s'.  In other words, out of all pairs (i, w) where i + |w| < suffixBeginPos and 
    // s[i, i + |w| - 1] = w, we want to find the pair with the maximum |w|.  
    // Imagine if we read w in the suffix tree.  If reading wx reaches a final state, then there is an occurrence of w in s starting at
    // index i = |s| - |xw| = |s| - |x| - |w| and ending at |s| - |x| - |w| + |w| = |s| - |x| = i + |w|.  
    // Since i + |w| decreases as |x| increases, w can satisfy i + |w| < suffixBeginPos
    // if and only if for the longest x such that wx leads to a final state, |s| - |x| < suffixBeginPos.
    // Using a very simple O(|s|) DFS search (findLeastEndpointsForWordsLeadingToExplicitStates()), we can, for all explicit states S corresponding
    // to words w, find the smallest ending position of an occurrence of w in s.
    // Now, imagine we have read some word w in our suffix automaton and reached a final state S after passing through states S0, S1, ... Sm (S0 is the
    // root state; Sm = S, O(m) = O(|s|) - then w is the suffix of s starting from suffixBeginPos = |s| - |w|.  We're looking for a prefix w' of w that 
    // appears in p' i.e. whose smallest ending position is < suffixBeginPos.
    // As a prefix of w, reading w' take us through the first k Ancestors S0, S1, ... Sk of S for some k <= m.  So we could very easily
    // find the largest w' that leads to an explicit state (we'll deal with w' that ends partway through a transition in a mo) that is a prefix of w'
    // and whose first occurrence in s ends before suffixBeginPos: starting from Sm, count backwards through Sm, Sm-1, ... S0 (the corresponding
    // words will be in strictly decreasing order of length) until we find one whose corresponding word ends before suffixBeginPos.
    // This (mostly) solves the problem, but there are O(|s|) final states and each has O(|s|) ancestors, leading to O(|s|^2) worst case, so
    // that's no good.
    // But we can do better: we are trying to find the largest k such that the word corresponding to Sk ends before suffixBeginPos.
    // Now, imagine a k where the word w' corresponding to Sk ends at or after suffixBeginPos.  What can we say about w'x, for any
    // x, |x| > 1? The smallest endpoint of w'x in s will be at least 1 larger than the smallest endpoint of w' in s, so this won't do.
    // But all of Sk+1, Sk+2, ... etc can be expressed as w'x for some x; in other words, if we looked at S(m/2) and found that the corresponding
    // word's least endpoint was >= suffixBeginPos, then we could eliminate the later *half* of S's ancestors: that's right, it's a binary search!
    // So for each final state S we can find the largest k such that Sk's corresponding word's least endpoint < suffixBeginPos in O(log|S|).
    // Almost there: we now have an algorithm for finding, for each suffix s', the longest word w in p' such that w is a prefix of s' in O(|S|log|S|),
    // provided that w leads to an explicit state i.e. we ignore the case where reading a word that leads us partway through a transition
    // might give us a better w.  How do we deal with this?
    // Such a w would lead us partway through the transition between Sk and Sk+1, where k is the largest k such that Sk's corresponding word's
    // least endpoint is < suffixBeginPos.  Let x be the word corresponding to Sk+1 and let e be x's least endpoint (so e >= suffixBeginPos).
    // x is equal to wy for some y.  What is the least endpoint of w?
    // If f is the longest string such that reading wf leads to a final state, then the least endpoint of w is |s| - |wf| + |w| =
    // |s| - |f|.  But to reach a final state after w, we must pass through Sk+1, and we know the longest f' such that reading
    // xf' leads to a final state; thus f = uf'.
    // Thus the least endpoint of w = |s| - |f| = |s| - (|f'| + |u|) = |s| - |f'| - |u| = e - |u| i.e. if w leads to partway along
    // the transition from Sk to Sk+1, then its least endpoint is e - |u|, where |u| is the number of letters remaining on the
    // transition to Sk+1.  Thus, if we known Sk and Sk+1 (which we do) we can find the longest w leading partway along the 
    // transition from Sk to Sk+1 whose least endpoint < suffixBeginPos by trying to finding a u such that |u| is less than
    // or equal to the number of letters on the transition from Sk to Sk+1 and e - u < suffixBeginPos.
    // And that's everything we need - hooray!
    int T;
    cin >> T;
    for (int t = 0; t < T; t++)
    {
        int N, A, B;
        cin >> N >> A >> B;

        string s;
        cin >> s;
        const auto longestSubstringOfPrefixThatIsPrefixOfSuffix = findLongestCommonPrefixBetweenPrefixAndSuffix(s);
        cout << findMinCost(longestSubstringOfPrefixThatIsPrefixOfSuffix, N, A, B) << endl;
    }
}


