// Simon St James (ssjgz) - 2019-11-03
// 
// Solution to: https://www.codechef.com/NOV19A/problems/LSTBTF
//
#include <iostream>
#include <vector>
#include <cmath>

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

bool isSquare(const int N)
{
    const int sqrtN = sqrt(N);
    return (sqrtN * sqrtN == N) || ((sqrtN + 1) * (sqrtN + 1) == N);
}

bool isNumericallyLessThan(const string& lhsNumAsString, const string& rhsNumAsString)
{
    if (lhsNumAsString.length() < rhsNumAsString.length())
        return true;
    if (lhsNumAsString.length() > rhsNumAsString.length())
        return false;
    return lhsNumAsString < rhsNumAsString;
}

string findMinBeautifulNumberWithNDigits(int N, const vector<vector<int>>& sumOfSquaresLookup)
{
    string result;

    string numberAsString(N, '1');

    const int64_t squareDigitSumOrig = N;

    if (isSquare(squareDigitSumOrig))
        return numberAsString;

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

            int64_t requiredSquareDigitSum = nextSquare - squareDigitSum;
            const int64_t dbgRequiredSquareDigitSum = requiredSquareDigitSum;
            const auto dbgNumTrailingDigitsToReplace = numTrailingDigitsToReplace;

            if (!bestReplacementDigits.empty() && requiredSquareDigitSum > 9 * 9 * bestReplacementDigits.length())
            {
                return string(N - bestReplacementDigits.length(), '1') + bestReplacementDigits;
            }

            if (sumOfSquaresLookup[numTrailingDigitsToReplace][requiredSquareDigitSum] != -1)
            {
                string replacementTrailingDigits;
                while (requiredSquareDigitSum > 0)
                {
                    const int nextDigitValue = sumOfSquaresLookup[numTrailingDigitsToReplace][requiredSquareDigitSum];
                    assert(nextDigitValue != -1);
                    replacementTrailingDigits += '0' + nextDigitValue;

                    requiredSquareDigitSum -= nextDigitValue * nextDigitValue;
                    numTrailingDigitsToReplace--;
                }
                assert(!replacementTrailingDigits.empty());

                if (bestReplacementDigits.empty() || isNumericallyLessThan(replacementTrailingDigits, bestReplacementDigits))
                {
                    bestReplacementDigits = replacementTrailingDigits;
                }
                break;
            }

            numTrailingDigitsToReplace++;

        }
        nextSquareRoot++;
    }


    return "";
}


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
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
    const int maxVal = 2040;     // Arrived at empirically by exploring all N = 1 - 1'000'000.
    const int maxNumDigits = 30; //     "                "                "                
    // sumOfSquaresLookup[d][v] gives the first digit in the minimum representation of v
    // using exactly d digits if v can be expressed with d digits, else -1.
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
        int minDigits = 1000;
        for (int numDigits = 1; numDigits <= maxNumDigits; numDigits++)
        {
            //cout << "val: " << val << " numDigits: " << numDigits << " sumOfSquaresLookup[numDigits][val]: " << sumOfSquaresLookup[numDigits][val] << endl;
            if (sumOfSquaresLookup[numDigits][val] != -1)
            {
                canGenerate = true;
                //break;
                minDigits = min(minDigits, numDigits);
            }
        }
        //cout << "val: " << val << " can be generated with " << minDigits << " digits.   Less than upper bound: = " << ((val / 81) + 4 - minDigits) << endl;
        assert(minDigits <= (val / 81) + 4);
        assert(canGenerate);
    }
    for (int numDigits = 1; numDigits <= maxNumDigits; numDigits++)
    {
        for (int val = 1; val <= maxVal; val++)
        {
            //cout << "val: " << val << " numDigits: " << numDigits << " sumOfSquaresLookup[numDigits][val]: " << sumOfSquaresLookup[numDigits][val] << endl;
        }
    }
#if 0
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
#endif

    const auto T = read<int>();


    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();

        cout << findMinBeautifulNumberWithNDigits(N, sumOfSquaresLookup) << endl;
    }

    assert(cin);
}
