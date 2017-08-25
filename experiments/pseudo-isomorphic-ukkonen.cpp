#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <limits>
#include <memory>
#include <algorithm>
#include <cassert>

#define PSEUDO_ISOMORPHIC

using namespace std;

const int alphabetSize = 27; // Include the magic '{' for making final states explicit - assumes the input string has no '{''s, obviously!
struct StateData
{
    char letterPermutation[alphabetSize] = {};
    int wordLength = 0;
};

class LetterPermutation
{
    public:
        LetterPermutation() = default;
        void permuteUnpermutedLetter(const char originalLetter)
        {
            assert(!hasPermutedLetter(originalLetter));
            m_permutedLetter[originalLetter - 'a'] = 'a' + m_numLettersPermuted;
            //cout << " permutation: " << this << " char " << originalLetter << " permuted to " << permutedLetter(originalLetter) << endl;
            m_numLettersPermuted++;
        }
        void permuteUnpermutedLetter(const char originalLetter, char permutedLetter)
        {
            assert(!hasPermutedLetter(originalLetter));
            m_permutedLetter[originalLetter - 'a'] = permutedLetter;
        }
        bool hasPermutedLetter(const char originalLetter)
        {
            return m_permutedLetter[originalLetter - 'a'] != '\0';
        }
        char permutedLetter(const char originalLetter)
        {
            assert(originalLetter >= 'a' && originalLetter <= '{');
            if (!hasPermutedLetter(originalLetter))
            {
                cout << "letter " << originalLetter << " has no permutation!" << endl;
            }
            assert(hasPermutedLetter(originalLetter));
            const char result = m_permutedLetter[originalLetter - 'a'];
            //cout << " permutation: " << this << " permutedLetter: " << originalLetter << " = " << m_permutedLetter[originalLetter - 'a'] << endl;
            if (!(result >= 'a' && result <= '{'))
            {
                cout << " permutation: " << this << " invalid mapping from " << originalLetter << " to " << result << endl;
                assert(false);
            }
            return result;
        }
    private:
        char m_permutedLetter[alphabetSize] = {'\0'};
        int m_numLettersPermuted = 0;
};

string canonicaliseString(const string& s, LetterPermutation* letterPermutation = nullptr, bool assertNoUnknown = false)
{
    //cout << " canonicaliseString: " << s << " letterPermutation: " << letterPermutation << endl;
    LetterPermutation letterPermutationLocal;
    LetterPermutation* letterPermutationToUse = letterPermutation;
    if (!letterPermutationToUse)
        letterPermutationToUse = &letterPermutationLocal;

    string canonicalised;

    for (const auto letter : s)
    {
        if (!letterPermutationToUse->hasPermutedLetter(letter))
        {
            if (assertNoUnknown)
            {
                cout << "Error: letter " << letter << " has no permutation!" << endl;
                assert(false);
            }
            letterPermutationToUse->permuteUnpermutedLetter(letter); 
        }
        canonicalised += letterPermutationToUse->permutedLetter(letter);
    }
    return canonicalised;
}

