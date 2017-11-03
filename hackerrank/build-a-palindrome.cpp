#define BRUTE_FORCE
#define RANDOM
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#undef BRUTE_FORCE
#undef RANDOM
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
            //cout << "pairConcat: " << pairConcat << endl;
            const auto stringConcatPos = x.length();
            appendString(pairConcat);

            //cout << "# states: " << m_states.size() << endl;

            makeFinalStatesExplicitAndMarkThemAsFinal();

            for (auto& state : m_states)
            {
                if (state.isFinal)
                    state.data.isFinalY = true;
            }

            //cout << "Floop" << endl;

#if 1
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
                    //cout << "transition start-end (0-relative): " << transition.substringFollowed.startIndex - 1 << "," << transition.substringFollowed.endIndex - 1 << endl;
                    //cout << "stringConcatPos: " << stringConcatPos << endl;
                    bool needToRemoveTransition = false;
                    const auto containsUnusedChar = (transition.substringFollowed.startIndex - 1 <= stringConcatPos && realEndIndex >= stringConcatPos);
                    if (containsUnusedChar)
                    {
                        transition.substringFollowed.endIndex = stringConcatPos;
                        const auto transitionBeginsWithUnusedChar = (transition.substringFollowed.startIndex - 1 == stringConcatPos);
                        if (transitionBeginsWithUnusedChar)
                        {
                            //cout << "need to remove transition" << endl;
                            needToRemoveTransition = true;
                            state.data.isFinalX = true;
                        }
                        else
                        {
                            transition.nextState->data.isFinalX = true;
                            transition.nextState->data.isFinalY = false;
                        }
                        //cout << "marking as isFinalX" << endl;
                        transition.nextState->transitions.clear();
                        transition.nextState->data.isPruned = true;
                    }

                    if (needToRemoveTransition)
                        transitionIter = state.transitions.erase(transitionIter);
                    else
                        transitionIter++;
                }
            }
#endif
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
                string dbgStringFollowed() const
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

enum class SubstringMemberShip
{
    OriginalString,
    ReversedString
};

struct Occurrence
{
    Occurrence(int posInString, SubstringMemberShip containingString)
       : containingString{containingString}, posInString{posInString} 
    {
    }
    SubstringMemberShip containingString;
    int posInString = -1;
};

bool operator<(const Occurrence& lhs, const Occurrence& rhs)
{
    if (lhs.containingString != rhs.containingString)
        return lhs.containingString < rhs.containingString;
    return lhs.posInString < rhs.posInString;
}

vector<int> maxEvenPalindromeAt;
vector<int> maxOddPalindromeAt;

