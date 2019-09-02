// Simon St James (ssjgz) - 2019-09-02
// 
// Solution to: https://www.codechef.com/problems/ALC002
//
#include <iostream>
#include <deque>
#include <numeric>

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

        deque<int64_t> a(N); // Deque's implement fast removal of prefixes.
        for (auto& aElement : a)
        {
            aElement = read<int64_t>();
        }

        while (a.size() >= K)
        {
            // Create the new b.
            deque<int64_t> b;
            while (a.size() >= K)
            {
                // Remove blocks of size K from the front of a, and 
                // append their sums to b.
                const int64_t sumOfKBlock = std::accumulate(a.begin(), a.begin() + K, static_cast<int64_t>(0));
                a.erase(a.begin(), a.begin() + K);
                b.push_back(sumOfKBlock);
            }
            // Copy the remaining elements of a into b.
            b.insert(b.end(), a.begin(), a.end());

            // Prepare for the next iteration, if there is one, or to print
            // out the final result otherwise.
            a = b;
        }

        for (const auto x : a)
        {
            cout << x << " ";
        }
        cout << endl;
    }

    assert(cin);
}
