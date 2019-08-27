// Simon St James (ssjgz) - 2019-XX-XX
//
// Solution for "Special Pairs" - https://www.hackerearth.com/practice/algorithms/dynamic-programming/bit-masking/practice-problems/algorithm/special-pairs-7/
//
//#define SUBMISSION
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

string asBinary(int value)
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
    set<int> blah(a.begin(), a.end());

    vector<int> numWithBit(maxNumBitsInA + 1, 0);
    
    int64_t result = a.size() * a.size();

    for (int bitNum = maxNumBitsInA; bitNum >= 0; bitNum--)
    {
        cout << "bitNum: " << bitNum << endl;
        cout << " blah: " << endl;
        for (const auto x : blah)
        {
            cout << " " << asBinary(x) << endl;
        }
        cout << endl;

        int numNew = 0;
        while (!blah.empty() && *std::prev(blah.end()) >= (1 << bitNum))
        {
            const int toRemove = *std::prev(blah.end());
            const string toRemoveAsBinary = asBinary(toRemove);
            cout << " toRemoveAsBinary: " << toRemoveAsBinary << endl;

            for (int bitNum = maxNumBitsInA - 1; bitNum >= 0; bitNum--)
            {
                if (toRemoveAsBinary[maxNumBitsInA - 1 - bitNum] == '1')
                {
                    cout << "  bitNum: " << bitNum << " is set" << endl;
                    numWithBit[bitNum]++;
                }
            }
            blah.erase(std::prev(blah.end()));
            numNew++;
        }
        cout << "bitNum: " << bitNum << " numNew: " << numNew << " numWithBit: " << numWithBit[bitNum] << endl;
        result -= numNew * numWithBit[bitNum];
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
        cout << 1 << endl;
        const int N = rand() % 1000 + 1;
        const int maxA = rand() % 1'000'000 + 1;
        cout << N << endl;
        for (int i = 0; i < N; i++)
        {
            cout << (rand() % maxA) << " ";
        }
        cout << endl;
        return 0;
    }

    cout << "maxNumBitsInA: " << maxNumBitsInA << endl;
    cout << "asBinary(1027): " << asBinary(1027) << endl;
    
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
        const auto solutionOptimised = solveOptimised();
        cout << solutionOptimised << endl;
#endif
    }

    assert(cin);
}
