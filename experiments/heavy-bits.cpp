// Simon St James (ssjgz) - 2019-09-30
// 
// Solution to: TODO - Heavy Bits link
//
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <set>
#include <limits>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

using namespace std;


/**
 * Simple vector-ish data structure that allows negative indices.
 */
template<typename T>
class Vec
{
    public:
        Vec(int minIndex, int maxIndex, int initialValue)
            : m_minIndex(minIndex),
            m_maxIndex(maxIndex),
            m_numElements(maxIndex - minIndex + 1),
            m_vector(m_numElements, initialValue)
    {

    }
        int minIndex() const
        {
            return m_minIndex;
        }
        int maxIndex() const
        {
            return m_maxIndex;
        }
        T& operator[](int index)
        {
            assert(index >= m_minIndex);
            assert(index <= m_maxIndex);
            assert(index - m_minIndex < m_vector.size());
            return m_vector[index - m_minIndex];
        }
        const T& operator[](int index) const
        {
            assert(index >= m_minIndex);
            assert(index <= m_maxIndex);
            assert(index - m_minIndex < m_vector.size());
            return m_vector[index - m_minIndex];
        }
    private:
        int m_minIndex = -1;
        int m_maxIndex = -1;
        int m_numElements = -1;
        vector<T> m_vector;
};


/**
 * Simple implementation of Ukkonen's algorithm:
 *  https://en.wikipedia.org/wiki/Ukkonen's_algorithm
 * for online construction of suffix trees.
 * @author Simon St James, Jan 2017.
 */
constexpr auto alphabetSize = 2; // 0 and 1.
class SuffixTree
{
    public:
        SuffixTree(const string& s);
        SuffixTree(const SuffixTree& other) = delete;

        struct State;
        State* rootState() const;
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
            Transition(State *nextState, const Substring& substringFollowed)
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

        void finalise();

        string m_currentString;

        vector<State> m_states;
        State *m_root = nullptr;
        State *m_auxiliaryState = nullptr;

        // "Persistent" versions of s & k from Algorithm 2 in Ukkonen's paper!
        State *m_s;
        int m_k;

        void appendLetter(char letter);

        std::pair<State*, int> update(State* s, int k, int i);
        pair<bool, State*> testAndSplit(State* s, int k, int p, int letterIndex);
        std::pair<State*, int> canonize(State* s, int k, int p);

        State *createNewState(State* parent = nullptr);

        decltype(State::transitions.begin()) findTransitionIter(State* state, int letterIndex, bool assertFound = true);
        int t(int i);
};

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

int weight(const string& binaryString)
{
    int num0s = 0;
    int num1s = 0;
    for (const auto& bit : binaryString)
    {
        assert(bit == '0' || bit == '1');
        if (bit == '0')
            num0s++;
        if (bit == '1')
            num1s++;
    }
    return max(num0s, num1s);
}

int64_t solveBruteForce(const string& s)
{
    int64_t result = 0;
    set<string> distinctSubstrings;
    for (int startPos = 0; startPos < s.size(); startPos++)
    {
        for (int length = 1; startPos + length <= s.size(); length++)
        {
            const string substring = s.substr(startPos, length);
            //cout << "substring: " << substring << endl;
            distinctSubstrings.insert(substring);
        }
    }

    for (const auto substring : distinctSubstrings)
    {
        result += weight(substring);
    }

    return result;
}

int64_t sumOfBlah(int num0sSoFar, int num1sSoFar, int beginIndex, const string& B)
{
    int64_t result = 0;
    for (int index = beginIndex; index < B.size(); index++)
    {
        if (B[index] == '0')
            num0sSoFar++;
        if (B[index] == '1')
            num1sSoFar++;


        result += max(num0sSoFar, num1sSoFar);
    }
    return result;
}

struct Query
{
    int num0sSoFar = -1;
    int num1sSoFar = -1;
    bool subtractFromResult = false;
};

