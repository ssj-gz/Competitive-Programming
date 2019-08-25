// Simon St James (ssjgz) - 2019-08-24
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <limits>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

using namespace std;

struct StateData
{
    int wordLength = -1;
};

/**
 * Simple implementation of Ukkonen's algorithm:
 *  https://en.wikipedia.org/wiki/Ukkonen's_algorithm
 * for online construction of suffix trees.
 * @author Simon St James, Jan 2017.
 */
class SuffixTree
{
    public:
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
        SuffixTree()
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
            finaliseAux(m_root, nullptr);
        }

        State* rootState() const
        {
            return m_root;
        }
    private:
        static const int alphabetSize = 26;
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
        void finaliseAux(State* state, Transition* transitionFromParent)
        {
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

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

int64_t solveBruteForce(const string& s)
{
    int64_t result = 0;
    set<string> seenSubstrings;

    for (int i = 0; i < s.size(); i++)
    {
        for (int j = i; j < s.size(); j++)
        {
            const int numLetters = 26;
            int letterHistogram[numLetters] = {};
            for (int k = i; k <= j; k++)
            {
                letterHistogram[s[k] - 'a']++;
            }
            int numLettersWithOddOccurrence = 0;
            for (int letterIndex = 0; letterIndex < numLetters; letterIndex++)
            {
                if ((letterHistogram[letterIndex] % 2) == 1)
                    numLettersWithOddOccurrence++;
            }
            const string substring = s.substr(i, j - i + 1);
            cout << "substring: " << substring << endl;
            if (numLettersWithOddOccurrence == 0 || numLettersWithOddOccurrence == 1)
            {
                cout << "Palindromic: " << substring << endl;
                if (seenSubstrings.find(substring) == seenSubstrings.end())
                {
                    result++;
                    cout << " new: " << result << " " << substring << endl;
                    seenSubstrings.insert(substring);
                }
                else
                {
                    cout << " already seen" << endl;
                }
            }
        }
    }
    
    return result;
}

uint32_t xorSum(const string& s)
{
    uint32_t xorSum = 0;
    for (const auto letter : s)
    {
        xorSum = xorSum ^ (1 << (letter - 'a'));
    }
    return xorSum;
}

string currentString;

struct XorSumRangeQuery
{
    int startIndex = -1;
    int endIndex = -1;

    uint32_t baseXor = 0;

    int answerForQuery = 0;
};

void buildXorSumRangeQueries(SuffixTree::State* state, uint32_t xorSumSoFar, const vector<int>& prefixXorSumLookup, vector<XorSumRangeQuery>& queries)
{
    for (const auto& transition : state->transitions)
    {
        uint32_t newXorSum = xorSumSoFar;
        newXorSum = newXorSum ^ (prefixXorSumLookup[transition.substringFollowed.endIndex]);
        if (transition.substringFollowed.startIndex - 1 >= 0)
        {
            newXorSum = newXorSum ^ (prefixXorSumLookup[transition.substringFollowed.startIndex - 1]);
        }
        queries.push_back({transition.substringFollowed.startIndex, transition.substringFollowed.endIndex, xorSumSoFar});
        buildXorSumRangeQueries(transition.nextState, newXorSum, prefixXorSumLookup, queries);
    }

}

int64_t solveOptimised(const string& s)
{
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

    std::map<uint32_t, int> numPrefixesWithXorSum;
    numPrefixesWithXorSum[0] = 1; // Empty prefix.

    for (int i = 0; i < s.size(); i++)
    {
        for (auto startQuery : queriesBeginningAtIndex[i])
        {
            if (i != 0)
            {
                startQuery->baseXor = startQuery->baseXor ^ prefixXorSumLookup[i - 1];
            }
            startQuery->answerForQuery -= numPrefixesWithXorSum[startQuery->baseXor];
            for (int i = 0; i < 26; i++)
            {
                startQuery->answerForQuery -= numPrefixesWithXorSum[startQuery->baseXor ^ (1 << i)];
            }
        }

        numPrefixesWithXorSum[prefixXorSumLookup[i]]++;

        for (auto endQuery : queriesEndingAtIndex[i])
        {
            endQuery->answerForQuery += numPrefixesWithXorSum[endQuery->baseXor];
            for (int i = 0; i < 26; i++)
            {
                endQuery->answerForQuery += numPrefixesWithXorSum[endQuery->baseXor ^ (1 << i)];
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
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        //const int N = rand() % 100'000;
        const int N =  100'000;
        const int maxLetter = rand() % 26 + 1;

        cout << 1 << endl;
        //cout << N << endl;
        for (int i = 0; i < N; i++)
        {
            cout << static_cast<char>('a' + rand() % maxLetter);
        }
        cout << endl;

        return 0;
    }
    
    const int T = read<int>();
    
    for (int t = 0; t < T; t++)
    {
        const string s = read<string>();
        currentString = s;
#ifdef BRUTE_FORCE
        const auto solutionBruteForce = solveBruteForce(s);
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
        const auto solutionOptimised = solveOptimised(s);
        cout << "solutionOptimised: " << solutionOptimised << endl;

        assert(solutionOptimised == solutionBruteForce);
#else
        const auto solutionOptimised = solveOptimised(s);
        cout << solutionOptimised << endl;
#endif
    }

    assert(cin);
}
