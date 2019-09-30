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

const int64_t Mod = 1'000'000'007ULL;

class ModNum
{
    public:
        ModNum(int64_t n = 0)
            : m_n{n}
        {
            assert(n >= 0);
        }
        ModNum& operator+=(const ModNum& other)
        {
            m_n = (m_n + other.m_n) % Mod;
            return *this;
        }
        ModNum& operator*=(const ModNum& other)
        {
            m_n = (m_n * other.m_n) % Mod;
            return *this;
        }
        int64_t value() const { return m_n; };
    private:
        int64_t m_n;
};

ModNum operator+(const ModNum& lhs, const ModNum& rhs)
{
    ModNum result(lhs);
    result += rhs;
    return result;
}

ModNum operator*(const ModNum& lhs, const ModNum& rhs)
{
    ModNum result(lhs);
    result *= rhs;
    return result;
}

ostream& operator<<(ostream& os, const ModNum& toPrint)
{
    os << toPrint.value();
    return os;
}

bool operator==(const ModNum& lhs, const ModNum& rhs)
{
    return lhs.value() == rhs.value();
}

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

ModNum solveBruteForce(const string& s)
{
    ModNum result = 0;
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

void solveOptimisedAux(SuffixTree::State* state, const string& B, const int num1sSoFar, const int num0sSoFar, ModNum& result)
{
    // TODO - optimise all this - we should be able to process a transition in O(1)!
    for (const auto& transition : state->transitions)
    {
        auto nextState = transition.nextState;
        auto nextNum0sSoFar = num0sSoFar;
        auto nextNum1sSoFar = num1sSoFar;
        for (int index = transition.substringFollowed.startIndex; index <= transition.substringFollowed.endIndex; index++)
        {
            if (B[index] == '0')
                nextNum0sSoFar++;
            if (B[index] == '1')
                nextNum1sSoFar++;

            result += max(nextNum0sSoFar, nextNum1sSoFar);
        }
        solveOptimisedAux(nextState, B, nextNum1sSoFar, nextNum0sSoFar, result);
    }
}

ModNum solveOptimised(const string& B)
{
    ModNum result;

    SuffixTree suffixTree(B);

    solveOptimisedAux(suffixTree.rootState(), B, 0, 0, result);

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
