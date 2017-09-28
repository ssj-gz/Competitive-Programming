// Simon St James (ssjgz) 2017-09-28 15:21
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cassert>

using namespace std;

bool isPrime(int n)
{
    if (n < 2)
        return false;
    if (n == 2)
        return true;
    for (int i = 2; i <= sqrt(n); i++)
    {
        if ((n % i) == 0)
            return false;
    }
    return true;
}

int main()
{
    // Fairly tricky. As an intermediate step, introduce "almost-Chloe numbers" : these are 4-digit numbers
    // (possibly with leading 0s) with each consecutive triple of digits and each consecutive quadruple of 
    // digits summing to primes e.g. 0025, 0302 and 6526 are all almost-Chloe numbers. Let semi-Chloe numbers
    // be Chloe numbers which may begin with 0.
    // The first four digits of any Chloe Number is always an almost-Chloe number. Conversely, let 
    // ABCDxxxxx be any semi-Chloe number (i.e. A = 0 is allowed); then, prepending a digit
    // d (possibly with d = 0) will give a semi-Chloe-number if and only if
    // d + A + B + C + D is prime and dABC is an almost-Chloe number (simple; proof is left as an exercise for the reader ;)).
    //
    // So imagine we maintained, for each successive number of digits n, an array (a, say) where the ABCDth entry (ABCD is a 4-digit number, so 
    // the array would have 10000 entries) is the number of semi-Chloe-numbers with n digits beginning with ABCD. 
    // How do we update this numbers with n + 1 digits?
    //
    // Well, for each of the ABCDs, we could try prepending each digit d = 0 ... 9 in turn; we know that we'll have a semi-Chloe-number 
    // if and only if d + A + B + C is prime and dABC is an almost-Chloe number; a simple
    // recurrence relation. Then we'll easily be able to use this to build a lookup, for each n, of the number of Chloe numbers
    // with n digits (it's the sum over all ABCD, A != 0, of a[ABCD]).
    //
    // Unfortunately, this is a little computationally intractable; there are 10000 steps to build the array for each successive
    // number of digits, and 400'000 digits - far too big :/
    //
    // Of course, we don't actually need all 10000 quadruples ABCD in our array a; we could instead index into a with the
    // index of the almost-Chloe number ABCD in the list of all almost-Chloe numbers.  The number of almost-Chloe numbers 
    // is much less than 10000, so this becomes more tractable.
    //
    // But we can do better: we don't need to find all d to prepend at every iteration; rather, we can build a lookup table of dABC for
    // each ABCD such that d + A + B + C is prime and dABC is an almost-Chloe number.  This is almostChloeNumberExtensionIndexLookup,
    // and if ABCD has index i in the list of all almost-Chloe numbers, and if d1, ... dk are digits that satisfy di + A + B + C
    // and diABC is an almost-Chloe number with index ji, then almostChloeNumberExtensionIndexLookup[i] would give the list j1, j2 ... jk.
    // This lookup table allows us to be even more efficient.
    //
    // And that's it: once we have the numChloeNumbersWithNDigits array built up with our recurrence relation and the step that filters out
    // numbers that are semi-Chloe but not full-Chloe, we just have to look this up for each query n.

    const int64_t modulus = 1'000'000'007ULL;
    const int numCounterDigits = 4;
    vector<int> digits(numCounterDigits);
    vector<int> almostChloeNumbers;

    vector<int64_t> numChloeNumbersWithNDigits;
    numChloeNumbersWithNDigits.push_back(0);
    numChloeNumbersWithNDigits.push_back(0);
    numChloeNumbersWithNDigits.push_back(0);
    numChloeNumbersWithNDigits.push_back(0);

    while (true)
    {
        bool isAlmostChloeNumber = true;
        bool isAlmostChloeNumberIgnoringLeading0s = true;
        int numDigits = 0;
        for (int i = 0; i < numCounterDigits; i++)
        {
            if (digits[i] != 0)
                numDigits = i + 1;
        }
        for (int numInSequence = 3; numInSequence <= 5; numInSequence++)
        {
            for (int i = 0; i < numCounterDigits - numInSequence + 1; i++)
            {
                int sequenceSum = 0;
                for (int j = i; j < i + numInSequence; j++)
                {
                    sequenceSum += digits[j];
                }
                if (!isPrime(sequenceSum))
                {
                    isAlmostChloeNumber = false;
                    if (i + numInSequence <= numDigits)
                    {
                        // Looks like Chloe-ness is actually an "innocent until proven guilty" kind of thing, so
                        // a number with 3 digits can be a Chloe number.
                        isAlmostChloeNumberIgnoringLeading0s = false;
                    }
                    break;
                }
            }
        }
        if (isAlmostChloeNumber)
        {
            int almostChloeNumber = 0;
            for (const auto digit : vector<int>(digits.rbegin(), digits.rend()))
            {
                almostChloeNumber *= 10;
                almostChloeNumber += digit;
            }
            almostChloeNumbers.push_back(almostChloeNumber);
        }
        if (isAlmostChloeNumberIgnoringLeading0s)
        {
            numChloeNumbersWithNDigits[numDigits]++;
        }

        int digitIndex = 0;
        while (digitIndex < numCounterDigits && digits[digitIndex] == 9)
        {
            digits[digitIndex] = 0;
            digitIndex++;
        }
        if (digitIndex == numCounterDigits)
            break;
        digits[digitIndex]++;
    }

    // Build the lookup table, almostChloeNumberExtensionIndexLookup.
    vector<vector<int>> almostChloeNumberExtensionIndexLookup(almostChloeNumbers.size());
    for (int i = 0; i < almostChloeNumbers.size(); i++)
    {
        const int almostChloeNumber = almostChloeNumbers[i];
        for (int digit = 0; digit <= 9; digit++)
        {
            // If d is digit, and almostChloeNumber = ABCD, then extendedAlmostChloeNumber = dABCD.
            const int extendedAlmostChloeNumber = (digit * 10000) + almostChloeNumber;
            int digitSum = 0;
            for (const auto digitChar : to_string(extendedAlmostChloeNumber))
            {
                digitSum += digitChar - '0';
            }
            if (isPrime(digitSum))
            {
                // If d is digit, then the almostChloeNumber ABCD can be extended to dABCD (extendedAlmostChloeNumber) which has a prime sum.
                // Note that in this case, dABCD is a semi-Chloe Number if and only if dABC is an Almost Chloe Number.  dABC is extendedAlmostChloeNumber / 10.
                const auto shiftedAlmostChloeNumberIter = find(almostChloeNumbers.begin(), almostChloeNumbers.end(), extendedAlmostChloeNumber / 10);
                if (shiftedAlmostChloeNumberIter != almostChloeNumbers.end())
                {
                    const int shiftedAlmostChloeNumberIndex = distance(almostChloeNumbers.begin(), shiftedAlmostChloeNumberIter);
                    almostChloeNumberExtensionIndexLookup[i].push_back(shiftedAlmostChloeNumberIndex);
                }
            }
        }
    }

    const int maxN = 400'000;
    // Will be updated for each new digit, via nextNumChloeNumbersBeginningWithAlmostChloeNumber.  Actually, numChloeNumbersBeginningWithAlmostChloeNumber[i]
    // is the number of numbers with the current number of digits, beginning with the almostChloeNumber with index i.
    vector<int64_t> numChloeNumbersBeginningWithAlmostChloeNumber(almostChloeNumbers.size(), 1); 

    for (int numDigits = 5; numDigits <= maxN + 1 ; numDigits++)
    {
        int64_t numChloeNumbers = 0;
        for (int almostChloeNumberIndex = 0; almostChloeNumberIndex < almostChloeNumbers.size(); almostChloeNumberIndex++)
        {
            const int almostChloeNumber = almostChloeNumbers[almostChloeNumberIndex];
            if (almostChloeNumber >= 1000) // Only count those without leading 0's.
                numChloeNumbers = (numChloeNumbers + numChloeNumbersBeginningWithAlmostChloeNumber[almostChloeNumberIndex]) % modulus;
        }
        numChloeNumbersWithNDigits.push_back(numChloeNumbers);
        vector<int64_t> nextNumChloeNumbersBeginningWithAlmostChloeNumber(almostChloeNumbers.size());
        for (int almostChloeNumberIndex = 0; almostChloeNumberIndex < almostChloeNumbers.size(); almostChloeNumberIndex++)
        {
            for (const auto extendedAlmostChloeNumberIndex : almostChloeNumberExtensionIndexLookup[almostChloeNumberIndex])
            {
                nextNumChloeNumbersBeginningWithAlmostChloeNumber[extendedAlmostChloeNumberIndex] = (nextNumChloeNumbersBeginningWithAlmostChloeNumber[extendedAlmostChloeNumberIndex] + numChloeNumbersBeginningWithAlmostChloeNumber[almostChloeNumberIndex]) % modulus;
            }
        }
        numChloeNumbersBeginningWithAlmostChloeNumber = nextNumChloeNumbersBeginningWithAlmostChloeNumber;
    }
    assert(numChloeNumbersWithNDigits.size() == maxN + 1);

    int Q;
    cin >> Q;

    for (int q = 0; q < Q; q++)
    {
        int n;
        cin >> n;
        cout << numChloeNumbersWithNDigits[n] << endl;
    }
}
