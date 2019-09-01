// Simon St James (ssjgz) - 2019-09-01
//
// Solution to: https://www.codechef.com/problems/TRIP2/
//
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
//#define NDEBUG
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

std::pair<bool, vector<int>> solveBruteForce(int N, int K, const vector<int>& a)
{
    std::pair<bool, vector<int>> result = { false, vector<int>() };
    const int numUndecided = count(a.begin(), a.end(), -1);

    vector<int> filledIn = vector<int>(numUndecided, 1);

    while (true)
    {
        vector<int> newA = a;
        int undecidedIndex = 0;
        for (auto& x : newA)
        {
            if (x == -1)
            {
                x = filledIn[undecidedIndex];
                undecidedIndex++;
            }
        }

#if 0
        cout << "newA: " << endl;
        for (const auto x : newA)
        {
            cout << x << " ";
        }
        cout << endl;
#endif

        if (unique(newA.begin(), newA.end()) == newA.end())
        {
            cout << "found!" << endl;
            for (const auto x : newA)
            {
                cout << x << " ";
            }
            cout << endl;

            result.first = true;
            result.second = newA;
            break;
        }

        int index = 0;
        while (index < filledIn.size() && filledIn[index] == K)
        {
            filledIn[index] = 1;
            index++;
        }

        if (index == filledIn.size())
            break;

        filledIn[index]++;
    }
    
    return result;
}

std::pair<bool, vector<int>> solveOptimised(int N, int K, const vector<int>& aOriginal)
{
    std::pair<bool, vector<int>> result = { false, vector<int>() };

    vector<int> a(aOriginal);
    a.insert(a.begin(), 0);
    a.insert(a.end(), 0);

    int posOfLastDecided = 0;
    for (int i = 1; i < a.size(); i++)
    {
        if (a[i] != -1 && i - posOfLastDecided > 1)
        {

            auto nextDistinct = [&K](int value)
            {
                int newValue = value + 1;
                if (newValue == K + 1)
                    newValue = 1;
                assert(1 <= newValue && newValue <= K);
                assert(value != newValue);

                return newValue;
            };


            if (K == 2)
            {
                if (a[i] != 0 && a[posOfLastDecided] != 0)
                {
                    const bool decidedAreDifferent = (a[i] != a[posOfLastDecided]);
                    const bool distanceIsOdd = (i - posOfLastDecided + 1) % 2 == 1;
                    if (decidedAreDifferent && distanceIsOdd)
                        return { false, vector<int>() };
                    if (!decidedAreDifferent && !distanceIsOdd)
                        return { false, vector<int>() };
                }
            }
            // We know we can succeed - let's just figure out what to use to fill in the gaps!
            int firstNewDecided = 1;
            for (int j = posOfLastDecided + 1; j < i; j++)
            {
                a[j] = firstNewDecided;
                firstNewDecided = nextDistinct(firstNewDecided);
            }
            int dbgNumAdjustments = 0;
            while (a[posOfLastDecided] == a[posOfLastDecided + 1] || a[i - 1] == a[i])
            {
                // This block will run at most twice.
                for (int j = posOfLastDecided + 1; j < i; j++)
                {
                    a[j] = nextDistinct(a[j]);
                }
                dbgNumAdjustments++;
            }
            assert(dbgNumAdjustments <= 2);
            assert(a[posOfLastDecided + 1] != a[posOfLastDecided]);
            assert(a[i - 1] != a[i]);
        }

        if (a[i] != -1)
            posOfLastDecided = i;
    }

    a.pop_back();
    a.erase(a.begin());
    
    return {true, a};
}


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        cout << 1 << endl;

        while (true)
        {
            const int N = rand() % 8 + 1;
            const int K = rand() % 8 + 2;
            const int numUndecided = rand() % (N + 1);
            vector<int> a;
            for (int i = 0; i < numUndecided; i++)
                a.push_back(-1);

            while (a.size() < N)
            {
                a.push_back(rand() % K + 1);
            }
            random_shuffle(a.begin(), a.end());

            bool valid = true;
            for (int i = 1; i < N; i++)
            {
                if (a[i] != -1 && a[i - 1] != -1 && a[i] == a[i - 1])
                    valid = false;
            }

            if (valid)
            {
                cout << N << " " << K << endl;
                for (const auto x : a)
                    cout << x << " ";
                cout << endl;
                break;
            }
        }

        return 0;
    }
    
    const int T = read<int>();
    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();
        const int K = read<int>();

        vector<int> a(N);

        for (int i = 0; i < N; i++)
        {
            a[i] = read<int>();
        }

#ifdef BRUTE_FORCE
#if 1
        const auto solutionBruteForce = solveBruteForce(N, K, a);
        cout << "solutionBruteForce: " << (solutionBruteForce.first ? "YES" : "NO") << endl;
#endif
#if 1
        const auto solutionOptimised = solveOptimised(N, K, a);
        cout << "solutionOptimised:  " << (solutionOptimised.first ? "YES" : "NO") << endl;
        if (solutionOptimised.first)
        {
            auto a = solutionOptimised.second;
            for (const auto x : a)
            {
                cout << x << " ";
            }
            cout << endl;
            assert(unique(a.begin(), a.end()) == a.end());

        }

        assert(solutionOptimised.first == solutionBruteForce.first);
#endif
#else
        const auto solutionOptimised = solveOptimised(N, K, a);
        cout << (solutionOptimised.first ? "YES" : "NO") << endl;
        if (solutionOptimised.first)
        {
            auto a = solutionOptimised.second;
            assert(unique(a.begin(), a.end()) == a.end());
#if 1
            for (const auto x : a)
            {
                cout << x << " ";
            }
            cout << endl;
#endif

        }
#endif
    }

    assert(cin);
}
