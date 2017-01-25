#define SUBMISSION
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
            int index = -1;
        };
    public:
        static const char markerChar = '#';
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
        void truncateStringsContainingMarker()
        {
            vector<int> orderedMarkerPositions;
            int currentPos = 0;
            while (m_currentString.find(markerChar, currentPos) != string::npos)
            {
                const int markerPos = m_currentString.find(markerChar, currentPos);
                orderedMarkerPositions.push_back(markerPos);
                currentPos = markerPos + 1;
            }
            truncateStringsContainingMarkerAux(m_root, orderedMarkerPositions);
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
                string description() const
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
        static const int alphabetSize = 27; // 'a' - 'z', plus markerChar.
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
            newState->index = m_states.size() - 1;
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
            // Ukkonen's algorithm uses 1-indexed strings throughout; adjust for this.
            return letterIndex(m_currentString[i - 1]);
        }
        int letterIndex(char letter)
        {
            // Ukkonen's algorithm uses 1-indexed alphabet
            // throughout; adjust for this.
            if (letter == markerChar)
                return 27;
            return letter - 'a' + 1;
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

        void truncateStringsContainingMarkerAux(State* state, const vector<int>& orderedMarkerPositions)
        {
            bool transitionRemoved = false;
            do
            {
                transitionRemoved = false;
                for (auto transitionIter = state->transitions.begin(); transitionIter != state->transitions.end(); transitionIter++)
                {
                    Transition& transition = *transitionIter;
                    const auto substringStartIndex = transition.substringFollowed.startIndex - 1;
                    const auto substringEndIndex = (transition.substringFollowed.endIndex == openTransitionEnd ? m_currentString.size() - 1: transition.substringFollowed.endIndex - 1);
                    const auto candidateMarkerPosIter = lower_bound(orderedMarkerPositions.begin(), orderedMarkerPositions.end(), substringStartIndex);
                    if (candidateMarkerPosIter != orderedMarkerPositions.end())
                    {
                        const auto candidateMarkerPos = *candidateMarkerPosIter;
                        if (candidateMarkerPos > substringEndIndex)
                            continue;
                        if (candidateMarkerPos == substringStartIndex)
                        {
                            state->transitions.erase(transitionIter);
                            transitionRemoved = true;
                            break;
                        }
                        else
                        {
                            transition.substringFollowed.endIndex = candidateMarkerPos;
                        }
                    }
                }
            } while (transitionRemoved);
            for (auto& transition : state->transitions)
            {
                truncateStringsContainingMarkerAux(transition.nextState, orderedMarkerPositions);
            }
        }

        long findStateIndex(State* s)
        {
            auto statePos = find_if(m_states.begin(), m_states.end(), [s](const unique_ptr<State>& state) { return state.get() == s; });
            assert(statePos != m_states.end());
            assert(s->index == statePos - m_states.begin());
            return statePos - m_states.begin();
        }
};

void buildSubstringsGeneratedAfterStateTable(SuffixTreeBuilder::Cursor substringCursor, long& numSubstringsGenerated, vector<long>& numSubstringsGeneratedAtStateId)
{
    if (substringCursor.isOnExplicitState())
    {
        vector<char> sortedNextLetters = substringCursor.nextLetters();
        sort(sortedNextLetters.begin(), sortedNextLetters.end());
        for (const auto nextLetter : sortedNextLetters)
        {
            auto cursorAfterLetter(substringCursor);
            cursorAfterLetter.followLetter(nextLetter);
            numSubstringsGenerated++;
            buildSubstringsGeneratedAfterStateTable(cursorAfterLetter, numSubstringsGenerated, numSubstringsGeneratedAtStateId);
        }
        numSubstringsGeneratedAtStateId[substringCursor.stateId()] = numSubstringsGenerated;
    }
    else
    {
        const auto remainderOfCurrentTransition = substringCursor.remainderOfCurrentTransition();
        // Follow remainder of transition in constant time and recurse.
        numSubstringsGenerated += remainderOfCurrentTransition.length();
        auto cursorAfter(substringCursor);
        cursorAfter.followNextLetters(remainderOfCurrentTransition.length());
        buildSubstringsGeneratedAfterStateTable(cursorAfter, numSubstringsGenerated, numSubstringsGeneratedAtStateId);
    }
}

