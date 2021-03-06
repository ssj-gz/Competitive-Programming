// Simon St James (ssjgz) - 2019-08-25
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <map>
#include <limits>

#include <cassert>

using namespace std;

constexpr int alphabetSize = 26;

/**
 * Simple implementation of Ukkonen's algorithm:
 *  https://en.wikipedia.org/wiki/Ukkonen's_algorithm
 * for online construction of suffix trees.
 * @author Simon St James, Jan 2017.
 */
class SuffixTree
{
    public:
        SuffixTree()
        {
            m_states.reserve(1'000'000);

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
        SuffixTree(const SuffixTree& other) = delete;
        void appendString(const string& stringToAppend)
        {
            for (auto letter : stringToAppend)
            {
                appendLetter(letter);
            }
        }
        void finalise()
        {
            // Set the correct value for transitions whose ends are openTransitionEnd, and make
            // startIndex/ endIndex 0-relative.
            for (auto& state : m_states)
            {
                for (auto& transition : state.transitions)
                {
                    if (transition.substringFollowed.endIndex == openTransitionEnd)
                        transition.substringFollowed.endIndex = m_currentString.size();

                    transition.substringFollowed.startIndex--;
                    transition.substringFollowed.endIndex--;
                }
            }

        }

        struct State;
        State* rootState() const
        {
            return m_root;
        }
        struct Substring
        {
            Substring(int startIndex, int endIndex)
                : startIndex(startIndex), endIndex(endIndex)
            {
            }

            int startIndex = -1;
            int endIndex = -1;
        };
        struct Transition
        {
            Transition(State *nextState, const Substring& substringFollowed, const string& currentString)
                : nextState(nextState), substringFollowed(substringFollowed)
            {
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
    private:
        static const int openTransitionEnd = numeric_limits<int>::max();

        string m_currentString;

        vector<State> m_states;
        State *m_root = nullptr;
        State *m_auxiliaryState = nullptr;

        // "Persistent" versions of s & k from Algorithm 2 in Ukkonen's paper!
        State *m_s; 
        int m_k;

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

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

// Represents the query: "how many k, startIndex <= k <= endIndex, have
// at most 1 bit set in the binary representation of prefixXorSumLookup[k] ^ baseXor?"
struct XorSumRangeQuery
{
    int startIndex = -1;
    int endIndex = -1;

    uint32_t baseXor = 0;

    int answerForQuery = 0;
};

void buildXorSumRangeQueries(SuffixTree::State* state, uint32_t substringXorSumSoFar, const vector<int>& prefixXorSumLookup, vector<XorSumRangeQuery>& queries)
{
    for (const auto& transition : state->transitions)
    {
        uint32_t newSubstringXorSum = substringXorSumSoFar;
        newSubstringXorSum = newSubstringXorSum ^ (prefixXorSumLookup[transition.substringFollowed.endIndex]);

        uint32_t queryBaseXor = substringXorSumSoFar;
        if (transition.substringFollowed.startIndex > 0)
        {
            newSubstringXorSum = newSubstringXorSum ^ prefixXorSumLookup[transition.substringFollowed.startIndex - 1];
            queryBaseXor = queryBaseXor ^ prefixXorSumLookup[transition.substringFollowed.startIndex - 1];
        }

        queries.push_back({transition.substringFollowed.startIndex, transition.substringFollowed.endIndex, queryBaseXor});

        // Recurse via this transition.
        buildXorSumRangeQueries(transition.nextState, newSubstringXorSum, prefixXorSumLookup, queries);
    }

}

int64_t findDistinctAnagramPalindromeSubstrings(const string& s)
{
    vector<int> valuesWithAtMost1BitSet = { 0 };
    for (int bit = 0; bit < alphabetSize; bit++)
    {
        valuesWithAtMost1BitSet.push_back(1 << bit);
    }

    int64_t result = 0;
    SuffixTree suffixTree;
    suffixTree.appendString(s);
    suffixTree.finalise();

    vector<int> prefixXorSumLookup;
    uint32_t prefixXorSum = 0;
    for (const auto letter : s)
    {
        prefixXorSum = prefixXorSum ^ (1 << (letter - 'a'));
        prefixXorSumLookup.push_back(prefixXorSum);
    }

    vector<XorSumRangeQuery> queries;
    buildXorSumRangeQueries(suffixTree.rootState(), 0, prefixXorSumLookup, queries);

    vector<vector<XorSumRangeQuery*>> queriesBeginningAtIndex(s.size());
    vector<vector<XorSumRangeQuery*>> queriesEndingAtIndex(s.size());
    for (auto& query : queries)
    {
        queriesBeginningAtIndex[query.startIndex].push_back(&query);
        queriesEndingAtIndex[query.endIndex].push_back(&query);
    }

    map<uint32_t, int> numPrefixesWithXorSum;

    numPrefixesWithXorSum.clear();
    numPrefixesWithXorSum[0] = 1; // Empty prefix.

    // Sweep from left to right, preparing queries that start at the current index;
    // updating numPrefixesWithXorSum; and finally answering queries that end at the
    // current index.
    for (int i = 0; i < s.size(); i++)
    {
        for (auto startQuery : queriesBeginningAtIndex[i])
        {
            for (const auto atMost1BitSet : valuesWithAtMost1BitSet)
            {
                const auto numPrefixesIter = numPrefixesWithXorSum.find(startQuery->baseXor ^ atMost1BitSet);
                if (numPrefixesIter != numPrefixesWithXorSum.end())
                {
                    startQuery->answerForQuery -= numPrefixesIter->second;
                }
            }
        }

        numPrefixesWithXorSum[prefixXorSumLookup[i]]++;

        for (auto endQuery : queriesEndingAtIndex[i])
        {
            for (const auto atMost1BitSet : valuesWithAtMost1BitSet)
            {
                const auto numPrefixesIter = numPrefixesWithXorSum.find(endQuery->baseXor ^ atMost1BitSet);
                if (numPrefixesIter != numPrefixesWithXorSum.end())
                {
                    endQuery->answerForQuery += numPrefixesIter->second;
                }
            }
        }
    }

    for (const auto& query : queries)
    {
        result += query.answerForQuery;
    }

    return result;
}


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    
    const int T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const string s = read<string>();
        cout << findDistinctAnagramPalindromeSubstrings(s) << endl;
    }

    assert(cin);
}
