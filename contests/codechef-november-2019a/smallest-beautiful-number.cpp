// Simon St James (ssjgz) - 2019-11-03
// 
// Solution to: https://www.codechef.com/NOV19A/problems/LSTBTF
//
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <cmath>

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
bool isSquare(const int N)
{
    const int sqrtN = sqrt(N);
    return (sqrtN * sqrtN == N) || ((sqrtN + 1) * (sqrtN + 1) == N);
}

bool isBeautiful(const string& numberAsString)
{
    int64_t sumOfDigitsSquared = 0;
    for (const auto digit : numberAsString)
    {
        const int64_t digitValue = digit - '0';
        sumOfDigitsSquared += digitValue * digitValue;
    }
    return isSquare(sumOfDigitsSquared);
}

#if 1
string solveBruteForce(int N)
{
    string result;

    string numberAsString(N, '1');

    while (true)
    {
        if (isBeautiful(numberAsString))
            return numberAsString;
        int index = N - 1;
        while (index >= 0 && numberAsString[index] == '9')
        {
            numberAsString[index] = '1';
            index--;
        }
        if (index == -1)
            break;

        numberAsString[index]++;
    }
    
    return "";
}
#endif


#if 1
string solveOptimised(int N)
{
    string result;

    string numberAsString(N, '1');

    int64_t squareDigitSum = N;

    while (true)
    {
        if (isSquare(squareDigitSum))
            return numberAsString;
        int index = N - 1;
        while (index >= 0 && numberAsString[index] == '9')
        {
            squareDigitSum = squareDigitSum - (9 * 9) + (1 * 1);
            numberAsString[index] = '1';
            index--;
        }
        if (index == -1)
            break;

        const int digitValue = numberAsString[index] - '0';
        squareDigitSum -= digitValue * digitValue;
        numberAsString[index]++;
        squareDigitSum += (digitValue + 1) * (digitValue + 1);
    }

    return "";

}
#endif


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
            const int N = 1 + rand() % 10000;
            cout << N << endl;
        }

        return 0;
    }
#if 0
    int maxBlah = 0;
    for (int N = 1; N <= 1'000'000; N++)
    {
        if (isSquare(N))
            continue;
        const int sqrtN = sqrt(N);
        assert(sqrtN * sqrtN < N);
        assert((sqrtN + 1) * (sqrtN + 1) > N);
        cout << "N: " << N << endl;
        maxBlah = max(maxBlah, (sqrtN + 1) * (sqrtN + 1) - N);
    }
    cout << "maxBlah: " << maxBlah << endl;
#endif
    
    // TODO - read in testcase.
    const auto T = read<int>();


    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();

#ifdef BRUTE_FORCE
#if 1
        const auto solutionBruteForce = solveBruteForce(N);
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
#endif
#if 1
        const auto solutionOptimised = solveOptimised(N);
        cout << "solutionOptimised:  " << solutionOptimised << endl;

        assert(solutionOptimised == solutionBruteForce);
#endif
#else
        const auto solutionOptimised = solveOptimised();
        cout << solutionOptimised << endl;
#endif
    }

    assert(cin);
}
