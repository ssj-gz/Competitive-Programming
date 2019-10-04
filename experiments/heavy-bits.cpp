// Simon St James (ssjgz) - 2019-09-30
// 
// Solution to: TODO - Heavy Bits link
//
#define SUBMISSION
#define BRUTE_FORCE
#define VERIFY_RESULTS
#ifdef SUBMISSION
#undef BRUTE_FORCE
#undef VERIFY_RESULTS
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
            m_vector(maxIndex - minIndex + 1, initialValue)
    {

    }
    T& operator[](int index)
    {
        assert(index >= m_minIndex);
        assert(index <= m_maxIndex);
        assert(index - m_minIndex < m_vector.size());
        return m_vector[index - m_minIndex];
    }
    private:
        int m_minIndex = -1;
        int m_maxIndex = -1;
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

        decltype(State::transitions.begin()) findTransition(State* state, int letterIndex, bool assertFound = true);
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
    int num0sInPrefix = -1;
    int num1sInPrefix = -1;
    bool subtractFromResult = false;
};

void solveOptimisedAux(SuffixTree::State* state, const string& B, const int num0sSoFar, const int num1sSoFar, const vector<int>& num0sInPrefixLen, vector<vector<Query>>& queriesForIndex)
{
    // TODO - optimise all this - we should be able to process a transition in O(1)!
    for (const auto& transition : state->transitions)
    {
        auto nextState = transition.nextState;
        const auto num0sInSubstring = num0sInPrefixLen[transition.substringFollowed.endIndex + 1] - num0sInPrefixLen[transition.substringFollowed.startIndex];
        const auto num1sInSubstring = transition.substringFollowed.endIndex - transition.substringFollowed.startIndex + 1 - num0sInSubstring;
        const auto nextNum0sSoFar = num0sSoFar + num0sInSubstring;
        const auto nextNum1sSoFar = num1sSoFar + num1sInSubstring;

        queriesForIndex[transition.substringFollowed.startIndex].push_back({num0sSoFar, num1sSoFar, false});
        if (transition.substringFollowed.endIndex + 1 < B.size())
        {
            queriesForIndex[transition.substringFollowed.endIndex + 1].push_back({nextNum0sSoFar, nextNum1sSoFar, true});
        }
        solveOptimisedAux(nextState, B, nextNum0sSoFar, nextNum1sSoFar, num0sInPrefixLen, queriesForIndex);
    }
}

