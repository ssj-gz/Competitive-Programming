// Simon St James (ssjgz) - 2019-12-20
// 
// Solution to: https://www.codechef.com/problems/CMPRSS/
//
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
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

string compress(const vector<int>& a)
{
    string compressed;
    for (int rangeBeginIndex = 0; rangeBeginIndex < a.size();)
    {
        // As with STL iterators, rangeEndIndex is the index *one past* the range
        // of consecutive numbers beginning at rangeBeginIndex
        int rangeEndIndex = rangeBeginIndex + 1;
        for (; rangeEndIndex < a.size(); rangeEndIndex++)
        {
            if (a[rangeEndIndex] != a[rangeBeginIndex] + (rangeEndIndex - rangeBeginIndex))
                break;
        }
        if (rangeEndIndex - rangeBeginIndex > 2)
        {
            compressed += to_string(a[rangeBeginIndex]) + "..." + to_string(a[rangeEndIndex - 1]);
        }
        else if (rangeEndIndex - rangeBeginIndex > 1)
        {
            compressed += to_string(a[rangeBeginIndex]) + "," + to_string(a[rangeEndIndex - 1]);
        }
        else
        {
            compressed += to_string(a[rangeBeginIndex]);
        }
        rangeBeginIndex = rangeEndIndex;
        if (rangeBeginIndex != a.size())
            compressed += ",";
    }
    
    return compressed;
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
            const int N = 1 + rand() % 20;
            const int maxA = N + rand() % 30;
            cout << N << endl;
            vector<int> nums;
            for (int i = 1; i <= maxA; i++)
            {
                nums.push_back(i);
            }
            random_shuffle(nums.begin(), nums.end());
            vector<int> a(nums.begin(), nums.begin() + N);
            sort(a.begin(), a.end());
            for (int i = 0; i < N; i++)
            {
                cout << a[i];
                if (i != N - 1)
                    cout << " ";
            }
            cout << endl;
        }

        return 0;
    }
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {

        const int N = read<int>();
        vector<int> a(N);
        for (auto& x : a)
            x = read<int>();

        cout << compress(a) << endl;
    }

    assert(cin);
}
