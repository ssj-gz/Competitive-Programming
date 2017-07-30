#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cassert>
using namespace std;

const long m = 1000000000 + 7;

// NB: quickPower was copied from a solution  to another challenge 
// I solved - "Matrix Tracing", I think(?)
long quickPower(long n, int m, int modulus)
{
    // Raise n to the m mod modulus using as few multiplications as 
    // we can e.g. n ^ 8 ==  (((n^2)^2)^2).
    long result = 1;
    int power = 0;
    while (m > 0)
    {
        if (m & 1)
        {
            long subResult = n;
            for (int i = 0; i < power; i++)
            {
                subResult = (subResult * subResult) % modulus;
            }
            result = (result * subResult) % modulus;
        }
        m >>= 1;
        power++;
    }
    return result;
}

long nCr(long n, long r)
{
    if (n < r)
        return 0;
    if (n == r)
        return 1;
    if (r == 0)
        return 1;

    assert(r == 2 || r == 4);
    const static long inverseOf24Factorial = quickPower(24, m - 2, m);
    const static long inverseOf2 = quickPower(2, m - 2, m);
    assert((inverseOf24Factorial * 24) % m == 1);
    assert((inverseOf2 * 2) % m == 1);

    if (r == 2)
    {
        return (((n * (m + n - 1)) % m) * inverseOf2) % m;
    }
    else
    {
        long result = inverseOf24Factorial;
        for (int i = 0; i < 4; i++)
        {
            result = (result * (m + n - i)) % m;
        }
        return result;
    }
    assert(false);
    return -1;
}

