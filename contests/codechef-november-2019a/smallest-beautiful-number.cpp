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

bool isNumericallyLessThan(const string& lhsNumAsString, const string& rhsNumAsString)
{
    if (lhsNumAsString.length() < rhsNumAsString.length())
        return true;
    if (lhsNumAsString.length() > rhsNumAsString.length())
        return false;
    return lhsNumAsString < rhsNumAsString;
}

int64_t largestThig = 0;
int largestThog = 0;
#if 1
string solveOptimised(int N, const vector<vector<int>>& sumOfSquaresLookup)
{
    string result;

    string numberAsString(N, '1');

    const int64_t squareDigitSumOrig = N;

    if (isSquare(squareDigitSumOrig))
        return "";

    const int sqrtN = sqrt(N);
    assert(sqrtN * sqrtN < N);
    int64_t nextSquareRoot = sqrtN + 1;
    assert(nextSquareRoot * nextSquareRoot > N);

    string bestReplacementDigits;

    while (true)
    {
        const int64_t nextSquare = nextSquareRoot * nextSquareRoot;
        //cout << "nextSquare: " << nextSquare << " N: " << N << endl;
        int numTrailingDigitsToReplace = 1;
        while (numTrailingDigitsToReplace < sumOfSquaresLookup.size())
        {
            const int64_t squareDigitSum = squareDigitSumOrig - numTrailingDigitsToReplace;
            //numberAsString.pop_back();

            int64_t requiredSquareDigitSum = nextSquare - squareDigitSum;
            const int64_t dbgRequiredSquareDigitSum = requiredSquareDigitSum;
            const auto dbgNumTrailingDigitsToReplace = numTrailingDigitsToReplace;
            //cout << "requiredSquareDigitSum: " << requiredSquareDigitSum << " bestReplacementDigits.length(): " << bestReplacementDigits.length() << endl;
            if (!bestReplacementDigits.empty() && requiredSquareDigitSum > 9 * 9 * bestReplacementDigits.length())
            {
                return /*string(N - bestReplacementDigits.length(), '1') +*/ bestReplacementDigits;
            }
            //cout << "numTrailingDigitsToReplace: " << numTrailingDigitsToReplace << " requiredSquareDigitSum: " << requiredSquareDigitSum << " sumOfSquaresLookup[numTrailingDigitsToReplace][requiredSquareDigitSum]: " << sumOfSquaresLookup[numTrailingDigitsToReplace][requiredSquareDigitSum] << endl;
            if (sumOfSquaresLookup[numTrailingDigitsToReplace][requiredSquareDigitSum] != -1)
            {
                //cout << "Found!" << endl;
                string replacementTrailingDigits;
                while (requiredSquareDigitSum > 0)
                {
                    const int nextDigitValue = sumOfSquaresLookup[numTrailingDigitsToReplace][requiredSquareDigitSum];
                    //cout << "nextDigitValue: " << nextDigitValue << endl;
                    assert(nextDigitValue != -1);
                    replacementTrailingDigits += '0' + nextDigitValue;

                    requiredSquareDigitSum -= nextDigitValue * nextDigitValue;
                    numTrailingDigitsToReplace--;
                    //cout << " new requiredSquareDigitSum: " << requiredSquareDigitSum << endl;
                }
                assert(!replacementTrailingDigits.empty());
                largestThig = max(largestThig, dbgRequiredSquareDigitSum);
                largestThog = max(largestThog, dbgNumTrailingDigitsToReplace);

                if (bestReplacementDigits.empty() || isNumericallyLessThan(replacementTrailingDigits, bestReplacementDigits))
                {
                    bestReplacementDigits = replacementTrailingDigits;
                    //cout << "New bestReplacementDigits: " << replacementTrailingDigits << endl;

                }
                break;
            }

            numTrailingDigitsToReplace++;

        }
        nextSquareRoot++;
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
    const int maxVal = 2040;
    const int maxNumDigits = 30;
    vector<vector<int>> sumOfSquaresLookup(maxNumDigits + 1, vector<int>(maxVal + 1, -1));
    sumOfSquaresLookup[0][0] = 1;
    for (int numDigits = 1; numDigits <= maxNumDigits; numDigits++)
    {
        for (int digitVal = 9; digitVal >= 1; digitVal--)
        {
            for (int val = 0; val <= maxVal; val++)
            {
                if (sumOfSquaresLookup[numDigits - 1][val] == -1)
                    continue;
                if (val + digitVal * digitVal <= maxVal)
                {
                    sumOfSquaresLookup[numDigits][val + digitVal * digitVal] = digitVal;
                }
            }
        }
    }
    for (int val = 1; val <= maxVal; val++)
    {
        bool canGenerate = false;
        for (int numDigits = 1; numDigits <= maxNumDigits; numDigits++)
        {
            //cout << "val: " << val << " numDigits: " << numDigits << " sumOfSquaresLookup[numDigits][val]: " << sumOfSquaresLookup[numDigits][val] << endl;
            if (sumOfSquaresLookup[numDigits][val] != -1)
            {
                canGenerate = true;
                break;
            }
        }
        assert(canGenerate);
    }
    for (int numDigits = 1; numDigits <= maxNumDigits; numDigits++)
    {
        for (int val = 1; val <= maxVal; val++)
        {
            //cout << "val: " << val << " numDigits: " << numDigits << " sumOfSquaresLookup[numDigits][val]: " << sumOfSquaresLookup[numDigits][val] << endl;
        }
    }
    {
        for (int N = 1; N <= 1'000'000; N++)
        {
            cout << "N: " << N << endl;
            //const auto solutionBruteForce = solveBruteForce(N);
            //cout << "solutionBruteForce: " << solutionBruteForce << endl;
            const auto solutionOptimised = solveOptimised(N, sumOfSquaresLookup);
            cout << "solutionOptimised:  " << solutionOptimised << endl;
            //assert(solutionOptimised == solutionBruteForce);
        }
    }
    cout << "largestThig: " << largestThig << endl;
    cout << "largestThog: " << largestThog << endl;

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
        const auto solutionOptimised = solveOptimised(N, sumOfSquaresLookup);
        cout << "solutionOptimised:  " << solutionOptimised << endl;

        assert(solutionOptimised == solutionBruteForce);
#endif
#else
        const auto solutionOptimised = solveOptimised(N, sumOfSquaresLookup);
        cout << solutionOptimised << endl;
#endif
    }

    assert(cin);
}
