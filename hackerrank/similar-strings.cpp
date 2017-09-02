#define SUBMISSION
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

constexpr int maxN = 50'000;
constexpr int log2(int N, int exponent = 0, int powerOf2 = 1)
{
    return (powerOf2 >= N) ? exponent : log2(N, exponent + 1, powerOf2 * 2);
}
constexpr int log2MaxN = log2(maxN);

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
            assert(hasPermutedLetter(originalLetter));
            const char result = m_permutedLetter[originalLetter - 'a'];
            return result;
        }
    private:
        char m_permutedLetter[alphabetSize] = {'\0'};
        int m_numLettersPermuted = 0;
};

string isoNormaliseString(const string& s, LetterPermutation* letterPermutation = nullptr, bool assertNoUnknown = false, bool readOnly = true)
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

    string isoNormalised;

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
        isoNormalised += letterPermutationToUse->permutedLetter(letter);
    }
    return isoNormalised;
}

/**
 * Simple implementation of Ukkonen's algorithm:
 *  https://en.wikipedia.org/wiki/Ukkonen's_algorithm
 * for online construction of suffix trees.
 * Modified for use with "Pseudo-Ismorphic" Strings.
 * @author Simon St James, Jan 2017.
 */
class PseudoIsomorphicSuffixTree
{
    public:
        struct StateData
        {
            array<int, log2MaxN> stateIdJumpLookup;
        };
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
            int wordLength = 0;

            StateData stateData;
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
            // "Fill-in" permutations that were waiting for this letter.
            auto& permutationsMissingLetter = m_permutationsMissingLetter[letter - 'a'];
            for (auto permutationMissingLetter : permutationsMissingLetter)
            {
                permutationMissingLetter->permuteUnpermutedLetter(letter);
            }
            permutationsMissingLetter.clear();

            // Make a note that the permutation for this new suffix is missing every letter but
            // the one we've just added!
            m_isoNormalisedSuffixPermutations.push_back(LetterPermutation());
            LetterPermutation* newSuffixPermutation = &(m_isoNormalisedSuffixPermutations.back());
            newSuffixPermutation->permuteUnpermutedLetter(letter);
            for (int letterIndex = 0; letterIndex < alphabetSize; letterIndex++)
            {
                if (letterIndex != letter - 'a')
                {
                    m_permutationsMissingLetter[letterIndex].push_back(newSuffixPermutation);
                }
            }

            // Keep nextOccurenceOfLetterIndexAtOrAfter up-to-date.
            auto& nextOccurrenceOfLetterAtOrAfter = m_nextOccurenceOfLetterIndexAtOrAfter[letter - 'a'];
            nextOccurrenceOfLetterAtOrAfter.insert(nextOccurrenceOfLetterAtOrAfter.end(), m_currentString.size() + 1 - nextOccurrenceOfLetterAtOrAfter.size(), m_currentString.size());
            assert(nextOccurrenceOfLetterAtOrAfter.size() == m_currentString.size());

            // Add to the Suffix Tree!
            m_currentString += letter;
            const auto updateResult = update(m_s, m_k, m_currentString.size());
            m_s = updateResult.first;
            m_k = updateResult.second;
            const auto canonizeResult = canonize(m_s, m_k, m_currentString.size(),(m_numSuffixLinksTraversed < m_isoNormalisedSuffixPermutations.size() ?  &(m_isoNormalisedSuffixPermutations[m_numSuffixLinksTraversed]) : &allLettersToA));
            m_s = canonizeResult.first;
            m_k = canonizeResult.second;