string findString(const int k, const SuffixTreeBuilder::Cursor cursor, const vector<long>& numSubstringsGeneratedAtStateId)
{
    auto cursorAfterFollowingLetterToNextState = [](const SuffixTreeBuilder::Cursor& cursor, char letter)
    {
        auto cursorAfterTransition(cursor);
        cursorAfterTransition.followLetter(letter);
        if (!cursorAfterTransition.isOnExplicitState())
            cursorAfterTransition.followToTransitionEnd();
        return cursorAfterTransition;
    };
    assert(cursor.isOnExplicitState());
    const vector<char>& nextLetters = cursor.nextLetters();
    if (nextLetters.empty())
    {
        const long numSubstringsGeneratedAtState = numSubstringsGeneratedAtStateId[cursor.stateId()];
        const long overshotBy = numSubstringsGeneratedAtState - k;
        // This state has generated a number of substrings in excess of k (or maybe equal to).  The desired result is
        // the current substring pointed to by cursor, with the last "overshotBy" characters
        // removed (each such character would have been responsible for generated one substring).
        const auto stringFollowed = cursor.stringFollowed();
        return stringFollowed.substr(0, stringFollowed.size() - overshotBy);
    }
    char nextLetterToFollow = '\0';
    long minStringsGeneratorByChildGreaterThanK = numeric_limits<long>::max();
    // Find the letter that leads to a state with has numSubstringsGeneratedAtState at least k,
    // and which has minimal numSubstringsGeneratedAtState among all such states.
    for (const auto nextLetter : nextLetters)
    {
        const auto cursorAfterTransition = cursorAfterFollowingLetterToNextState(cursor, nextLetter);
        const long numSubstringsGeneratedAtState = numSubstringsGeneratedAtStateId[cursorAfterTransition.stateId()];
        if (numSubstringsGeneratedAtState >= k && numSubstringsGeneratedAtState <= minStringsGeneratorByChildGreaterThanK)
        {
            minStringsGeneratorByChildGreaterThanK = numSubstringsGeneratedAtState;
            nextLetterToFollow = nextLetter;
        }
    }
    if (!nextLetterToFollow)
    {
        // If we're here, then K is greater than the number of strings generated at *any* state.
        return "INVALID";
    }
    // Recurse into next state.
    const auto cursorAfterTransition = cursorAfterFollowingLetterToNextState(cursor, nextLetterToFollow);
    return findString(k, cursorAfterTransition, numSubstringsGeneratedAtStateId);
}

void printResults(const vector<string>& w, const vector<long>& ks)
{
    SuffixTreeBuilder unionOfWSuffixTree;
    string markerDelimitedConcat;
    for (const auto& s : w)
    {
        markerDelimitedConcat += s + SuffixTreeBuilder::markerChar;
    }
    unionOfWSuffixTree.appendString(markerDelimitedConcat);
    // After truncating the strings containing marker, the suffix tree will represent
    // exactly the union of all the substrings of all of the w's.
    unionOfWSuffixTree.truncateStringsContainingMarker();

    // For each state, find out how many substrings would have been generated in a (lexicographic) DFS
    // when all children of that state have been explored.
    vector<long> numSubstringsGeneratedAtStateId(unionOfWSuffixTree.numStates(), -1);
    long numSubstringsGenerated = 0;
    buildSubstringsGeneratedAfterStateTable(unionOfWSuffixTree.initialCursor(), numSubstringsGenerated, numSubstringsGeneratedAtStateId);

    for (const auto k : ks)
    {
        cout << findString(k, unionOfWSuffixTree.initialCursor(), numSubstringsGeneratedAtStateId) << endl;
    }
}

int main() {
    // Fairly easy one.  Firstly, we have the issue of forming a suffix tree that represents the
    // union of all substrings of all of the w's.  This can be formed quite easily: nominate a 
    // markerChar - a character that won't appear in any of the w's; '#' is chosen here - 
    // and concatenate all the w's together into one string s, separated by markerChars 
    // ie if w = { 'aa', 'abc', 'def' } (n = 3), then the string s formed would be aa#abc#def#
    // (the trailing # can be removed if you want; I didn't bother).
    // Then add s to a suffix tree, and perform an operation that does a DFS on the suffix 
    // tree, stopping whenever we encounter a markerChar and truncating the transition
    // that leads to it to one character before the markerChar; this will give us
    // the desired unionOfWSuffixTree.  A naive implementation of this DFS will take O(|s|^2), 
    // but a bit of cunning allows us to determine the first location of a markerChar in a transition 
    // in the suffix tree in O(log(n)) (n = number of strings, remember), so the whole process
    // is O(|S|xlog(n)).  This is all handled via truncateStringsContainingMarker().
    // So: we have a suffix tree representing precisely all substrings of all n w's.  Imagine performing a 
    // DFS on this tree where, whenever we encounter a choice of which letter to follow next,
    // we choose them in alphabetical order: this DFS would sketch out all substrings in 
    // lexicographical order.  We can count the number of substrings generated as we go (each char we follow
    // would generate one extra substring), and  we can also speed up from O(|s|^2) to O(|s|) by not following 
    // transitions character by character/ but instead adding the number of characters in each transition to 
    // the count of the number of substrings.  For each *explicit* state, we associate a count of the number
    // of substrings that would have been generated in the DFS when all children of that state have been explored.
    // This is handled by buildSubstringsGeneratedAfterStateTable(), and the results for each state 
    // stored in numSubstringsGeneratedAtStateId.

    // Finally, we can use this numSubstringsGeneratedAtStateId to find the kth string by performing 
    // (yet another!) DFS of the suffix tree: this time, when given a choice of the next letter to pick, we choose
    // the one that leads to the state *whose numSubstringsGeneratedAtStateId is at least k, and whose 
    // numSubstringsGeneratedAtStateId is minimal among such states*.  Transitions are simply skipped; we 
    // care only about explicit states.  If there is no such letter i.e. all child states have 
    // numSubstringsGeneratedAtStateId strictly less than k, there we can stop: there is no kth word.
    // When we reach a terminal state, the kth element will be some prefix of the current substring that we've 
    // followed so far. Noting that each successive character in this substring adds one extra substring to the list 
    // of those found, we simply truncate the current substring by the value of 
    // numSubstringsGeneratedAtStateId - k for this terminal state, and that is the answer!

    long n;
    cin >> n;
    vector<string> w;
    for (int i = 0; i < n; i++)
    {
        string s;
        cin >> s;
        w.push_back(s);
    }
    long q;
    cin >> q;
    vector<long> k(q);
    for (int i = 0; i < q; i++)
    {
        cin >> k[i];
    }
    printResults(w, k);
    return 0;
}