long numShortPalindromes(const string& s)
{
    const string::size_type stringLen = s.size();
    // ---
    // NB: immediately after submitting this, I thought of a much simpler way of 
    // doing it, although this simpler way would take O(n) space.
    // I prefer the original approach, below: although it is more complex, it uses 
    // O(1) space (and a very small amount, at that), and with a little
    // work could be transformed into an "online" class: hand an object of that class
    // a character, and it would immediately be able to tell you how many Short Palindromes
    // are possible with the characters fed to it so far, all without allocating any memory.
    // ---
    // NB2: Some of the ASCII diagrams below rely on a monospace font which, bewilderingly,
    // hackerrank doesn't use :/

    // Basic approach: we express a "short-palindrome" as OIIO, where
    // "O" represents the "outer" character, and "I" the "inner".
    // We ignore the case where O == I (i.e. where the palindrome is 
    // just the same character repeated four times) until near the end.

    // Given 
    //  YYYYYYYYYOXXXXXXXXO
    //           ^iLast   ^i

    // where Y can be any char and X is any char but "O", we can find the 
    // number of palindromes of the form OIIO for each I occurring in the prefix of s
    // up to and including index i based on the number
    // of such palindromes occurring in the prefix of s up to (and including)
    // index iLast. The O at "i" is called a "capping outer".  Any occurrences of I
    // amongst the X's are called "potential inner I for outer O"'s.
    // The rules for deducing the new number of OIIO palindromes up to index i are 
    // found below: see the comment "We've found a capping outer".

    // This leaves the problem of calculating the number of OIIO the very first time
    // we know that there exists a palindrome OIIO for outer O and inner I.
    // Consider the prefix:

    //   YYYYOXXXXI
    //            ^i

    // where Y is any char but O and X is any char but I.  We say that the I at index i
    // *primes* the outer O: no OIIO palindromes can occur until O is primed by I.
    // The number of O's occurring before I primed O, and the number of I's that occurred since
    // I primed O are pivotal values in calculating the first "batch" of OIIO palindromes;
    // see the comment "First palindrome with this inner and outer" for more detail
    // on how this is performed.

    // Once we've found all OIIO palindromes for all chars O and I where  O != I, it is a simple matter to 
    // find all XXXX-palindromes: it is simply the number of occurrences of X, choose 4.
    const int numLetters = 26;
    long numSeen[numLetters] = {0};
    long numPrimedOuterByInner[numLetters][numLetters] = { { 0} };
    long numInnerSincePrimingOuter[numLetters][numLetters] = { { 0} };
    long numPalindromesOuterInner[numLetters][numLetters] = { {0} };
    bool hasPalindromeOuterInner[numLetters][numLetters] = { { false }};
    long numPalindromesWithDistinctFirst3OuterInner[numLetters][numLetters] = { {0} };
    long numPotentialInnersForOuter[numLetters][numLetters] = { {0} };
    long numHalfPalindromesOuterInner[numLetters][numLetters] = {{0}};

    for (string::size_type i = 0; i < stringLen; i++)
    {
        const int charIndex = s[i] - 'a';
        for (int outer = 0; outer < numLetters; outer++)
        {
            const int inner = charIndex;
            if (inner == outer)
                continue;

            // When we see a first inner, count the number of outers preceding it.
            if (numPrimedOuterByInner[outer][inner] == 0 && numSeen[outer] > 0)
            {
                numPrimedOuterByInner[outer][inner] = numSeen[outer];
            }
            // Update the number of inner's since we saw the very first outer.
            if (numPrimedOuterByInner[outer][inner] > 0)
            {
                numInnerSincePrimingOuter[outer][inner]++;
            }
            // Keep track of the number of inner's since the last outer.
            // These won't contribute anything to OIIO until we reach
            // a capping outer, hence the name "potential".
            if (numSeen[outer] > 0)
            {
                numPotentialInnersForOuter[outer][inner]++;
            }
            // Update "half palindromes".
            numHalfPalindromesOuterInner[outer][inner] += numSeen[outer];

        }
        for (int inner = 0; inner < numLetters; inner++)
        {
            const int outer = charIndex;
            if (inner == outer)
                continue;
            if (hasPalindromeOuterInner[outer][inner])
            {
                // We've reached a capping outer; update the number of OIIO palindromes, based on
                // the last time we saw an outer.  Let iLast be the index of the last outer we saw.
                // For every palindrome ending at or before iLast, where the first 3 indices are distinct,
                // we can extend to a new palindrome ending at the current outer simply by replacing the 
                // 4th index by the current index.
                // This leaves the number of OIIO palindromes found so far with 3 the first 3 indices
                // distinct completely unchanged.
                numPalindromesOuterInner[outer][inner] = (numPalindromesOuterInner[outer][inner] + numPalindromesWithDistinctFirst3OuterInner[outer][inner]) % m;
                if (numPotentialInnersForOuter[outer][inner] >= 2)
                {
                    // For each occurrence o of outer prior to or at iLast, we can form a new palindrome ending at the current outer
                    //  formed of o plus 2 of the potential inners plus the current outer.
                    // The number of occurrences of outer prior to or at iLast is of course the number of outer's 
                    // seen so far, excepting the current one.
                    const long numNewPalindromesDueToPairsOfPotentials = (numSeen[outer] * nCr(numPotentialInnersForOuter[outer][inner], 2)) % m;
                    numPalindromesOuterInner[outer][inner] = (numPalindromesOuterInner[outer][inner] + numNewPalindromesDueToPairsOfPotentials) % m;
                    // Each of these new palindromes we formed has the first three elements distinct.
                    numPalindromesWithDistinctFirst3OuterInner[outer][inner] = (numPalindromesWithDistinctFirst3OuterInner[outer][inner] + numNewPalindromesDueToPairsOfPotentials) % m;
                }
                // For each half-palindrome ending before or at iLast, we can extend it to a new palindrome ending at the current outer by incorporating one of the 
                // potential inners plus the current outer.
                const long numHalfPalindromesBeforeOrAtILast = numHalfPalindromesOuterInner[outer][inner] - numPotentialInnersForOuter[outer][inner] * numSeen[outer]; // i.e. remove the pairs ending at one of the potentials.
                const long numNewPalindromesDueToHalfPalindromessPlusOnePotential = numHalfPalindromesBeforeOrAtILast * numPotentialInnersForOuter[outer][inner];
                numPalindromesOuterInner[outer][inner] = (numPalindromesOuterInner[outer][inner] + numNewPalindromesDueToHalfPalindromessPlusOnePotential) % m;
                // Each of these new palindromes we formed has the first three elements distinct.
                numPalindromesWithDistinctFirst3OuterInner[outer][inner] = (numPalindromesWithDistinctFirst3OuterInner[outer][inner] + numNewPalindromesDueToHalfPalindromessPlusOnePotential) % m;
            }

        }
        for (int inner = 0; inner < numLetters; inner++)
        {
            const int outer = charIndex;
            if (!hasPalindromeOuterInner[outer][inner] && numPrimedOuterByInner[outer][inner] > 0 &&
                    numInnerSincePrimingOuter[outer][inner] >= 2)
            {
                // First palindrome with this inner and outer.
                // Each of the triples with one of the outer's occurring before the first inner plus two of the inners 
                // found since the first outer can be extended to a short palindrome ending at the current outer
                // simply by incorporating the current outer.
                long numNewPalindromes = (numPrimedOuterByInner[outer][inner] * nCr(numInnerSincePrimingOuter[outer][inner], 2)) % m;
                // Every outer occurring since the inner primed the first outer can be matched with a pair of potentials, plus the 
                // current outer, to form a new palindrome e.g.
                // OOOIOOOOIIIO
                //    ^ here, the inner primed the outer.
                //     ^^^^ any of these four outers can be matched with any pair of the next 3 potentials plus the capping O to form a new palindrome.
                if (numSeen[outer] != numPrimedOuterByInner[outer][inner])
                {
                    const long numOutersSinceInnerPrimedOuter = numSeen[outer] - numPrimedOuterByInner[outer][inner]; 
                    numNewPalindromes = (numNewPalindromes + (numOutersSinceInnerPrimedOuter * nCr(numPotentialInnersForOuter[outer][inner], 2))) % m;
                }
                numPalindromesOuterInner[outer][inner] = (numPalindromesOuterInner[outer][inner] + numNewPalindromes) % m;
                hasPalindromeOuterInner[outer][inner] = true;
                // Each of these new palindromes we formed has the first three elements distinct.
                numPalindromesWithDistinctFirst3OuterInner[outer][inner] = numNewPalindromes;
            }
        }

        for (int inner = 0; inner < numLetters; inner++)
        {
            // Reset inner potentials when we encounter an outer.
            const int outer = charIndex;
            numPotentialInnersForOuter[outer][inner] = 0;
        }

        numSeen[charIndex]++;

    }
    long numPalindromes = 0;
    // Add up the palindromes where the inner and outer letters are distinct.
    for (int inner = 0; inner < numLetters; inner++)
    {
        for (int outer = 0; outer < numLetters; outer++)
        {
            if (outer == inner)
                continue;
            numPalindromes = (numPalindromes + numPalindromesOuterInner[outer][inner]) % m;
        }
    }
    // Add up the palindromes where the outer and inner are the same.
    for (int letter = 0; letter < numLetters; letter++)
    {
        const long numPalindromesJustThisLetter = nCr(numSeen[letter], 4);
        numPalindromesOuterInner[letter][letter] = numPalindromesJustThisLetter;
        numPalindromes = (numPalindromes + numPalindromesJustThisLetter) % m;
    }

    return numPalindromes;
}


int main() {
    string s;
    cin >> s;
    cout << numShortPalindromes(s);
    return 0;
}
