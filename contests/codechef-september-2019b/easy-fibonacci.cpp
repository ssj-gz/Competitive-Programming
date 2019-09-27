// Simon St James (ssjgz) - 2019-09-07
// 
// Solution to: https://www.codechef.com/SEPT19B/problems/FIBEASY
//
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

int main(int argc, char* argv[])
{
    // As the name suggests, pretty easy :)
    //
    // Let's deal with the Fibonacci part, first.
    //
    // We have:
    //
    //    F_n = F_(n-1) + F_(n-2)
    //
    // We're only interested in Fibonacci values mod 10, so:
    //
    //    F_n mod 10 = (F_(n - 1) mod 10 + F_(n - 2) mod 10) mod 10
    //
    // Now, F_n mod 10 depends only on the previous two values, 
    // F_(n - 1) mod 10 and F_(n - 2) mod 10, so if there existed an
    // m such that
    //
    //    F_(m - 1) mod 10 = F_(n - 1) mod 10 and 
    //    F_(m - 2) mod 10 = F_(n - 2) mod 10 and 
    //
    // then we would have F_m mod 10 = F_n mod 10; F_(m + 1) mod 10 = F_(n + 1) mod 10; etc
    // i.e. the set of Fibonacci numbers would repeat over and over i.e. F_n mod 10
    // would be periodic.
    //
    // Further, F_(n - 1) mod 10 can take only 10 possible values, as can F_(n - 2) mod 10;
    // thus the pair (F_(n - 1) mod 10, F_(n - 2) mod 10) can take only at most 10 x 10 = 100 possible
    // values, so by the Pigeonhole Principle there must be a repetition of these pairs i.e.
    // there does exist such an m as described above so F_n mod 10 must be periodic.
    //
    // As it happens, after doing a search for such a repetition, the first repetition is (very 
    // conveniently for us!) a repertition of F_0 mod 10 and F_1 mod 10, so bascially 
    // F_n mod 10 is just equal to the cycle fibonacciMod10Cycle repeated over and over again.
    //
    // What about the "D process" part? Well, instead of placing Fibonacci values into the initial
    // D, let's put the integers 1, 2, ... N into it:
    //
    //    D = (1, 2, ... , N)
    //
    // Let's iterate:
    //
    //    D = (2, 4, 6, ... )
    //
    // And again:
    //
    //    D = (4, 8, 12, ... )
    //
    // And again:
    //
    //    D = (8, 16, 24, ... )
    //
    // Hopefully the pattern is clear: if D starts off with 1, 2, ... N, then after the ith iteration,
    // D will consist of the subset of these numbers that are divisible by 2^i.  When D gets down
    // to just one element, that element will be the largest power of 2 in 1, 2, ... N 
    // i.e. i is the largest value such that 2^i <= N.
    //
    // But instead of 1, 2, ... N, we were supposed to put Fibonacci values into the initial D and find
    // the last remaining one.  From above, we just then need to find the largestPowerOf2 <= N and output
    // that largestPowerOf2'th Fibonacci number, which we can easily do using the cyclic property of fibonacciMod10Cycle.
    //
    // And that's it!
    ios::sync_with_stdio(false);

    // Compute fibonacciMod10Cycle.
    vector<int> fibonacciMod10Cycle;
    int64_t fnPrevPrevMod10 = 0;
    int64_t fnPrevMod10 = 1;
    fibonacciMod10Cycle.push_back(fnPrevPrevMod10);
    fibonacciMod10Cycle.push_back(fnPrevMod10);
    while (true)
    {
        int64_t fn_mod10 = (fnPrevPrevMod10 + fnPrevMod10) % 10;
        fibonacciMod10Cycle.push_back(fn_mod10);

        fnPrevPrevMod10 = fnPrevMod10;
        fnPrevMod10 = fn_mod10;

        if (fnPrevPrevMod10 == 0 && fnPrevMod10 == 1)
        {
            // Everything will repeat from here on in.
            fibonacciMod10Cycle.pop_back();
            fibonacciMod10Cycle.pop_back();
            break;
        }
    }

    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int64_t N = read<int64_t>(); 

        int64_t largestPowerOf2 = 1;
        while (largestPowerOf2 <= N)
        {
            largestPowerOf2 <<= 1;
        }
        largestPowerOf2 >>= 1;
        const int64_t fibonacciIndex = (largestPowerOf2 - 1); // Make 0-relative.

        cout << fibonacciMod10Cycle[fibonacciIndex % fibonacciMod10Cycle.size()] << endl;
    }

    assert(cin);
}