void solveOptimisedAux(SuffixTree::State* state, const string& B, const int num0sSoFar, const int num1sSoFar, const vector<int>& num0sInPrefix, vector<vector<Query>>& queriesForIndex)
{
    // TODO - optimise all this - we should be able to process a transition in O(1)!
    for (const auto& transition : state->transitions)
    {
        auto nextState = transition.nextState;
        const auto num0sInSubstring = num0sInPrefix[transition.substringFollowed.endIndex + 1] - num0sInPrefix[transition.substringFollowed.startIndex];
        const auto num1sInSubstring = transition.substringFollowed.endIndex - transition.substringFollowed.startIndex + 1 - num0sInSubstring;
        const auto nextNum0sSoFar = num0sSoFar + num0sInSubstring;
        const auto nextNum1sSoFar = num1sSoFar + num1sInSubstring;

        queriesForIndex[transition.substringFollowed.startIndex].push_back({num0sSoFar, num1sSoFar, false});
        if (transition.substringFollowed.endIndex + 1 < B.size())
        {
            queriesForIndex[transition.substringFollowed.endIndex + 1].push_back({nextNum0sSoFar, nextNum1sSoFar, true});
        }
        solveOptimisedAux(nextState, B, nextNum0sSoFar, nextNum1sSoFar, num0sInPrefix, queriesForIndex);
    }
}

int64_t solveOptimised(const string& B)
{
    int64_t result = 0;
    const int N = B.size();

    vector<int> num0sInPrefix;
    int num0sSoFar = 0;
    num0sInPrefix.push_back(num0sSoFar);
    for (const auto bit : B)
    {
        if (bit == '0')
            num0sSoFar++;
        num0sInPrefix.push_back(num0sSoFar);
    }

    SuffixTree suffixTree(B);

    vector<vector<Query>> queriesForIndex(N);
    solveOptimisedAux(suffixTree.rootState(), B, 0, 0, num0sInPrefix, queriesForIndex);

    Vec<int> nextIndexWithSuffixBalance(-N, +N, -1);

    vector<int64_t> sumOf0sStartingAt(N + 1, 0);
    vector<int64_t> sumOf1sStartingAt(N + 1, 0);
    for (int i = N - 1; i >= 0; i--)
    {
        const auto suffixLength = N - i;
        if (B[i] == '0')
        {
            sumOf0sStartingAt[i] = suffixLength + sumOf0sStartingAt[i + 1];
            sumOf1sStartingAt[i] = sumOf1sStartingAt[i + 1];
        }
        else
        {
            sumOf1sStartingAt[i] = suffixLength + sumOf1sStartingAt[i + 1];
            sumOf0sStartingAt[i] = sumOf0sStartingAt[i + 1];
        }
    }

    int num0sInSuffix = 0;
    int num1sInSuffix = 0;
    for (int index = N - 1; index >= 0; index--)
    {
        const char bit = B[index];
        assert(bit == '0' || bit == '1');
        if (bit == '0')
            num0sInSuffix++;
        if (bit == '1')
            num1sInSuffix++;
        const auto currentSuffixBalance = num0sInSuffix - num1sInSuffix;
        nextIndexWithSuffixBalance[currentSuffixBalance] = index;
        for (const auto& query : queriesForIndex[index])
        {
            //cout << "index: " << index << " query.num0sSoFar: " << query.num0sSoFar << " query.num1sSoFar: " << query.num1sSoFar << endl;
            const auto dbgQueryResult = sumOfBlah(query.num0sSoFar, query.num1sSoFar, index, B);
            const auto prefixBalance = query.num0sSoFar - query.num1sSoFar;
            int dbgBalanceIndex = index - 1;
            if (prefixBalance != 0)
            {
                int num0s = query.num0sSoFar;
                int num1s = query.num1sSoFar;
                while (num0s != num1s && dbgBalanceIndex + 1 < N)
                {
                    dbgBalanceIndex++;
                    if (B[dbgBalanceIndex] == '0')
                        num0s++;
                    if (B[dbgBalanceIndex] == '1')
                        num1s++;
                }
                if (dbgBalanceIndex == N - 1)
                {
                    dbgBalanceIndex = -2;
                }
            }
            // balanceIndex: the smallest index >= index such that prefixBalance + balance[s[index, balanceIndex]] = 0.
            int balanceIndex = index - 1;
            if (prefixBalance != 0)
            {
                const auto desiredSuffixBalance = currentSuffixBalance + prefixBalance;
                //cout << "prefixBalance: " << prefixBalance << " currentSuffixBalance: " << currentSuffixBalance << " desiredSuffixBalance: " << desiredSuffixBalance << endl;
                balanceIndex = nextIndexWithSuffixBalance[desiredSuffixBalance] - 1;
            }
            //cout << "balanceIndex: " << balanceIndex << " dbgBalanceIndex: " << dbgBalanceIndex << endl;
            assert(balanceIndex == dbgBalanceIndex);

            int64_t queryResult = 0;

            if (balanceIndex == -2)
            {
                assert(prefixBalance != 0);
                if (prefixBalance > 0)
                {
                    // More 0's than 1's in the prefix.
                    queryResult += sumOf0sStartingAt[index] + (N - index) * query.num0sSoFar ;
                }
                else
                {
                    // More 1's than 0's in the prefix.
                    queryResult += sumOf1sStartingAt[index] + (N - index) * query.num1sSoFar ;
                }
                assert(queryResult == dbgQueryResult);
            }

            result += (query.subtractFromResult ? -1 : 1) * dbgQueryResult;
        }
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
        // TODO - generate randomised test.
        //const int T = rand() % 100 + 1;
        const int T = 1;
        cout << T << endl;

        for (int t = 0; t < T; t++)
        {
            const int N = rand() % 100 + 1;
            cout << N << endl;
            for (int i = 0; i < N; i++)
            {
                cout << static_cast<char>('0' + (rand() % 2));
            }
            cout << endl;
        }

        return 0;
    }

    // TODO - read in testcase.
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        read<int>();
        const auto s = read<string>();

#ifdef BRUTE_FORCE
        const auto solutionBruteForce = solveBruteForce(s);
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
#if 1
        const auto solutionOptimised = solveOptimised(s);
        cout << "solutionOptimised:  " << solutionOptimised << endl;

        assert(solutionOptimised == solutionBruteForce);
#endif
#else
        const auto solutionOptimised = solveOptimised();
        cout << solutionOptimised << endl;
#endif
    }

    assert(cin);
}

