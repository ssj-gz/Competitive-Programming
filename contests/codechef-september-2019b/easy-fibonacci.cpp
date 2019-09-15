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
    ios::sync_with_stdio(false);

    vector<int> fibonacciMod10Cycle;

    int64_t fnPrevPrevMod10 = 0;
    int64_t fnPrevMod10 = 1;
    fibonacciMod10Cycle.push_back(fnPrevPrevMod10);
    fibonacciMod10Cycle.push_back(fnPrevMod10);

    int i = 0;
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
        i++;
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
        const int64_t fibonacciIndex = (largestPowerOf2 - 1);

        cout << fibonacciMod10Cycle[fibonacciIndex % fibonacciMod10Cycle.size()] << endl;
    }

    assert(cin);
}