            m_numNonPseudoIsomorphicSubstrings += m_numLeafStates;
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
        int64_t numNonPseudoIsomorphicSubstrings() const
        {
            return m_numNonPseudoIsomorphicSubstrings;
        }
        string currentString() const
        {
            return m_currentString;
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
                StateData stateData()
                {
                    assert(isOnExplicitState());
                    return m_state->stateData;
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
        Cursor cursorFromStateId(int stateId)
        {
            return Cursor(&(m_states[stateId]), m_currentString, m_root);
        }
        static Cursor invalidCursor()
        {
            return Cursor();
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

        deque<LetterPermutation> m_isoNormalisedSuffixPermutations;
        LetterPermutation allLettersToA;
        array<deque<LetterPermutation*>, alphabetSize> m_permutationsMissingLetter;
        array<deque<int>, alphabetSize> m_nextOccurenceOfLetterIndexAtOrAfter;

        int64_t m_numLeafStates = 0;
        int64_t m_numNonPseudoIsomorphicSubstrings = 0;

        std::pair<State*, int> update(State* s, int k, int i)
        {
            State* oldr = m_root;
            assert(m_numSuffixLinksTraversed < m_isoNormalisedSuffixPermutations.size());
            const int letterIndex = m_isoNormalisedSuffixPermutations[m_numSuffixLinksTraversed].permutedLetter(m_currentString[i - 1]) - 'a' + 1;
            assert(m_numSuffixLinksTraversed < m_isoNormalisedSuffixPermutations.size());
            const auto testAndSplitResult = testAndSplit(s, k, i - 1, letterIndex, &(m_isoNormalisedSuffixPermutations[m_numSuffixLinksTraversed]));
            auto isEndPoint = testAndSplitResult.first;
            auto r = testAndSplitResult.second;
            assert(r);
            while (!isEndPoint)
            {
                auto rPrime = createNewState(r);
                rPrime->wordLength = -1;
                r->transitions.push_back(Transition(rPrime, Substring(i, openTransitionEnd), &(m_isoNormalisedSuffixPermutations[m_numSuffixLinksTraversed]), m_currentString));
                assert(m_numSuffixLinksTraversed < m_isoNormalisedSuffixPermutations.size());
                m_numLeafStates++;
                if (oldr != m_root)
                {
                    oldr->suffixLink = r;
                    assert(isoNormalisedStringToState(oldr).size() == isoNormalisedStringToState(r).size() + 1);
                }
                oldr = r;

                assert(isoNormaliseString(m_currentString.substr(m_numSuffixLinksTraversed)).find(isoNormalisedStringToState(s)) == 0);
                if (!s->suffixLink)
                    addSuffixLink(s, m_numSuffixLinksTraversed);
                assert(s->suffixLink);
                assert(isoNormalisedStringToState(s).size() <= 1 || isoNormalisedStringToState(s->suffixLink) == isoNormaliseString(isoNormalisedStringToState(s).substr(1)));
                m_numSuffixLinksTraversed++;
                const bool emptySuffix = m_currentString.substr(m_numSuffixLinksTraversed).empty();
                const auto canonizeResult = canonize(s->suffixLink, k, i - 1, (s->suffixLink == m_auxiliaryState) ? &allLettersToA : &(m_isoNormalisedSuffixPermutations[m_numSuffixLinksTraversed]));
                const auto oldK = k;
                s = canonizeResult.first;
                k = canonizeResult.second;
                assert(emptySuffix || isoNormaliseString(m_currentString.substr(m_numSuffixLinksTraversed)).find(isoNormalisedStringToState(s)) == 0);

                assert(m_numSuffixLinksTraversed < m_isoNormalisedSuffixPermutations.size() || emptySuffix);
                const int letterIndex = emptySuffix ? 1 : m_isoNormalisedSuffixPermutations[m_numSuffixLinksTraversed].permutedLetter(m_currentString[i - 1]) - 'a' + 1;
                const auto testAndSplitResult = testAndSplit(s, k, i - 1, letterIndex, emptySuffix ? &allLettersToA : &(m_isoNormalisedSuffixPermutations[m_numSuffixLinksTraversed]));
                isEndPoint = testAndSplitResult.first;
                r = testAndSplitResult.second;
                assert(r);
            }

            if (oldr != m_root)
            {
                assert(s == r);
                if(oldr->wordLength != s->wordLength + 1)
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
            // This is a new addition, and was not needed in Ukkonnen's original algorithm.
            // The basic approach is to find the parent of s; find this parent's suffix link
            // (recursively computing it if it is not already defined); work out the 
            // substring of m_currentString that we have to follow from s's parent to s; translate
            // this to the sequence of letters we'd have to follow from s's parent's suffix link
            // (recall: traversing a suffix link changes the current suffix of m_currentString we are
            // following, and hence the LetterPermutation); and replaying this translated sequence of chars
            // from s's parent's suffix link to get the suffix link of s (after canonizing and potentially 
            // making it explict, of course!).
            assert(s && !s->suffixLink);
            auto sParent = s->parent;
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
                    LetterPermutation suffixIncreasePermutation;
                    // Very hard to document, but this essentially performs the equivalent of:
                    //  isoNormaliseString(isoNormaliseString(m_currentString.substr(m_numSuffixLinksTraversed)).substr(1), &suffixIncreasePermutation, false, false);
                    // It's essentially used to map a sequence of letters from the old suffix to the new suffix.
                    // It kind of just maps a letter to the letter before it i.e. b -> a, c -> b, d -> c or itself (b -> b, c -> c, etc) depending on each letters
                    // next occurence compared to nextOccurrenceOfFirstLetter.
                    LetterPermutation& oldSuffixPermutation = m_isoNormalisedSuffixPermutations[m_numSuffixLinksTraversed];
                    const char firstLetter = m_currentString[m_numSuffixLinksTraversed];
                    const char firstLetterPermuted = oldSuffixPermutation.permutedLetter(firstLetter);
                    int largestMappedToLetterIndexBeforeNextFirstLetter = -1;
                    auto nextOccurenceOfLetterAfter = [=](const char letter, int pos)
                    {
                        pos++;
                        const auto& nextOccurenceAtOrAfter = m_nextOccurenceOfLetterIndexAtOrAfter[letter - 'a'];
                        if (pos >= nextOccurenceAtOrAfter.size())
                            return -1;

                        return nextOccurenceAtOrAfter[pos];
                    };
                    const int nextOccurrenceOfFirstLetter = nextOccurenceOfLetterAfter(firstLetter, numSuffixLinksTraversed);
                    for (int i = 0; i < alphabetSize; i++)
                    {
                        const char letter = 'a' + i;
                        if (letter != firstLetter)
                        {
                            if (oldSuffixPermutation.hasPermutedLetter(letter))
                            {
                                const char mappedByOldSuffix = oldSuffixPermutation.permutedLetter(letter);
                                if (nextOccurrenceOfFirstLetter != -1 && nextOccurenceOfLetterAfter(letter, m_numSuffixLinksTraversed) > nextOccurrenceOfFirstLetter)
                                {
                                    suffixIncreasePermutation.permuteUnpermutedLetter(mappedByOldSuffix, mappedByOldSuffix);
                                }
                                else
                                {
                                    suffixIncreasePermutation.permuteUnpermutedLetter(mappedByOldSuffix, mappedByOldSuffix - 1);
                                    largestMappedToLetterIndexBeforeNextFirstLetter = max(largestMappedToLetterIndexBeforeNextFirstLetter, mappedByOldSuffix - 1 - 'a');
                                }
                            }
                        }
                    }
                    if (nextOccurrenceOfFirstLetter != -1)
                    {
                        suffixIncreasePermutation.permuteUnpermutedLetter(firstLetterPermuted, 'a' + largestMappedToLetterIndexBeforeNextFirstLetter + 1);
                    }

                    // The compoundPermutation permutation essentially translates from letters on the transition from s's parent to s to 
                    // letters in the next suffix; following the letters on the transition with this permutation
                    // applied from parentSuffixLink gets us towards our suffix link for s.
                    LetterPermutation compoundPermutation;
                    for (int i = 0; i < alphabetSize; i++)
                    {
                        const char originalLetter = 'a' + i;
                        char compoundPermutedLetter = 'z';
                        if (transition.letterPermutation->hasPermutedLetter(originalLetter))
                        {
                            compoundPermutedLetter = transition.letterPermutation->permutedLetter(originalLetter);
                            if (suffixIncreasePermutation.hasPermutedLetter(compoundPermutedLetter))
                            {
                                compoundPermutedLetter = suffixIncreasePermutation.permutedLetter(compoundPermutedLetter);
                                compoundPermutation.permuteUnpermutedLetter(originalLetter, compoundPermutedLetter);
                            }
                        }
                    }
                    assert(parentSuffixLink);
                    if (sParent != m_root)
                        assert(isoNormalisedStringToState(parentSuffixLink) == isoNormaliseString(isoNormalisedStringToState(sParent).substr(1)));

                    const auto p = transition.substringFollowed.endIndex;
                    const auto k = (parentSuffixLink == m_auxiliaryState ? transition.substringFollowed.startIndex + 1 : transition.substringFollowed.startIndex);
                    const auto preCanonizeLength = p - k + 1;
                    if (parentSuffixLink == m_auxiliaryState)
                        parentSuffixLink = m_root;
                    // Follow the letters on the transition, mapped to letters in the next suffix, by (mis?)using canonize.
                    const auto canonizeResult = canonize(parentSuffixLink, k, p, &compoundPermutation);
                    const auto suffixLinkCanonized = canonizeResult.first;
                    const auto kCanonized = canonizeResult.second;
                    // The suffix link may not be explicit; (ab?)use testAndSplitResult to ensure it is.
                    const char unusedChar = 'a' + alphabetSize - 1;
                    const auto testAndSplitResult = testAndSplit(suffixLinkCanonized, kCanonized, p, unusedChar - 'a' + 1, &compoundPermutation);
                    s->suffixLink = testAndSplitResult.second;
                    break;
                }
            }
            assert(isoNormalisedStringToState(s->suffixLink).size() + 1 == isoNormalisedStringToState(s).size());
            assert(isoNormalisedStringToState(s->suffixLink) == isoNormaliseString(isoNormalisedStringToState(s).substr(1)));
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
                    return {true, s};
                else
                {
                    LetterPermutation* letterPermutationOnTransition = tkTransitionIter->letterPermutation;
                    s->transitions.erase(tkTransitionIter);
                    auto r = createNewState(s);
                    r->wordLength = s->wordLength + p - k + 1;
                    s->transitions.push_back(Transition(r, Substring(kPrime, kPrime + p - k), letterPermutationOnTransition, m_currentString));
                    r->transitions.push_back(Transition(sPrime, Substring(kPrime + p - k + 1, pPrime), letterPermutationOnTransition, m_currentString));
                    sPrime->parent = r;
                    return {false, r};
                }
            }
            else
            {
                const auto tTransitionIter = findTransitionIter(s, letterIndex, false);
                if (tTransitionIter == s->transitions.end())
                    return {false, s};
                else
                    return {true, s};
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
                while (pPrime - kPrime <= p - k)
                {
                    k = k + pPrime - kPrime + 1;
                    s = sPrime;
                    if (k <= p)
                    {
                        assert(s != m_auxiliaryState);
                        const char letterToFollowFromState = letterPermutation->permutedLetter(m_currentString[k - 1]);
                        auto tkTransitionIter = findTransitionIter(s, letterToFollowFromState - 'a' + 1);
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
};

int bruteForce(const string& s, int l, int r)
{
    int numFound = 0;
    const string normalisedRangeToFind = isoNormaliseString(s.substr(l, r - l + 1));
    for (int i = 0; i < s.size(); i++)
    {
        const string normalisedRange = isoNormaliseString(s.substr(i, r - l + 1));
        if (normalisedRange == normalisedRangeToFind)
            numFound++;
    }
    return numFound;
}

using Cursor = PseudoIsomorphicSuffixTree::Cursor;

int main()
{
    int n, q;
    cin >> n >> q;

    string s;
    cin >> s;

    PseudoIsomorphicSuffixTree treeBuilder;
    treeBuilder.appendString(s);
    treeBuilder.makeFinalStatesExplicitAndMarkThemAsFinal();

    cout << "log2MaxN: " << log2MaxN << endl;

    for (int i = 0; i < q; i++)
    {
        int l, r;
        cin >> l >> r;
        l--;
        r--;

#define BRUTE_FORCE
#ifdef BRUTE_FORCE
        cout << bruteForce(s, l, r) << endl;
#endif
    }
}
