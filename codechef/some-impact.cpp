// Simon St James (ssjgz) - 2019-08-02
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


bool canFindImpactSites(int N, int K, int64_t M, int64_t X0)
{
    if (K == 1)
        return M == N;

    if (K == 2)
    {
        // Special case for K == 2.
        // First, obtain the representation of M is a binary string
        // (LSB first).
        string impactSites;
        while (M > 0)
        {
            if ((M % 2) == 0)
            {
                impactSites.push_back('0');
            }
            else
            {
                impactSites.push_back('1');
                N--;
            }
            M >>= 1;
        }
        if (N == 0)
            return true;
        if (N < 0)
        {
            // We require more impacts that we have.
            return false;
        }
        // We have impacts to make up!  Note that 2 ** k == 2 * (2 ** k - 1):
        // that is, we can, in the "binary" representation of M (which is LSB-first,
        // remember), replace the substring "01" with "20" or "02" with "10"
        // and still have the same "value" - it's no longer a *binary* string,
        // precisely, but imagine that the value of the string is still
        //
        //   d0 * (2 ** 0) + d1 * (2 ** 1) + ... + dl * (2 ** l)
        //
        // where now di can be 0, 1 or 2.  NOTE: these replacements must not make the
        // leftmost digit, which represents the number of impacts at X0, equal to 2.
        //
        // Note that either of the two replacements described above increases the
        // number of impacts by 1.  Keep making this replacement until either
        // we cannot do so any more, or we have made up the remaining impacts.
        while (true)
        {
            bool increasedNumImpacts = false;
            for (int i = 1; i < impactSites.size() - 1 && N > 0; i++)
            {
                if (impactSites[i] == '0' && impactSites[i + 1] > impactSites[i])
                {
                    impactSites[i] = '2';
                    impactSites[i + 1]--;
                    N--;
                    increasedNumImpacts = true;
                }
            }
            if (N  == 0 || !increasedNumImpacts)
                break;
        }
        return (N == 0);
    }
   

    // General K > 3 case.  Break M into (unique) representation
    //
    //   dl * (K ** l) + d_l-1 * (K ** (l - 1)) + ... + d0 * (K ** 0)
    //
    // No digit is allowed to be greater than 2.  A digit other than
    // A0 is allowed to be 2.  A0, representing the number of impacts
    // at X0, can only be 0 or 1.
    int distFromX0 = 0;
    while (M > 0)
    {
        const int digit = M % K;
        if (digit == 0)
        {
            // Do nothing.
        }
        else if (digit == 1)
        {
            N--;
        }
        else if (digit == 2)
        {
            // Impacts can occur to the left or the right
            // of X0, so we can have up to *two* with the 
            // same contribution, as long as distFromX0 > 0.
            if (distFromX0 == 0)
                return false;
            N -= 2;
        }
        else
        {
            return false;
        }


        M -= digit;
        M /= K;
        distFromX0++;
    }

    return (N == 0);
}

int main(int argc, char* argv[])
{
    const int T = read<int>();
    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();
        const int K = read<int>();
        const int64_t M = read<int64_t>();
        const int64_t X0 = read<int64_t>();

        const auto hasImpactSites = canFindImpactSites(N, K, M, X0);
        cout << (hasImpactSites ? "yes" : "no") << endl;
    }
}

