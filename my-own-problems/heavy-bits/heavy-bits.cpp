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
//#define NDEBUG
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

        vector<Transition>::iterator findTransitionIter(State* state, int letterIndex, bool assertFound = true);
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
    int numbsInPrefix[2] = {-1, -1};
    bool subtractFromResult = false;
};

void solveOptimisedAux(SuffixTree::State* state, const string& B, const int num0sSoFar, const int num1sSoFar, const vector<int>& num0sInPrefixLen, vector<vector<Query>>& queriesForIndex)
{
    for (const auto& transition : state->transitions)
    {
        auto nextState = transition.nextState;
        const auto num0sInSubstring = num0sInPrefixLen[transition.substringFollowed.endIndex + 1] - num0sInPrefixLen[transition.substringFollowed.startIndex];
        const auto num1sInSubstring = transition.substringFollowed.endIndex - transition.substringFollowed.startIndex + 1 - num0sInSubstring;
        const auto nextNum0sSoFar = num0sSoFar + num0sInSubstring;
        const auto nextNum1sSoFar = num1sSoFar + num1sInSubstring;

        queriesForIndex[transition.substringFollowed.startIndex].push_back({{num0sSoFar, num1sSoFar}, false});
        //cout << "query - suffixBeginIndex: " << (transition.substringFollowed.startIndex) << " num0sInPrefix: " << num0sSoFar << " num1sInPrefix: " << num1sSoFar << " total prefix: " << (num0sSoFar + num1sSoFar) <<endl;
        if (transition.substringFollowed.endIndex + 1 < B.size())
        {
            //cout << "query - suffixBeginIndex: " << (transition.substringFollowed.endIndex + 1) << " num0sInPrefix: " << nextNum0sSoFar << " num1sInPrefix: " << nextNum1sSoFar << " total prefix: " << (nextNum0sSoFar + nextNum1sSoFar) <<endl;
            queriesForIndex[transition.substringFollowed.endIndex + 1].push_back({{nextNum0sSoFar, nextNum1sSoFar}, true});
        }
        solveOptimisedAux(nextState, B, nextNum0sSoFar, nextNum1sSoFar, num0sInPrefixLen, queriesForIndex);
    }
}

