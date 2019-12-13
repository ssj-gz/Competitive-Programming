// Simon St James (ssjgz) - 2019-12-13
// 
// Solution to: https://www.codechef.com/problems/CB2000
//
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
        ModNum& operator*=(const ModNum& other)
        {
            m_n = (m_n * other.m_n) % Mod;
            return *this;
        }
        int64_t value() const { return m_n; };
    private:
        int64_t m_n;
};

ModNum operator*(const ModNum& lhs, const ModNum& rhs)
{
    ModNum result(lhs);
    result *= rhs;
    return result;
}

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

ModNum findProductOfDigitPairDiffs(const string& N)
{
    ModNum result = 1;
    int numOfDigit[10] = {};
    for (const auto digit : N)
    {
        numOfDigit[digit - '0']++;
    }
    for (int i = 0; i <= 9; i++)
    {
        if (numOfDigit[i] > 1)
        {
            // There are two copies of the same digit: the difference
            // between them is 0, so the result will be multiplied by
            // 0, so the result will be 0.
            return 0;
        }
    }
    // If we're here, then all digits are distinct.
    // If we iterate over all pairs {d_1, d_2} where d_1 < d_2
    // and d_1 and d_2 both occur in N and multiply the differences,
    // then we have our result.
    for (int i = 0; i <= 9; i++)
    {
        if (numOfDigit[i] == 0)
            continue;
        for (int j = i + 1; j <= 9; j++)
        {
            if (numOfDigit[j] == 0)
                continue;
            result *= ModNum(abs(j - i));

        }
    }
    
    return result;
}


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const auto N = read<string>();

        cout << findProductOfDigitPairDiffs(N).value() << endl;
    }

    assert(cin);
}