int64_t solveOptimised(const string& B)
{
    int64_t result = 0;
    const int N = B.size();

    vector<int> num0sInPrefixLen;
    int num0sSoFar = 0;
    num0sInPrefixLen.push_back(num0sSoFar);
    for (const auto bit : B)
    {
        if (bit == '0')
            num0sSoFar++;
        num0sInPrefixLen.push_back(num0sSoFar);
    }

    SuffixTree suffixTree(B);

    vector<vector<Query>> queriesForIndex(N);
    solveOptimisedAux(suffixTree.rootState(), B, 0, 0, num0sInPrefixLen, queriesForIndex);

    constexpr auto NeverBalanced = -2;

    Vec<int> nextIndexWithSuffixBalance(-N, +N, -1);
    vector<int64_t> sumOfWeightStartingAt(N + 1, 0);
    vector<int64_t> sumOf0sStartingAt(N + 1, 0);
    vector<int64_t> sumOf1sStartingAt(N + 1, 0);
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

        const auto suffixLength = N - index;
        if (bit == '0')
        {
            sumOf0sStartingAt[index] = suffixLength + sumOf0sStartingAt[index + 1];
            sumOf1sStartingAt[index] = sumOf1sStartingAt[index + 1];
        }
        else
        {
            sumOf1sStartingAt[index] = suffixLength + sumOf1sStartingAt[index + 1];
            sumOf0sStartingAt[index] = sumOf0sStartingAt[index + 1];
        }

        const auto nextBalanceIndex = nextIndexWithSuffixBalance[currentSuffixBalance] - 1; // i.e. - nextBalanceIndex is the next index strictly greater than "index" such that s[index, currentSuffixBalance] is balanced.

        if (nextBalanceIndex == NeverBalanced)
        {
            if (bit == '0')
                sumOfWeightStartingAt[index] = sumOf0sStartingAt[index];
            else
                sumOfWeightStartingAt[index] = sumOf1sStartingAt[index];
        }
        else
        {
            auto num0sInRange = num0sInPrefixLen[nextBalanceIndex + 1];
            num0sInRange -= num0sInPrefixLen[index];
            if (bit == '0')
            {
                sumOfWeightStartingAt[index] = sumOf0sStartingAt[index];
                sumOfWeightStartingAt[index] -= sumOf0sStartingAt[nextBalanceIndex + 1];
                const auto suffixLen = N - (nextBalanceIndex + 1);
                sumOfWeightStartingAt[index] -= num0sInRange * suffixLen;
            }
            else
            {
                auto num1sInRange = (nextBalanceIndex - index + 1) - num0sInRange;
                sumOfWeightStartingAt[index] = sumOf1sStartingAt[index];
                sumOfWeightStartingAt[index] -= sumOf1sStartingAt[nextBalanceIndex + 1];
                const auto suffixLen = N - (nextBalanceIndex + 1);
                sumOfWeightStartingAt[index] -= num1sInRange * suffixLen;
            }
            assert(sumOfWeightStartingAt[index] >= 0);
            assert((nextBalanceIndex - index + 1) % 2 == 0);
            sumOfWeightStartingAt[index] += sumOfWeightStartingAt[nextBalanceIndex + 1] + ((nextBalanceIndex - index + 1) / 2) * (N - (nextBalanceIndex + 1));

        }
#ifdef VERIFY_RESULTS
        {
            int64_t debugSumOfWeightStartingAt = 0;
            int num0s = 0;
            int num1s = 0;
            for (int i = index; i < N; i++)
            {
                if (B[i] == '0')
                    num0s++;
                if (B[i] == '1')
                    num1s++;
                debugSumOfWeightStartingAt += max(num0s, num1s);
            }
            assert(debugSumOfWeightStartingAt == sumOfWeightStartingAt[index]);
        }
#endif

        nextIndexWithSuffixBalance[currentSuffixBalance] = index;


        for (const auto& query : queriesForIndex[index])
        {
            // balanceIndex: the smallest index >= index such that prefixBalance + balance[s[index, balanceIndex]] = 0.
            const auto prefixBalance = query.num0sInPrefix - query.num1sInPrefix;
            int balanceIndex = index - 1;
            if (prefixBalance != 0)
            {
                const auto desiredSuffixBalance = currentSuffixBalance + prefixBalance;
                balanceIndex = nextIndexWithSuffixBalance[desiredSuffixBalance] - 1;
            }

            int64_t queryResult = 0;

            if (balanceIndex == NeverBalanced)
            {
                assert(prefixBalance != 0);
                if (prefixBalance > 0)
                {
                    // More 0's than 1's in the prefix.
                    queryResult += sumOf0sStartingAt[index] + (N - index) * query.num0sInPrefix ;
                }
                else
                {
                    // More 1's than 0's in the prefix.
                    queryResult += sumOf1sStartingAt[index] + (N - index) * query.num1sInPrefix ;
                }
            }
            else if (prefixBalance == 0)
            {
                queryResult += sumOfWeightStartingAt[balanceIndex + 1] + ((balanceIndex - index + 1) / 2 + query.num1sInPrefix) * (N - (balanceIndex + 1));
            }
            else
            {

                auto num0sInRange = num0sInPrefixLen[balanceIndex + 1];
                num0sInRange -= num0sInPrefixLen[index];
                const auto rangeLen = balanceIndex + 1 - index;
                assert(query.num0sInPrefix != query.num1sInPrefix);
                const auto afterBalanceSuffixLen = N - (balanceIndex + 1);
                if (query.num0sInPrefix > query.num1sInPrefix)
                {
                    queryResult = sumOf0sStartingAt[index];
                    queryResult -= sumOf0sStartingAt[balanceIndex + 1];
                    queryResult -= num0sInRange * afterBalanceSuffixLen;
                    queryResult += rangeLen * query.num0sInPrefix;
                }
                else
                {
                    auto num1sInRange = rangeLen - num0sInRange;
                    queryResult = sumOf1sStartingAt[index];
                    queryResult -= sumOf1sStartingAt[balanceIndex + 1];
                    queryResult -= num1sInRange * afterBalanceSuffixLen;
                    queryResult += rangeLen * query.num1sInPrefix;
                }
                assert(queryResult >= 0);
                assert((balanceIndex - index + 1 + query.num0sInPrefix + query.num1sInPrefix) % 2 == 0);
                queryResult += sumOfWeightStartingAt[balanceIndex + 1] +  ((balanceIndex - index + 1 + query.num0sInPrefix + query.num1sInPrefix) / 2) * (N - (balanceIndex + 1));

            }
#ifdef VERIFY_RESULTS
            const auto dbgQueryResult = sumOfBlah(query.num0sInPrefix, query.num1sInPrefix, index, B);
            {
                int dbgBalanceIndex = index - 1;
                if (prefixBalance != 0)
                {
                    int num0s = query.num0sInPrefix;
                    int num1s = query.num1sInPrefix;
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
                        dbgBalanceIndex = NeverBalanced;
                    }
                }
                assert(balanceIndex == dbgBalanceIndex);
            }
            assert(queryResult == dbgQueryResult);
#endif

            result += (query.subtractFromResult ? -1 : 1) * queryResult;
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
            const int N = rand() % 1'000'000 + 1;
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
        const auto solutionOptimised = solveOptimised(s);
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
        const auto tkTransitionIter = findTransition(s, t(k));
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
        auto tTransitionIter = findTransition(s, letterIndex, false);
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
        auto tkTransitionIter = findTransition(s, t(k));
        auto sPrime = tkTransitionIter->nextState;
        auto kPrime = tkTransitionIter->substringFollowed.startIndex;
        auto pPrime = tkTransitionIter->substringFollowed.endIndex;
        while (pPrime - kPrime <= p - k)
        {
            k = k + pPrime - kPrime + 1;
            s = sPrime;
            if (k <= p)
            {
                tkTransitionIter = findTransition(s, t(k));
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

decltype(SuffixTree::State::transitions.begin()) SuffixTree::findTransition(State* state, int letterIndex, bool assertFound)
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
