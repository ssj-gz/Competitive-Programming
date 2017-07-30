#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <cmath>
#include <cstdio>
#include <vector>
#include <deque>
#include <list>
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
            bool isFinal = false; // Only holds correct value if "makeFinalStatesExplicitAndMarkThemAsFinal" is called.
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
            // Trick described in Ukkonen's paper - add an unused character - '{', here, as it's 'z' + 1:
            // then all final states will become explicit.  Obviously, doesn't work if the input string
            // has actually used '{'!
            // We can then delete all '{' transitions, and make explicit states that had a '{' transition
            // final, and all states that were originally followed by a transition ending in '{' final.

            const char unusedLetter = '{';
            appendLetter(unusedLetter);

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
                bool isOnExplicitState() const
                {
                    return (m_transition == nullptr);
                }
                bool isOnFinalState() const
                {
                    assert(isOnExplicitState());
                    return m_state->isFinal;
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
                        bool hasNext()
                        {
                            return transitionIterator != endtransitionIterator;
                        }
                        char nextLetter()
                        {
                            return (*(cursor->m_string))[transitionIterator->substringFollowed.startIndex - 1];
                        }
                        Cursor afterFollowingNextLetter()
                        {
                            Cursor afterCursor(*cursor);
                            afterCursor.enterTransitionAndSkipLetter(*transitionIterator);
                            transitionIterator++;
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
                        const Cursor* cursor;
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
        };
        Cursor rootCursor() const
        {
            return Cursor(m_root, m_currentString, m_root);
        }
    private:
        static const int alphabetSize = 27; // Include the magic '{' for making final states explicit - assumes the input string has no '{''s, obviously!
        static const int openTransitionEnd = numeric_limits<int>::max();

        string m_currentString;

        deque<State> m_states;
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


struct GeneContribution
{
    GeneContribution(int geneIndex, long health)
        : geneIndex{geneIndex}, health{health}
    {
    }
    int geneIndex = -1;
    long health = 0;
};

class GeneContributionCalculator
{
    public:
        void addGeneContribution(const GeneContribution& geneContribution)
        {
            if (m_geneContributions.empty())
            {
                m_geneContributions.push_back(GeneContribution(-1, 0)); // Sentinel (lower).
            }
            m_geneContributions.push_back(geneContribution);
        };
        void buildLookup()
        {
            if (m_geneContributions.empty())
                return;
            // NB: m_geneContributions will be sorted in order of geneIndex at this point (that's the way the data is read in!)
            // A bit ugly, but build up a table of cumulative health, so that m_geneContributions[i].health represents
            // the total health of all genes in m_geneContributions with geneIndex < m_geneContributions[i].geneIndex.
            // We re(and mis-)use m_geneContributions for this lookup table as otherwise we risk running out of memory!
            long cumulativeHealth = 0;
            for (auto geneContribIter = m_geneContributions.begin() + 1; geneContribIter != m_geneContributions.end(); geneContribIter++)
            {
                const long oldHealth = geneContribIter->health;
                geneContribIter->health = cumulativeHealth;
                cumulativeHealth += oldHealth;
            }
            m_geneContributions.push_back(GeneContribution(numeric_limits<int>::max(), cumulativeHealth)); // Sentinel (upper).
        };
        long calculateGeneContribution(int geneStartIndex, int geneEndIndex) const
        {
            if (m_geneContributions.empty())
                return 0;
            // Fairly standard prefix-sum type approach.
            auto lower = lower_bound(m_geneContributions.begin(), m_geneContributions.end(), GeneContribution(geneStartIndex, -1), geneIndexComparator);
            auto upper = lower_bound(m_geneContributions.begin(), m_geneContributions.end(), GeneContribution(geneEndIndex, -1), geneIndexComparator);
            if (upper->geneIndex == geneEndIndex)
            {
                upper++;
            }
            const long totalGeneContribution = upper->health - lower->health;
            return totalGeneContribution;
        }
    private:
        vector<GeneContribution> m_geneContributions;
        static bool geneIndexComparator(const GeneContribution& lhs, const GeneContribution& rhs)
        {
            return lhs.geneIndex < rhs.geneIndex;
        }

};

class TrieThing
{
    public:
        TrieThing()
        {
            m_nodes.push_back(Node());
            m_nodes.reserve(1'000'000);
        }
    private:
        struct Node;
    public:
        class NodeRef
        {
            public:
                NodeRef(const NodeRef& other)
                    : m_nodeIndex(other.m_nodeIndex),
                    m_posInUnsplitString(other.m_posInUnsplitString),
                    m_nodes(other.m_nodes)

            {

            }
                NodeRef()
                    : m_nodeIndex(-1),
                    m_posInUnsplitString(-1)
            {
            }
                bool isValid() const
                {
                    return m_nodeIndex != -1;
                }
                bool hasData() const
                {
                    if (!isValid())
                        return false;
                    const Node* node = &((*m_nodes)[m_nodeIndex]);
                    return (node->isSplit || node->unsplitString.empty());
                }
                vector<char> followableLetters() const
                {
                    vector<char> followableLetters;
                    if (!isValid())
                        return followableLetters;
#if 1
                    const Node* node = &((*m_nodes)[m_nodeIndex]);
                    if (node->isSplit)
                    {
                        for (const auto& nextNode : node->nodeIdAfterLetterIndex)
                        {
                            const char letter = 'a' + nextNode.first;
                            if (letter >= 'a' && letter <= 'z')
                                followableLetters.push_back(letter);
                        }
                    }
                    else
                    {
                        if (m_posInUnsplitString < node->unsplitString.size())
                        {
                            const char letter = node->unsplitString[m_posInUnsplitString];
                            if (letter >= 'a' && letter <= 'z')
                                followableLetters.push_back(letter);
                        }
                        else
                        {
                            NodeRef skipped(node->nextNodeIndex, 0, m_nodes);
                            return skipped.followableLetters();
                        }
                    }
#endif

                    return followableLetters;
                }
                NodeRef afterFollowingLetter(const char letter)
                {
                    assert(isValid());
#if 1
                    const Node* node = &((*m_nodes)[m_nodeIndex]);
                    if (node->isSplit)
                    {
                        const int letterIndex = letter - 'a';
                        for (const auto& nextNode : node->nodeIdAfterLetterIndex)
                        {
                            if (nextNode.first == letterIndex)
                            {
                                const int nextNodeIndex = nextNode.second;
                                return NodeRef(nextNodeIndex, 0, m_nodes);
                            }
                        }
                        assert(false);
                    }
                    else
                    {
                        NodeRef after = *this;
                        bool skipped = false;
                        while (after.m_posInUnsplitString >= node->unsplitString.size() && !node->isSplit)
                        {
                            const int nextNodeIndex = node->nextNodeIndex;
                            assert(nextNodeIndex != -1);
                            node = &((*m_nodes)[nextNodeIndex]);
                            after.m_nodeIndex = nextNodeIndex;
                            after.m_posInUnsplitString = 0;
                            skipped = true;
                        }
                        if (skipped)
                        {
                            return after.afterFollowingLetter(letter);
                        }
                        assert(after.m_posInUnsplitString < node->unsplitString.size());
                        assert(node->unsplitString[after.m_posInUnsplitString] == letter);
                        if (after.m_posInUnsplitString < node->unsplitString.size() - 1)
                        {
                            return NodeRef(after.m_nodeIndex, after.m_posInUnsplitString + 1, m_nodes);
                        }
                        else
                        {
                            if (node->nextNodeIndex != -1)
                                return NodeRef(node->nextNodeIndex, 0, m_nodes);
                            else
                                return NodeRef();
                        }
                    }
#endif
                    assert(false);
                    return NodeRef();

                }
            private:
                int m_nodeIndex;
                // Only used for unsplit nodes.
                int m_posInUnsplitString;

                const vector<Node>* m_nodes = nullptr;

                NodeRef(int nodeIndex, int posInUnsplitString, const vector<Node>* nodes)
                    : m_nodeIndex(nodeIndex),
                    m_posInUnsplitString(posInUnsplitString),
                    m_nodes(nodes)
            {
            }
                friend class TrieThing;

                int nodeIndex() const
                {
                    return m_nodeIndex;
                }
                int posInUnsplitString() const
                {
                    return m_posInUnsplitString;
                }
        };
        NodeRef startNode() const
        {
            NodeRef startNodeRef (0, m_nodes[0].isSplit ? -1 : 0, &m_nodes) ;
            return startNodeRef;
        }
        NodeRef addString(const string& stringToAdd)
        {
            return addString(stringToAdd, 0, stringToAdd.size());
        }
        NodeRef addString(const string& stringToAdd, string::size_type pos, string::size_type count)
        {
            {
                // Clumsy hack to make sure that following stringToAdd leads to a Node that is 
                // not reachable by any other path i.e. that stringToAdd has "all to itself".
                // We will store gene information in this Node.
                NodeRef startNodeRef = startNode();
                string hackString(stringToAdd.substr(pos, count));
                hackString.push_back('|');
                startNodeRef = startNode();
                addSuffix(startNodeRef, hackString, 0, hackString.size());
                hackString.back() = '{';
                startNodeRef = startNode();
                addSuffix(startNodeRef, hackString, 0, hackString.size());
                hackString.pop_back();
                hackString.back() = '|';
                startNodeRef = startNode();
                addSuffix(startNodeRef, hackString, 0, hackString.size());
                hackString.back() = '{';
                startNodeRef = startNode();
                addSuffix(startNodeRef, hackString, 0, hackString.size());
            }
            NodeRef startNodeRef(startNode());
            return addSuffix(startNodeRef, stringToAdd, pos, count);
        }

        void buildGeneContributionLookups()
        {
            for (auto& node : m_nodes)
            {
                node.data.buildLookup();
            }
        }

        GeneContributionCalculator& data(const NodeRef& nodeRef)
        {
            Node* node = &(m_nodes[nodeRef.m_nodeIndex]);
            assert(nodeRef.hasData());
            return node->data;
        }
        const GeneContributionCalculator& data(const NodeRef& nodeRef) const
        {
            const Node* node = &(m_nodes[nodeRef.m_nodeIndex]);
            assert(nodeRef.hasData());
            return node->data;
        }

    private:
        static const int numLetters = 26;
        struct Node
        {
            bool isSplit = false;
            // Used if and only if not isSplit.
            string unsplitString;
            int nextNodeIndex = -1;

            // Used if and only if isSplit.
            vector<pair<int, int>> nodeIdAfterLetterIndex;

            GeneContributionCalculator data;
        };
        vector<Node> m_nodes;
        NodeRef addSuffix(NodeRef& prefixNodeRef, const string& suffix)
        {
            return addSuffix(prefixNodeRef, suffix, 0, suffix.size());
        }
        NodeRef addSuffix(NodeRef& prefixNodeRef, const string& suffix, string::size_type pos, string::size_type count)
        {
            int nodeIndex = prefixNodeRef.nodeIndex();
            int posInUnsplitString = prefixNodeRef.posInUnsplitString();
            Node* node = &(m_nodes[nodeIndex]);
            auto createNewNode = [&m_nodes = this->m_nodes]()
            {
                const bool exceededCapacity = m_nodes.size() == m_nodes.capacity();
                if (exceededCapacity)
                {
                    assert(false && "Exceeding vector capacity not supported!");
                }
                m_nodes.emplace_back();
                Node *newNode = &(m_nodes.back());
                return newNode;
            };
            while (count > 0)
            {
                if (node->isSplit)
                {
                    int nextNodeIndex = -1;
                    posInUnsplitString = -1;
                    const auto letter = suffix[pos];
                    const int letterIndex = letter - 'a';
                    assert(letterIndex >= 0 && letterIndex < 28);
                    for (const auto& x : node->nodeIdAfterLetterIndex)
                    {
                        if (x.first == letterIndex)
                        {
                            nextNodeIndex = x.second;
                            break;
                        }
                    }
                    if (nextNodeIndex == -1)
                    {
                        // Create new unsplit node with the remainder of the string.
                        Node *newNode = createNewNode();
                        newNode->unsplitString = suffix.substr(pos + 1, count);
                        nextNodeIndex = (m_nodes.size() - 1);
                        node->nodeIdAfterLetterIndex.push_back(make_pair(letterIndex, nextNodeIndex));
                        nodeIndex = nextNodeIndex;
                        posInUnsplitString = count - 1;
                        break;
                    }
                    else
                    {
                        pos++;
                        count--;
                        node = &(m_nodes[nextNodeIndex]);
                        nodeIndex = nextNodeIndex;
                        continue;
                    }

                }
                else
                {
                    if (posInUnsplitString == -1)
                        posInUnsplitString = 0;
                    if (posInUnsplitString == node->unsplitString.size())
                    {
                        if (node->nextNodeIndex == -1)
                        {
                            // Append the remaining to the end: we're done.
                            node->unsplitString += suffix.substr(pos, count);
                            posInUnsplitString = node->unsplitString.size();
                            break;
                        }
                        else
                        {
                            const int nextNodeIndex = node->nextNodeIndex;
                            node = &(m_nodes[nextNodeIndex]);
                            nodeIndex = nextNodeIndex;
                            posInUnsplitString = 0;
                            continue;
                        }
                    }
                    else
                    {
                        const size_t remainingInUnsplitString = node->unsplitString.size() > posInUnsplitString ? node->unsplitString.size() - posInUnsplitString : 0;
                        const int maxComparableLength = min(remainingInUnsplitString, count);
                        const char* nodeString = &(node->unsplitString[posInUnsplitString]);
                        const char* suffixToAdd = &(suffix[pos]);
                        int differentIndex = -1;
                        for (int i = 0; i < maxComparableLength; i++)
                        {
                            if (nodeString[i] != suffixToAdd[i])
                            {
                                differentIndex = i;
                                break;
                            }
                        }
                        if (differentIndex == -1)
                        {
                            posInUnsplitString += maxComparableLength;
                            count -= maxComparableLength;
                            pos += maxComparableLength;
                            continue;
                        }
                        else
                        {
                            // Strings are different - need to create new node.
                            assert(node->unsplitString.size() > 0);

                            Node *splitterNode = nullptr;
                            int splitterNodeId = -1;
                            if (posInUnsplitString + differentIndex == 0)
                            {
                                // Current node is the split node.
                                splitterNode = node;
                                splitterNodeId = nodeIndex;
                            }
                            else
                            {
                                // Create new splitter node.
                                splitterNode = createNewNode();
                                splitterNodeId = (m_nodes.size() - 1);
                            }
                            splitterNode->isSplit = true;

                            // Break out a node representing the old string and splice it into place.
                            Node* continuationNode = createNewNode();
                            const int continuationNodeId = (m_nodes.size() - 1);
                            continuationNode->unsplitString = (posInUnsplitString + differentIndex + 1) < node->unsplitString.size() ? node->unsplitString.substr(posInUnsplitString + differentIndex + 1) :"";
                            continuationNode->nextNodeIndex = node->nextNodeIndex;

                            // Add the remainder of the new suffix to a new, unsplit node.
                            Node* newUnSplitNode = createNewNode();
                            const int newUnSplitNodeId = (m_nodes.size() - 1);
                            newUnSplitNode->unsplitString = suffix.substr(pos + differentIndex + 1, count - differentIndex - 1);

                            node->nextNodeIndex = splitterNodeId;

                            assert(differentIndex < node->unsplitString.size());
                            assert(pos + differentIndex < suffix.size());
                            splitterNode->nodeIdAfterLetterIndex.push_back(make_pair(node->unsplitString[posInUnsplitString + differentIndex] - 'a', continuationNodeId));
                            splitterNode->nodeIdAfterLetterIndex.push_back(make_pair(suffix[pos + differentIndex] - 'a', newUnSplitNodeId));
                            splitterNode->nextNodeIndex = -1;

                            node->unsplitString = node->unsplitString.substr(0, posInUnsplitString + differentIndex);

                            if (prefixNodeRef.nodeIndex() == nodeIndex && prefixNodeRef.posInUnsplitString() >= posInUnsplitString + differentIndex)
                            {
                                if (prefixNodeRef.posInUnsplitString() == posInUnsplitString + differentIndex)
                                {
                                    prefixNodeRef = NodeRef(splitterNodeId, -1, &m_nodes);
                                }
                                else
                                {
                                    prefixNodeRef = NodeRef(continuationNodeId, prefixNodeRef.posInUnsplitString() - (posInUnsplitString + differentIndex + 1), &m_nodes);
                                }
                            }
                            nodeIndex = newUnSplitNodeId;
                            posInUnsplitString = newUnSplitNode->unsplitString.size();
                            break;

                        }
                    }
                }
                assert(false);
            }
            if (!node->isSplit && posInUnsplitString == -1)
                posInUnsplitString = 0;
            return NodeRef(nodeIndex, posInUnsplitString, &m_nodes);
        }
};

const TrieThing* geneTrie;
int geneStartIndex;
int geneEndIndex;
long totalHealth = 0;
string strand;

void computeStrandHealthAux(TrieThing::NodeRef geneTrieCursor, SuffixTreeBuilder::Cursor strandCursor, long healthToAddAtFinalStates)
{
    const TrieThing& geneTrie = *::geneTrie;

    auto calcGeneContributionToHealth = [](const TrieThing::NodeRef& geneTrieCursor)
    {
        // TODO - optimise this using a kind of "prefix sum" technique.
        long healthContribution = 0;
        if (geneTrieCursor.hasData())
        {
            healthContribution += ::geneTrie->data(geneTrieCursor).calculateGeneContribution(geneStartIndex, geneEndIndex);
        }
        return healthContribution;
    };

    if (strandCursor.isOnExplicitState())
    {
        if (strandCursor.isOnFinalState())
        {
            totalHealth += healthToAddAtFinalStates;
        }

        const vector<char> lettersFollowableInGeneTrie = geneTrieCursor.followableLetters();
        SuffixTreeBuilder::Cursor::NextLetterIterator nextStrandLetterIterator = strandCursor.getNextLetterIterator();

        while (nextStrandLetterIterator.hasNext())
        {
            const char letter = nextStrandLetterIterator.nextLetter();
            TrieThing::NodeRef nextGeneTrieCursor;
            long nextHealthToAdd = healthToAddAtFinalStates;
            const bool letterIsFollowableInGeneTrie = (find(lettersFollowableInGeneTrie.begin(), lettersFollowableInGeneTrie.end(), letter) != lettersFollowableInGeneTrie.end());
            if (letterIsFollowableInGeneTrie)
            {
                nextGeneTrieCursor = geneTrieCursor.afterFollowingLetter(letter);
                nextHealthToAdd += calcGeneContributionToHealth(nextGeneTrieCursor);
            }

            SuffixTreeBuilder::Cursor nextStrandCursor = nextStrandLetterIterator.afterFollowingNextLetter();
            computeStrandHealthAux(nextGeneTrieCursor, nextStrandCursor, nextHealthToAdd);
        }
    }
    else
    {
        SuffixTreeBuilder::Cursor nextStrandCursor(strandCursor);
        TrieThing::NodeRef nextGeneTrieCursor(geneTrieCursor);
        long nextHealthToAdd = healthToAddAtFinalStates;
        while (!nextStrandCursor.isOnExplicitState() && nextGeneTrieCursor.isValid())
        {
            const vector<char> lettersFollowableInStrand = nextStrandCursor.nextLetters();
            const vector<char> lettersFollowableInGeneTrie = nextGeneTrieCursor.followableLetters();

            const char nextLetter = lettersFollowableInStrand.front();

            const bool letterIsFollowableInGeneTrie = (find(lettersFollowableInGeneTrie.begin(), lettersFollowableInGeneTrie.end(), nextLetter) != lettersFollowableInGeneTrie.end());
            if (letterIsFollowableInGeneTrie)
            {
                nextGeneTrieCursor = nextGeneTrieCursor.afterFollowingLetter(nextLetter);
                nextHealthToAdd += calcGeneContributionToHealth(nextGeneTrieCursor);
            }
            else
            {
                nextGeneTrieCursor = TrieThing::NodeRef();
            }

            nextStrandCursor.followNextLetter();
        }
        if (!nextStrandCursor.isOnExplicitState())
        {
            nextStrandCursor.followToTransitionEnd();
        }
        computeStrandHealthAux(nextGeneTrieCursor, nextStrandCursor, nextHealthToAdd);
    }
}

long computeStrandHealth(const TrieThing& geneTrie, const string& strand, int geneStartIndex, int geneEndIndex)
{
    SuffixTreeBuilder strandSuffixTree;
    strandSuffixTree.appendString(strand);
    strandSuffixTree.makeFinalStatesExplicitAndMarkThemAsFinal();
    ::geneStartIndex = geneStartIndex;
    ::geneEndIndex = geneEndIndex;
    ::strand = strand;
    ::geneTrie = &geneTrie;

    totalHealth = 0;
    computeStrandHealthAux(geneTrie.startNode(), strandSuffixTree.rootCursor(), 0);
    return ::totalHealth;
}

int main() {
    // We're clearly expected to use the Aho-Corasick algorithm here, but I thought it might be interesting to
    // try and use my existing TrieThing and SuffixTreeBuilder to solve it! :)
    // Pro-tip: it wasn't.
    // Should all be fairly explanatory: stick the Genes in a TrieThing in such a way that we can retrieve
    // the "contributions" (gene index + health) for each Gene i.e. if we have genes
    //  0 3 aa
    //  1 5 ab
    //  2 7 aa
    // then following aa in the tree will give us our pairs (0, 3) & (2, 7).
    // Stick the strand in a suffix tree and follow it letter-by-letter, also following the Genes in the Trie "in tandem".
    // Let w be the string followed in the suffix tree so far: if w is in the Gene Trie, grab the contributions to the health
    // from the genes corresponding to w and accumulate them in a healthToAddAtFinalStates variable.
    // When we actually reach a final state, add the accumulated health to totalHealth: at this point, we've "really" found
    // an instance of the genes corresponding to prefixes of w in the strand.
    // We can start skipping down transitions if we cannot follow further letters of w in the Gene Trie.
    // That's about it - was quite hard as making the final states explicit and marking them as final was very
    // slow originally, but is now much faster due to a trick mentioned in Ukkonen's paper - see makeFinalStatesExplicitAndMarkThemAsFinal().
    int numGenes;
    cin >> numGenes;
    vector<string> genes(numGenes);
    for (int i = 0; i < numGenes; i++)
    {
        cin >> genes[i];
    }
    vector<long> geneHealths(numGenes);
    for (int i = 0; i < numGenes; i++)
    {
        cin >> geneHealths[i];
    }

    TrieThing genesTrie;
    for (int i = 0; i < numGenes; i++)
    {
        TrieThing::NodeRef geneRef = genesTrie.addString(genes[i]);
        genesTrie.data(geneRef).addGeneContribution(GeneContribution(i, geneHealths[i]));
    }
    genesTrie.buildGeneContributionLookups();

    int numStrands;
    cin >> numStrands;
    long healthiest = std::numeric_limits<long>::min();
    long unhealthiest = std::numeric_limits<long>::max();
    for (int i = 0; i < numStrands; i++)
    {
        int start, end;
        cin >> start >> end;
        string strand;
        cin >> strand;
        const long strandHealth = computeStrandHealth(genesTrie, strand, start, end);
        healthiest = max(healthiest, strandHealth);
        unhealthiest = min(unhealthiest, strandHealth);
    }
    cout << unhealthiest << " " << healthiest << endl;
    return 0;

}


