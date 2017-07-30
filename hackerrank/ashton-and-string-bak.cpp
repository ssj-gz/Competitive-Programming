//#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#define NO_VERIFY_UKKONEN
#endif
#include <cmath>
#include <cstdio>
#include <vector>
#include <set>
#include <map>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <memory>
#include <limits>
#include <cassert>

using namespace std;
const int numLetters = 26;

/**
 * Simple implementation of Ukkonen's algorithm:
 *  https://en.wikipedia.org/wiki/Ukkonen's_algorithm
 * for online construction of suffix trees.
 * This is almost a straight transcription of the algorithm
 * from this paper into C++, so please refer to that if
 * you want to know how it works! 
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
            int startIndex = -1;
            int endIndex = -1;
        };
        struct Transition
        {
            Transition(State *nextState, const Substring& substringFollowed)
                : nextState(nextState), substringFollowed(substringFollowed)
            {
            }
            int substringLength(int fullStringLength) const
            {
                const auto startIndex = substringFollowed.startIndex - 1;
                const auto endIndex = (substringFollowed.endIndex == openTransitionEnd ? fullStringLength - 1: substringFollowed.endIndex - 1);
                return endIndex - startIndex + 1;
            }

            State *nextState = nullptr;
            Substring substringFollowed;
        };
        struct State
        {
            vector<Transition> transitions;
            State* suffixLink = nullptr;
            State* parent = nullptr;
        };
    public:
        SuffixTreeBuilder()
        {
            // Perform the initial steps (i.e. those occurring before the "while" loop)
            // from Algorithm 2.
            m_root = createNewState();
            m_auxiliaryState = createNewState();

            for (int i = 0; i < alphabetSize; i++)
            {
                m_auxiliaryState->transitions.push_back(Transition(m_root, Substring(-(i + 1), -(i + 1))));
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
        void finalise()
        {
            finaliseAux(m_root, nullptr);
        }
        /**
         * Class used to navigate the suffix tree.  Can be invalidated by making changes to the tree!
         */
        class Cursor
        {
            public:
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
                bool isOnExplicitState() const
                {
                    return (m_transition == nullptr);
                }
                int posInTransition() const
                {
                    assert(!isOnExplicitState());
                    return m_posInTransition;
                }
                vector<char> nextLetters() const
                {
                    vector<char> nextLetters;
                    if (m_transition == nullptr)
                    {
                        for (const auto& transition : m_state->transitions)
                        {
                            nextLetters.push_back((*m_string)[transition.substringFollowed.startIndex - 1]);
                        }
                    }
                    else
                    {
                        nextLetters.push_back((*m_string)[m_transition->substringFollowed.startIndex + m_posInTransition - 1]);
                    }
                    return nextLetters;
                }
                bool canFollowLetter(char letter)
                {   
                    const vector<char>& nextLetters = this->nextLetters();
                    return find(nextLetters.begin(), nextLetters.end(), letter) != nextLetters.end();
                } 
                void followLetter(char letter)
                {
                    assert(canFollowLetter(letter));
                    if (m_transition == nullptr)
                    {
                        for (const auto& transition : m_state->transitions)
                        {
                            if (-transition.substringFollowed.startIndex == (letter - 'a' + 1))
                            {
                                m_transition = &transition;
                                break;
                            }
                            if ((*m_string)[transition.substringFollowed.startIndex - 1] == letter)
                            {
                                m_transition = &transition;
                                break;
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
#ifndef NO_VERIFY_UKKONEN
                    Cursor verify(*this);
                    for (auto index = startIndex; index != startIndex + numLetters; index++)
                    {
                        verify.followLetter(letters[index]);
                    }
#endif
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
#ifndef NO_VERIFY_UKKONEN
                    assert(verify == *this);
#endif
                }
                void followNextLetter()
                {
                    const auto nextLetters = this->nextLetters();
                    assert(nextLetters.size() == 1);
                    followLetter(nextLetters.front());
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
                        int startIndex() const
                        {
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
                void followSuffixLink()
                {
                    if (isOnExplicitState() && m_state->suffixLink)
                    {
                        if (m_state->suffixLink)
                        {
                            m_state = m_state->suffixLink;
                            movedToExplicitState();
                        }
                    }
                    else
                    {
                        assert((m_state->suffixLink == nullptr) == m_state->transitions.empty());
                        if (isOnExplicitState())
                        {
                            assert(m_state->parent && m_state->parent->suffixLink);
                            auto parentsSuffixLink = m_state->parent->suffixLink;
                            const Transition* transitionFromParent = findTransitionFromParent();
                            auto suffixLinkCursor = Cursor(parentsSuffixLink, *m_string, m_root);
                            const auto substringToFollow = transitionFromParent->substringFollowed;
                            suffixLinkCursor.followLetters(*m_string, substringToFollow.startIndex - 1, substringToFollow.endIndex - substringToFollow.startIndex + 1);
                            *this = suffixLinkCursor; 
                        }
                        else
                        {
                            assert(m_state->suffixLink);
                            auto suffixLinkCursor = Cursor(m_state->suffixLink, *m_string, m_root);
                            const auto substringToFollow = SuffixTreeBuilder::Substring(m_transition->substringFollowed.startIndex, m_transition->substringFollowed.startIndex + m_posInTransition + -1);
                            suffixLinkCursor.followLetters(*m_string, substringToFollow.startIndex - 1, substringToFollow.endIndex - substringToFollow.startIndex + 1);
                            *this = suffixLinkCursor; 
                        }
                    }
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
                        const Transition* transitionFromParent = findTransitionFromParent();
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
                string description()
                {
                    stringstream idStream;
                    idStream << "state: " << m_state << " transition: " << m_transition << " m_posInTransition: " << m_posInTransition;
                    return idStream.str();
                }
            private:
                Cursor(State* state, const string& str, State* root)
                    : m_state{state}, 
                    m_string{&str},
                    m_root{root}
                {
                }

                void movedToExplicitState()
                {
                    assert(m_state);
                    m_transition = nullptr;
                    m_posInTransition = -1;
                }

                const Transition* findTransitionFromParent()
                {
                    const Transition* transitionFromParent = nullptr;
                    for (const Transition& transition : m_state->parent->transitions)
                    {
                        if (transition.nextState == m_state)
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
                const Transition *m_transition = nullptr;
                int m_posInTransition = -1;
                const string* m_string = nullptr;
                State *m_root = nullptr;
        };
        Cursor initialCursor() const
        {
            return Cursor(m_root, m_currentString, m_root);
        }
        void dumpGraph()
        {
            dumpGraphAux(m_root, "");
        }
        vector<string> dumpExplicitStrings()
        {
            vector<string> strings;
            dumpExplicitStringsAux(m_root, "", strings);
            sort(strings.begin(), strings.end(), [](const string& lhs, const string& rhs) { return lhs.size() < rhs.size(); });
            return strings;
        }
    private:
        static const int alphabetSize = 26;
        static const int openTransitionEnd = numeric_limits<int>::max();

        string m_currentString;

        vector<unique_ptr<State>> m_states;
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
                r->transitions.push_back(Transition(rPrime, Substring(i, openTransitionEnd)));
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
                    s->transitions.push_back(Transition(r, Substring(kPrime, kPrime + p - k)));
                    r->transitions.push_back(Transition(sPrime, Substring(kPrime + p - k + 1, pPrime)));
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
            m_states.push_back(make_unique<State>());
            State *newState = m_states.back().get();
            newState->parent = parent;
            return newState;
        }
        decltype(State::transitions.begin()) findTransitionIter(State* state, int letterIndex, bool assertFound = true)
        {
            for (auto transitionIter = state->transitions.begin(); transitionIter != state->transitions.end(); transitionIter++)
            {
                if (t(transitionIter->substringFollowed.startIndex) == letterIndex || transitionIter->substringFollowed.startIndex == -letterIndex)
                    return transitionIter;
            }
            if (assertFound)
                assert(false);
            return state->transitions.end();
        };
        int t(int i)
        {
            // Ukkonen's algorithm uses 1-indexed strings and 1-indexed alphabet
            // throughout; adjust for this.
            return m_currentString[i - 1] - 'a' + 1;
        }

        void dumpGraphAux(State* s, const string& indent)
        {
            cout << indent << "state: " << s << " " << findStateIndex(s) << " suffix link: " << (s->suffixLink ? findStateIndex(s->suffixLink) : 0) << " parent: " << (s->parent ? findStateIndex(s->parent) : 0);
            const bool isTerminal = (s->transitions.empty());
            assert((s->suffixLink == nullptr) == isTerminal);
            if (isTerminal)
            {
                cout << " (terminal)" << endl;
                return;
            }
            cout << endl;
            for (const auto& transition : s->transitions)
            {
                const auto substringStartIndex = transition.substringFollowed.startIndex - 1;
                const auto substringEndIndex = (transition.substringFollowed.endIndex == openTransitionEnd ? m_currentString.size() - 1: transition.substringFollowed.endIndex - 1);
                cout << indent + " " << "transition: " << substringStartIndex << "," << substringEndIndex << (substringEndIndex == m_currentString.size() - 1 ? " (open) " : "") << " " << m_currentString.substr(substringStartIndex, substringEndIndex - substringStartIndex + 1) << " next state: " << findStateIndex(transition.nextState) << endl;
                dumpGraphAux(transition.nextState, indent + "  ");
            }
        }
        void dumpExplicitStringsAux(State* s, const string& currentString, vector<string>& destStrings)
        {
            if (s->transitions.empty())
            {
                destStrings.push_back(currentString);
                return;
            }
            for (const auto& transition : s->transitions)
            {
                const auto substringStartIndex = transition.substringFollowed.startIndex - 1;
                const auto substringEndIndex = (transition.substringFollowed.endIndex == openTransitionEnd ? m_currentString.size() - 1: transition.substringFollowed.endIndex - 1);
                const auto newCurrentString = currentString + m_currentString.substr(substringStartIndex, substringEndIndex - substringStartIndex + 1);
                dumpExplicitStringsAux(transition.nextState, newCurrentString, destStrings);
            }
        }

        void finaliseAux(State* state, Transition* transitionFromParent)
        {
            if (!state->suffixLink)
            {
                assert(state->transitions.empty());
                // Change the Substring end.
                assert(transitionFromParent->substringFollowed.endIndex == openTransitionEnd);
                transitionFromParent->substringFollowed.endIndex = m_currentString.size(); // 1-relative.
            }
            else
            {
                for (auto& transition : state->transitions)
                {
                    finaliseAux(transition.nextState, &transition);
                }
            }
        }

        long findStateIndex(State* s)
        {
            auto statePos = find_if(m_states.begin(), m_states.end(), [s](const unique_ptr<State>& state) { return state.get() == s; });
            assert(statePos != m_states.end());
            return statePos - m_states.begin();
        }
};

char bruteForce(const string& s, long k)
{
    set<string> allSubstrings;
    for (int i = 0; i < s.size(); i++)
    {
        for (int j = i; j < s.size(); j++)
        {
            const string substring = s.substr(i, j - i + 1);
            allSubstrings.insert(substring);
        }
    }
    string concatenatedSubstrings;
    string concatenatedSubstringsWithBreaks;
    for (const auto& substring : allSubstrings)
    {
        concatenatedSubstrings += substring;
        concatenatedSubstringsWithBreaks += substring + "|";
    }
    cout << "concatenatedSubstrings: " << concatenatedSubstrings << " concatenatedSubstrings.size(): " << concatenatedSubstrings.size() << endl;
    cout << "concatenatedSubstringsWithBreaks: " << concatenatedSubstringsWithBreaks << endl;

    if (k >= concatenatedSubstrings.size())
        return '\0';

    return concatenatedSubstrings[k - 1];
}

//#define EXHAUSTIVE

char computeResultAux(const string& s, long k, SuffixTreeBuilder::Cursor substringCursor, int lengthSoFar, long& sizeOfConcatenatedStrings)
{
    cout << "computeResultAux: cursor: " << substringCursor.description() << " followed: " << substringCursor.dbgStringFollowed() << " lengthSoFar: " << lengthSoFar << " sizeOfConcatenatedStrings: " << sizeOfConcatenatedStrings << endl;
    if (substringCursor.isOnExplicitState())
    {
        vector<char> sortedNextLetters = substringCursor.nextLetters();
        sort(sortedNextLetters.begin(), sortedNextLetters.end());
        const long newLengthSoFar = lengthSoFar + 1;
        for (const auto nextLetter : sortedNextLetters)
        {
            cout << "k: " << k << " sizeOfConcatenatedStrings: " << sizeOfConcatenatedStrings << " newLengthSoFar: " << newLengthSoFar << endl;
            if (k >= sizeOfConcatenatedStrings && k <= sizeOfConcatenatedStrings + newLengthSoFar)
            {
                cout << "Found during thingy: lengthSoFar: " << lengthSoFar << " followed: " << substringCursor.dbgStringFollowed() << endl;
                const auto substring = substringCursor.dbgStringFollowed() + nextLetter;
                cout << "substring: " << substring << endl;
                const int indexInSubstring = k - sizeOfConcatenatedStrings - 1;
                cout << "Index in substring: " << indexInSubstring << endl;
                cout << "Letter: " << substring[indexInSubstring] << endl;
                return substring[indexInSubstring];
            }
            sizeOfConcatenatedStrings += newLengthSoFar;
            auto cursorAfterLetter(substringCursor);
            cursorAfterLetter.followLetter(nextLetter);
            const char result = computeResultAux(s, k, cursorAfterLetter, newLengthSoFar, sizeOfConcatenatedStrings);
            if (result)
                return result;
        }
    }
    else
    {
        const long originalSizeOfConcatenatedStrings = sizeOfConcatenatedStrings;
        const auto remainderOfCurrentTransition = substringCursor.remainderOfCurrentTransition();
        int newLengthSoFar = lengthSoFar + 1;
        // TODO - optimise this - a closed-form solution is possible.
        long sizeOfConcatenatedStringsIncrease = 0;
        for (int i = 0; i < remainderOfCurrentTransition.length(); i++)
        {
            cout << "i: " << i << " remainderOfCurrentTransition.length(): " << remainderOfCurrentTransition.length() << " sizeOfConcatenatedStrings: " << sizeOfConcatenatedStrings << " newLengthSoFar: " << newLengthSoFar << endl;
            if (k >= sizeOfConcatenatedStrings && k <= sizeOfConcatenatedStrings + newLengthSoFar)
            {
                cout << "Found: sizeOfConcatenatedStrings: " << sizeOfConcatenatedStrings << " (sizeOfConcatenatedStrings + newLengthSoFar): " << (sizeOfConcatenatedStrings + newLengthSoFar) << endl;
                const auto substring = substringCursor.dbgStringFollowed() + s.substr(remainderOfCurrentTransition.startIndex() + i, remainderOfCurrentTransition.length() - i);
                cout << "substring: " << substring << endl;
                const int indexInSubstring = k - sizeOfConcatenatedStrings - 1;
                cout << "Index in substring: " << indexInSubstring << endl;
                cout << "Letter: " << substring[indexInSubstring] << endl;
                return substring[indexInSubstring];
            }
            //sizeOfConcatenatedStringsIncrease += newLengthSoFar;
            sizeOfConcatenatedStrings += newLengthSoFar;
            newLengthSoFar++;
            substringCursor.followNextLetter();
        }
        //sizeOfConcatenatedStrings += sizeOfConcatenatedStringsIncrease;
        //cout << " after: " << substringCursor.dbgStringFollowed() << endl;
        const char result = computeResultAux(s, k, substringCursor, newLengthSoFar, sizeOfConcatenatedStrings);
        if (result)
            return result;
    }
    return '\0';
}

char computeResult(const string& s, long k)
{
    SuffixTreeBuilder suffixTree;
    suffixTree.appendString(s);
    //suffixTree.dumpGraph();

    long sizeOfConcatenatedStrings = 0;
    return computeResultAux(s, k, suffixTree.initialCursor(), 0, sizeOfConcatenatedStrings);
}

int main() {
#ifndef EXHAUSTIVE
    int T;
    cin >> T;
    for (int t = 0; t < T; t++)
    {
        string s;
        cin >> s;
        long k;
        cin >> k;
        cout << "s: " << s << " k: " << k << endl;
        const auto result = computeResult(s, k);
        const auto bruteForceResult = bruteForce(s, k);
        cout << "optimised result: " << result << endl;
        cout << "Brute force: " << bruteForceResult << endl;
        cout << result << endl;
    }
#else
    string s = "a";
    const int numLetters = 3;
    while (true)
    {
        cout << "s: " << s <<  " size: " << s.size() << endl;

        long k = 1;
        while (true)
        {
            cout << " k: " << k << endl;
            const auto result = computeResult(s, k);
            const auto bruteForceResult = bruteForce(s, k);
            cout << "optimised result: " << (result ? string() + result : "null") << endl;
            cout << "Brute force: " << (bruteForceResult ? string() + bruteForceResult : "null")<< endl;
            if (bruteForceResult == '\0')
            {
                assert(result == bruteForceResult);
                break;
            }
            assert(result == bruteForceResult);
            k++;
        }

        int index = 0;
        while (index < s.size() && s[index] == 'a' + numLetters)
        {
            s[index] = 'a';
            index++;
        }
        if (index == s.size())
        {
            s.push_back('a');
        }
        else
        {
            s[index]++;
        }
    }
#endif
    return 0;
}