SuffixTree::SuffixTree(const string& s)
{
    // Size of a suffix tree representing s cannot exceed 2 x |s| - 1, but we have two auxilliary states, so account for them.
    m_states.reserve(s.size() * 2 + 1); 

    m_root = createNewState();
    m_auxiliaryState = createNewState();

    for (int i = 0; i < alphabetSize; i++)
    {
        m_auxiliaryState->transitions.push_back(Transition(m_root, Substring(-(i + 1), -(i + 1))));
    }
    m_root->suffixLink = m_auxiliaryState;

    m_s = m_root;
    m_k = 1;


    for (auto letter : s)
    {
        appendLetter(letter);
    }
    finalise();
}

void SuffixTree::finalise()
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

SuffixTree::State* SuffixTree::rootState() const
{
    return m_root;
}

void SuffixTree::appendLetter(char letter)
{
    m_currentString += letter;
    const auto updateResult = update(m_s, m_k, m_currentString.size());
    m_s = updateResult.first;
    m_k = updateResult.second;
    const auto canonizeResult = canonize(m_s, m_k, m_currentString.size());
    m_s = canonizeResult.first;
    m_k = canonizeResult.second;
}

std::pair<SuffixTree::State*, int> SuffixTree::update(State* s, int k, int i)
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
pair<bool, SuffixTree::State*> SuffixTree::testAndSplit(State* s, int k, int p, int letterIndex)
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
std::pair<SuffixTree::State*, int> SuffixTree::canonize(State* s, int k, int p)
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

SuffixTree::State *SuffixTree::createNewState(State* parent)
{
    m_states.push_back(State());
    State *newState = &(m_states.back());
    newState->parent = parent;
    return newState;
}

decltype(SuffixTree::State::transitions.begin()) SuffixTree::findTransitionIter(State* state, int letterIndex, bool assertFound)
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

int SuffixTree::t(int i)
{
    // Ukkonen's algorithm uses 1-indexed strings throughout and alphabet throughout; adjust for this.
    return m_currentString[i - 1] - '0' + 1;
}
