// Simon St James (ssjgz) - 2019-09-02
// 
// Solution to: https://www.codechef.com/problems/ALC002
//
#include <iostream>
#include <vector>
#include <algorithm>

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
    // Very simple - just do what the question says.
    // The array will shrink by (at worst) a factor of 2
    // each iteration, so no need for any cleverness :)
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();
        const int K = read<int>();

        vector<int64_t> a(N);
        for (auto& aElement : a)
        {
            aElement = read<int64_t>();
        }

        vector<int64_t> b = a; // Deal with the case where N < K.
        while (a.size() >= K)
        {
            // Create the new b.
            // It's more efficient to reverse the array "a" and pop_back()
            // than to leave it the right way and pop from the front.
            b.clear();
            reverse(a.begin(), a.end());
            while (a.size() >= K)
            {
                int64_t sum = 0;
                for (int i = 0; i < K; i++)
                {
                    sum += a.back();
                    a.pop_back();
                }
                b.push_back(sum);
            }
            // Unreverse, and copy the remaining into b.
            reverse(a.begin(), a.end());
            b.insert(b.end(), a.begin(), a.end());

            a = b;
        }

        for (const auto x : b)
        {
            cout << x << " ";
        }
        cout << endl;
    }

    assert(cin);
}
