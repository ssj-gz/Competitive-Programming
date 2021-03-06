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
    return lhsNumAsString < rhsNumAsString; // Same number of digits? Lexicographical comparison will work.
}

string findMinBeautifulNumberWithNDigits(int N, const vector<vector<int>>& sumOfSquaredDigitsLookup)
{
    const int64_t squareDigitSumOrig = N;

    if (isSquare(squareDigitSumOrig))
        return string (N, '1');

    const int sqrtN = sqrt(N);
    assert(sqrtN * sqrtN < N);
    int64_t nextSquareRoot = sqrtN + 1;
    assert(nextSquareRoot * nextSquareRoot > N);

    string bestReplacementDigits;
    while (true)
    {
        const int64_t nextSquare = nextSquareRoot * nextSquareRoot;
        int numTrailingDigitsToReplace = 1;
        while (numTrailingDigitsToReplace < sumOfSquaredDigitsLookup.size())
        {
            // Sum of digits squared if we lopped off the last numTrailingDigitsToReplace of the N 1's.
            const int64_t sumOfDigitsSquared = squareDigitSumOrig - numTrailingDigitsToReplace; 

            int64_t requiredSquareDigitSum = nextSquare - sumOfDigitsSquared;

            if (!bestReplacementDigits.empty() && requiredSquareDigitSum > 9 * 9 * bestReplacementDigits.length())
            {
                // With each further iteration, requiredSquareDigitSum is just going to keep on increasing,
                // and it's already sufficiently large that we'll need to replace more digits than are in
                // bestReplacementDigits - the current value of bestReplacementDigits is the best we're ever
                // going to get.
                return string(N - bestReplacementDigits.length(), '1') + bestReplacementDigits;
            }

            if (sumOfSquaredDigitsLookup[numTrailingDigitsToReplace][requiredSquareDigitSum] != -1)
            {
                // We can form requiredSquareDigitSum using numTrailingDigitsToReplace digits - use
                // our dp lookup table sumOfSquaredDigitsLookup to reconstruct the smallest numTrailingDigitsToReplace
                // digit number whose sum of digits square is requiredSquareDigitSum.
                string replacementTrailingDigits;
                while (requiredSquareDigitSum > 0)
                {
                    const int nextDigitValue = sumOfSquaredDigitsLookup[numTrailingDigitsToReplace][requiredSquareDigitSum];
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
                // We've found the best value for this value of nextSquare; skip to the next.
                break;
            }

            numTrailingDigitsToReplace++;
        }
        nextSquareRoot++;
    }

    assert(false); // There is no N such that no N-digit number is beautiful.
    return "";
}


int main(int argc, char* argv[])
{
    // QUICK EXPLANATION
    //
    // The smallest N-digit number with no 0's (leading or otherwise) is the number
    // consist of N 1's, and the sum-of-digit-squares for this number is N.
    // If N is square, then we're done; else, it makes sense to try and find a way,
    // for each successive square nextSquare > N, for each number of digits d = 1, 2, ...
    // to replace the last d of the N 1's with a d-digit number such that the newly-formed
    // number has a sum-of-digits-squared equal to nextSquare, and to find the smallest
    // replacement number that achieves this.
    //
    // Knowing when to stop considering new values of nextSquare requires a teeny little
    // bit of analysis.  Finding the maximum value of d (maxNumDigits) that we need to
    // consider before we can stop looking could probably be done with some analysis,
    // but I just went ahead and found it by exhaustively solving for all N = 1, 2, ... 1'000'000 :p
    //
    // We find that we only have to consider a few values of d and a few values of nextSquare
    // in order to be guaranteed to find the result for any given N up to 1'000'000 (computing the 
    // answer for such an N is very fast), and also that there is no such N such that no N-digit 
    // number is beautiful.
    //
    // LONGER EXPLANATION
    //
    // The code and comments hopefully speak for themselves - analysing the approximate run-time is the
    // hardest part, probably.
    //
    // TODO - just Skeletal notes at this point.
    //
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
    //
    // For X = 1 ... 80, we need at most 4 digits to get a number with sum-of-digit-squares = X (revealed by exhaustive search over such X).
    // Corollary: The maximum number of digits to form any X is X / (9 * 9) + 4.  (Prepend a 9 and decrease X by 9 * 9 - after
    // X / (9 * 9) steps, X will be less than 81, and so will need at most 4 further digits).
    //
    // Similarly, the maximum sum-of-digit-squares from a d-digit number is d * 9 * 9 - corollary: X requires at least X / (9 * 9) digits.
    // The difference between N and the next square after N is (I think) bounded by 2 x sqrt(N) + 1, so the first requiredSquareDigitSum isn't
    // going to be huge.  The one after that also isn't that much bigger - is the next value of requiredSquareDigitSum bounded by 2 * requiredSquareDigitSum,
    // approximately?

    ios::sync_with_stdio(false);
    const int maxSquareDigitSumValue = 2040;     // Arrived at empirically by exhaustively exploring all N = 1 to 1'000'000.  Occurs at N = 999987.
    const int maxNumDigits = 27;                 //     "                "                "                  "             .  Occurs at N = 958428, when trying to make the square 960400 = 980^2.
    // sumOfSquaredDigitsLookup[d][v] gives the first digit in the minimum decimal value
    // of exactly d digits (no 0's, leading or otherwise) whose sum-of-digits-square is v, or -1 if there is no
    // such d-digit decimal.
    vector<vector<int>> sumOfSquaredDigitsLookup(maxNumDigits + 1, vector<int>(maxSquareDigitSumValue + 1, -1));
    sumOfSquaredDigitsLookup[0][0] = 1;
    for (int numDigits = 1; numDigits <= maxNumDigits; numDigits++)
    {
        // Count down the digits, so that if multiple leading digits give
        // the same square-digit-sum value, only the smallest (and thus, the one corresponding
        // to the minimal decimal value) is stored.
        for (int digitVal = 9; digitVal >= 1; digitVal--)
        {
            for (int squareDigitSumValue = 0; squareDigitSumValue <= maxSquareDigitSumValue; squareDigitSumValue++)
            {
                if (sumOfSquaredDigitsLookup[numDigits - 1][squareDigitSumValue] == -1)
                    continue;
                if (squareDigitSumValue + digitVal * digitVal <= maxSquareDigitSumValue)
                {
                    sumOfSquaredDigitsLookup[numDigits][squareDigitSumValue + digitVal * digitVal] = digitVal;
                }
            }
        }
    }

    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();

        cout << findMinBeautifulNumberWithNDigits(N, sumOfSquaredDigitsLookup) << endl;
    }

    assert(cin);
}
