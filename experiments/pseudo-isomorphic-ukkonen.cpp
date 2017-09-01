//#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <array>
#include <deque>
#include <set>
#include <string>
#include <limits>
#include <memory>
#include <algorithm>
#include <chrono>
#include <cassert>

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
            if (!(result >= 'a' && result <= '{'))
            {
                cout << " permutation: " << this << " invalid mapping from " << originalLetter << " to " << result << endl;
                assert(false);
            }
            return result;
        }
        void clear()
        {
            m_numLettersPermuted = 0;
            std::fill(begin(m_permutedLetter), end(m_permutedLetter), '\0');
        }
    private:
        char m_permutedLetter[alphabetSize] = {'\0'};
        int m_numLettersPermuted = 0;
};

string canonicaliseString(const string& s, LetterPermutation* letterPermutation = nullptr, bool assertNoUnknown = false, bool readOnly = true)
{
    LetterPermutation letterPermutationLocal;
    LetterPermutation* letterPermutationToUse = letterPermutation;
    if (letterPermutation && readOnly)
    {
        letterPermutationLocal = *letterPermutation;
    }
    if (!letterPermutationToUse)
    {
        letterPermutationToUse = &letterPermutationLocal;
    }

    string canonicalised;

    for (const auto letter : s)
    {
        if (!letterPermutationToUse->hasPermutedLetter(letter))
        {
            if (assertNoUnknown && letterPermutationToUse == letterPermutation)
            {
                cout << "Error: letter " << letter << " has no permutation in permutation: " << letterPermutationToUse << endl;
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
class PseudoIsomorphicSuffixTree
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
            Transition(State *nextState, const Substring& substringFollowed, LetterPermutation* letterPermutation, const string& currentString)
                : nextState(nextState), substringFollowed(substringFollowed), letterPermutation{letterPermutation}
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
        PseudoIsomorphicSuffixTree()
        {
            m_states.reserve(1'000'000);

            m_root = createNewState();
            m_auxiliaryState = createNewState();

            for (int i = 0; i < alphabetSize; i++)
            {
                m_auxiliaryState->transitions.push_back(Transition(m_root, Substring(-(i + 1), -(i + 1)), &allLettersToA, m_currentString));
                allLettersToA.permuteUnpermutedLetter('a' + i, 'a');
            }
            m_root->suffixLink = m_auxiliaryState;

            m_s = m_root;
            m_k = 1;
        }
        PseudoIsomorphicSuffixTree(const PseudoIsomorphicSuffixTree& other) = delete;
        void appendLetter(char letter)
        {
            auto& permutationsMissingLetter = m_permutationsMissingLetter[letter - 'a'];
            for (auto permutationMissingLetter : permutationsMissingLetter)
            {
                permutationMissingLetter->permuteUnpermutedLetter(letter);
            }
            permutationsMissingLetter.clear();

            m_normalisedSuffixPermutations.push_back(LetterPermutation());
            LetterPermutation* newSuffixPermutation = &(m_normalisedSuffixPermutations.back());
            newSuffixPermutation->permuteUnpermutedLetter(letter);
            for (int letterIndex = 0; letterIndex < alphabetSize; letterIndex++)
            {
                if (letterIndex != letter - 'a')
                {
                    m_permutationsMissingLetter[letterIndex].push_back(newSuffixPermutation);
                }
            }

            for (int letterIndex = 0; letterIndex < alphabetSize; letterIndex++)
            {
                m_nextOccurenceOfLetterIndexAtOrAfter[letterIndex].push_back(-1);
            }
            auto nextOccurenceOfLetterIndexAtOrAfterRevIter = m_nextOccurenceOfLetterIndexAtOrAfter[letter - 'a'].rbegin();
            while (nextOccurenceOfLetterIndexAtOrAfterRevIter != m_nextOccurenceOfLetterIndexAtOrAfter[letter - 'a'].rend() && *nextOccurenceOfLetterIndexAtOrAfterRevIter == -1)
            {
                *nextOccurenceOfLetterIndexAtOrAfterRevIter = m_currentString.size();
                nextOccurenceOfLetterIndexAtOrAfterRevIter++;
            }

            m_currentString += letter;
            const auto updateResult = update(m_s, m_k, m_currentString.size());
            m_s = updateResult.first;
            m_k = updateResult.second;
            const auto canonizeResult = canonize(m_s, m_k, m_currentString.size(),(m_numSuffixLinksTraversed < m_normalisedSuffixPermutations.size() ?  &(m_normalisedSuffixPermutations[m_numSuffixLinksTraversed]) : &allLettersToA));
            m_s = canonizeResult.first;
            m_k = canonizeResult.second;
        }
        void appendString(const string& stringToAppend)
        {
            int numAppended = 0;
            for (auto letter : stringToAppend)
            {
                appendLetter(letter);
                numAppended++;
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
                    while (!(!copy.m_state->parent && copy.isOnExplicitState()))
                    {
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

                friend class PseudoIsomorphicSuffixTree;
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

        deque<LetterPermutation> m_normalisedSuffixPermutations;
        LetterPermutation allLettersToA;
        array<deque<LetterPermutation*>, alphabetSize> m_permutationsMissingLetter;
        array<deque<int>, alphabetSize> m_nextOccurenceOfLetterIndexAtOrAfter;

        string normalisedStringToState(State* s)
        {
#ifndef NDEBUG
            Cursor blah(s, m_currentString, m_root);
            return blah.canonicalisedStringFollowed();
#else 
            return "";
#endif
        }

        std::pair<State*, int> update(State* s, int k, int i)
        {
            State* oldr = m_root;
            assert(m_numSuffixLinksTraversed < m_normalisedSuffixPermutations.size());
            const int letterIndex = m_normalisedSuffixPermutations[m_numSuffixLinksTraversed].permutedLetter(m_currentString[i - 1]) - 'a' + 1;
            assert(m_numSuffixLinksTraversed < m_normalisedSuffixPermutations.size());
            const auto testAndSplitResult = testAndSplit(s, k, i - 1, letterIndex, &(m_normalisedSuffixPermutations[m_numSuffixLinksTraversed]));
            auto isEndPoint = testAndSplitResult.first;
            auto r = testAndSplitResult.second;
            assert(r);
            while (!isEndPoint)
            {
                auto rPrime = createNewState(r);
                rPrime->data.wordLength = -1;
                r->transitions.push_back(Transition(rPrime, Substring(i, openTransitionEnd), &(m_normalisedSuffixPermutations[m_numSuffixLinksTraversed]), m_currentString));
                assert(m_numSuffixLinksTraversed < m_normalisedSuffixPermutations.size());
                verifyStateTransitions(r);
                if (oldr != m_root)
                {
                    oldr->suffixLink = r;
                    assert(normalisedStringToState(oldr).size() == normalisedStringToState(r).size() + 1);
                }
                oldr = r;

                assert(canonicaliseString(m_currentString.substr(m_numSuffixLinksTraversed)).find(normalisedStringToState(s)) == 0);
                if (!s->suffixLink)
                    addSuffixLink(s, m_numSuffixLinksTraversed);
                assert(s->suffixLink);
                assert(normalisedStringToState(s).size() <= 1 || normalisedStringToState(s->suffixLink) == canonicaliseString(normalisedStringToState(s).substr(1)));
                m_numSuffixLinksTraversed++;
                const bool emptySuffix = m_currentString.substr(m_numSuffixLinksTraversed).empty();
                const auto canonizeResult = canonize(s->suffixLink, k, i - 1, (s->suffixLink == m_auxiliaryState) ? &allLettersToA : &(m_normalisedSuffixPermutations[m_numSuffixLinksTraversed]));

                const auto oldK = k;
                s = canonizeResult.first;
                k = canonizeResult.second;
                assert(emptySuffix || canonicaliseString(m_currentString.substr(m_numSuffixLinksTraversed)).find(normalisedStringToState(s)) == 0);

                assert(m_numSuffixLinksTraversed < m_normalisedSuffixPermutations.size() || emptySuffix);
                const int letterIndex = emptySuffix ? 1 : m_normalisedSuffixPermutations[m_numSuffixLinksTraversed].permutedLetter(m_currentString[i - 1]) - 'a' + 1;
                const auto testAndSplitResult = testAndSplit(s, k, i - 1, letterIndex, emptySuffix ? &allLettersToA : &(m_normalisedSuffixPermutations[m_numSuffixLinksTraversed]));
                isEndPoint = testAndSplitResult.first;
                r = testAndSplitResult.second;
                assert(r);
            }

            if (oldr != m_root)
            {
                assert(s == r);
                if(oldr->data.wordLength != s->data.wordLength + 1)
                {
                    assert(!oldr->suffixLink);
                }
                else
                {
                    oldr->suffixLink = s;

                }
            }
            return {s, k};


        }

        void addSuffixLink(State* s, int numSuffixLinksTraversed)
        {
            assert(s && !s->suffixLink);
            auto sParent = s->parent;
            verifyStateTransitions(sParent);
            for (int i = 0; i < m_currentString.size(); i++)
            {
                assert(canonicaliseString(m_currentString.substr(i)) == canonicaliseString(canonicaliseString(m_currentString).substr(i)));
            }
            for (const auto& transition : sParent->transitions)
            {
                if (transition.nextState == s)
                {
                    auto parentSuffixLink = sParent->suffixLink;
                    if (!parentSuffixLink)
                    {
                        addSuffixLink(sParent, numSuffixLinksTraversed);
                        parentSuffixLink = sParent->suffixLink;
                    }
                    assert(parentSuffixLink);
                    LetterPermutation compoundPermutation;
                    {
                        LetterPermutation suffixIncreasePermutation;
                        // Very hard to document, but this essentially performs the equivalent of:
                        //  canonicaliseString(canonicaliseString(m_currentString.substr(m_numSuffixLinksTraversed)).substr(1), &suffixIncreasePermutation, false, false);
                        // It's essentially used to map a sequence of letters from the old suffix to the new suffix.
                        // It kind of just maps a letter to the letter before it i.e. b -> a, c -> b, d -> c or itself (b -> b, c -> c, etc) depending on each letters
                        // next occurence compared to nextOccurrenceOfFirstLetter.
                        LetterPermutation suffixIncreasePermutationOpt;
                        LetterPermutation& oldSuffixPermutation = m_normalisedSuffixPermutations[m_numSuffixLinksTraversed];
                        const char firstLetter = m_currentString[m_numSuffixLinksTraversed];

                        const char firstLetterPermuted = oldSuffixPermutation.permutedLetter(firstLetter);
                        int largestMappedToLetterIndexBeforeNextFirstLetter = -1;
                        int nextOccurrenceOfFirstLetter = m_nextOccurenceOfLetterIndexAtOrAfter[firstLetter - 'a'][m_numSuffixLinksTraversed + 1];
                        for (int i = 0; i < alphabetSize; i++)
                        {
                            const char letter = 'a' + i;
                            if (letter != firstLetter)
                            {
                                if (m_normalisedSuffixPermutations[m_numSuffixLinksTraversed].hasPermutedLetter(letter))
                                {
                                    const char mappedByOldSuffix = m_normalisedSuffixPermutations[m_numSuffixLinksTraversed].permutedLetter(letter);
                                    if (nextOccurrenceOfFirstLetter != -1 && m_nextOccurenceOfLetterIndexAtOrAfter[i][m_numSuffixLinksTraversed + 1] > nextOccurrenceOfFirstLetter)
                                    {
                                        suffixIncreasePermutationOpt.permuteUnpermutedLetter(mappedByOldSuffix, mappedByOldSuffix);
                                    }
                                    else
                                    {
                                        suffixIncreasePermutationOpt.permuteUnpermutedLetter(mappedByOldSuffix, mappedByOldSuffix - 1);
                                        largestMappedToLetterIndexBeforeNextFirstLetter = max(largestMappedToLetterIndexBeforeNextFirstLetter, mappedByOldSuffix - 1 - 'a');
                                    }
                                }
                            }
                        }
                        if (nextOccurrenceOfFirstLetter != -1)
                        {
                            suffixIncreasePermutationOpt.permuteUnpermutedLetter(firstLetterPermuted, 'a' + largestMappedToLetterIndexBeforeNextFirstLetter + 1);
                        }
                        suffixIncreasePermutation = suffixIncreasePermutationOpt;

                        for (int i = 0; i < alphabetSize; i++)
                        {
                            const char originalLetter = 'a' + i;
                            char compoundPermutedLetter = '\0';
                            if (transition.letterPermutation->hasPermutedLetter(originalLetter))
                            {
                                compoundPermutedLetter = transition.letterPermutation->permutedLetter(originalLetter);
                                if (suffixIncreasePermutation.hasPermutedLetter(compoundPermutedLetter))
                                {
                                    compoundPermutedLetter = suffixIncreasePermutation.permutedLetter(compoundPermutedLetter);
                                }
                                else
                                {
                                    compoundPermutedLetter = '\0';
                                }
                            }
                            if (compoundPermutedLetter != '\0')
                            {
                                compoundPermutation.permuteUnpermutedLetter(originalLetter, compoundPermutedLetter);
                            }
                            else
                            {
                                compoundPermutation.permuteUnpermutedLetter(originalLetter, 'a');
                            }
                        }
                    }
                    assert(parentSuffixLink);
                    if (sParent != m_root)
                        assert(normalisedStringToState(parentSuffixLink) == canonicaliseString(normalisedStringToState(sParent).substr(1)));

                    const auto p = transition.substringFollowed.endIndex;
                    const auto oldK = (parentSuffixLink == m_auxiliaryState ? transition.substringFollowed.startIndex + 1 : transition.substringFollowed.startIndex);
                    const auto preCanonizeLength = p - oldK + 1;
                    if (parentSuffixLink == m_auxiliaryState)
                        parentSuffixLink = m_root;
                    const auto canonizeResult = canonize(parentSuffixLink, oldK, p, &compoundPermutation);
                    const auto parentSuffixLinkCanonized = canonizeResult.first;
                    const auto kCanonized = canonizeResult.second;
                    const auto postCanonizeLength = p - kCanonized + 1;
                    const auto numLettersFollowedDuringCanonization = preCanonizeLength - postCanonizeLength;
                    if (postCanonizeLength == 0)
                    {
                        s->suffixLink = parentSuffixLinkCanonized;
                    }
                    else
                    {
                        const auto transitionFromParentSuffixLink = findTransitionIter(parentSuffixLinkCanonized, compoundPermutation.permutedLetter(m_currentString[kCanonized - 1]) - 'a' + 1);
                        if (postCanonizeLength == transitionFromParentSuffixLink->substringFollowed.length(m_currentString.length()))
                        {
                            s->suffixLink = transitionFromParentSuffixLink->nextState;
                        }
                        else
                        {
                            const auto testAndSplitResult = testAndSplit(parentSuffixLinkCanonized, kCanonized, p - 0, alphabetSize, &compoundPermutation);

                            // Need to find the suffix link for testAndSplitResult.second, too :(
                            assert(!testAndSplitResult.first);
                            s->suffixLink = testAndSplitResult.second;
                        }
                    }
                    break;
                }
            }
            assert(normalisedStringToState(s->suffixLink).size() + 1 == normalisedStringToState(s).size());
            assert(normalisedStringToState(s->suffixLink) == canonicaliseString(normalisedStringToState(s).substr(1)));
        }
        pair<bool, State*> testAndSplit(State* s, int k, int p, int letterIndex, LetterPermutation* letterPermutation)
        {
            assert(s);
            if (k <= p)
            {
                const char letterToFollowFromState = letterPermutation->permutedLetter(m_currentString[k - 1]);
                const auto tkTransitionIter = findTransitionIter(s, letterToFollowFromState - 'a' + 1);
                auto sPrime = tkTransitionIter->nextState;
                auto kPrime = tkTransitionIter->substringFollowed.startIndex;
                auto pPrime = tkTransitionIter->substringFollowed.endIndex;
                assert(tkTransitionIter->letterPermutation);
                if (letterIndex == tkTransitionIter->letterPermutation->permutedLetter(m_currentString[kPrime + p - k]) - 'a' + 1)
                {
                    return {true, s};
                }
                else
                {
                    LetterPermutation* letterPermutationOnTransition = tkTransitionIter->letterPermutation;
                    s->transitions.erase(tkTransitionIter);
                    auto r = createNewState(s);
                    r->data.wordLength = s->data.wordLength + p - k + 1;
                    s->transitions.push_back(Transition(r, Substring(kPrime, kPrime + p - k), letterPermutationOnTransition, m_currentString));
                    verifyStateTransitions(s);
                    r->transitions.push_back(Transition(sPrime, Substring(kPrime + p - k + 1, pPrime), letterPermutationOnTransition, m_currentString));
                    verifyStateTransitions(r);
                    sPrime->parent = r;
                    return {false, r};
                }
            }
            else
            {
                const auto tTransitionIter = findTransitionIter(s, letterIndex, false);
                if (tTransitionIter == s->transitions.end())
                {
                    return {false, s};
                }
                else
                {
                    return {true, s};
                }
            }
        }
        std::pair<State*, int> canonize(State* s, int k, int p, LetterPermutation* letterPermutation)
        {
            assert(s);
            const auto originalS = s;
            if (p < k)
                return {s, k};
            else
            {
                const char letterToFollowFromState = letterPermutation->permutedLetter(m_currentString[k - 1]);
                assert(letterToFollowFromState >= 'a' && letterToFollowFromState <= 'z');
                auto tkTransitionIter = findTransitionIter(s, letterToFollowFromState - 'a' + 1);
                auto sPrime = tkTransitionIter->nextState;
                auto kPrime = tkTransitionIter->substringFollowed.startIndex;
                auto pPrime = tkTransitionIter->substringFollowed.endIndex;
                bool sWasAux = (originalS == m_auxiliaryState);
                while (pPrime - kPrime <= p - k)
                {
                    k = k + pPrime - kPrime + 1;
                    s = sPrime;
                    if (k <= p)
                    {
                        assert(s != m_auxiliaryState);
                        const char letterToFollowFromState =   letterPermutation->permutedLetter(m_currentString[k - 1]);
                        auto tkTransitionIter = findTransitionIter(s, letterToFollowFromState - 'a' + 1);
                        sPrime = tkTransitionIter->nextState;
                        kPrime = tkTransitionIter->substringFollowed.startIndex;
                        pPrime = tkTransitionIter->substringFollowed.endIndex;
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
            Cursor blah(state, m_currentString, m_root);
            set<char> canonicalNextChars;
            for (auto transitionIter = state->transitions.begin(); transitionIter != state->transitions.end(); transitionIter++)
            {
                if (transitionIter->substringFollowed.startIndex >= 0)
                {
                    assert(transitionIter->letterPermutation);
                    const char canonicalNextChar = transitionIter->letterPermutation->permutedLetter(m_currentString[transitionIter->substringFollowed.startIndex - 1]);
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
            for (auto transitionIter = state->transitions.begin(); transitionIter != state->transitions.end(); transitionIter++)
            {
                if (transitionIter->substringFollowed.startIndex >= 0)
                {
                    assert(transitionIter->substringFollowed.startIndex != 0);
                    const char transitionLetter = transitionIter->letterPermutation->permutedLetter(m_currentString[transitionIter->substringFollowed.startIndex - 1]);
                    if (transitionLetter - 'a' + 1 == letterIndex)
                    {
                        return transitionIter;
                    }
                }

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
                    assert(normalisedStrings.find(normalisedString) == normalisedStrings.end());
                    normalisedStrings.insert(normalisedString);
                }

                Cursor nextExplicitState(cursor);
                nextExplicitState.followToTransitionEnd();
                dumpNormalisedStringsAux(nextExplicitState, s + normalisedRemainderOfTransition, normalisedStrings);
            }
        }
};

using Cursor = PseudoIsomorphicSuffixTree::Cursor;


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
            const string canonicalisedSubstring = canonicaliseString(substring, nullptr, false, false);
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
#define BRUTE_FORCE
#ifdef BRUTE_FORCE
    set<string> normalisedStringsBruteForce = bruteForce(s);
#endif
    //for (const auto& str : bruteForce(s))
    //{
    //}
    PseudoIsomorphicSuffixTree treeBuilder;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    treeBuilder.appendString(s);

    std::chrono::steady_clock::time_point end= std::chrono::steady_clock::now();
    std::cout << "Time to build PseudoIsomorphicSuffixTree = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() / 1000.0 << " for string of length: " << s.size() << std::endl;

#ifdef BRUTE_FORCE
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
#endif

}


int main()
{
    //bruteForce("abcdefgdsfsdfskldhygauslkjglksjvlksjfdvh");
//#define EXHAUSTIVE
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
#define RANDOM
#ifdef RANDOM
    while (true)
    {
        const int n = rand() % 1000;
        const int m = rand() % 26 + 1;
        string s(n, '\0');
        for (int i = 0; i < n; i++)
        {
            s[i] = 'a' + rand() % m;
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
