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
        // First, obtain the representation of M as a binary string
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
            // This requires more impacts than we are allowed to use.
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
        // where now di can be 0, 1 or 2 (for i != 0).  NOTE: these replacements must not make the
        // leftmost digit, d0, which represents the number of impacts at X0, equal to 2.
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
    // Lol - that was embarrassing - lots of dumb mistakes on this one
    // (like assuming the number of impacts that contributed K ** l
    // was either 0 or 1, when in fact it can be 2 - and missing the 
    // special case for K == 2 as my testcase generator was rigged
    // not to generate K == 2 testcases, for some reason(!)).
    //
    // Anyway, fairly easy to solve.  Firstly, note that if Xi != X0,
    // then an impact at Xi or at -Xi both contribute the same value K ** (|Xi - X0)
    // to the total impact at X0.  Secondly, note that if l is the smallest distance 
    // from X0 such that the effect of an impact at l+1 exceeds M (i.e. K ** (l + 1) > M,
    // but K ** l <= M), then we need not consider impacts at a distance exceeding l.
    //
    // Thus, we can represent the total impact for a set X = {X1, X2, ... XN} as
    //
    //    A_l * (K ** l) + A_l-1 * (K ** l) + ... + A_0 * (K ** 0) 
    //
    // subject to A_i = 0, 1 or 2 for i != 0; A_0 - that is, the number of impacts at X0 - equal to
    // 0 or 1; and sum [ i = 1, 2, ... , l ] { A_i } = N.
    //
    // Now, for K > 2 (K == 2 must be dealt with separately), we have the following
    // two facts - these are fairly well-known, and I present them without proof.
    //
    //   For any i, for any B_p's subject to 0 <= B_p < K, (K ** i) > B_(i-1) * (K ** (i - 1)) + B_(i-2) * (K ** (i - 2)) + ... + B_0 * (K ** 0) (1)
    //   
    //   For any M, the set of B_0, B_1, ... B_l (where each B_i < K) such that
    // 
    //     M = B_l * (K ** l) + B_l-1 * (K ** l) + ... + B_0 * (K ** 0)
    //
    //   is unique (2)
    //
    // (Neither (1) nor (2) are true for K == 2).
    //
    // (We don't actually use (1) anywhere, but hopefully it gives a hint as to why (2) is true!).
    //
    // How does this help? Well, it essentially gives us the algorithm for computing the (unique) possible
    // values of all A_i - simply use the well-known method for expressing M in base K - the A_i are simply
    // the "digits" of this representation.  Since there is only one possible value for each A_i, then if any of the
    // constraints are violated e.g. if some A_i > 2, or A_0 > 1, or the sum of the A_i's is not N, then
    // there is no set of impacts that give the required M total impact effect at X0.
    //
    // The K == 2 case is a bit harder, as both (1) and (2) are violated: for example, let M = 512.
    // Then M can be written as M = 1 * (2 ** 9) (A_9 is 1; all other A_is are 0) *or* as M = 2 * (2 ** 8) (A_8 is 2,
    // all other A_is are 0).
    //
    // We can deal with this quite simply, though: again, find the base K representation of M (a binary string).
    //
    // If this has exactly N digits equal to 1, then we're done.  If it has more than N digits equal to 1, then
    // we can't satisfy the problem, and again we're done.  Otherwise, if the number of digits equal to 1 i.e.
    // the sum of the A_is is < N, then we can try transforming it in a way that meets the constraints but
    // has a greater digit sum by exploiting the ambiguity mentioned above. As an example, if M = 512 and N = 2:
    //
    //    M = 1000000000 (base 2)
    // 
    // which has a digit sum of 1, falling short of the required N = 2.  But M also equals
    //
    //    M = 0200000000 (base-ish 2-ish ;))
    //
    // which has a digit sum of 2, fulfilling the requirements.  (This is no longer properly base 2, but we treat
    // it as if it is i.e. the value of 0200000000 is 0 * (2 ** 9) + 2 * (2 ** 8) + 0 * (2 ** 7) + ... + 0 * (2 ** 0)).
    //
    // If we can repeatedly transform the original binary representation of M into one that still fulfils the restrictions
    // but has digit sum equal to N, then we're done (we can fulfil the requirement); if we can't, then we're also done 
    // (we can't fulfil the requirement).  Hopefully the code in canFindImpactSites explains this a bit better :)
    //
    // And that's it!
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