set<Occurrence>  findOccurrencesOfWordCorrespondingToCursor(Cursor cursor, int stringLength, const string& dbgString)
{
    //assert(cursor.stateData().wordLength == cursor.dbgStringFollowed().length());
    //cout << "cursor: " << cursor.dbgStringFollowed() << endl;
    assert(cursor.isOnExplicitState());
    assert(cursor.stateData().wordLength != -1);
    vector<set<Occurrence>> childrenOccurences;

    //const auto dbgReversedString = string(dbgString.rbegin(), dbgString.rend());

    const auto wordLength = cursor.stateData().wordLength;


    set<Occurrence> occurencesOfWord;
    if (cursor.stateData().isFinalX)
    {
        const auto positionInX = stringLength - wordLength;
        occurencesOfWord.insert({positionInX, SubstringMemberShip::OriginalString});
        //cout << "substring " << cursor.dbgStringFollowed() << " occurs at position " << positionInX << " in string" << endl;
    }
    if (cursor.stateData().isFinalY)
    {
        assert(cursor.isOnFinalState());
        //const auto positionInY = 2 * stringLength + 1 - cursor.stateData().wordLength;
        const auto positionInY = stringLength - wordLength;
        occurencesOfWord.insert({positionInY, SubstringMemberShip::ReversedString});
        //cout << "substring " << cursor.dbgStringFollowed() << " occurs at position " << positionInY << " in reversed string" << endl;
    }

    auto nextLetterIterator = cursor.getNextLetterIterator();
    while (nextLetterIterator.hasNext())
    {
        auto nextCursor = nextLetterIterator.afterFollowingNextLetter();
        if (!nextCursor.isOnExplicitState())
            nextCursor.followToTransitionEnd();
        childrenOccurences.push_back(findOccurrencesOfWordCorrespondingToCursor(nextCursor, stringLength, dbgString));

        nextLetterIterator++;
    }

    if (cursor.stateData().wordLength != 0)
    {
        sort(childrenOccurences.begin(), childrenOccurences.end(), [](const auto& lhs, const auto& rhs) { return rhs.size() > lhs.size(); });
        if (!childrenOccurences.empty())
        {
            // At this point, occurencesOfWord will contain at most two elements.  Since the asymptotic guarantees rely on never (or rarely) having a large
            // set copied element by element into a small set, and since the largest element of childrenOccurences will almost certainly be bigger than
            // two elements, we swap occurencesOfWord and the largest child.
            swap(occurencesOfWord, *childrenOccurences.begin());
        }
        else
        {
            // A bit of a hack, but ensure each of the (up to two) elements of occurencesOfWord due to this being a final state are compared against each other.
            childrenOccurences.push_back(occurencesOfWord);
            occurencesOfWord.clear();
        }

        for (const auto& childOccurences : childrenOccurences)
        {
            for (const auto occurrence : childOccurences)
            {
                //cout << " occurrence: " << occurrence.posInString << " in " << (occurrence.containingString == SubstringMemberShip::OriginalString ? "Original" : "Reversed") << " wordLength: " << wordLength << endl;
                for (auto checkingOddLengthPalindrome : { true, false})
                {
                    const SubstringMemberShip otherStringMembership = static_cast<SubstringMemberShip>(1 - static_cast<int>(occurrence.containingString));
                    const auto otherStringPos = stringLength - occurrence.posInString - (checkingOddLengthPalindrome ? 1 : 0);
                    const bool foundOtherHalfOfPalindrome = (occurencesOfWord.find({otherStringPos, otherStringMembership}) != occurencesOfWord.end());
                    if (foundOtherHalfOfPalindrome)
                    {
                        const int palindromeLength = wordLength * 2 - (checkingOddLengthPalindrome ? 1 : 0);
                        //cout << "  Found odd " << cursor.dbgStringFollowed() << endl;
                        auto& maxPalindromeAt = (checkingOddLengthPalindrome ? maxOddPalindromeAt : maxEvenPalindromeAt);
                        if (occurrence.containingString == SubstringMemberShip::OriginalString)
                        {
                            maxPalindromeAt[occurrence.posInString] = max(maxPalindromeAt[occurrence.posInString], palindromeLength);
                        }
                        else
                        {
                            maxPalindromeAt[otherStringPos] = max(maxPalindromeAt[otherStringPos], palindromeLength);
                        }
                    }
                }
                occurencesOfWord.insert(occurrence);
            }
        }
    }

    return occurencesOfWord;
}

vector<int> maxEvenPalindromesBruteForce(const string& a)
{
    vector<int> result(a.size());
    for (int i = 0; i < a.size(); i++)
    {
        int right = i;
        int left = i - 1;
        int size = 0;
        while (left >= 0 && right < a.size() && a[left] == a[right])
        {
            size += 2;
            left--;
            right++;
        }
        result[i] = size;
    }
    return result;
}

vector<int> maxOddPalindromesBruteForce(const string& a)
{
    vector<int> result(a.size());
    for (int i = 0; i < a.size(); i++)
    {
        int right = i + 1;
        int left = i - 1;
        int size = 1;
        while (left >= 0 && right < a.size() && a[left] == a[right])
        {
            size += 2;
            left--;
            right++;
        }
        result[i] = size;
    }
    return result;
}

