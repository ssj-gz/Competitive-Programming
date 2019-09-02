// Simon St James (ssjgz) - 2019-09-02
// 
// Solution to: https://www.codechef.com/problems/ALC002
//
#include <iostream>
#include <vector>
#include <algorithm>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

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
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        // TODO - generate randomised test.
        //const int T = rand() % 100 + 1;
        const int T = 1;
        cout << T << endl;

        for (int t = 0; t < T; t++)
        {
            const int N = rand() % 100'000;
            //const int K = rand() % 100'000;
            const int K = 2;

            cout << N << " " << K << endl;
            for (int i = 0; i < N; i++)
            {
                cout << (rand() % 100001) << " ";
            }
            cout << endl;

        }

        return 0;
    }
    
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

        vector<int64_t> b = a;
        while (a.size() >= K)
        {
            b.clear();

            // It's more efficient to reverse the array and pop_back()
            // than to leave it the right way and pop from the front.
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
