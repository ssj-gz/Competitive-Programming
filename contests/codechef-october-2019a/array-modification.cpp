// Simon St James (ssjgz) - 2019-10-06
// 
// Solution to: https://www.codechef.com/OCT19A/problems/MARM
//
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>

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

vector<int> solveBruteForce(const vector<int>& aOriginal, int64_t K)
{
    const int N = aOriginal.size();
    vector<int> a(aOriginal);
    for (int64_t i = 0; i <= K - 1; i++)
    {
        vector<int> nextA(a);
        nextA[i % N] = a[i % N] ^ a[N - (i % N) - 1];

        a = nextA;
    }
    
    return a;
}

vector<int> findStateAfterKOps(const vector<int>& aOriginal, int64_t K)
{
    const int N = aOriginal.size();

    const bool kWasGreaterThanOrEqualToN = (K >= N);
    K = K % (N * 3);
    auto result = solveBruteForce(aOriginal, K);
    if ((N % 2) == 1 && kWasGreaterThanOrEqualToN)
    {
        result[N / 2] = 0;
    }

    return result;
}


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();
        const int64_t K = read<int64_t>();

        vector<int> a(N);
        for (auto& x : a)
            x = read<int>();

        const auto stateAfterKOps = findStateAfterKOps(a, K);
        for (const auto x : stateAfterKOps)
        {
            cout << x << " ";
        }
        cout << endl;
    }

    assert(cin);
}
