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

int largestNumIterations = 0;
int64_t largestSumToMake = 0;
int largestNumDigits = 0;
string findMinBeautifulNumberWithNDigits(int N, const vector<vector<int>>& sumOfSquaresLookup)
{
    const int64_t squareDigitSumOrig = N;

    if (isSquare(squareDigitSumOrig))
        return string (1, '1');

    const int sqrtN = sqrt(N);
    assert(sqrtN * sqrtN < N);
    int64_t nextSquareRoot = sqrtN + 1;
    assert(nextSquareRoot * nextSquareRoot > N);

    string bestReplacementDigits;

    int numIterations = 0;
    while (true)
    {
        const int64_t nextSquare = nextSquareRoot * nextSquareRoot;
        cout << "N: " << N << " nextSquare: " << nextSquare << endl;
        //cout << "nextSquare: " << nextSquare << " N: " << N << endl;
        int numTrailingDigitsToReplace = 1;
        while (numTrailingDigitsToReplace < sumOfSquaresLookup.size())
        {
            //cout << "numTrailingDigitsToReplace: " << numTrailingDigitsToReplace << endl;
            const int64_t squareDigitSum = squareDigitSumOrig - numTrailingDigitsToReplace;

            int64_t requiredSquareDigitSum = nextSquare - squareDigitSum;
            const int64_t dbgRequiredSquareDigitSum = requiredSquareDigitSum;
            const auto dbgNumTrailingDigitsToReplace = numTrailingDigitsToReplace;

            if (!bestReplacementDigits.empty() && requiredSquareDigitSum > 9 * 9 * bestReplacementDigits.length())
            {
                //cout << "N: " << N << " Took: " << numIterations << " iterations" << endl;
                return /*string(N - bestReplacementDigits.length(), '1') +*/ bestReplacementDigits;
                //return string(N - bestReplacementDigits.length(), '1') + bestReplacementDigits;
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

                cout << "Making " << dbgRequiredSquareDigitSum << " requires: " << replacementTrailingDigits /*<< " (" << (string(N - replacementTrailingDigits.length(), '1') + replacementTrailingDigits) << ")"*/;

                if (bestReplacementDigits.empty() || isNumericallyLessThan(replacementTrailingDigits, bestReplacementDigits))
                {
                    cout << " (better than previous best " << bestReplacementDigits << ")";
                    bestReplacementDigits = replacementTrailingDigits;
                    largestNumIterations = max(largestNumIterations, numIterations);
                    if (dbgRequiredSquareDigitSum > largestSumToMake )
                    {
                        largestSumToMake = max(largestSumToMake, dbgRequiredSquareDigitSum);
                        //cout << endl << "new largest largestSumToMake: " << dbgRequiredSquareDigitSum << " N: " << N << endl;
                    }
                    if (largestNumDigits <  dbgNumTrailingDigitsToReplace)
                    {
                        largestNumDigits = dbgNumTrailingDigitsToReplace;
                        //cout << endl << "new largest largestNumDigits: " << largestNumDigits << " N: " << N << endl;
                        
                    }
                    //cout << "N: " << N << " stilluseful: " << numIterations << endl;
                }
                cout << endl;
                break;
            }

            numTrailingDigitsToReplace++;

        }
        nextSquareRoot++;
        numIterations++;
        //largestNumIterations = max(largestNumIterations, numIterations);
    }


    return "";
}


int main(int argc, char* argv[])
{
//  Example about having to check multiple squares.  N = 23 is the one where we still getting different results after the most number of iterations.
//    N: 23 nextSquare: 25
//    N: 23 nextSquare: 36
//    N: 23 nextSquare: 49
//    Making 29 requires: 234 (11111111111111111111234) (better than previous best )
//    N: 23 nextSquare: 64
//    Making 44 requires: 226 (11111111111111111111226) (better than previous best 234)
//    N: 23 nextSquare: 81
//    Making 61 requires: 346 (11111111111111111111346)
//    N: 23 nextSquare: 100
//    Making 81 requires: 2238 (11111111111111111112238)
//    N: 23 nextSquare: 121
//    Making 100 requires: 68 (11111111111111111111168) (better than previous best 226)
//    N: 23 nextSquare: 144
//    Making 125 requires: 2269 (11111111111111111112269)
//    N: 23 nextSquare: 169
//    Making 149 requires: 289 (11111111111111111111289)
//    N: 23 nextSquare: 196
//    N: 23 Took: 9 iterations
//    11111111111111111111168
    // N = 24 has the most iterations (i.e. square numbers) that must be checked (12 of them).


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
    const int maxVal = 2040;     // Arrived at empirically by exploring all N = 1 - 1'000'000.  Occurs at N = 999987.
    const int maxNumDigits = 27; //     "                "                "                  .  Occurs at N = 958428, when trying to make the square 960400 = 980^2.
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
            //cout << "N: " << N << endl;
            //const auto solutionBruteForce = solveBruteForce(N);
            //cout << "solutionBruteForce: " << solutionBruteForce << endl;
            const auto blah = findMinBeautifulNumberWithNDigits(N, sumOfSquaresLookup);
            cout << "Q: 2 lines" << endl;
            cout << 1 << endl;
            cout << N << endl;
            cout << "A: 1 lines" << endl;
            cout << blah << endl;
            //cout << "solutionOptimised:  " << solutionOptimised << endl;
            //assert(solutionOptimised == solutionBruteForce);
        }
        //cout << "largestThig: " << largestThig << endl;
        //cout << "largestThog: " << largestThog << endl;
        //cout << "largestNumIterations: " << largestNumIterations << endl;
        //cout << "largestSumToMake: " << largestSumToMake <<  endl;
        return 0;
    }
#endif

    const auto T = read<int>();


    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();

        cout << findMinBeautifulNumberWithNDigits(N, sumOfSquaresLookup) << endl;
    }

    assert(cin);
}