void findLargestSuffixOfAInB(Cursor cursor, int lengthOfSubstringOfBSoFar, const int lengthOfA, vector<int>& results)
{
    assert(cursor.isOnExplicitState());
    //cout << "string: " << cursor.dbgStringFollowed() << " lengthOfSubstringOfBSoFar: " << lengthOfSubstringOfBSoFar << " isSubstringOfX? " << cursor.stateData().isSubstringOfX << " isSubstringOfY? " << cursor.stateData().isSubstringOfY << endl;
    if (cursor.stateData().isFinalX)
    {
        //cout << "string: " << cursor.dbgStringFollowed() << " is suffix of A" << endl;
        const int suffixBeginPos = lengthOfA - cursor.stateData().wordLength;
        results[suffixBeginPos] = lengthOfSubstringOfBSoFar;
        //cout << "Setting " << suffixBeginPos << " to " << lengthOfSubstringOfBSoFar << endl;
    }
    auto nextLetterIterator = cursor.getNextLetterIterator();
    while (nextLetterIterator.hasNext())
    {
        auto nextCursor = nextLetterIterator.afterFollowingNextLetter();
        if (!nextCursor.isOnExplicitState())
            nextCursor.followToTransitionEnd();

        const auto nextLengthOfSubstringOfBSoFar = (nextCursor.stateData().isSubstringOfY ? nextCursor.stateData().wordLength : lengthOfSubstringOfBSoFar);
        if (nextCursor.stateData().isSubstringOfY)
        {
            //cout << "string " << nextCursor.dbgStringFollowed() << " is substring of B" << endl;
        }
        findLargestSuffixOfAInB(nextCursor, nextLengthOfSubstringOfBSoFar, lengthOfA, results);

        nextLetterIterator++;
    }
}

vector<int> findLargestSuffixOfAInB(const string& a, const string& b)
{
    SuffixTreeBuilder suffixTree;
    suffixTree.addStringPairAndMarkFinalStates(a, b);

    vector<int> result(a.size() + 1);

    findLargestSuffixOfAInB(suffixTree.rootCursor(), 0, a.size(), result);

    return result;
}

