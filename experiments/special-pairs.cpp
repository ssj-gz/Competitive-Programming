// Simon St James (ssjgz) - 2019-XX-XX
//
// Solution for "Special Pairs" - https://www.hackerearth.com/practice/algorithms/dynamic-programming/bit-masking/practice-problems/algorithm/special-pairs-7/
//
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

using namespace std;

constexpr int maxAValue = 1'000'000;
constexpr int log2(int N, int exponent = 0, int powerOf2 = 1)
{
            return (powerOf2 >= N) ? exponent : log2(N, exponent + 1, powerOf2 * 2);
}
constexpr int maxNumBitsInA = log2(maxAValue);


template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

int64_t solveBruteForce(const vector<int>& a)
{
    int64_t result = 0;

    for (int i = 0; i < a.size(); i++)
    {
        for (int j = 0; j < a.size(); j++)
        {
            if ((a[i] & a[j]) == 0)
                result++;
        }
    }

    return result;
}

string asBinary(int64_t value)
{
    string asBinary;
    while (asBinary.size() < maxNumBitsInA)
    {
        asBinary.push_back(static_cast<char>('0' + (value & 1)));
        value >>= 1;
    }
    reverse(asBinary.begin(), asBinary.end());
    return asBinary;
};

int64_t solveOptimised(const vector<int>& a)
{
    const int halfBinaryMax = 2048;
    struct Bucket
    {
        vector<int> secondHalvesOfAInBucket;
        vector<int> numSecondHalvesOfAThatGiveZeroWhenAndedWith;
    };
    vector<Bucket> buckets(halfBinaryMax);
    for (auto& ble : buckets)
    {
        ble.numSecondHalvesOfAThatGiveZeroWhenAndedWith.resize(halfBinaryMax);
    }

    // The values are between 0 and halfBinaryMax.
    vector<vector<int>> valuesThatGiveZeroWhenAndedWith(halfBinaryMax);

    for (int i = 0; i < halfBinaryMax; i++)
    {
        for (int j = 0; j < halfBinaryMax; j++)
        {
            if ((i & j) == 0)
            {
                valuesThatGiveZeroWhenAndedWith[i].push_back(j);
            }
        }

    }

    for (const auto aElement : a)
    {
        const int bucket = (aElement & (((static_cast<int64_t>(1) << 11) - 1) << 10)) >> 10;
        const int secondHalfOfA = aElement & ((1 << 11) - 1);
        assert(0 <= bucket && bucket < buckets.size());
        assert(0 <= secondHalfOfA && secondHalfOfA < halfBinaryMax);
        buckets[bucket].secondHalvesOfAInBucket.push_back(secondHalfOfA);
        for (const auto x : valuesThatGiveZeroWhenAndedWith[secondHalfOfA])
        {
            buckets[bucket].numSecondHalvesOfAThatGiveZeroWhenAndedWith[x]++;
        }
    }

    int64_t result = 0;
    for (int i = 0; i < buckets.size(); i++)
    {
        for (int j = 0; j < buckets.size(); j++)
        {
            if ((i & j) == 0)
            {
                for (const auto secondHalfOfA : buckets[i].secondHalvesOfAInBucket)
                {
                    result += buckets[j].numSecondHalvesOfAThatGiveZeroWhenAndedWith[secondHalfOfA];
                }
            }
        }

    }

    return result;
}


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int T = 10;
        cout << T << endl;
        //const int N = rand() % 10'000 + 1;
        for (int t = 0; t < T; t++)
        {
            const int N = 100'000;
            const int maxA = rand() % 1'000'000 + 1;
            cout << N << endl;
            for (int i = 0; i < N; i++)
            {
                cout << (rand() % maxA) << " ";
            }
            cout << endl;
        }
        return 0;
    }

#if 0
    cout << "maxNumBitsInA: " << maxNumBitsInA << endl;
    cout << "asBinary(1027): " << asBinary(1027) << endl;

    for (int i = 0; i < 2048; i++)
    {
        int numForI = 0;
        for (int j = 0; j < 2048; j++)
        {
            if ((i & j) == 0)
            {
                numForI++;
            }
        }
        cout << "i: " << i << " numForI: " << numForI << endl;
    }
#endif
    
    const int T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();
        vector<int> a(N);
        for (auto& aElement : a)
        {
            aElement = read<int>();
        }

#ifdef BRUTE_FORCE
        const auto solutionBruteForce = solveBruteForce(a);
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
        const auto solutionOptimised = solveOptimised(a);
        cout << "solutionOptimised: " << solutionOptimised << endl;

        assert(solutionOptimised == solutionBruteForce);
#else
        const auto solutionOptimised = solveOptimised(a);
        cout << solutionOptimised << endl;
#endif
    }

    assert(cin);
}