int64_t solveOptimised(const string& B)
{
    int64_t result = 0;
    const int N = B.size();

    vector<int> numbsInPrefixLen[2];
    int numbsSoFar[2] = { 0, 0 };
    numbsInPrefixLen[0].push_back(numbsSoFar[0]);
    numbsInPrefixLen[1].push_back(numbsSoFar[1]);
    for (const auto bit : B)
    {
        numbsSoFar[bit - '0']++;
        numbsInPrefixLen[0].push_back(numbsSoFar[0]);
        numbsInPrefixLen[1].push_back(numbsSoFar[1]);
    }

    SuffixTree suffixTree(B);

    vector<vector<Query>> queriesForIndex(N);
    solveOptimisedAux(suffixTree.rootState(), B, 0, 0, numbsInPrefixLen[0], queriesForIndex);

    constexpr auto NeverBalanced = -2;

    Vec<int> nextIndexWithSuffixBalance(-N, +N, -1);
    vector<int64_t> sumOfWeightStartingAt(N + 1, 0);
    vector<int64_t> sumOfbsStartingAt[2] = {vector<int64_t>(N + 1, 0), vector<int64_t>(N + 1, 0)};
    int numbsInSuffix[2] = {0, 0};
    for (int suffixBeginIndex = N - 1; suffixBeginIndex >= 0; suffixBeginIndex--)
    {
        const int bitValue = B[suffixBeginIndex] - '0';

        const auto suffixLength = N - suffixBeginIndex;

        sumOfbsStartingAt[bitValue][suffixBeginIndex] = suffixLength + sumOfbsStartingAt[bitValue][suffixBeginIndex + 1];
        sumOfbsStartingAt[1 - bitValue][suffixBeginIndex] = sumOfbsStartingAt[1 - bitValue][suffixBeginIndex + 1];

        auto blah = [&](const int numbsInPrefix[2], const int suffixBeginIndex) -> int64_t
        {
            if (suffixBeginIndex == N)
                return 0;

            const auto prefixBalance = numbsInPrefix[0] - numbsInPrefix[1];
            if (prefixBalance == 0)
            {
                return sumOfWeightStartingAt[suffixBeginIndex] + numbsInPrefix[0] * (N - suffixBeginIndex);
            }
            
            auto num0sInSuffix = numbsInPrefixLen[0][N] - numbsInPrefixLen[0][suffixBeginIndex];
            auto num1sInSuffix = numbsInPrefixLen[1][N] - numbsInPrefixLen[1][suffixBeginIndex];

            const auto currentSuffixBalance = num0sInSuffix - num1sInSuffix;

            const auto mostPopulousPrefixBit = (prefixBalance > 0 ? 0 : 1);
            const auto desiredSuffixBalance = currentSuffixBalance + prefixBalance;
            // balanceIndex: the smallest index >= suffixBeginIndex such that prefixBalance + balance[s[suffixBeginIndex, balanceIndex]] = 0.
            const auto balanceIndex = nextIndexWithSuffixBalance[desiredSuffixBalance] - 1;
            if (balanceIndex == NeverBalanced)
            {
                return sumOfbsStartingAt[mostPopulousPrefixBit][suffixBeginIndex] + (N - suffixBeginIndex) * numbsInPrefix[mostPopulousPrefixBit] ;
            }
            else
            {
                const int numbsInRange[2] = { numbsInPrefixLen[0][balanceIndex + 1] - numbsInPrefixLen[0][suffixBeginIndex], numbsInPrefixLen[1][balanceIndex + 1] - numbsInPrefixLen[1][suffixBeginIndex] };
                const auto rangeLen = balanceIndex + 1 - suffixBeginIndex;
                assert(numbsInPrefix[0] != numbsInPrefix[1]);
                const auto afterBalanceSuffixLen = N - (balanceIndex + 1);

                int64_t result = 0;
                result = sumOfbsStartingAt[mostPopulousPrefixBit][suffixBeginIndex] - sumOfbsStartingAt[mostPopulousPrefixBit][balanceIndex + 1];
                result -= numbsInRange[mostPopulousPrefixBit] * afterBalanceSuffixLen;
                result += rangeLen * numbsInPrefix[mostPopulousPrefixBit];

                assert(result >= 0);
                assert((balanceIndex - suffixBeginIndex + 1 + numbsInPrefix[0] + numbsInPrefix[1]) % 2 == 0);
                // TODO - replace "balanceIndex - suffixBeginIndex + 1" with rangeLen and "(N - (balanceIndex + 1))" with afterBalanceSuffixLen?
                result += sumOfWeightStartingAt[balanceIndex + 1] +  ((balanceIndex - suffixBeginIndex + 1 + numbsInPrefix[0] + numbsInPrefix[1]) / 2) * (N - (balanceIndex + 1));
                
                return result;
            }
        };

        int numOfbsInFakePrefix[2] = {0, 0};
        numOfbsInFakePrefix[bitValue] = 1;
        sumOfWeightStartingAt[suffixBeginIndex] = blah(numOfbsInFakePrefix, suffixBeginIndex + 1) + 1; // "+1" for the contribution of just the 1-char prefix, on its own.

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
        numbsInSuffix[bitValue]++;
        const auto currentSuffixBalance = numbsInSuffix[0] - numbsInSuffix[1];
        nextIndexWithSuffixBalance[currentSuffixBalance] = suffixBeginIndex;

        for (const auto& query : queriesForIndex[suffixBeginIndex])
        {
            cout << "query - suffixBeginIndex: " << suffixBeginIndex << " num0sInSuffix: " << query.numbsInPrefix[0] << " num1sInSuffix: " << query.numbsInPrefix[1] << " total prefix: " << (query.numbsInPrefix[0] + query.numbsInPrefix[1]) <<endl;
            const auto queryResult = blah(query.numbsInPrefix, suffixBeginIndex);

#ifdef VERIFY_RESULTS
            const auto dbgQueryResult = sumOfBlah(query.numbsInPrefix[0], query.numbsInPrefix[1], suffixBeginIndex, B);
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
#if 0
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
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
#else
        string s;
        for (int i = 0; i < 100'000; i++)
        {
            if (rand() % 10'000 == 0)
                s += '0';
            else
                s += '1';
        }
        for (int i = 0; i < 900'000; i++)
        {
            if (rand() % 2 == 0)
                s += '0';
            else
                s += '1';
        }
        cout << 1 << endl;
        cout << s.size() << endl;
        cout << s << endl;
#endif

        return 0;
    }

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

vector<SuffixTree::Transition>::iterator SuffixTree::findTransitionIter(State* state, int letterIndex, bool assertFound)
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