vector<int> findLargestSuffixOfAInBBruteForce(const string& a, const string& b)
{
    vector<int> result(a.size() + 1);
    for (int suffixBeginPos = 0; suffixBeginPos <= a.size(); suffixBeginPos++)
    {
        //cout << "suffixBeginPos: " <<suffixBeginPos << endl;
        for (int suffixLength = 0; suffixBeginPos + suffixLength <= a.size(); suffixLength++)
        {
            const auto suffix = a.substr(suffixBeginPos, suffixLength);
            const auto inB = (b.find(suffix) != string::npos);
            //cout << "suffix: " << suffix << " in b?" << inB << endl;
            if (inB)
            {
                result[suffixBeginPos] = suffixLength;
            }
            else
                break;
        }
    }
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

void findLongestPalindromes(const string& a)
{
    // Manacher's Algorithm: see https://en.wikipedia.org/wiki/Longest_palindromic_substring for details.
    maxOddPalindromeAt.clear();
    maxEvenPalindromeAt.clear();

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
    maxEvenPalindromeAt.resize(a.size());
    maxOddPalindromeAt.resize(a.size());
    for (int i = 0; i < a.size(); i++)
    {
        maxEvenPalindromeAt[i] = p[i * 2];
        maxOddPalindromeAt[i] = p[i * 2 + 1];
    }
    //maxOddPalindromeAt = p;
    //maxEvenPalindromeAt = p;
#if 0
    int len = 0; c = 0;
    for (int i = 1; i<s2.length(); i++) {
        if (len<p[i]) {
            len = p[i]; c = i;
        }
    }
    char[] ss = Arrays.copyOfRange(s2, c-len, c+len+1);
    return String.valueOf(removeBoundaries(ss));
#endif
}

string findLongestAHeavyOrBalancedPalindrome(const string&a, const string& b)
{
    string result;
    //cout << "a: " << a << " size: " << a.size() << endl;
    //cout << "a reversed: " << string(a.rbegin(), a.rend()) << endl;
    const auto aReversed = reversed(a);
    SuffixTreeBuilder suffixTree;
    suffixTree.addStringPairAndMarkFinalStates(a, aReversed);
    //maxOddPalindromeAt.clear();
    //maxEvenPalindromeAt.clear();
    //maxOddPalindromeAt.resize(a.size());
    //maxEvenPalindromeAt.resize(a.size());
    //findOccurrencesOfWordCorrespondingToCursor(suffixTree.rootCursor(), a.size(), a);
    findLongestPalindromes(a);

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
#ifdef BRUTE_FORCE
    const auto maxEvenPalindromesBruteForce = ::maxEvenPalindromesBruteForce(a);
    const auto maxOddPalindromesBruteForce = ::maxOddPalindromesBruteForce(a);
#if 0
    for (int i = 0; i < maxOddPalindromeAt.size(); i++)
    {
        cout << "i: " << i << " maxOddPalindromeAt: " << maxOddPalindromeAt[i] << " maxOddPalindromesBruteForce: " << maxOddPalindromesBruteForce[i] << endl;
        cout << "i: " << i << " maxEvenPalindromeAt: " << maxEvenPalindromeAt[i] << " maxEvenPalindromesBruteForce: " << maxEvenPalindromesBruteForce[i] << endl;
    }
#endif
    for (int i = 0; i < maxEvenPalindromesBruteForce.size(); i++)
    {
        cout << "i: " << i << " maxOddPalindromesBruteForce: " << maxOddPalindromesBruteForce[i] << endl;
        cout << "i: " << i << " maxEvenPalindromesBruteForce: " << maxEvenPalindromesBruteForce[i] << endl;
    }
    for (int i = 0; i < maxOddPalindromeAt.size(); i++)
    {
        cout << "i: " << i << " maxOddPalindromeAt: " << maxOddPalindromeAt[i] << endl;
    }
    //assert(maxOddPalindromeAt == maxOddPalindromesBruteForce);
    //assert(maxEvenPalindromeAt == maxEvenPalindromesBruteForce);
#endif
    const auto largestSuffixOfAInBAtPos = findLargestSuffixOfAInB(aReversed, b);
#ifdef BRUTE_FORCE
    const auto largestSuffixOfAInBAtPosBruteForce = findLargestSuffixOfAInBBruteForce(aReversed, b);
    for (int i = 0; i < largestSuffixOfAInBAtPos.size(); i++)
    {
        //cout << "i: " << i << " largestSuffixOfAInBAtPos: " << largestSuffixOfAInBAtPos[i] << " largestSuffixOfAInBAtPosBruteForce: " << largestSuffixOfAInBAtPosBruteForce[i] << endl;
    }
    assert(largestSuffixOfAInBAtPos == largestSuffixOfAInBAtPosBruteForce);
#endif

    //cout << "constructing palindromes a: " << a << " b: " << b << endl;
    for (auto checkingOddLengthPalindrome : { true, false })
    {
        for (int i = 0; i < maxOddPalindromeAt.size(); i++)
        {
            const int maxSurroundingPalindromeLength = (checkingOddLengthPalindrome ? maxOddPalindromeAt[i] : maxEvenPalindromeAt[i]);
            const int maxSurroundingPalindromeBeginPos = (checkingOddLengthPalindrome ? i - (maxSurroundingPalindromeLength - 1) / 2 : i - (maxSurroundingPalindromeLength / 2));
            //cout << "is odd? " << checkingOddLengthPalindrome << " i: " << i << " maxSurroundingPalindromeLength: " << maxSurroundingPalindromeLength << " maxSurroundingPalindromeBeginPos: " << maxSurroundingPalindromeBeginPos << endl;
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
                //cout << "maxSurroundingPalindromeLength is 0!" << endl;
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
                    //cout << " found b-letter " << static_cast<char>('a' + bLetterIndex) << " at index: " << posOfBLetterInSurroundingPalindrome << endl;
                    posOfEarliestBLetterInSurroundingPalindrome = min(posOfEarliestBLetterInSurroundingPalindrome, posOfBLetterInSurroundingPalindrome);
                }
                if (posOfEarliestBLetterInSurroundingPalindrome >= i)
                {
                    // Can't use this surrounding palindrome to construct a palindrome with non-empty sB.
                    continue;
                }
                const int moveLeftOfSurroundingPalindromeBy = posOfEarliestBLetterInSurroundingPalindrome + 1 - surroundingPalindromeBeginPos;
                assert(moveLeftOfSurroundingPalindromeBy > 0);
                //cout << "moveLeftOfSurroundingPalindromeBy: " << moveLeftOfSurroundingPalindromeBy << endl;
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
                //cout << "extension: " << extension << " posOfExtensionInReversed: " << posOfExtensionInReversed << " extensionLength: " << extensionLength << endl;
                const auto sA = reversed(extension) + surroundingPalindrome;
                assert(!sA.empty());
                assert(a.find(sA) != string::npos);
                const auto sB = extension;
                assert(!sB.empty());
                assert(b.find(sB) != string::npos);
                const auto constructedPalindrome = sA + sB;
                assert(constructedPalindrome == reversed(constructedPalindrome));
                assert(constructedPalindrome.size() == constructedPalindromeLength);
                //cout << "constructedPalindrome: " << constructedPalindrome << " extension: " << extension << " surroundingPalindrome: " << surroundingPalindrome << endl;
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
            //cout << "balanced palindrome: " << palindrome << endl;
            assert(palindrome == reversed(palindrome));
            updateResult(result, palindrome);
        }
    }
    return result;
}

