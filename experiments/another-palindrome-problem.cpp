// Simon St James (ssjgz) - 2019-08-27
// https://www.codechef.com/problems/PALINXOR
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>

#include <cassert>

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

int64_t calcXorOfNumCentredAroundPos(const vector<ModNum>& numCentredAroundPos)
{
    int64_t result = 0;
    for (int i = 0; i < numCentredAroundPos.size(); i++)
    {
        result = result ^ (numCentredAroundPos[i] * (i + 1)).value();
    }
    return result;
}

vector<ModNum> findNumCentredAroundEachPos(const string& s)
{
    const int n = s.size();

    // NB: numWithPrefixAndSuffixLength does *not* consider palindromes where the subsequences 
    // we choose from prefix and suffix are both empty.
    vector<vector<ModNum>> numWithPrefixAndSuffixLength(n + 1, vector<ModNum>(n + 1, 0));

    for (int prefixLength = 1; prefixLength <= n; prefixLength++)
    {
        for (int suffixLength = 1; suffixLength <= n && prefixLength + suffixLength <= n; suffixLength++)
        {
            numWithPrefixAndSuffixLength[prefixLength][suffixLength] += numWithPrefixAndSuffixLength[prefixLength - 1][suffixLength] +
                                                                        numWithPrefixAndSuffixLength[prefixLength][suffixLength - 1] -
                                                                        numWithPrefixAndSuffixLength[prefixLength -1][suffixLength - 1];
            const auto lastLetterOfPrefix = s[prefixLength - 1];
            const auto firstLetterOfSuffix = s[n - suffixLength];
            if (lastLetterOfPrefix == firstLetterOfSuffix)
            {
                numWithPrefixAndSuffixLength[prefixLength][suffixLength] += 
                    1 + // Match consisting of just firstLetterOfSuffix and lastLetterOfPrefix.
                    numWithPrefixAndSuffixLength[prefixLength -1][suffixLength - 1]; // All other matches that match firstLetterOfSuffix and lastLetterOfPrefix.
            }

        }
    }

    // Now that we have numWithPrefixAndSuffixLength, we can calculate numCentredAroundPos for each of the n positions.
    vector<ModNum> numCentredAroundPos;
    for (int prefixLength = 0; prefixLength < n; prefixLength++)
    {
        const int suffixLength = n - 1 - prefixLength;
        numCentredAroundPos.push_back(1 + // Matches where both subsequence from prefix and suffix are empty.
                                      numWithPrefixAndSuffixLength[prefixLength][suffixLength] // All other subsequences centred around current element.
                                      );
    }
    
    return numCentredAroundPos;
}


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    
    const int T = read<int>();

    for (int t = 0; t < T; t++)
    {

        const auto s = read<string>();
        const auto numCentredAroundPos = findNumCentredAroundEachPos(s);
        cout << calcXorOfNumCentredAroundPos(numCentredAroundPos) << endl;
    }

    assert(cin);
}
