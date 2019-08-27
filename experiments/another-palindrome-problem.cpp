// Simon St James (ssjgz) - 2019-XX-XX
// https://www.codechef.com/problems/PALINXOR
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

using namespace std;

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
        ModNum& operator-=(const ModNum& other)
        {
            m_n = (Mod + m_n - other.m_n) % Mod;
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

ModNum operator-(const ModNum& lhs, const ModNum& rhs)
{
    ModNum result(lhs);
    result -= rhs;
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

vector<ModNum> solveBruteForce(const string& s)
{
    vector<ModNum> result(s.size());
    vector<bool> useCharacter(s.size(), false);

    while (true)
    {
        string subsequence;
        for (int i = 0; i < s.size(); i++)
        {
            if (useCharacter[i])
                subsequence.push_back(s[i]);
        }
        //cout << "subsequence: " << subsequence << endl;
        if (subsequence.size() % 2 == 1)
        {
            if (subsequence == string(subsequence.rbegin(), subsequence.rend()))
            {
                int halfPalindromeLength = subsequence.size() / 2 + 1;
                //cout << "s: " << s << " subsequence: " << subsequence << " halfPalindromeLength: " << halfPalindromeLength << endl;
                int centre = 0;
                while (true)
                {
                    assert(centre < useCharacter.size());
                    //cout << "centre: " << centre << " useCharacter: " << useCharacter[centre] << endl;
                    if (useCharacter[centre])
                    {
                        halfPalindromeLength--;
                        if (halfPalindromeLength <= 0)
                            break;
                    }
                    centre++;
                }
                result[centre] = result[centre] + 1;
            }
        }


        int index = 0;
        while (index < s.size() && useCharacter[index])
        {
            useCharacter[index] = false;
            index++;
        }

        if (index == s.size())
            break;
        useCharacter[index] = true;
    }
    
    return result;
}

int64_t calcXorThing(const vector<ModNum>& array)
{

    int64_t result = 0;
    for (int i = 0; i < array.size(); i++)
    {
        result = result ^ (array[i] * (i + 1)).value();
    }
    return result;
}

vector<ModNum> solveOptimised(const string& s)
{
    vector<ModNum> result;
    const int n = s.size();

    vector<vector<ModNum>> P(n + 1, vector<ModNum>(n + 1, 0));

    for (int prefixLength = 1; prefixLength <= n; prefixLength++)
    {
        for (int suffixLength = 1; suffixLength <= n && prefixLength + suffixLength <= n; suffixLength++)
        {
            P[prefixLength][suffixLength] += P[prefixLength - 1][suffixLength] +
                                             P[prefixLength][suffixLength - 1] -
                                             P[prefixLength -1][suffixLength - 1];
            if (s[prefixLength - 1] == s[n - suffixLength])
            {
                P[prefixLength][suffixLength] += 1 + P[prefixLength -1][suffixLength - 1];
            }

        }
    }

    for (int prefixLength = 0; prefixLength < n; prefixLength++)
    {
        const int suffixLength = n - 1 - prefixLength;
        result.push_back(1 + P[prefixLength][suffixLength]);
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
        cout << 1 << endl;
        const int N = rand() % 20 + 1;
        const int maxLetter = rand() % 26 + 1;
        string s;
        for (int i = 0; i < N; i++)
        {
            s.push_back(static_cast<char>('a' + rand() % maxLetter));
        }
        cout << s << endl;
        return 0;
    }
    
    const int T = read<int>();

    for (int t = 0; t < T; t++)
    {

        const auto s = read<string>();
#ifdef BRUTE_FORCE
        const auto solutionBruteForce = solveBruteForce(s);
        cout << "solutionBruteForce: ";
        for (const auto x : solutionBruteForce)
        {
            cout << x << " ";
        }
        cout << "(" << calcXorThing(solutionBruteForce) << ")";
        cout << endl;
        const auto solutionOptimised = solveOptimised(s);
        cout << "solutionOptimised: ";
        for (const auto x : solutionOptimised)
        {
            cout << x << " ";
        }
        cout << "(" << calcXorThing(solutionOptimised) << ")";
        cout << endl;

        assert(solutionOptimised == solutionBruteForce);
#else
        const auto solutionOptimised = solveOptimised();
        cout << solutionOptimised << endl;
#endif
    }

    assert(cin);
}
