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

int solveBruteForce(int64_t N)
{
    int64_t fn_2_mod10 = 0;
    int64_t fn_1_mod10 = 1;

    vector<int> a;
    a.push_back(fn_2_mod10);
    a.push_back(fn_1_mod10);
    for (int i = 0; i < N - 2; i++)
    {
        //cout << "fn: " <<  fn << " mod 10: " << (fn % 10) << endl;
        int64_t fn_mod10 = (fn_2_mod10 + fn_1_mod10) % 10;
        a.push_back(fn_mod10);

        fn_2_mod10 = fn_1_mod10;
        fn_1_mod10 = fn_mod10;
    }

    while (a.size() > 1)
    {
        vector<int> b;
        for (int i = 1; i < a.size(); i += 2)
        {
            b.push_back(a[i]);
        }
        a = b;
        cout << "new A:" << endl;
        for (const auto x : a)
        {
            cout << " " << x;
        }
        cout << endl;
    }
    return a.front();
}


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);

    vector<int> fibonacciMod10Cycle;

    int64_t fn_2_mod10 = 0;
    int64_t fn_1_mod10 = 1;
    fibonacciMod10Cycle.push_back(fn_2_mod10);
    fibonacciMod10Cycle.push_back(fn_1_mod10);

    int i = 0;
    while (true)
    {
        //cout << "fn: " <<  fn << " mod 10: " << (fn % 10) << endl;
        int64_t fn_mod10 = (fn_2_mod10 + fn_1_mod10) % 10;
        fibonacciMod10Cycle.push_back(fn_mod10);
        cout << "n: " << (i + 2) << " fn_mod10: " << fn_mod10 << endl;


        fn_2_mod10 = fn_1_mod10;
        fn_1_mod10 = fn_mod10;

        if (fn_2_mod10 == 0 && fn_1_mod10 == 1)
        {
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

        cout << "fibonacciMod10Cycle: " << endl;
        for (int i = 0; i < fibonacciMod10Cycle.size(); i++)
        {
            cout << " N: " << (i + 1) << " f: " << fibonacciMod10Cycle[i] << endl;
        }
        int64_t largestPowerOf2 = 1;
        while (largestPowerOf2 <= N)
        {
            largestPowerOf2 <<= 1;
        }
        largestPowerOf2 >>= 1;
        const int64_t fibonacciIndex = (largestPowerOf2 - 1);

        const auto solutionBruteForce = solveBruteForce(N);
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
        cout << "solutionOptimised: " << fibonacciMod10Cycle[fibonacciIndex % fibonacciMod10Cycle.size()] << endl;
    }


    assert(cin);
}
