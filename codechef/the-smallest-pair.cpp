// Simon St James (ssjgz) - 2019-10-31
// 
// Solution to: https://www.codechef.com/problems/SMPAIR
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

int sumOfTwoSmallest(vector<int> a)
{
    // Easiest way is to sort a and take two smallest elements,
    // but this approach is O(N) instead of O(N log2 N) :)
    auto minElementIter = std::min_element(a.begin(), a.end());
    const auto minElement = *minElementIter;
    a.erase(minElementIter);
    const auto secondMinElement = *std::min_element(a.begin(), a.end());

    return minElement + secondMinElement;
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
            const int N = 2 + rand() % 100;
            cout << N << endl;
            const int maxValue = 1 + rand() % 100;

            for (int i = 0; i < N; i++)
            {
                cout << (1 + rand() % maxValue);
                if (i != N - 1)
                    cout << " ";
            }
            cout << endl;
        }

        return 0;
    }
    
    // TODO - read in testcase.
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {

        const int N = read<int>();

        vector<int> a(N);
        for (auto& aValue : a)
            aValue = read<int>();
        const auto solutionBruteForce = sumOfTwoSmallest(a);
        cout << solutionBruteForce << endl;
    }

    assert(cin);
}