void assertIsCanonical(const string& s)
{
    assert(s == canonicaliseString(s));
}

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
            LetterPermutation* letterPermutation = nullptr;
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
            //m_root->data.wordLength = 1;
            m_auxiliaryState = createNewState();
            cout << " m_auxiliaryState: " << m_auxiliaryState << endl;

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
            const auto canonizeResult = canonize(m_s, m_k, m_currentString.size(),(m_numSuffixLinksTraversed < m_normalisedSuffixPermutations.size() ?  &(m_normalisedSuffixPermutations[m_numSuffixLinksTraversed]) : &allLettersToA));
            m_s = canonizeResult.first;
            m_k = canonizeResult.second;
            dumpGraph();
            dumpNormalisedStrings();
        }
        void appendString(const string& stringToAppend)
        {
            int numAppended = 0;
            for (auto letter : stringToAppend)
            {
                cout << "Appending " << (numAppended + 1) << " of " << stringToAppend.size() << " : " << letter << endl;
                appendLetter(letter);
                numAppended++;
            }
        }
        void computeSuffixNormalisationPermutations(const string& s)
        {
            cout << "Blee: " << s.size() << endl;
            m_normalisedSuffixPermutations.resize(s.size());
            for (int i = 0; i < s.size(); i++)
            {
               canonicaliseString(s.substr(i), &(m_normalisedSuffixPermutations[i]));
            }
            for (int i = 0; i < alphabetSize; i++)
            {
                allLettersToA.permuteUnpermutedLetter('a' + i, 'a');
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
                LetterPermutation* letterPermutation()
                {
                    assert(m_transition);
                    assert(m_transition->letterPermutation);
                    return m_transition->letterPermutation;
                }
                StateData& stateData()
                {
                    assert(isOnExplicitState());
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
                    //cout << "follow letter: " << letter << endl;
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
                    //cout << "follow letters" << endl;
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
                            assert(transitionIterator->letterPermutation);
                            return transitionIterator->letterPermutation->permutedLetter((*(cursor->m_string))[transitionIterator->substringFollowed.startIndex - 1]);
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
                string canonicalisedStringFollowed() const
                {
                    Cursor copy(*this);
                    string stringFollowed;
                    //cout << "canonicalisedStringFollowed!" << endl;
                    while (!(!copy.m_state->parent && copy.isOnExplicitState()))
                    {
                        //cout << " copy state: " << copy.m_state << " root: " << m_root << endl;
                        if (copy.isOnExplicitState())
                        {
                            auto transitionFromParent = findTransitionFromParent(copy.m_state);
                            stringFollowed = canonicaliseString(m_string->substr(transitionFromParent->substringFollowed.startIndex - 1, transitionFromParent->substringFollowed.length(m_string->size())),  transitionFromParent->letterPermutation, true) + stringFollowed;
                            copy.m_state = copy.m_state->parent;
                        }
                        else
                        {
                            assert(false); // Don't use m_transition, m_posInTransition!!
                            assert(m_transition->substringFollowed.startIndex >= 0);
                            stringFollowed = canonicaliseString(m_string->substr(m_transition->substringFollowed.startIndex - 1 + m_posInTransition, m_transition->substringFollowed.length(m_string->size() - m_posInTransition)),  m_transition->letterPermutation, true) + stringFollowed;
                            copy.movedToExplicitState();
                        }
                        //cout << " stringFollowed: " << stringFollowed << endl;
                    }
                    assertIsCanonical(stringFollowed);
                    return stringFollowed;
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
                    return const_cast<Transition*>(const_cast<const Cursor*>(this)->findTransitionFromParent(state));
                }
                const Transition* findTransitionFromParent(State* state = nullptr) const
                {
                    if (!state)
                        state = m_state;
                    const Transition* transitionFromParent = nullptr;
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
        void dumpGraph()
        {
            dumpGraphAux(m_root, "");
        }
        set<string> dumpNormalisedStrings()
        {
            cout << "dumpNormalisedStrings: " << m_currentString << endl;
            set<string> normalisedStrings;
            dumpNormalisedStringsAux(rootCursor(), "", normalisedStrings);
            return normalisedStrings;
        }

    private:
        static const int openTransitionEnd = numeric_limits<int>::max();

        string m_currentString;

        vector<State> m_states;
        State *m_root = nullptr;
        State *m_auxiliaryState = nullptr;

        // "Persistent" versions of s & k from Algorithm 2 in Ukkonen's paper!
        State *m_s; 
        int m_k;
        int m_numSuffixLinksTraversed = 0;

        vector<LetterPermutation> m_normalisedSuffixPermutations;
        LetterPermutation allLettersToA;

        string normalisedStringToState(State* s)
        {
            Cursor blah(s, m_currentString, m_root);
            return blah.canonicalisedStringFollowed();
        }

        std::pair<State*, int> update(State* s, int k, int i)
        {
            cout << "update: " << " k: " << k << " i: " << i << " suffix: " << m_currentString.substr(i - 1) << " m_numSuffixLinksTraversed: " << m_numSuffixLinksTraversed << " s: " << s << endl;
            State* oldr = m_root;
#ifdef PSEUDO_ISOMORPHIC
            //const int letterIndex = canonicaliseString(m_currentString.substr(m_numSuffixLinksTraversed, i - 1 - m_numSuffixLinksTraversed)).back() + 1;
            //cout << "suffix: " << m_currentString.substr(m_numSuffixLinksTraversed) << " canonicalised: " << canonicaliseString(m_currentString.substr(m_numSuffixLinksTraversed)) << endl;
            //const int letterIndex = canonicaliseString(m_currentString.substr(m_numSuffixLinksTraversed)).back() - 'a' + 1;
            //const int letterIndex = m_currentString.substr(m_numSuffixLinksTraversed).empty() ? 1 : canonicaliseString(m_currentString.substr(m_numSuffixLinksTraversed)).back() - 'a' + 1;
            Cursor blah(s, m_currentString, m_root);
            auto flopple = blah.stringFollowed() + '|' + m_currentString.substr(k - 1, i - k + 1); 
            cout << " flopple: " << flopple << endl;
            flopple.erase(remove(flopple.begin(), flopple.end(), '|'), flopple.end());
            //cout << " flopple canonical: " << canonicaliseString(flopple) << endl;
            //const int letterIndex = canonicaliseString(flopple).back()  - 'a' + 1;
            //const int letterIndex = canonicaliseString(canonicaliseString(canonicaliseString(blah.stringFollowed()) + m_currentString.substr(k - 1, i - k)) + m_currentString[i - 1]).back() - 'a' + 1;
            //const int letterIndex = canonicaliseString(blah.stringFollowed() + m_currentString.substr(k - 1, i - k + 1)).back() - 'a' + 1;
            assert(m_numSuffixLinksTraversed < m_normalisedSuffixPermutations.size());
            const int letterIndex = m_normalisedSuffixPermutations[m_numSuffixLinksTraversed].permutedLetter(m_currentString[i - 1]) - 'a' + 1;
            cout << "letterIndex: " << letterIndex << endl;
            //const int letterIndex = m_currentString.substr(m_numSuffixLinksTraversed).empty() ? 1 : canonicaliseString(m_currentString.substr(m_numSuffixLinksTraversed)).back() - 'a' + 1;
            //const int letterIndex2 = m_currentString.substr(m_numSuffixLinksTraversed).empty() ? 1 : canonicaliseString(blah.stringFollowed() + m_currentString[i - 1]).back() - 'a' + 1;
            //assert(letterIndex == letterIndex2);
#else
            const int letterIndex = t(i);
#endif
            assert(m_numSuffixLinksTraversed < m_normalisedSuffixPermutations.size());
            const auto testAndSplitResult = testAndSplit(s, k, i - 1, letterIndex, &(m_normalisedSuffixPermutations[m_numSuffixLinksTraversed]));
            //int suffixBeginPos = i - (s->data.wordLength + k);
            //auto blah = [i](State* s, int k)
            //{
                //return i - (s->data.wordLength + (k - 1));
            //};
            //cout << "s: " << s << endl;
            //int suffixBeginPos = blah(s, k);
            //cout << "s->data.wordLength: " << s->data.wordLength << " k: " << k << " suffixBeginPos: " << suffixBeginPos << endl;
            auto isEndPoint = testAndSplitResult.first;
            auto r = testAndSplitResult.second;
            assert(r);
            while (!isEndPoint)
            {
                auto rPrime = createNewState(r);
                rPrime->data.wordLength = -1;
                {
                    Cursor rblah(r, m_currentString, m_root);
                    //cout << " r: " << rblah.stringFollowed() << " canonicalised: " << canonicaliseString(rblah.stringFollowed()) << endl;
                }
                r->transitions.push_back(Transition(rPrime, Substring(i, openTransitionEnd), m_currentString));
                assert(m_numSuffixLinksTraversed < m_normalisedSuffixPermutations.size());
                r->transitions.back().letterPermutation = &(m_normalisedSuffixPermutations[m_numSuffixLinksTraversed]);
                verifyStateTransitions(r);
                if (oldr != m_root)
                {
                    oldr->suffixLink = r;
                    cout << " made suffix link from " << oldr << " to " << r << endl;
                    assert(normalisedStringToState(oldr).size() == normalisedStringToState(r).size() + 1);
                }
                oldr = r;
                cout << " oldr now: " << r << endl;

#if 0
                if (r != m_auxiliaryState)
                {
                    Cursor blah(r, m_currentString, m_root);
                    const char nextLetter = m_currentString[m_numSuffixLinksTraversed];
                    //cout << "Blee: " << m_currentString.substr(m_numSuffixLinksTraversed) << endl;
                    //cout << " s is root: " << (r == m_root) << endl;
                    auto nextLetterIterator = blah.getNextLetterIterator();
                    //cout << " letters from here: " << endl;
                    //while (nextLetterIterator.hasNext())
                    //{
                    //}
                    //cout << "Followed a suffix link: " << blah.stringFollowed() << endl;
                    const auto remainder = m_currentString.substr(m_numSuffixLinksTraversed + r->data.wordLength, k);
                    //cout << " remainder: " <<  remainder << endl;
                    //cout << " r->data.wordLength: " << r->data.wordLength << endl;
                    //cout << "m_numSuffixLinksTraversed: " << m_numSuffixLinksTraversed << endl;
                    blah.followLetters(remainder);
                    //cout << " followed remainder: " << blah.stringFollowed() << endl;

                    if (!m_currentString.substr(m_numSuffixLinksTraversed).empty())
                    {
                        cout << "blee: " << m_currentString.substr(m_numSuffixLinksTraversed) << " bloo: " << blah.stringFollowed() << endl;
                        //assert(m_currentString.substr(m_numSuffixLinksTraversed).find(blah.stringFollowed()) == 0);
                        //assert(m_currentString.substr(m_numSuffixLinksTraversed) == blah.stringFollowed());
                    }
                }
#endif
                cout << " s: " << s << " s->suffixLink: " << s->suffixLink << endl;
                if (!s->suffixLink)
                    addSuffixLink(s, m_numSuffixLinksTraversed);
                assert(s->suffixLink);
                //suffixBeginPos++;
                m_numSuffixLinksTraversed++;
                const bool emptySuffix = m_currentString.substr(m_numSuffixLinksTraversed).empty();
                //const auto oldNormalised = 
                //assert(m_numSuffixLinksTraversed < m_normalisedSuffixPermutations.size());
                const auto normalisedStringToS = normalisedStringToState(s);
                const auto normalisedStringToSuffixLink = normalisedStringToState(s->suffixLink);
                cout << " normalisedStringToS: " << normalisedStringToS << endl;
                cout << " normalisedStringToSuffixLink: " << normalisedStringToSuffixLink << endl;
                const auto canonizeResult = canonize(s->suffixLink, k, i - 1, emptySuffix ? &allLettersToA : &(m_normalisedSuffixPermutations[m_numSuffixLinksTraversed]));
                if (s->suffixLink == m_auxiliaryState)
                {
                    cout << "s suffixlink = aux (" << m_auxiliaryState << ") canonize: " << canonizeResult.first << " k: " << k << " i: " << i << endl;
                }

                const auto oldK = k;
                cout << "s: " << s << " root: " << m_root << " aux: " << m_auxiliaryState << " suffix link: " << s->suffixLink << endl;
                cout << " old k: " << oldK << endl;
                if (!(normalisedStringToS.size() == normalisedStringToSuffixLink.size() + 1 || (s == m_root)))
                {
                    cout << "Uh-oh!" << endl;
                    dumpGraph();
                }
                assert(normalisedStringToS.size() == normalisedStringToSuffixLink.size() + 1 || (s == m_root));
                s = canonizeResult.first;
                k = canonizeResult.second;
                assert(emptySuffix || canonicaliseString(m_currentString.substr(m_numSuffixLinksTraversed)).find(normalisedStringToState(s)) == 0);
                cout << " new k: " << k << endl;
                //assert(normalisedStringToS.size() + oldK == normalisedStringToState(s).size() + k + 0 || (s == m_root));
                cout << " traversed suffix link: s now: " << s << endl;
                //cout << " s: " << s << endl;
                //cout << " s->data.wordLength: " << s->data.wordLength << " k: " << k << " suffixBeginPos: " << suffixBeginPos << " i: " << i << " isroot: " << (s == m_root) << " is aux: " << (s == m_auxiliaryState) << endl;
                //cout << " blah: " << blah(s, k) << endl;
                //assert(k != 0);
                //assert(s->data.wordLength + k == suffixBeginPos || s == m_auxiliaryState);
                //assert(blah(s, k) == suffixBeginPos);
                //cout << " blee: " << (blah(s, k) - suffixBeginPos) << endl;


#ifdef PSEUDO_ISOMORPHIC
                cout << "suffix: " << m_currentString.substr(m_numSuffixLinksTraversed) << endl;
                if (emptySuffix)
                {
                    cout << "Empty suffix!" << endl;
                    assert(s == m_auxiliaryState);
                    cout << " m_currentString[i - 1]: " << m_currentString[i - 1] << endl;
                }
                else
                {
                    cout << " not empty suffix!" << endl;
                }
                //assert(!m_currentString.substr(m_numSuffixLinksTraversed).empty());
                //const int letterIndex = m_currentString.substr(m_numSuffixLinksTraversed).empty() ? 1 : canonicaliseString(m_currentString.substr(m_numSuffixLinksTraversed)).back() - 'a' + 1;
                //Cursor blah(s, m_currentString, m_root);
                //const int letterIndex = m_currentString.substr(m_numSuffixLinksTraversed).empty() ? 1 :canonicaliseString(blah.stringFollowed() + m_currentString.substr(k - 1, i - k + 0) + m_currentString[i - 1]).back() - 'a' + 1;
                //const int letterIndex = m_currentString.substr(m_numSuffixLinksTraversed).empty() ? 1 : canonicaliseString(canonicaliseString(canonicaliseString(blah.stringFollowed()) + m_currentString.substr(k - 1, i - k)) + m_currentString[i - 1]).back() - 'a' + 1;
                //const int letterIndex = m_currentString.substr(m_numSuffixLinksTraversed).empty() ? 1 : canonicaliseString(canonicaliseString(blah.stringFollowed()) + m_currentString.substr(k - 1, i - k + 1)).back() - 'a' + 1;
                //const int letterIndex = m_currentString.substr(m_numSuffixLinksTraversed).empty() ? 1 :canonicaliseString(blah.stringFollowed() + m_currentString.substr(k - 1, i - k + 1)).back() - 'a' + 1;
                //const int letterIndex = m_currentString.substr(m_numSuffixLinksTraversed).empty() ? 1 : canonicaliseString(m_currentString.substr(m_numSuffixLinksTraversed)).back() - 'a' + 1;
                //const int letterIndex2 = m_currentString.substr(m_numSuffixLinksTraversed).empty() ? 1 : canonicaliseString(blah.stringFollowed() + m_currentString[i - 1]).back() - 'a' + 1;
                //assert(letterIndex == letterIndex2);
                cout << " m_numSuffixLinksTraversed: " << m_numSuffixLinksTraversed << " m_normalisedSuffixPermutations.size(): " << m_normalisedSuffixPermutations.size() << endl;
                assert(m_numSuffixLinksTraversed < m_normalisedSuffixPermutations.size() || emptySuffix);
                cout << " still in update: " << " k: " << k << " i: " << i << " suffix: " << m_currentString.substr(i - 1) << " m_numSuffixLinksTraversed: " << m_numSuffixLinksTraversed << " s: " << s << endl;
                const int letterIndex = emptySuffix ? 1 : m_normalisedSuffixPermutations[m_numSuffixLinksTraversed].permutedLetter(m_currentString[i - 1]) - 'a' + 1;
                cout << "letterIndex: " << letterIndex << " m_currentString[i - 1]: " << m_currentString[i - 1] << endl;
#else
                const int letterIndex = t(i);
#endif
                const auto testAndSplitResult = testAndSplit(s, k, i - 1, letterIndex, emptySuffix ? &allLettersToA : &(m_normalisedSuffixPermutations[m_numSuffixLinksTraversed]));
                isEndPoint = testAndSplitResult.first;
                r = testAndSplitResult.second;
                assert(r);
                //dumpGraph();
                cout << "isEndPoint? " << isEndPoint << endl;
                if (isEndPoint)
                {
                    assert(s == r);
                }
                cout << " r: " << r << endl;
            }

            if (oldr != m_root)
            {
                assert(s == r);
                if(oldr->data.wordLength != s->data.wordLength + 1)
                {
                    cout << "Whoops - suffix links are wrong!" << endl;
                    cout << "oldr: " << oldr << " s: " << s << endl;
                    dumpGraph();
                }
                else
                {
                    oldr->suffixLink = s;

                }
                //assert(oldr->suffixLink);
                //cout << " made suffix link from " << oldr << " to " << oldr->suffixLink << endl;
                //cout << "normalisedStringToState(oldr): " << normalisedStringToState(oldr) << " normalisedStringToState(s): " << normalisedStringToState(oldr->suffixLink) << endl;

                //if (normalisedStringToState(oldr).size() != normalisedStringToState(oldr->suffixLink).size() + 1)
                //{
                    //cout << "Uh - oh! "<< endl;
                    //dumpGraph();
                //}
                //assert(oldr->data.wordLength == oldr->suffixLink->data.wordLength + 1);
                //assert(normalisedStringToState(oldr).size() == normalisedStringToState(oldr->suffixLink).size() + 1);
            }
            return {s, k};


        }

        void addSuffixLink(State* s, int numSuffixLinksTraversed)
        {
            assert(s && !s->suffixLink);
            auto sParent = s->parent;
            bool foundTransition = false;
            for (const auto& transition : sParent->transitions)
            {
                if (transition.nextState == s)
                {
                    //const auto canonizeResult = canonize(s->suffixLink, k, i - 1, m_numSuffixLinksTraversed >= m_normalisedSuffixPermutations.size() ? &allLettersToA : &(m_normalisedSuffixPermutations[m_numSuffixLinksTraversed]));
                    LetterPermutation compoundPermutation;
                    LetterPermutation* suffixPermutation = (numSuffixLinksTraversed < m_normalisedSuffixPermutations.size() ? &(m_normalisedSuffixPermutations[numSuffixLinksTraversed]) : &allLettersToA);
                    //for (int i = 0; i < alphabetSize; i++)
                    //{
                    //compoundPermutation.permuteUnpermutedLetter('a' + i, suffixPermutation->permutedLetter(transition->
                    //}
                    const auto firstLetterOnTransition = m_currentString[transition.substringFollowed.startIndex - 1];
                    cout << " firstLetterOnTransition: " << firstLetterOnTransition << endl;
                    const auto firstLetterOnTransitionPermuted = transition.letterPermutation->permutedLetter(firstLetterOnTransition);
                    cout << " firstLetterOnTransitionPermuted: " << firstLetterOnTransitionPermuted << endl;
                    //const auto firstLetterOnTransitionCompoundPermuted = suffixPermutation->permutedLetter(firstLetterOnTransitionPermuted);
                    //cout << " firstLetterOnTransitionCompoundPermuted: " << firstLetterOnTransitionCompoundPermuted << endl;
                    //compoundPermutation.permuteUnpermutedLetter(firstLetterOnTransition, firstLetterOnTransitionCompoundPermuted);
                    cout << " transition to oldr - startIndex: " << transition.substringFollowed.startIndex << " endIndex: " << transition.substringFollowed.endIndex << endl;
                    cout << " m_numSuffixLinksTraversed: " << m_numSuffixLinksTraversed << endl;
                    auto suffixLink = sParent->suffixLink == m_auxiliaryState ? m_root : sParent->suffixLink;
                    //const auto testAndSplitResult = testAndSplit(suffixLink, transition.substringFollowed.startIndex, transition.substringFollowed.endIndex, alphabetSize, &compoundPermutation);
                    // This is almost certainly wrong - surely, must need to use the compoundPermutation???
                    const auto testAndSplitResult = testAndSplit(suffixLink, transition.substringFollowed.startIndex, transition.substringFollowed.endIndex - 1, alphabetSize, transition.letterPermutation);

                    // Need to find the suffix link for testAndSplitResult.second, too :(
                    s->suffixLink = testAndSplitResult.second;
                    cout << " repaired(?) suffix links!" << " added new state: " << testAndSplitResult.second << " - " << normalisedStringToState(testAndSplitResult.second) << endl;
                    dumpGraph();
                    //assert(testAndSplitResult.second->data.wordLength <= 1);
                    //testAndSplitResult.second->suffixLink = m_root; // TODO - this is wrong!
                    foundTransition = true;
                    break;
                }
            }
            assert(foundTransition);
        }
        pair<bool, State*> testAndSplit(State* s, int k, int p, int letterIndex, LetterPermutation* letterPermutation)
        {
            assert(s);
            cout << "testAndSplit: adding letter: " << static_cast<char>(letterIndex - 1 + 'a') << " index: " << letterIndex << endl;
            Cursor blah(s, m_currentString, m_root);
            cout << "state canonicalisedStringFollowed: " << blah.canonicalisedStringFollowed() << endl;
            cout << " s: " << s << " root: " << m_root << endl;
            cout << "k: " << k << " p: " << p << endl;
            if (k <= p)
            {
                cout << " not character after state" << endl;
                //const char letterToFollowFromState = canonicaliseString(m_currentString.substr(m_numSuffixLinksTraversed, s->data.wordLength) + m_currentString[k - 1]).back();
                //const char letterToFollowFromState = canonicaliseString(canonicaliseString(m_currentString.substr(m_numSuffixLinksTraversed, s->data.wordLength)) + m_currentString[k - 1]).back();
                const char letterToFollowFromState = letterPermutation->permutedLetter(m_currentString[k - 1]);
                //cout << "m_numSuffixLinksTraversed: " << m_numSuffixLinksTraversed << " s wordlength: " << s->data.wordLength << " string: " << m_currentString.substr(m_numSuffixLinksTraversed, s->data.wordLength) << " canonicalised: " << canonicaliseString(m_currentString.substr(m_numSuffixLinksTraversed, s->data.wordLength)) <<  " current suffix canonicalised: " << canonicaliseString(m_currentString.substr(m_numSuffixLinksTraversed)) << endl;
                //const char letterToFollowFromState = canonicaliseString(m_currentString.substr(m_numSuffixLinksTraversed))[s->data.wordLength];
                cout << " letterToFollowFromState: " << letterToFollowFromState << endl;
                Cursor blah(s, m_currentString, m_root);
                //cout << " to s: " << blah.stringFollowed()  << endl;
                //cout << " canonicalised to s: " << canonicaliseString(blah.stringFollowed()) << endl;
#ifdef PSEUDO_ISOMORPHIC
                const auto tkTransitionIter = findTransitionIter(s, letterToFollowFromState - 'a' + 1);
#else
                const auto tkTransitionIter = findTransitionIter(s, t(k));
#endif
                auto sPrime = tkTransitionIter->nextState;
                auto kPrime = tkTransitionIter->substringFollowed.startIndex;
                auto pPrime = tkTransitionIter->substringFollowed.endIndex;
                cout << "kPrime: " << kPrime << " pPrime: " << pPrime << endl;
                assert(pPrime < 0 || (p - k <= pPrime - kPrime));

                //cout << " kPrime: " << kPrime << " pPrime: " << pPrime << endl;
                //Cursor blahPrime(sPrime, m_currentString, m_root);
                //cout << " blahPrime: " << blahPrime.stringFollowed() << endl;
                //const string bloo = 
                //cout << " blooPrime: " << m_currentString.substr(m_numSuffixLinksTraversed, s->data.wordLength) +  '|' + m_currentString.substr(kPrime - 1, pPrime - kPrime + 1) << endl;
                //cout << " blooPrime2: " << canonicaliseString(m_currentString.substr(m_numSuffixLinksTraversed, s->data.wordLength) +  m_currentString.substr(kPrime - 1, pPrime - kPrime + 1)) << endl;
                //cout << "flarp: " << (canonicaliseString(m_currentString.substr(m_numSuffixLinksTraversed, s->data.wordLength) + m_currentString.substr(kPrime - 1, pPrime - kPrime + 1)).back() - 'a' + 1) << endl;
                //cout << "p - k: " << (p - k) << " m_currentString.substr(kPrime - 1): " <<  m_currentString.substr(kPrime - 1) << " length: " <<  m_currentString.substr(kPrime - 1).length() << endl;
                //cout << " flarpPrime: " << (canonicaliseString(m_currentString.substr(m_numSuffixLinksTraversed, s->data.wordLength) + m_currentString.substr(kPrime - 1, p - k + 2))) << endl;
                //cout << "gleep: " << canonicaliseString(canonicaliseString(blah.stringFollowed()) + m_currentString.substr(kPrime - 1, p - k + 2)) << endl;
                //cout << "gloop: " << canonicaliseString(blah.stringFollowed() + m_currentString.substr(kPrime - 1, p - k + 2)) << endl;
                
#ifdef PSEUDO_ISOMORPHIC
                //if (letterIndex ==  canonicaliseString(canonicaliseString(blah.stringFollowed()) + m_currentString.substr(kPrime - 1, p - k + 2)).back() - 'a' + 1) // TODO - this probably isn't right.
                //if (letterIndex == letterPermutation->permutedLetter(m_currentString[kPrime + p - k]) - 'a' + 1) // TODO - this probably isn't right.
                assert(tkTransitionIter->letterPermutation);
                if (letterIndex == tkTransitionIter->letterPermutation->permutedLetter(m_currentString[kPrime + p - k]) - 'a' + 1) // TODO - this probably isn't right.
                {
                    cout << " is end point!" << endl;
                    return {true, s};
                }
#else
                if (letterIndex == t(kPrime + p - k + 1))
                    return {true, s};
#endif
                else
                {
                    cout << "Splitting" << endl;
                    LetterPermutation* letterPermutationOnTransition = tkTransitionIter->letterPermutation;
                    s->transitions.erase(tkTransitionIter);
                    auto r = createNewState(s);
                    cout << " created new split state: " << r << endl;
                    r->data.wordLength = s->data.wordLength + p - k + 1;
                    s->transitions.push_back(Transition(r, Substring(kPrime, kPrime + p - k), m_currentString));
                    s->transitions.back().letterPermutation = letterPermutationOnTransition;
                    verifyStateTransitions(s);
                    r->transitions.push_back(Transition(sPrime, Substring(kPrime + p - k + 1, pPrime), m_currentString));
                    r->transitions.back().letterPermutation = letterPermutationOnTransition;
                    verifyStateTransitions(r);
                    sPrime->parent = r;
                    Cursor blah(r, m_currentString, m_root);
                    Cursor blahPrime(sPrime, m_currentString, m_root);
                    //cout << " New middle state: " << blah.stringFollowed() << " canonicalised: " << canonicaliseString(blah.stringFollowed()) << endl;
                    //cout << " sPrime : " << blahPrime.stringFollowed() << " canonicalised: " << canonicaliseString(blahPrime.stringFollowed()) << endl;
                    cout << " is not end point!" << endl;
                    return {false, r};
                }
            }
            else
            {
                cout << " character after state" << endl;
                //const char letterToFollowFromState = canonicaliseString(m_currentString.substr(m_numSuffixLinksTraversed, s->data.wordLength) + m_currentString[k - 1]).back();
                //cout << " letterToFollowFromState: " << letterToFollowFromState << endl;
#ifdef PSEUDO_ISOMORPHIC
                const auto tTransitionIter = findTransitionIter(s, letterIndex, false);
#else
                const auto tTransitionIter = findTransitionIter(s, letterIndex, false);
#endif
                if (tTransitionIter == s->transitions.end())
                {
                    cout << " is not end point!" << endl;
                    return {false, s};
                }
                else
                {
                    cout << " is end point!" << endl;
                    return {true, s};
                }
            }
        }
        std::pair<State*, int> canonize(State* s, int k, int p, LetterPermutation* letterPermutation)
        {
            assert(s);
            Cursor blah(s, m_currentString, m_root);
            const auto originalS = s;
            //const auto canonicalOriginal = canonicaliseString((s == m_auxiliaryState ? "" :  blah.stringFollowed()) + m_currentString.substr(k - 1, p - k + 1)); 
            //const auto canonicalOriginal = canonicaliseString((s == m_auxiliaryState ? "" :  blah.stringFollowed()) + m_currentString.substr(k - 1, p - k + 1)); 
            const auto canonicalOriginal = normalisedStringToState(s) + canonicaliseString(m_currentString.substr(k - 1, p - k + 1), letterPermutation);
            //cout << "canonize - canonicalOriginal: " << canonicalOriginal << " k: " << k << " p: " << p << endl;
            cout << " canonize - normalisedStringToState(originalS): " << normalisedStringToState(originalS) << endl;
            if (p < k)
                return {s, k};
            else
            {
                Cursor blah(s, m_currentString, m_root);
                if (s != m_auxiliaryState)
                {
                    //cout << " canonize: s: " << blah.stringFollowed() << " canonical s: " << canonicaliseString(blah.stringFollowed()) << " k: " << k << " p: " << p << endl;
                }
                //const char letterToFollowFromState =  (s == m_auxiliaryState) ? 'a' : canonicaliseString(m_currentString.substr(m_numSuffixLinksTraversed))[s->data.wordLength];
#ifdef PSEUDO_ISOMORPHIC
                const char letterToFollowFromState =  (s == m_auxiliaryState) ? 'a' : letterPermutation->permutedLetter(m_currentString[k - 1]);
                //cout << " canonize letterToFollowFromState: " << letterToFollowFromState << " suffix: " << m_currentString.substr(m_numSuffixLinksTraversed) << " suffix canonical: " << canonicaliseString(m_currentString.substr(m_numSuffixLinksTraversed)) << " wordLength: " << s->data.wordLength  << endl;
                assert(letterToFollowFromState >= 'a' && letterToFollowFromState <= 'z');
                auto tkTransitionIter = findTransitionIter(s, letterToFollowFromState - 'a' + 1);
#else
                auto tkTransitionIter = findTransitionIter(s, t(k));
#endif
                auto sPrime = tkTransitionIter->nextState;
                auto kPrime = tkTransitionIter->substringFollowed.startIndex;
                auto pPrime = tkTransitionIter->substringFollowed.endIndex;
                bool sWasAux = (originalS == m_auxiliaryState);
                while (pPrime - kPrime <= p - k)
                {
                    k = k + pPrime - kPrime + 1;
                    s = sPrime;
                    //Cursor blah(s, m_currentString, m_root);
                    //cout << " canonize: s: " << blah.stringFollowed() << " canonical s: " << canonicaliseString(blah.stringFollowed()) << " k: " << k << " p: " << p << endl;
                    if (k <= p)
                    {
#ifdef PSEUDO_ISOMORPHIC
                        //const char letterToFollowFromState = canonicaliseString(m_currentString.substr(m_numSuffixLinksTraversed) + m_currentString[k - 1])[s->data.wordLength];
                        assert(s != m_auxiliaryState);
                        //const char letterToFollowFromState = canonicaliseString(m_currentString.substr(m_numSuffixLinksTraversed))[s->data.wordLength];
                        //const char letterToFollowFromState =  (s == m_auxiliaryState) ? 'a' : canonicaliseString(blah.stringFollowed() + m_currentString[k - 1]).back();
                        const char letterToFollowFromState =  (s == m_auxiliaryState) ? 'a' : letterPermutation->permutedLetter(m_currentString[k - 1]);
                        //cout << " canonize letterToFollowFromState: " << letterToFollowFromState << " suffix: " << m_currentString.substr(m_numSuffixLinksTraversed) << " suffix canonical: " << canonicaliseString(m_currentString.substr(m_numSuffixLinksTraversed)) << " wordLength: " << s->data.wordLength  << endl;
                        assert(letterToFollowFromState >= 'a' && letterToFollowFromState <= 'z');
                        auto tkTransitionIter = findTransitionIter(s, letterToFollowFromState - 'a' + 1);
#else
                        tkTransitionIter = findTransitionIter(s, t(k));
#endif
                        sPrime = tkTransitionIter->nextState;
                        kPrime = tkTransitionIter->substringFollowed.startIndex;
                        pPrime = tkTransitionIter->substringFollowed.endIndex;
                    }
                    if (!sWasAux)
                    {
                        cout << " canonize - normalisedStringToState(s): " << normalisedStringToState(s) << endl;
                        const auto canonizedNew = normalisedStringToState(s) + canonicaliseString(m_currentString.substr(k - 1, p - k + 1), letterPermutation);
                        cout << " canonizedNew: " << canonizedNew  << " canonicalOriginal: " << canonicalOriginal << " s changed: " << (s != originalS) << endl;
                        assert(canonizedNew == canonicalOriginal);
                    }
                    sWasAux = false;
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
        void verifyStateTransitions(State* state)
        {
            cout << "verifyStateTransitions: " << state << endl;
            Cursor blah(state, m_currentString, m_root);
            set<char> canonicalNextChars;
            for (auto transitionIter = state->transitions.begin(); transitionIter != state->transitions.end(); transitionIter++)
            {
                if (transitionIter->substringFollowed.startIndex >= 0)
                {
                    assert(transitionIter->letterPermutation);
                    const char canonicalNextChar = transitionIter->letterPermutation->permutedLetter(m_currentString[transitionIter->substringFollowed.startIndex - 1]);
                    cout << " canonicalNextChar: " << canonicalNextChar << " string followed: " << blah.stringFollowed() << " actual char: " << m_currentString[transitionIter->substringFollowed.startIndex - 1] << " (" << transitionIter->substringFollowed.startIndex << ")" << endl;
                    assert(canonicalNextChars.find(canonicalNextChar) == canonicalNextChars.end());
                    canonicalNextChars.insert(canonicalNextChar);
                }
            }
            if (state->suffixLink)
            {
                assert(state == m_root ||  normalisedStringToState(state->suffixLink) == canonicaliseString(normalisedStringToState(state).substr(1)));
            }

        }
        decltype(State::transitions.begin()) findTransitionIter(State* state, int letterIndex, bool assertFound = true)
        {
            cout << " findTransitionIter letterIndex: " << letterIndex << endl;
            for (auto transitionIter = state->transitions.begin(); transitionIter != state->transitions.end(); transitionIter++)
            {
#ifdef PSEUDO_ISOMORPHIC
                if (transitionIter->substringFollowed.startIndex >= 0)
                {
                    assert(transitionIter->substringFollowed.startIndex != 0);
                    const char transitionLetter = transitionIter->letterPermutation->permutedLetter(m_currentString[transitionIter->substringFollowed.startIndex - 1]);
                    cout << " found: " << transitionLetter << " index: " << (transitionLetter - 'a' + 1) << endl;
                    if (transitionLetter - 'a' + 1 == letterIndex)
                    {
                        return transitionIter;
                    }
                }
#else
                if (transitionIter->substringFollowed.startIndex >= 0 && t(transitionIter->substringFollowed.startIndex) == letterIndex)
                    return transitionIter;
#endif

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
        void dumpGraphAux(State* s, const string& indent)
        {
            cout << indent << "state: " << s << " " << s->index << " suffix link: " << (s->suffixLink ? s->suffixLink->index : 0) << " parent: " << (s->parent ? s->parent->index : 0);
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
                cout << indent + " " << "transition: " << substringStartIndex << "," << substringEndIndex << (substringEndIndex == m_currentString.size() - 1 ? " (open) " : "") << " " << m_currentString.substr(substringStartIndex, substringEndIndex - substringStartIndex + 1) << " normalised: " << canonicaliseString(m_currentString.substr(substringStartIndex, substringEndIndex - substringStartIndex + 1), transition.letterPermutation, true) << " next state: " << transition.nextState->index << endl;
                dumpGraphAux(transition.nextState, indent + "  ");
            }
        }
        void dumpNormalisedStringsAux(Cursor cursor, const string& s, set<string>& normalisedStrings)
        {
            if (!s.empty())
            {
                cout << " normalised string (explicit): " << s << endl;
                assert(normalisedStrings.find(s) == normalisedStrings.end());
                normalisedStrings.insert(s);
            }
            if (cursor.isOnExplicitState())
            {
                auto nextLetterIterator = cursor.getNextLetterIterator();
                while (nextLetterIterator.hasNext())
                {
                    dumpNormalisedStringsAux(nextLetterIterator.afterFollowingNextLetter(), s + nextLetterIterator.nextLetter(), normalisedStrings);
                    nextLetterIterator++;
                }
            }
            else
            {
                const string normalisedRemainderOfTransition = canonicaliseString(m_currentString.substr(cursor.remainderOfCurrentTransition().startIndex(), cursor.remainderOfCurrentTransition().length()), cursor.letterPermutation(), true);
                string normalisedString = s;
                for (int i = 0; i < normalisedRemainderOfTransition.length() - 1; i++)
                {
                    normalisedString += normalisedRemainderOfTransition[i];
                    cout << " normalised string (non-explicit): " << normalisedString << endl;
                    assert(normalisedStrings.find(normalisedString) == normalisedStrings.end());
                    normalisedStrings.insert(normalisedString);
                }

                Cursor nextExplicitState(cursor);
                nextExplicitState.followToTransitionEnd();
                dumpNormalisedStringsAux(nextExplicitState, s + normalisedRemainderOfTransition, normalisedStrings);
            }
        }
};

using Cursor = SuffixTreeBuilder::Cursor;


set<string> bruteForce(const string& s)
{
    set<string> substrings;
    set<string> canonicalisedSubstrings;
    for (int i = 0; i < s.size(); i++)
    {
        string substring;
        for (int j = 0; i + j < s.size(); j++)
        {
            substring.push_back(s[i + j]);
            cout << "substring: " << substring << " canonicalised: " << canonicaliseString(substring) << endl;
            const string canonicalisedSubstring = canonicaliseString(substring);
            substrings.insert(substring);
            canonicalisedSubstrings.insert(canonicalisedSubstring);
        }
    }
    cout << " # distinct canonicalised substrings: " << canonicalisedSubstrings.size() << " # distinct substrings: " << substrings.size() << endl;
    return canonicalisedSubstrings;
}


void verify(Cursor cursor, int wordLength)
{
    if (cursor.isOnExplicitState())
    {
        if (!(cursor.stateData().wordLength == wordLength || !cursor.getNextLetterIterator().hasNext()))
        {
            cout << "wordLength: " << wordLength << " state word length: " << cursor.stateData().wordLength << " is leaf: " << !(cursor.getNextLetterIterator().hasNext()) << endl;
            cout << "string followed: " << cursor.stringFollowed() << endl;
        }
        assert(cursor.stateData().wordLength == wordLength || !cursor.getNextLetterIterator().hasNext());
        auto nextLetterIterator = cursor.getNextLetterIterator();
        while (nextLetterIterator.hasNext())
        {
            verify(nextLetterIterator.afterFollowingNextLetter(), wordLength + 1);

            nextLetterIterator++;
        }
    }
    else
    {
        Cursor nextCursor(cursor);
        const int numRemaining = cursor.remainderOfCurrentTransition().length();
        nextCursor.followNextLetters(numRemaining);
        verify(nextCursor, wordLength + numRemaining);
    }
}

void doStuff(const string& s)
{
    cout << "doStuff: " << s << endl;
    //cout << "brute force: " << endl;
    set<string> normalisedStringsBruteForce = bruteForce(s);
    //for (const auto& str : bruteForce(s))
    //{
        //cout << " " << str << endl;
    //}
    SuffixTreeBuilder treeBuilder;
    treeBuilder.computeSuffixNormalisationPermutations(s);
    treeBuilder.appendString(s);

    treeBuilder.dumpGraph();
    verify(treeBuilder.rootCursor(), 0);
    set<string> normalisedStringsOptimised = treeBuilder.dumpNormalisedStrings();
    for (const auto& str : normalisedStringsOptimised)
    {
        cout << " looking for " << str << " in brute forced:" << endl;
        assert(normalisedStringsBruteForce.find(str) != normalisedStringsBruteForce.end());
    }
    for (const auto& str : normalisedStringsBruteForce)
    {
        cout << " looking for " << str << " in optimised:" << endl;
        assert(normalisedStringsOptimised.find(str) != normalisedStringsOptimised.end());
    }

    //cout << "s: " << s << " canonicalised: " << canonicaliseString(s) << endl;
}


int main()
{
    //bruteForce("abcdefgdsfsdfskldhygauslkjglksjvlksjfdvh");
#define EXHAUSTIVE
#ifdef EXHAUSTIVE
    string s = "a";
    const int numLetters = 4;
    while (true)
    {
        cout << "s: " << s <<  " size: " << s.size() << endl;

        doStuff(s);

        int index = 0;
        while (index < s.size() && s[index] == 'a' + numLetters)
        {
            s[index] = 'a';
            index++;
        }
        if (index == s.size())
        {
            s.push_back('a');
            cout << "string length: " << s.length() << endl;
        }
        else
        {
            s[index]++;
        }
    }
    return 0;
#endif
//#define RANDOM
#ifdef RANDOM
    while (true)
    {
        const int n = rand() % 100;
        string s(n, '\0');
        for (int i = 0; i < n; i++)
        {
            s[i] = 'a' + rand() % 26;
        }
        doStuff(s);
    }
    return 0;
#endif

    int T;
    cin >> T;
    for (int t = 1; t <= T; t++)
    {
        string s;
        cin >> s;


        doStuff(s);

    }
}