string findLongestPalindrome(const string&a, const string& b)
{
    string result;
    updateResult(result, findLongestAHeavyOrBalancedPalindrome(a, b));
    updateResult(result, findLongestAHeavyOrBalancedPalindrome(reversed(b), reversed(a)));
    //updateResult(result, findLongestAHeavyPalindrome(b, a));
    if (result.empty())
        result = "-1";

    return result;
}


string findLongestPalindromeBruteForce(const string&a, const string& b)
{
    //cout << "findLongestPalindromeBruteForce" << endl;
    string result;
    const int minLength = 1;
    for (int aSuffix = 0; aSuffix < a.size(); aSuffix++)
    {
        //cout << "aSuffix: " << aSuffix << endl;
        for (int aLength = minLength; aSuffix + aLength <= a.size(); aLength++)
        {
            const auto sA = a.substr(aSuffix, aLength);
            //cout << " aLength: " << aLength << endl;
            for (int bSuffix = 0; bSuffix < b.size(); bSuffix++)
            {
                for (int bLength = minLength; bSuffix + bLength <= b.size(); bLength++)
                {
                    const auto sB = b.substr(bSuffix, bLength);
                    const auto sAB = sA + sB;
                    if (sAB == reversed(sAB) && sAB.length() >= result.length())
                    {
                        const string oldResult = result;
                        if (sAB.length() > result.length())
                            result = sAB;
                        result = min(result, sAB);
                        if (oldResult != result)
                        {
                            //cout << "Found new best: " << sA << "|" << sB << endl;
                        }
                    }
                }
            }
        }
    }
    if (result.empty())
        result = "-1";
    return result;
}


int main()
{
#if 0
    const int numLetters = 3;
    const int n = 100000;
    const int numTestcases = 10;
    cout << numTestcases << endl;
    for (int i = 0; i < numTestcases; i++)
    {
        for (int j = 1; j <= 2; j++)
        {
            string s;
            for (int i = 0; i < n; i++)
            {
                s += static_cast<char>(rand() % numLetters + 'a');
            }
            cout << s << endl;
        }
    }
    return 0;

#endif
#ifndef RANDOM
    int q;
    cin >> q;

    for (int i = 0; i < q; i++)
    {
#else
    srand(time(0));
    while (true)
    {
#endif
        string a;
        string b;

#ifndef RANDOM
        cin >> a >> b;
#else
        const int maxAlphabetSize = 5;
        const int maxNumLetters = 20;
        for (int j = 1; j <= 2; j++)
        {
            string s;
            const int numLetters = rand() % maxNumLetters + 1;
            const int alphabetSize = rand() % maxAlphabetSize + 1;
            for (int i = 0; i < numLetters; i++)
            {
                s += static_cast<char>(rand() % alphabetSize + 'a');
            }
            if (j == 1)
                a = s;
            else 
                b = s;
        }
#endif

        //cout << "a: " << a << " b: " << b << endl;

        const auto result = findLongestPalindrome(a, b);
        cout << result << endl;
#ifdef BRUTE_FORCE
        const auto resultBruteForce = findLongestPalindromeBruteForce(a, b);
        cout << "result: " << result << " resultBruteForce: " << resultBruteForce << endl; 
        assert(result == resultBruteForce);
#endif
    }
}
