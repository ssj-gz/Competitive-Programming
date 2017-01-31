#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#define NO_VERIFY_UKKONEN
#endif
#include <cmath>
#include <cstdio>
#include <vector>
#include <stack>
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
            Transition(State *nextState, const Substring& substringFollowed)
                : nextState(nextState), substringFollowed(substringFollowed)
            {
            }
            int substringLength(int fullStringLength) const
            {
                return substringFollowed.length(fullStringLength);
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
            //cout << "m_auxiliaryState: " << m_auxiliaryState << endl;

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
                    //cout << " need to follow letter: " << letter << endl;
                    assert(canFollowLetter(letter));
                    if (m_transition == nullptr)
                    {
                        for (auto& transition : m_state->transitions)
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
                    //cout << "Need to follow letters: " << letters.substr(startIndex, numLetters) << " (" << numLetters << " of them)" << endl;
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
                    //cout << "followSuffixLink: " << description() << " " << stringFollowed() << endl;
                    auto state = m_state;
                    vector<SuffixTreeBuilder::Substring> substringsToFollowFromAncestorSuffixLink;
                    if (!isOnExplicitState())
                    {
                        auto substringFromExplicit = m_transition->substringFollowed;
                        substringFromExplicit.endIndex = substringFromExplicit.startIndex + m_posInTransition;
                        //cout << "Adding up to explicit: " << substringFromExplicit.startIndex << "," << substringFromExplicit.endIndex << endl;
                        substringsToFollowFromAncestorSuffixLink.push_back(substringFromExplicit);
                    }
                    while (!state->suffixLink)
                    {
                        //cout << "followSuffixLink loop - state: " << state << endl;
                        const Transition* transitionFromParent = findTransitionFromParent(state);
                        //cout << "state: " << state << " transitionFromParent: " << transitionFromParent << endl;

                        //cout << "Prepending transition from parent (state = " << state << "): " << transitionFromParent->substringFollowed.startIndex << "," << transitionFromParent->substringFollowed.endIndex << endl;
                        substringsToFollowFromAncestorSuffixLink.insert(substringsToFollowFromAncestorSuffixLink.begin(), transitionFromParent->substringFollowed);

                        state = state->parent;
                        //cout << "Ascended to parent: " << state << endl;
                    }
                    auto ancestorsSuffixLink = state->suffixLink;
                    //cout << "substringsToFollowFromAncestorSuffixLink.size(): " << substringsToFollowFromAncestorSuffixLink.size() << endl;
                    //cout << "stopped at " << state << endl;
                    if (state == m_root)
                    {
                        //cout << "Reached root" << endl;
                        assert(ancestorsSuffixLink);
                        if (!substringsToFollowFromAncestorSuffixLink.empty())
                        {
                            //cout << "Original first substring to follow: " << substringsToFollowFromAncestorSuffixLink.front().startIndex << "," << substringsToFollowFromAncestorSuffixLink.front().endIndex << endl;
                            substringsToFollowFromAncestorSuffixLink.front().startIndex++;
                            //cout << "Adjusted first substring to follow: " << substringsToFollowFromAncestorSuffixLink.front().startIndex << "," << substringsToFollowFromAncestorSuffixLink.front().endIndex << endl;
                        }
                        ancestorsSuffixLink = m_root;
                    }
                    //cout << "ancestorsSuffixLink: " << ancestorsSuffixLink << endl;
                    Cursor suffixLinkCursor = Cursor(ancestorsSuffixLink, *m_string, m_root);
                    //cout << "Working our way back up from ancestor suffix link" << endl;
                    for(const auto substring : substringsToFollowFromAncestorSuffixLink)
                    {
                        //cout << " substring: " << substring.startIndex << "," << substring.endIndex << endl;
                        suffixLinkCursor.followLetters(*m_string, substring.startIndex - 1, substring.length(m_string->length()));
                    }
                    *this = suffixLinkCursor;
                }
                bool canMoveUp()
                {
                    return (m_state != m_root || m_transition);
                }
                char moveUp()
                {
                    //cout << "canMoveUp: " << description() << endl;
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

                Transition* findTransitionFromParent(State* state = nullptr)
                {
                    if (!state)
                        state = m_state;
                    //cout << "findTransitionFromParent: m_state: " << state << " parent state: " << state->parent << endl;
                    Transition* transitionFromParent = nullptr;
                    for (Transition& transition : state->parent->transitions)
                    {
                        //cout << " nextState: " << transition.nextState << endl;
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
        };
        Cursor rootCursor() const
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
        void makeFinalStatesExplicit()
        {
            //cout << "makeFinalStatesExplicit" << endl;
            {
                Cursor finalState = rootCursor();
                finalState.followLetters(m_currentString);
                assert(finalState.isOnExplicitState());
                State* previousState = nullptr;
                while (finalState != rootCursor())
                {
                    //cout << "final state: " << finalState.stringFollowed() << endl;
                    if (!finalState.isOnExplicitState())
                    {
                        //cout << "Need to split state: " << finalState.stringFollowed() << endl;
                        auto state = finalState.m_state;
                        assert(state->suffixLink);
                        auto transition = finalState.m_transition;
                        auto posInTransition = finalState.posInTransition();
                        auto newExplicitState = createNewState(state);
                        auto originalNextState = transition->nextState;
                        //cout << "posInTransition: " << posInTransition << endl;
                        transition->nextState = newExplicitState;
                        //cout << "Original substring: " << transition->substringFollowed.startIndex << "," << transition->substringFollowed.endIndex << endl;
                        const bool transitionWasOpen = (transition->substringFollowed.endIndex == openTransitionEnd);
                        const auto originalTransitionSubstring = transition->substringFollowed;
                        //cout << "transitionWasOpen: " << transitionWasOpen << endl;
                        //if (!transitionWasOpen)
                            //transition->substringFollowed.endIndex -= posInTransition;
                        //else
                            transition->substringFollowed.endIndex = transition->substringFollowed.startIndex + posInTransition - 1;

                        //cout << "Adjusted to substring: " << transition->substringFollowed.startIndex << "," << transition->substringFollowed.endIndex << endl;

                        Transition newTransition(originalNextState, originalTransitionSubstring);
                        newTransition.substringFollowed.startIndex += posInTransition;
                        if (transitionWasOpen)
                            newTransition.substringFollowed.endIndex = openTransitionEnd;
                        newExplicitState->transitions.push_back(newTransition);
                        originalNextState->parent = newExplicitState;
                        //cout << "newly added transition: " << newTransition.substringFollowed.startIndex << "," << newTransition.substringFollowed.endIndex << endl;
                        assert(newTransition.substringFollowed.length(m_currentString.size()) > 0);

                        finalState = Cursor(newExplicitState, m_currentString, m_root);
                        //cout << "Bleep :" << finalState.isOnExplicitState() << endl;
                        //cout << "Created new final state: " << finalState.stringFollowed() << " " << newExplicitState << endl;
                        //dumpGraph();
                        Cursor suffixLink(state, m_currentString, m_root);
                        suffixLink.followLetters(m_currentString, transition->substringFollowed.startIndex - 1, posInTransition);
                        finalState = suffixLink;
                        //cout << "About to follow suffix link from " << finalState.description() << endl;
                        assert(finalState.isOnExplicitState());
                        if (previousState)
                        {
                            previousState->suffixLink = finalState.m_state;
                        }
                        previousState = finalState.m_state;
                        finalState.followSuffixLink();
                        //cout << "followed suffix link; now : " << finalState.description() << endl;
                    }
                    else
                    {
                        //cout << "About to follow suffix link from " << finalState.description() << endl;
                        previousState = finalState.m_state;
                        finalState.followSuffixLink();
                        //cout << "followed suffix link; now : " << finalState.description() << endl;
                    }
                }
            }
#if 0
            {
                cout << "Whizzing through suffix links" << endl;
                Cursor finalState = rootCursor();
                finalState.followLetters(m_currentString);
                int dbgIterNum = 0;
                while (finalState != rootCursor())
                {
                    cout << "Following ... "  << dbgIterNum++ << endl;
                    finalState.followSuffixLink();
                    cout << "... done" << endl;
                }
            }
            cout << "Correcting suffix links" << endl;
            // Correct suffix links.
            {
                //cout << "Correcting suffix links" << endl;
                //dumpGraph();
                Cursor finalState = rootCursor();
                finalState.followLetters(m_currentString);
                Cursor prevFinalState(finalState);
                //cout << "Shifting final state to suffix link" << endl;
                finalState.followSuffixLink();
                assert(finalState.stringFollowed() != m_currentString);
                int dbgIterNum = 0;
                do 
                {
                    //cout << "Loop: finalState: " << finalState.stringFollowed() << " prevFinalState: " << prevFinalState.stringFollowed() << endl;
                    cout << "finalState : " << finalState.description() << " iter: " << dbgIterNum++ << endl;
                    assert(finalState.isOnExplicitState());
                    assert(prevFinalState.isOnExplicitState());
                    assert(finalState != prevFinalState);

                    //cout << "setting suffixLink for state " << prevFinalState.m_state << " to " << finalState.m_state << endl;
                    prevFinalState.m_state->suffixLink = finalState.m_state;

                    prevFinalState.followSuffixLink();
                    finalState.followSuffixLink();
                }
                while (finalState != rootCursor());
            }
#endif

        };
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
            //assert((s->suffixLink == nullptr) == isTerminal);
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
                cout << indent + " " << "transition: " << &transition << " " << substringStartIndex << "," << substringEndIndex << (substringEndIndex == m_currentString.size() - 1 ? " (open) " : "") << " " << m_currentString.substr(substringStartIndex, substringEndIndex - substringStartIndex + 1) << " next state: " << findStateIndex(transition.nextState) << endl;
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

using Cursor = SuffixTreeBuilder::Cursor;

long bruteForce(const string& s)
{
    long result = 0;
    for (int i = 0; i < s.size(); i++)
    {
        const string suffix = s.substr(i);
        int commonPrefixLength = 0;
        for (int i = 0; i < suffix.length(); i++)
        {
            if (suffix[i] == s[i])
            {
                commonPrefixLength++;
            }
            else
                break;
        }
        //cout << "commonPrefixLength: " << commonPrefixLength << endl;
        result += commonPrefixLength;
    }
    return result;
}

void computeSumOfCommonPrefixLengthAux(const Cursor cursor, const string& s, const vector<int>& finalStateSuffixLengths, long& result, int depth)
{
    //cout << "computeSumOfCommonPrefixLengthAux: cursor: " << cursor.description() << " numLettersFollowed: " << numLettersFollowed << " lengthOfPrefixOfSFollowed: " << lengthOfPrefixOfSFollowed << " isOnExplicitState: " << cursor.isOnExplicitState() << " depth: " << depth <<  endl;
    struct State
    {
        Cursor cursor;
        long numLettersFollowed = -1;
        long lengthOfPrefixOfSFollowed = -1;
        vector<char> nextLetters;
        int indexIntoNextLetters = -1;
        bool haveRecursed = false;
    };

    State initialState;
    initialState.cursor = cursor;
    initialState.numLettersFollowed = 0;
    initialState.lengthOfPrefixOfSFollowed = 0;
    stack<State> states;
    states.push(initialState);
    while (!states.empty())
    {
        State& currentState = states.top();
        const auto haveFollowedPrefixOfS = (currentState.lengthOfPrefixOfSFollowed == currentState.numLettersFollowed);
        //cout << "Loop; #states: " << states.size() << " cursor: " << currentState.cursor.stringFollowed() << " haveFollowedPrefixOfS: " << haveFollowedPrefixOfS << " lengthOfPrefixOfSFollowed: " << currentState.lengthOfPrefixOfSFollowed << " numLettersFollowed: " << currentState.numLettersFollowed << endl;
        if (currentState.cursor.isOnExplicitState())
        {
            const auto isFinalState = (finalStateSuffixLengths[currentState.cursor.stateId()] != -1);
            if (currentState.indexIntoNextLetters == -1)
            {
                currentState.nextLetters = currentState.cursor.nextLetters();
                currentState.indexIntoNextLetters = 0;
                if (isFinalState)
                {
                    result += currentState.lengthOfPrefixOfSFollowed;
                    //cout << "added " << currentState.lengthOfPrefixOfSFollowed << " to result; now: " << result << endl;
                }
            }
            if (currentState.indexIntoNextLetters == currentState.nextLetters.size())
            {
                states.pop();
                continue;
            }
            const auto nextLetter = currentState.nextLetters[currentState.indexIntoNextLetters];
            const auto doesLetterContinueS = haveFollowedPrefixOfS && 
                                          (s[currentState.numLettersFollowed] == nextLetter);
            const auto newLengthOfPrefixOfSFollowed = (doesLetterContinueS ? currentState.lengthOfPrefixOfSFollowed + 1 : currentState.lengthOfPrefixOfSFollowed);
            auto cursorAfterLetter = currentState.cursor;
            cursorAfterLetter.followLetter(nextLetter);
            State newState;
            newState.cursor = cursorAfterLetter;
            newState.numLettersFollowed = currentState.numLettersFollowed + 1;
            newState.lengthOfPrefixOfSFollowed = newLengthOfPrefixOfSFollowed;
            states.push(newState);

            currentState.indexIntoNextLetters++;
            continue;
        }
        else
        {
            //cout << "Not explicit" << endl;
            if (currentState.haveRecursed)
            {
                states.pop();
                continue;
            }
            Cursor nextCursor(currentState.cursor);
            bool followedLetters = false;
            int newNumLettersFollowed = currentState.numLettersFollowed;
            int newLengthOfPrefixOfSFollowed = currentState.lengthOfPrefixOfSFollowed;
            if (haveFollowedPrefixOfS)
            {
                //cout << "Finding extra prefix letters in non-explicit" << endl;
                while (currentState.lengthOfPrefixOfSFollowed < s.length() && nextCursor.canFollowLetter(s[currentState.lengthOfPrefixOfSFollowed]) && !nextCursor.isOnExplicitState())
                {
                    //cout << "Found new!" << endl;
                    followedLetters = true;
                    nextCursor.followLetter(s[currentState.lengthOfPrefixOfSFollowed]);
                    newLengthOfPrefixOfSFollowed++;
                    newNumLettersFollowed++;
                }
            }
            if (!followedLetters)
            {
                //cout << "didn't follow letters" << endl;
                const auto remainderOfTransition = nextCursor.remainderOfCurrentTransition();
                nextCursor.followNextLetters(remainderOfTransition.length());
                newNumLettersFollowed += remainderOfTransition.length();
                assert(nextCursor.isOnExplicitState());
            }
            State newState;
            newState.cursor = nextCursor;
            newState.numLettersFollowed = newNumLettersFollowed;
            newState.lengthOfPrefixOfSFollowed = newLengthOfPrefixOfSFollowed;
            states.push(newState);
            currentState.haveRecursed = true;
            continue;
        }
    }
    return;
#if 0
    const auto haveFollowedPrefixOfS = (lengthOfPrefixOfSFollowed == numLettersFollowed);
    if (cursor.isOnExplicitState())
    {
        const auto isFinalState = (finalStateSuffixLengths[cursor.stateId()] != -1);
        //cout << "finalStateSuffixLengths[cursor.stateId()]: " << finalStateSuffixLengths[cursor.stateId()] << endl;
        if (isFinalState)
        {
            //cout << "reached final state: " << cursor.stringFollowed() << " prefix of S so far: " << lengthOfPrefixOfSFollowed << endl;
            result += lengthOfPrefixOfSFollowed;
        }
        const vector<char>& nextLetters = cursor.nextLetters();
        for (const auto nextLetter : nextLetters)
        {
            //cout << "Next letter: " << nextLetter << endl; 
            const auto doesLetterContinueS = haveFollowedPrefixOfS && 
                                          (s[numLettersFollowed] == nextLetter);
            const auto newLengthOfPrefixOfSFollowed = (doesLetterContinueS ? lengthOfPrefixOfSFollowed + 1 : lengthOfPrefixOfSFollowed);
            auto cursorAfterLetter = cursor;
            cursorAfterLetter.followLetter(nextLetter);
            computeSumOfCommonPrefixLengthAux(cursorAfterLetter, s, numLettersFollowed + 1, newLengthOfPrefixOfSFollowed, finalStateSuffixLengths, result, depth + 1);
        }
    }
    else
    {
        bool followedLetters = false;
        if (haveFollowedPrefixOfS)
        {
            while (lengthOfPrefixOfSFollowed < s.length() && cursor.canFollowLetter(s[lengthOfPrefixOfSFollowed]) && !cursor.isOnExplicitState())
            {
                followedLetters = true;
                cursor.followLetter(s[lengthOfPrefixOfSFollowed]);
                lengthOfPrefixOfSFollowed++;
                numLettersFollowed++;
            }
        }
        if (!followedLetters)
        {
            const auto remainderOfTransition = cursor.remainderOfCurrentTransition();
            cursor.followNextLetters(remainderOfTransition.length());
            numLettersFollowed += remainderOfTransition.length();
            assert(cursor.isOnExplicitState());
        }
        computeSumOfCommonPrefixLengthAux(cursor, s, numLettersFollowed, lengthOfPrefixOfSFollowed, finalStateSuffixLengths, result, depth + 1);
        
    }
#endif
}
long computeSumOfCommonPrefixLengths(const string& s)
{
    SuffixTreeBuilder suffixTree;
    suffixTree.appendString(s);
    //suffixTree.dumpGraph();
    //cout << "numStates original: " << suffixTree.numStates() << endl;
    suffixTree.makeFinalStatesExplicit();
    //suffixTree.dumpGraph();
    vector<int> finalStateSuffixLengths(suffixTree.numStates(), -1);
    //cout << "numStates: " << suffixTree.numStates() << endl;
    auto finalStateCursor = suffixTree.rootCursor();
    finalStateCursor.followLetters(s);
    int suffixLength = s.length();
    while (finalStateCursor != suffixTree.rootCursor())
    {
        //cout << "finalStateCursor: " << finalStateCursor.stringFollowed() << endl;
        //cout << "stateId: " << finalStateCursor.stateId() << endl;
        finalStateSuffixLengths[finalStateCursor.stateId()] = suffixLength;
        finalStateCursor.followSuffixLink();
    }
    for (auto blah : finalStateSuffixLengths)
    {
        //cout << "Blah: " << blah << endl;
    }
    //cout << "About to compute result" << endl;
    long result = 0;
    computeSumOfCommonPrefixLengthAux(suffixTree.rootCursor(), s, finalStateSuffixLengths, result, 0);
    return result;
}

//#define EXHAUSTIVE

int main() {
#ifndef EXHAUSTIVE
    long T;
    cin >> T;
    for (int t = 0; t < T; t++)
    {
        string s;
        cin >> s;
#ifndef SUBMISSION
        //const auto bruteForceResult = bruteForce(s);
        //cout << "bruteForceResult: " << bruteForceResult << endl;
#endif
        //cout << "s: " << s << endl;
        const auto optimisedResult = computeSumOfCommonPrefixLengths(s);
#ifndef SUBMISSION
        cout << "optimisedResult: " << optimisedResult << endl;
        //assert(bruteForceResult == optimisedResult);
#else
        cout << optimisedResult << endl;
#endif
    }
#else
    string s = "a";
    const int numLetters = 3;
    while (true)
    {
        cout << "s: " << s <<  " size: " << s.size() << endl;
        const auto bruteForceResult = bruteForce(s);
        cout << "bruteForceResult: " << bruteForceResult << endl;
        const auto optimisedResult = computeSumOfCommonPrefixLengths(s);
        cout << "optimisedResult: " << optimisedResult << endl;
        assert(bruteForceResult == optimisedResult);

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

