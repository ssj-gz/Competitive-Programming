// Simon St James (ssjgz) - 2017-09-02
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

using Cursor = PseudoIsomorphicSuffixTree::Cursor;

int main()
{
    // Oof - a very tough one to document, this one :/ I think it's probably best to outline how 
    // we'd solve this in the non-pseudo-isomorphic case (i.e. where, for each prefix P of S,
    // we find how many distinct substrings P has) and then show how to adapt this for the 
    // pseudo-isomorphic case.
    // 
    // Let s be our string.  Imagine that we build up a list of substrings formed by successive prefixes of P
    // i.e if s = abac, the successive prefixes of s are a, ab, aba, and abac.
    // For P = a, we just have one string (ignore the number in the brackets for now; I'll explain that later!)
    //  a (0)
    // For the list of substrings for the next prefix, ab, we can add the new letter, 'b', to all substrings for
    // the last P (including the empty substring):
    //  b (1)
    //  ab (0)
    // For the next prefix, aba, we do the same, this time adding the new 'a':
    //  a (2)
    //  ba (1)
    //  aba (0)
    // For the final prefix, abac, we add the new 'c':
    //  c (3)
    //  ac (2)
    //  bac (1)
    //  abac (0)
    // Note the number (i) associated with string si: si ends up being the ith suffix of s (i.e. in C++, == s.substr(i)).
    // In other words, if w' is the string labeled (k - 1) and w is the string labelled (k), then w' = w.substr(1).
    // We can express this algorithm very roughly as:
    //  Let substrings = {}.
    //  For each letter x of s:
    //   new_substrings = {}
    //   for each w (including empty string) in substrings
    //    add wx to new_substrings 
    //   substrings = new_substrings
    //
    // Alternatively, noting that each substring has a number (m) associated with it: 
    //
    //  Let substrings = {}.
    //  i = 0
    //  For each letter x of S:
    //   new_substrings = {}
    //   while k != -1
    //    let w = string labelled (k) in substrings or empty string if there is no such w.
    //    add wx to new_substrings, labelled with (k).
    //    k = k - 1
    //   substrings = new_substrings
    //
    // This isn't strictly accurate in general (it works here as wx never already exists), but it seems like we are starting
    // with the longest string w in the old list of substrings, adding wx to the new list of substrings, and setting w = w.substr(1)
    // i.e. setting w to its next suffix, and repeating until we can't go any further.
    // Let's look at how this would play out in a Suffix Automaton rather than a list of words: here, the longest substring
    // corresponds to a leaf-state, S say; adding x to it would consist of adding 1 to the end index of the transition leading to S;
    // setting w to w.substr(1) would correspond to jumping to the state S->suffixLink, and we'd stop when there are no
    // further suffix links to traverse.*
    //
    // Ukkonnen's algorithm basically follows the algorithm above translated to a Suffix Automaton rather than a list of words, in the
    // manner just described.
    // This algorithm would be O(|s|^2) (for each new letter of s, we have to add it to O(|s| words i.e. bump the end index of O(|s|) 
    // leaf states), but Ukkonnen exploits two clever tricks; firstly, it notes that if in running the algorithm we find that 
    // wx already exists for some w, then we can stop following suffix links for this new letter x: intuitively, if wx already exists,
    // then it was added in a previous iteration.  But then, if wx was added in a previous iteration, so were all suffixes of wx.
    // Thus, there is no point trying to add the remaining suffixes of wx - they're also already there!
    //
    // Secondly, it notes that many leaf states can never be extended (by adding x) to a word that is already in the suffix automaton;
    // for example, the state corresponding to the longest word so far certainly can't, as all words expressed in the automaton
    // must be strictly shorter than that word! Thus, we don't "really" have to bump the end index for these states:
    // we just mark the end index as ∞ (I actually use "-1" in my implementation) which has the implicit meaning "imagine we
    // bumped the end index by one for each new letter, but didn't really do it!".
    //
    // For a full understanding, I recommend studying Ukkonnen's original paper, but suffice to say that the naive algorithm
    // coupled with these two optimisations gives us a linear-time, online algorithm for computing the automaton representing each
    // successive prefix P of s, right up to s itself.
    //
    // The algorithm, with these optimisations, looks a little like this:
    //   For each letter x of s:
    //     Skip M suffix links for some known M (in my implementation, M == m_numSuffixLinksTraversed) so we end up in some state S;
    //     these links correspond to the leaf states whose end index is implicitly bumped.
    //     Repeat this:
    //       Check if adding the new letter x at S would lead to a word already contained in the automaton:
    //         - If yes, update M, and break (i.e. start handling the next x of S).
    //         - If no, create a new leaf state S' with a transition leading from S to S', with the end index set to ∞,
    //           and set S to S->suffixLink.
    //
    // So for each letter, we *implicitly* (not actually!) add x to M leaf states, essentially adding M new words to the list
    // of substrings contained in the suffix automaton; then we create some number (call it K) of new leaf states, each of 
    // which represents a new word being added to the list of substrings contained in the suffix automaton.
    // So, if we wanted, at each x in s, to know the number of substrings contained in the automaton, we might do something
    // like this:
    //
    //    numLeafStates = 0
    //    numSubstrings = 0
    //    For each letter x of s:
    //       numSubstrings += M
    //       incorporate x into suffix automaton, as described earlier;
    //       numSubstrings += number of new leaf states just added.      
    //       numLeafStates += number of new leaf states just added.
    //
    // As it happens, M + number of new leaf states just added is precisely equal to the current numLeafStates, so we can simplify:
    //
    //    numLeafStates = 0
    //    numSubstrings = 0
    //    For each letter x of s:
    //       incorporate x into suffix automaton, as described earlier;
    //       numLeafStates += number of new leaf states just added.
    //       numSubstrings += numLeafStates.      
    //
    // so this tells us how to answer the question in O(|s|) for the non-pseudo-isomorphic case.
    //
    // So, how does pseudo-isomorphism come into things?
    //
    // Firstly, note that two strings s = s1s2...sn and t = t1t2...tn where si, ti belong to the alphabet Σ are pseudo-isomorphic if and only if there is 
    // some permutation π: Σ -> Σ such that π(s1)π(s2)...π(sn) = t1t2...tn and that, as a consequence, the relation of pseudo-isomorphism
    // is an equivalence relation over the set of strings over Σ.   Now, if R is an equivalence class, then we can normalise each word
    // w in R in such a way that, if isoNormaliseString(w) is the normalisation procedure, then w, w' are in R (i.e. are pseudo-isomorphic)
    // if and only if isoNormaliseString(w) == isoNormaliseString(w').  We define isoNormaliseString(w) as follows:
    //
    //  Let π be an (initially empty) permutation π: Σ -> Σ.
    //  For each successive letter x in w:
    //   if π(x) is not yet defined, define π(x) to be the first letter in the alphabet that is not yet a target of a mapping from π.
    //   add π(x) to the result.
    //
    // For example, if w = xklxl:
    //  first letter is x; π is empty, so doesn't map x to anything.  The first letter in the alphabet that is not yet a target of a mapping from
    //   π is a; therefore, set π(x) = a.  Add π(x) = a to the output; output is now 'a'.
    //  next letter is k; π(k) is not yet defined.  The first letter in the alphabet that is not yet a target of a mapping from
    //   π is now b (as we just set 'a' to be the target of a mapping from 'x'), so set π(k) = b and add π(k) to the output; output is now 'ab'.
    //  next letter is l, again not yet mapped to anything: set π(l) to next unused letter = c, and add π(l) = c to the output, output is now 'abc'.
    //  next letter is k, and we defined π(k) = b earlier; add π(k) to output; output is now 'abcb'.
    //  next letter is x, and we defined π(x) = a earlier; add π(x) to output; output is now 'abcba'
    //  next letter is l, and we defined π(l) = c earlier; add π(l) to output; output is now 'abcbac'
    //
    // Thus, isoNormaliseString(xklxl) = abcbac, and in normalising xklxl, we have implicitly generated a permutation π which maps x -> a, 
    // k -> b, l -> c.
    //
    // Now, if we could, for each successive prefix P of s, construct an automaton that represented precisely the set of isoNormaliseString(w) where
    // w is a substring of P, and if we could easily calculate the number of strings represented by this automaton, we'd be done: each string
    // represented by the automaton would be non-pseudo-isomorphic to each other string represented by the automaton.
    //
    // Let's change tack slightly and go back to creating the lists of words in each P by adding each new letter to the list of words to
    // in the previous P, except this time, instead of the list of raw words, we'll store isonormalised versions of the words.  Let S = xkxl.
    //
    // P = x:
    //  a (0) (original word: x).
    // P = xk:
    //  a (1) (original word: k).
    //  ab (0) (original word: xk).
    // P = xkx:
    //  a (2) (original word: x)
    //  ab (1) (original word: kx).
    //  aba (0) (original word: xkx).
    // P = xkxl:
    //  a (3) (original word: l)
    //  ab (2) (original word: xl)
    //  abc (1) (original word: kxl).
    //  abac (0) (original word: xkxl).
    //
    // This gives us the complete list of isonormalised (and thus, mutually non-pseudo-ismorphic) words substrings of each P using much the same 
    // algorithm as we used to find distinct substrings of each P ignoring pseudo-ismorphism.  In fact, we can use (almost!) the *exact same* insights
    // as Ukkonnen did in the non-isomorphic case to create an online, O(|s|) algorithm for creating a suffix automaton representing the iso-normalised
    // substrings of each P, *and* use the exact same procedure (tracking the number of leaf nodes after adding each letter of s) to calculate, for each
    // P, the *number* of distinct iso-normalised substrings of P!  The differences from standard Ukkonnen are fairly minor; let's examine them.
    //
    // Note that when adding the new letter x to the original word w (labelled (k), say) and renormalising, the new word (still labelled (k)) does not
    // change in the first |w| letters; that is, isoNormaliseString(w) and isoNormaliseString(wx) share the same first |w| letters.
    // So we don't really need to re-normalise: instead, we can track the current permutation π of the alphabet induced by the word labelled (k), and
    // when we add x to it, update the permutation of the word labelled (k) if it doesn't already map x, and then add π(x) to isoNormaliseString(w)
    // to efficiently give us isoNormaliseString(wx).  For example, let's go from P = xkx to xkxl again, this time tracking the permutations:
    // P = xkx:
    //  a (2) (original word: x; permutation x -> a)
    //  ab (1) (original word: kx; permutation k -> a, x -> b).
    //  aba (0) (original word: xkx; permutation x -> a, k -> b).
    // P = xkxl; adding 'l'.
    //  a (3) (original word: l; permutation l -> a)
    //  ab (2) (original word: xl; permutation x -> a, l -> b)
    //  abc (1) (original word: kxl; permutation k -> a, x -> b, l -> c).
    //  abac (0) (original word: xkxl; permutation x -> a, k -> b, l -> c) [l wasn't in π, so we updated π with π(l) = c, and added π(l) to aba to give abac].
    //
    // 'l' hadn't been seen before, so we had to augment each permutation with it.  In our implementation, permutations of letters are stored as LetterPermutation's.
    //
    // As mentioned, the word labelled (k) ultimately corresponds to the suffix s.subtr(k).  Here, it also has a LetterPermutation that is 
    // added to or left alone (but not "changed") with each successive letter; i.e. every suffix of s has its own LetterPermutation.
    // This LetterPermutation for each suffix is one of the major enhancements to Ukonnen needed to get it working with pseudo-isomorphism and
    // in my implementation is represented by m_isoNormalisedSuffixPermutations and is crucial to the workings of Ukkonnen's canonize() and
    // testAndSplit() - these both involve answering a question along the lines of "I have an iso-normalised suffix of s, and need to extend
    // it by following some transition beginning with some raw letter x: which one do I follow so that I am still following
    // the same iso-normalised suffix of s?".  *Which* iso-normalised suffix of s we are currently following is determined by how many
    // suffix links we have followed so far: this is stored in m_numSuffixLinksTraversed and we see frequent references to
    // m_isoNormalisedSuffixPermutations[m_numSuffixLinksTraversed] throughout the code.  This is all rather hard to explain, sadly:
    // a good understanding of the original Ukkonnen helps, here, but it's still a bit of a pig :/
    // Another change to Ukkonnen's algorithm is that each transition, in addition to a start index and end index, now must
    // have a LetterPermutation added to it; without this, it is probably impossible to have a suffix automaton that
    // represents the set of isoNormalised substrings of s.  The final important change is that, in Ukkonnen's original algorithm,
    // when we needed to follow a suffix link from a state, we could depend on it having already been computed; for reasons that
    // are not 100% clear to me, this is not the case in the pseudo-isomorphic variant, so a addSuffixLink() method needed to be added.
    // 
    // That's about it - clear as mud, I'm afraid :/ As a brief re-cap: Ukkonnen's method takes a very simple-minded, O(|s|^2) algorithm
    // for online computation of a suffix automaton representing a string s (where each new letter in x is added, by traversing
    // suffix links starting with the longest suffix in the automaton, to each suffix already represented by the automaton) and adds a couple
    // of optimisations that bring this done to O(|s|), and also allows us to easily compute the number of distinct words represented by 
    // the automaton simply by tracking the number of "leaf" states in the automaton.
    // As it happens, using the same simple-minded algorithm but this time computing a suffix automaton that represents the *isonormalised*
    // substrings of s rather than the raw substrings of is amenable to the same optimisations and can be made O(|s|) and, further,
    // enables us to compute the number of distinct isonormalised substrings using exactly the same procedure.  The changes to Ukkonnen's algorithm
    // include tracking the number of suffixes traversed so that we know the LetterPermutation corresponding to the current 
    // suffix of s we are extending by x and incorporating LetterPermutation's into each transition we add.   Using this modified 
    // version of Ukkonnen's algorithm, we can find the number of isonormalised substrings of each prefix of s by calling numNonPseudoIsomorphicSubstrings(),
    // which is exactly what we want.

    // * This isn't strictly true; in an efficient Suffix Automaton, there is not a one-to-one correspondence between words in s
    // and states (there would be O(|s|^2) states!) - Ukonnen uses "reference pairs" to get around this.  For simplicity, though, let's 
    // assume that there is a one-to-one correspondence between words and states.  Additionally, we are ignoring for now
    // what happens if adding x to a word w leads to a word already in the automaton.
    string s;
    cin >> s;

    PseudoIsomorphicSuffixTree treeBuilder;
    for (const auto letter : s)
    {
        treeBuilder.appendLetter(letter);
        cout << treeBuilder.numNonPseudoIsomorphicSubstrings() << endl;
    }
}

