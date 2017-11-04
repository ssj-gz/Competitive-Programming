// Simon St James (ssjgz) - 2017-11-03 09:48
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <set>
#include <limits>
#include <cassert>
#include <utility>
#include <algorithm>

using namespace std;

struct StateData
{
    int wordLength = -1;
    bool isFinalX = false;
    bool isFinalY = false;

    bool isSubstringOfX = false;
    bool isSubstringOfY = false;
    
    bool isPruned = false;
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
            m_root->data.wordLength = 0;
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
        void addStringPairAndMarkFinalStates(const string& x, const string& y)
        {
            const auto stringConcatMarker = '|';
            const auto pairConcat = x + stringConcatMarker + y;
            const auto stringConcatPos = x.length();
            appendString(pairConcat);

            makeFinalStatesExplicitAndMarkThemAsFinal();

            for (auto& state : m_states)
            {
                if (state.isFinal)
                    state.data.isFinalY = true;
            }


            // Truncate transitions containing stringConcatMarker, and mark the state they lead to as isFinalX.
            for (auto& state : m_states)
            {
                for (auto transitionIter = state.transitions.begin(); transitionIter != state.transitions.end(); )
                {
                    Transition& transition = *transitionIter;
                    if (transition.substringFollowed.startIndex < 1)
                    {
                        transitionIter++;
                        continue;
                    }
                    const auto realEndIndex = (transition.substringFollowed.endIndex == openTransitionEnd ? static_cast<int>(m_currentString.size() - 1) : transition.substringFollowed.endIndex - 1);
                    transition.substringFollowed.endIndex = realEndIndex + 1;
                    bool needToRemoveTransition = false;
                    const auto containsUnusedChar = (transition.substringFollowed.startIndex - 1 <= stringConcatPos && realEndIndex >= stringConcatPos);
                    if (containsUnusedChar)
                    {
                        transition.substringFollowed.endIndex = stringConcatPos;
                        const auto transitionBeginsWithUnusedChar = (transition.substringFollowed.startIndex - 1 == stringConcatPos);
                        if (transitionBeginsWithUnusedChar)
                        {
                            needToRemoveTransition = true;
                            state.data.isFinalX = true;
                        }
                        else
                        {
                            transition.nextState->data.isFinalX = true;
                            // The state nextState has been truncated to before the word separator, so it can't be a final state for Y.
                            transition.nextState->data.isFinalY = false;
                        }
                        transition.nextState->transitions.clear();
                        transition.nextState->data.isPruned = true;
                    }

                    if (needToRemoveTransition)
                        transitionIter = state.transitions.erase(transitionIter);
                    else
                        transitionIter++;
                }
            }
            // Ensure we have a wordLength for all states.
            for (auto& state : m_states)
            {
                for (auto& transition : state.transitions)
                {
                    if (transition.nextState->data.wordLength == -1)
                    {
                        transition.nextState->data.wordLength = state.data.wordLength + transition.substringFollowed.length(m_currentString.size());
                    }
                }
            }
            // Update isSubstringOfX/isSubstringOfY for all reachable states.
            for (auto& state : m_states)
            {
                if (state.data.isFinalX)
                {
                    auto stateToMark = &state;
                    while (stateToMark && !stateToMark->data.isSubstringOfX)
                    {
                        stateToMark->data.isSubstringOfX = true;
                        stateToMark = stateToMark->parent;
                    }

                }
                if (state.data.isFinalY)
                {
                    auto stateToMark = &state;
                    while (stateToMark && !stateToMark->data.isPruned && !stateToMark->data.isSubstringOfY)
                    {
                        stateToMark->data.isSubstringOfY = true;
                        stateToMark = stateToMark->parent;
                    }
                }
            }
        }
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
            for (auto& state : m_states)
            {
                //cout << "state: " << &state << " wordLength: " << state.data.wordLength << endl;
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
        static const int alphabetSize = 28; // Include the magic '{' for making final states explicit and the magic '|' for acting as string concatenation marker - assumes the input string has no '{''s or '|''s, obviously!
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
                    r->data.wordLength = s->data.wordLength + p - k + 1;
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

void findLargestSuffixOfReversedAInB(Cursor cursor, int lengthOfSubstringOfBSoFar, const int lengthOfA, vector<int>& results)
{
    assert(cursor.isOnExplicitState());
    if (cursor.stateData().isFinalX)
    {
        const int suffixBeginPos = lengthOfA - cursor.stateData().wordLength;
        results[suffixBeginPos] = lengthOfSubstringOfBSoFar;
    }
    auto nextLetterIterator = cursor.getNextLetterIterator();
    while (nextLetterIterator.hasNext())
    {
        auto nextCursor = nextLetterIterator.afterFollowingNextLetter();
        if (!nextCursor.isOnExplicitState())
            nextCursor.followToTransitionEnd();

        const auto nextLengthOfSubstringOfBSoFar = (nextCursor.stateData().isSubstringOfY ? nextCursor.stateData().wordLength : lengthOfSubstringOfBSoFar);
        findLargestSuffixOfReversedAInB(nextCursor, nextLengthOfSubstringOfBSoFar, lengthOfA, results);

        nextLetterIterator++;
    }
}

// For each suffixBeginPos = 0, ... , a.size(), find the largest prefix of the suffix of a beginning
// at suffixBeginPos that is a substring of b.
vector<int> findLargestSuffixOfReversedAInB(const string& a, const string& b)
{
    SuffixTreeBuilder suffixTree;
    suffixTree.addStringPairAndMarkFinalStates(a, b);

    vector<int> result(a.size() + 1);

    findLargestSuffixOfReversedAInB(suffixTree.rootCursor(), 0, a.size(), result);

    return result;
}


void updateResult(string& result, const string& newCandidateResult)
{
    if (newCandidateResult.length() > result.length())
        result = newCandidateResult;
    else if (newCandidateResult.length() == result.length())
        result = min(result, newCandidateResult);
}

string reversed(const string& s)
{
    return {s.rbegin(), s.rend()};
}

auto findLongestPalindromes(const string& a)
{
    // Manacher's Algorithm: see https://en.wikipedia.org/wiki/Longest_palindromic_substring for details.

    string s2(a.size() * 2 + 1, '\0');
    // Add boundaries.
    for (int i = 0; i<(s2.length()-1); i = i+2) {
        s2[i] = '|';
        s2[i+1] = a[i/2];
    }
    s2[s2.length()-1] = '|';

    vector<int> p(s2.length());
    int c = 0, r = 0; // Here the first element in s2 has been processed.
    int m = 0, n = 0; // The walking indices to compare if two elements are the same
    for (int i = 1; i<s2.length(); i++) {
        if (i>r) {
            p[i] = 0; m = i-1; n = i+1;
        } else {
            int i2 = c*2-i;
            if (p[i2]<(r-i-1)) {
                p[i] = p[i2];
                m = -1; // This signals bypassing the while loop below. 
            } else {
                p[i] = r-i;
                n = r+1; m = i*2-n;
            }
        }
        while (m>=0 && n<s2.length() && s2[m]==s2[n]) {
            p[i]++; m--; n++;
        }
        if ((i+p[i])>r) {
            c = i; r = i+p[i];
        }
    }
    vector<int> maxOddPalindromeAt(a.size());
    vector<int> maxEvenPalindromeAt(a.size());
    for (int i = 0; i < a.size(); i++)
    {
        maxOddPalindromeAt[i] = p[i * 2 + 1];
        maxEvenPalindromeAt[i] = p[i * 2];
    }
    return make_pair(maxOddPalindromeAt, maxEvenPalindromeAt);
}

string findLongestAHeavyOrBalancedPalindrome(const string&a, const string& b)
{
    string result;
    const auto aReversed = reversed(a);
    SuffixTreeBuilder suffixTree;
    suffixTree.addStringPairAndMarkFinalStates(a, aReversed);
    const auto longestPalindromes = findLongestPalindromes(a);
    const auto maxOddPalindromeAt = longestPalindromes.first;
    const auto maxEvenPalindromeAt = longestPalindromes.second;

    // Make a lookup table so we can rapidly find the earliest occurrence in A of a letter in B in a given
    // range of A.
    const int alphabetSize = 26;
    bool isLetterIndexUsedInB[alphabetSize] = {};
    for (const auto letter : b)
    {
        isLetterIndexUsedInB[letter - 'a'] = true;
    }
    vector<vector<int>> nextOccurenceOfLetterIndexAtOrAfter(alphabetSize, vector<int>(a.size()));
    for (int letterIndex = 0; letterIndex < alphabetSize; letterIndex++)
    {
        int nextOccurenceOfLetter = -1;
        for (int i = a.size() - 1; i >= 0; i--)
        {
            if (a[i] -'a' == letterIndex)
                nextOccurenceOfLetter = i;
            nextOccurenceOfLetterIndexAtOrAfter[letterIndex][i] = nextOccurenceOfLetter;
            assert(nextOccurenceOfLetterIndexAtOrAfter[letterIndex][i] == -1 || nextOccurenceOfLetterIndexAtOrAfter[letterIndex][i] >= i);
        }
    }

    const auto largestSuffixOfAInBAtPos = findLargestSuffixOfReversedAInB(aReversed, b);

    for (auto checkingOddLengthPalindrome : { true, false })
    {
        // Go front back to front; otherwise, we might enter a situation where the extensions we find are suffixes of each other, leading to a performance problem when
        // we update the result.
        for (int i = maxOddPalindromeAt.size() - 1; i >= 0; i--) 
        {
            const int maxSurroundingPalindromeLength = (checkingOddLengthPalindrome ? maxOddPalindromeAt[i] : maxEvenPalindromeAt[i]);
            const int maxSurroundingPalindromeBeginPos = (checkingOddLengthPalindrome ? i - (maxSurroundingPalindromeLength - 1) / 2 : i - (maxSurroundingPalindromeLength / 2));
            if (maxSurroundingPalindromeBeginPos == string::npos)
                continue;
            assert(a.substr(maxSurroundingPalindromeBeginPos, maxSurroundingPalindromeLength) == reversed(a.substr(maxSurroundingPalindromeBeginPos, maxSurroundingPalindromeLength)));
            int posOfExtensionInReversed = a.size() - maxSurroundingPalindromeBeginPos;
            const int maxPalindromeExtensionLength = largestSuffixOfAInBAtPos[posOfExtensionInReversed];

            int surroundingPalindromeLength = maxSurroundingPalindromeLength;
            int surroundingPalindromeBeginPos = maxSurroundingPalindromeBeginPos;
            int extensionLength = maxPalindromeExtensionLength;

            if (extensionLength == 0)
            {
                // sB is the extension, so this would result in an empty sB, which is forbidden.
                // Choosing the maximal surrounding palindrome will leave to empty sB, which is not allowed.
                // We must instead shrink the surrounding palindrome until it loses a letter that is in B,
                // then pick that letter as the extension.
                int posOfEarliestBLetterInSurroundingPalindrome = numeric_limits<int>::max();
                for (int bLetterIndex = 0; bLetterIndex < alphabetSize; bLetterIndex++)
                {
                    if (!isLetterIndexUsedInB[bLetterIndex])
                        continue;

                    const int posOfBLetterInSurroundingPalindrome = nextOccurenceOfLetterIndexAtOrAfter[bLetterIndex][surroundingPalindromeBeginPos];
                    if (posOfBLetterInSurroundingPalindrome == -1)
                        continue;
                    posOfEarliestBLetterInSurroundingPalindrome = min(posOfEarliestBLetterInSurroundingPalindrome, posOfBLetterInSurroundingPalindrome);
                }
                if (posOfEarliestBLetterInSurroundingPalindrome >= i)
                {
                    // Can't use this surrounding palindrome to construct a palindrome with non-empty sB.
                    continue;
                }
                const int moveLeftOfSurroundingPalindromeBy = posOfEarliestBLetterInSurroundingPalindrome + 1 - surroundingPalindromeBeginPos;
                assert(moveLeftOfSurroundingPalindromeBy > 0);
                surroundingPalindromeBeginPos += moveLeftOfSurroundingPalindromeBy;
                surroundingPalindromeLength -= 2 * moveLeftOfSurroundingPalindromeBy;
                posOfExtensionInReversed -= moveLeftOfSurroundingPalindromeBy;
                extensionLength = 1; 
            }

            const int constructedPalindromeLength = surroundingPalindromeLength + 2 * extensionLength;
            if (constructedPalindromeLength >= result.length())
            {
                const auto surroundingPalindrome = a.substr(surroundingPalindromeBeginPos, surroundingPalindromeLength);
                assert(surroundingPalindrome == reversed(surroundingPalindrome));
                const auto extension = aReversed.substr(posOfExtensionInReversed, extensionLength);
                const auto sA = reversed(extension) + surroundingPalindrome;
                assert(!sA.empty());
                assert(a.find(sA) != string::npos);
                const auto sB = extension;
                assert(!sB.empty());
                assert(b.find(sB) != string::npos);
                const auto constructedPalindrome = sA + sB;
                assert(constructedPalindrome == reversed(constructedPalindrome));
                assert(constructedPalindrome.size() == constructedPalindromeLength);
                updateResult(result, constructedPalindrome);
            }
        }
    }
    // Balanced palindromes.
    for (int i = 0; i < largestSuffixOfAInBAtPos.size(); i++)
    {
        const int palindromeLength = 2 * largestSuffixOfAInBAtPos[i];
        if (palindromeLength > 0 && palindromeLength >= result.length())
        {
            const auto sB = aReversed.substr(i, palindromeLength / 2);
            assert(!sB.empty());
            assert(b.find(sB) != string::npos);
            const auto sA = reversed(sB);
            assert(!sA.empty());
            assert(a.find(sA) != string::npos);
            const auto palindrome = sA + sB;
            assert(palindrome == reversed(palindrome));
            updateResult(result, palindrome);
        }
    }
    return result;
}

string findLongestConstructiblePalindrome(const string&a, const string& b)
{
    string result;
    updateResult(result, findLongestAHeavyOrBalancedPalindrome(a, b));
    updateResult(result, findLongestAHeavyOrBalancedPalindrome(reversed(b), reversed(a)));
    if (result.empty())
        result = "-1";

    return result;
}

int main()
{
    // Slightly disappointed with this one; as we'll see, the problem largely reduces to finding, for 
    // all positions in a string, the largest odd palindrome "centered" around that position and the largest
    // even one: I knew that there was a linear-time algorithm for this (Manacher's Algorithm) but, of course,
    // I consider using other people's algorithms cheating, so I didn't want to do this :)
    //
    // After a while, I figured out a way of doing this using suffix trees in O(N * (log N)^2) but, sadly,
    // this wasn't quite fast enough (perhaps a factor of two to slow) so I had to resort to Manacher's after all :(
    //
    // Anyway, so: how do we solve the actual problem? Let sA be a string in A and sB a string in B; if sA + sB is a 
    // palindrome P, then we call P a "constructed palindrome".  The aim is to find the largest such P.
    //
    // There are three possibilities for P = sA + sB: |sA| > |sB|; |sA| == |sB|; and |sA| < |sB|.  We say that such a P
    // is A-heavy, balanced, and B-heavy, respectively.  Let's deal with the first one (P is A-heavy) first; the second is
    // easy, and the third follows from the first.  The full problem requires that |sB| > 0; for the time being, let's
    // ignore this and allow an empty sB.
    //
    // So let N be the length of the maximum constructed palindrome, and let P be any A-heavy palindrome of length N.  If P
    // can be constructed in more than one way, pick the one with the smallest sB.  If |P| is odd, then say that P is odd; else
    // say that P is even; we'll deal only with the odd case as the even follows from it very easily (NB: an odd constructed 
    // palindrome cannot be balanced, so can only be A-heavy or B-heavy).
    //
    // Clearly, P can be expressed as reversed(sB).P'.sB for some odd-length string P', where sA = reversed(sB).P' and the centre of P
    // is the centre of P'.  Let i be the position of sA in A; then again it is clear that the centre of P (and so, the centre of P') 
    // is at c = i + |sB| + |P' - 1| / 2.  Additionally, P' must itself be a palindrome since P is a palindrome and peeling off the last and
    // first elements of a palindrome still gives a palindrome; peeling off the first and last |sB| elements gives P'.
    //
    // Further, we see that, if we allow empty sB, then P' is the largest palindrome centred at c.  Assume otherwise; then there is some P''
    // such that |P''| == |P'| + 1, P'' is palindromic, and P'' is centred on c.  Let sB' be sB with the first letter removed; then
    // we can set sA' = reversed(sB').P'' and we see that P = sA' + sB' and |sB'| < |sB|, contradicting minimality of sB.
    //
    // We call P' the "surrounding palindrome" of c, and sB the "extension" of P'.  We have that reversed(extension) occurs in A and ends just
    // before P', and extension occurs in B.  It's easily seen that if P is maximal, then extension is the longest such string; assume otherwise:
    // then there is some sB', |sB'| > |sB|, such that reversed(sB') occurs in A and ends just before P' and sB' is in B: but then
    // we could construct a new palindrome with reversed(sB').P'.sB' that would be two longer than P, a contradiction.
    //
    // Putting all this together, we see that if we want to find the longest A-heavy odd-length constructed palindrome in A where we allow empty sB,
    // we need only, for each position c in A:
    // 
    //  1) find the longest surrounding palindrome P' around c;
    //  2) find the longest "extension", which is a substring of B such that reversed(sB) occurs in A and ends directly before P' in A.
    //
    // 1) is trivial using Manacher's Algorithm; but what of 2)?
    //
    // If we reverse A, it is equivalent to finding, for each prefix p of the suffix of reversed(A) ending just after P' in reversed(A'), the longest
    // p which is a substring of B.  This is fairly easy: let X = reversed(A), Y = B, and take the combined string X#Y, where "#" is a special symbol that does not occur in
    // X or Y, and find the suffix tree; then every string represented by this tree will either be a substring of X; a substring of Y; a substring of both X and Y; or 
    // a substring that contains the "#" symbol.  Say we did a DFS on the tree and reached a state s with a transition t that led to s' and included the "#" symbol
    // (determining whether t includes # can be trivially done in O(1)): if we truncated t to form a new transition t' that stopped just before the # and erased
    // t' if it ended up being empty, then we'd eradicate all substrings that contained #, leaving only substrings of X, Y, or both: additionally, 
    // we'd see that the string followed to s' (or s, if t' ended up being erased) is a suffix of X, so all ancestors of s (or s'!) are substrings of X.
    // In this way, we can obtain a suffix tree where the word leading to any state s is a substring of X, Y or both, and we can mark all s accordingly.
    // This is performed in addStringPairAndMarkFinalStates() (although technically we don't bother with doing a DFS and take an equivalent approach).
    //
    // It's then trivial to do another DFS from the root and find out, for each suffix of X, the largest prefix p of that suffix of X such that p is a substring of Y:
    // this is done in findLargestSuffixOfReversedAInB(). It's hopefully obvious that we need only consider explicit states in this DFS; for any words w and w' that
    // end up partially along the same transition t, w is a substring of X if and only if w' is a substring of X, and the same for Y.  
    
    // Since X = reversed(A), Y = B, we see that for any suffix of reversed(A), it's easy to find the largest prefix of that suffix that is a substring of B.
    //
    // So we can now find the largest odd-length A-heavy constructed palindrome P as long as we allow sB to be empty.  What if we don't allow empty sB?
    // We've seen that we can find the max P by, for each position c, finding the max surrounding palindrome P' and setting P = reversed(extension).P'.extension.
    // What if this gives an empty extension? Then its easy to see that the max constructable palindrome around c is formed by progressively shrinking P' (by trimming off
    // the pair of first and last letters) until the pair that we trim off are equal to a letter in B, b say: we then set P = b.P''.b, where P'' is the
    // trimmed P'.
    //
    // Dealing with even-length palindromes is pretty much identical so we omit it.  Dealing with balanced palindromes is easy: here, sB = reversed(sA),
    // and since reversed(sA) is a substring of reversed(A), and a substring of a string is a prefix of a suffix of that string, we need 
    // to find the largest prefix of a suffix of reversed(A) which is a substring of B; i.e. we just need the result of findLargestSuffixOfReversedAInB().
    //
    // What about the final case, B-heavy palindromes? Just note that if A' = reversed(B) and B' = reversed(A), then P is an B-heavy palindrome constructible
    // from A and B  if and only P is a A'-Heavy palindrome constructible from A' and B' i.e. we just swap A and B, reverse them, and go through
    // the whole process again, picking the best result out of both approaches.
    //
    // And that's it!
    int q;
    cin >> q;

    for (int i = 0; i < q; i++)
    {
        string a;
        string b;

        cin >> a >> b;

        const auto result = findLongestConstructiblePalindrome(a, b);
        cout << result << endl;
    }
}
