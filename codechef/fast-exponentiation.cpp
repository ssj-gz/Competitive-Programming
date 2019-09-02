// Simon St James (ssjgz) - 2019-09-02
// 
// Solution to: https://www.codechef.com/problems/FEXP
//
#include <iostream>
#include <cassert>

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

int64_t quickPower(int64_t n, int64_t m)
{
    // Raise n to the m mod modulus using as few multiplications as 
    // we can e.g. n ^ 8 ==  (((n^2)^2)^2).
    const int64_t modulus = 1'000'000'007ULL;
    int64_t result = 1;
    int64_t power = 0;
    while (m > 0)
    {
        if (m & 1)
        {
            int64_t subResult = n;
            for (int64_t i = 0; i < power; i++)
            {
                subResult = (subResult * subResult) % modulus;
            }
            result = (result * subResult) % modulus;
        }
        m >>= 1;
        power++;
    }
    return result;
}

int main()
{
    const int T = read<int>();
    for (int t = 0; t < T; t++)
    {
        const int64_t a = read<int64_t>();
        const int64_t b = read<int64_t>();

        cout << quickPower(a, b) << endl;
    }
}



