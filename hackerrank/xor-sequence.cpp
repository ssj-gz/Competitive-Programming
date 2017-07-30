// Simon St James (ssjgz) 2017-07-07 13:30:00
#include <iostream>

using namespace std;

uint64_t computeA0XordToAk(uint64_t k)
{
    if (k == 0)
        return 0;
    // Notation: Let m be the mth (0-relative) digit from the right
    // in a binary number; its contribution to that
    // number is then (1 << m).
    // We want to count the contribution of each digit m in A1^A2^...^Ak.
    // Expand a few terms of A1^A2^A3:
    //  A1^A2^A3 =
    //  1^A0^A2^A3 =
    //  1^A0^2^A1^A3 =
    //  1^A0^2^1^A0^A3 =
    //  1^A0^2^1^A0^3^A2 = 
    //  1^A0^2^1^A0^3^2^A1 = 
    //  1^A0^2^1^A0^3^2^1^A0 =
    //  (A0 ^^ 3) ^ (1 ^^ 3) ^ (2 ^^ 2) ^ (3 ^^ 1) ^ (4 ^^ 0) =
    //  (1 ^^ 3) ^ (2 ^^ 2) ^ (3 ^^ 1) ^ (4 ^^ 0).
    // where "x ^^ y" means x ^ x ^ ... ^ x, where x occurs y times.
    // In general, in can be proven by induction that
    // A1^A2^...^Ak = XOR(i = 0 .. k) { i ^^ (k - i + 1) }.
    // Let's sketch out the k = 8 case:
    //
    // i:    term:     i as binary:
    // 0     0 ^^ 9    0000 
    // 1     1 ^^ 8    0001 
    // 2     2 ^^ 7    0010 
    // 3     3 ^^ 6    0011 
    // 4     4 ^^ 5    0100 
    // 5     5 ^^ 4    0101 
    // 6     6 ^^ 3    0110 
    // 7     7 ^^ 2    0111 
    // 8     8 ^^ 1    1000 
    //
    // The value of A1^A2^...^A8 turns out to be 8
    // Let's pick m = 2.  We see that the m appears 7 times
    // in term corresponding to i = 2; 6 times in term corresponding
    // to i = 3; 3 times in term corresponding to i = 6; and twice
    // in term corresponding to i = 7.  It occurs in no other terms, so the
    // total number of occurrences is 7+6+3+2 = 18.  This is even, so
    // the m = 2 digit in A1^A2^...^A8 will be 0: if it had been odd, it would
    // have been 1.
    // In general, we would be able to compute A1^...^Ak using the following pseudo-C++:
    // 
    //   for (int m = 0; m <= 63; m++)
    //   {
    //       if (k < (1 << m))
    //           break; // Neither digit m nor subsequent digits will contribute to the result.
    //       int numM = 0;
    //       for (int i = 0; i <= k; i++)
    //       {
    //           if ((i & (1 << m)) != 0)
    //               numM += (k - i + 1);
    //       }
    //       if ((numM % 2) == 1)
    //           result |= (1 << m);
    //   }
    // 
    // but as you've probably guessed, this is too slow to compute, so we need to be smarter!
    // In the above pseudo-code, we've calculated the number of occurences of m, but really
    // we just need to know whether it's odd or even i.e. its parity.
    // I'm going to ignore m = 1 for the time being, as that is a much more awkward situation!
    // Note that, in a list of x from 0 to some high number, the value of m is cyclic: the
    // first (1 << m) times it is 0, then the next (1 << m) times it is 1, then the next
    // (1 << m) it is 0 again etc - it has period (1 << (m + 1)).
    // Let's split the k terms (ignoring the i = 0 term, which contributes nothing)  into the number
    // of full cycles of m and its remainder: the number of full cycles in k is k / (1 << (m + 1)); 
    // the remainder is k % (1 << (m + 1)).
    // In each "term cycle", note that m will occur in a solid run of (1 << m) terms.  If m > 1,
    // then the length of this run will be divisible by 4.  The contribution of m in each term
    // cycles between an odd number of m and an even number (in the example above, the first cycle
    // gives 7 m's in the first term of its run, and 6 in the next (and final) term in its run - 
    // odd-then-even, in this case).  If the length of this run is divisible by 4, then the sum
    // of the contributions of m will be even (pick any 4 consecutive integers - their sum will be even)
    // and so we can ignore the contributions of the full cycles.
    // There is a special case for m = 1: here, the run is of length 2, and the contribution to m is always
    // odd for each full cycle (7+6 = 13 for the first cycle in the example; 3+2 = 5 for the second) - therefore
    // the contribution to the parity of m from the full cycles for m = 1 is odd if there is an odd number of
    // full cycles and even otherwise.
    // Another special case for m = 0: as we can see from the example above, the contributions of the first full
    // cycle is k; the next full cycle, k - 2; etc.  Thus, the contribution is even if k is even or the number
    // of cycles is even, and odd otherwise i.e. its parity is numFullCycles * k. 
    // This concludes the contribution of full cycles to the parity of m; see fullCycleParity in the code below.
    //
    // We now need to deal with the partial cycle remainder.  Firstly, note that if we ended in the partial cycle
    // before reaching the run of 1's of the m digits - that is, if partialCycleRemainder < cycleLength / 2 -
    // then there are no contributions to m.
    // Otherwise, referring back to our pseudocode above, we need to calculate:
    //
    //       int numM = 0;
    //       for (int i = cycleLength / 2; i <= partialCycleRemainder; i++)
    //       {
    //           if ((i & (1 << m)) != 0)
    //               numM += (k - i + 1);
    //       }
    //       if ((numM % 2) == 1)
    //           result |= (1 << m);
    // 
    // Again, we only care about parity of numM, not the actual value of numM.
    // There are partialCycleRemainder - cycleLength / 2 terms in this expression; factoring out the constant parts
    // (the 'k' and the +1) and negating the "i" (as doing so does not alter the parity of i) we get:

    //       const uint64_t numTerms = partialCycleRemainder - cycleLength / 2;
    //       uint64_t numM = (k + 1) * numTerms;
    //       for (int i = cycleLength / 2; i <= partialCycleRemainder; i++)
    //       {
    //           if ((i & (1 << m)) != 0)
    //               numM += i;
    //       }
    //       if ((numM % 2) == 1)
    //           result |= (1 << m);
    // 
    // Since we are inside a run (as we are assuming partialCycleRemainder >= cycleLength / 2), the m digit will always be 1:
    //
    //       const uint64_t numTerms = partialCycleRemainder - cycleLength / 2;
    //       uint64_t numM = (k + 1) * numTerms;
    //       for (int i = cycleLength / 2; i <= partialCycleRemainder; i++)
    //       {
//               numM += i;
    //       }
    //       if ((numM % 2) == 1)
    //           result |= (1 << m);
    // 
    // So the contribution of the loop will be numTerms of an increasing sequence of numbers, which starts from cycleLength / 2
    // which, for m >= 2 is divisible by 4.  A sum of x terms in an increase sequence of numbers, where the first is divisible
    // by 4, will be even if x modulus 4 is 0 or 1, odd otherwise (try it!).  The m = 1 case also seems to work in this 
    // case, though I'm actually not too sure why, to be honest.  For m = 0, a run will be of length 1 and will always
    // start on an odd number if k is odd, and an even number if k is even (look at the table if this makes things clearer!), 
    // so the contribution of parity for m = 0 will be the parity of k.  That concludes the contribution of the parity cycle 
    // remainder: see partialCycleParity in the code below.
    // And that's about it: we then add the contributions of digit m to the result, as in the pseudo code!
    uint64_t value = 0;
    for (uint64_t m = 0; m <= 63; m++)
    {
        uint64_t twoToTheM = static_cast<uint64_t>(1) << m;
        if (twoToTheM > k)
            break;
        uint64_t cycleLength = twoToTheM * 2;

        uint64_t parityM = 0;

        uint64_t fullCycleParity = 0;
        const uint64_t numFullCycles = (k / cycleLength);
        if (m == 0)
        {
            fullCycleParity += numFullCycles * k;
        }
        else if (m == 1)
        {
            fullCycleParity += numFullCycles;
        }
        parityM += fullCycleParity;

        uint64_t partialCycleParity = 0;
        uint64_t partialCycleRemainder = k % cycleLength;
        if (partialCycleRemainder >= cycleLength / 2)
        {
            const uint64_t numTerms = (partialCycleRemainder - cycleLength / 2 + 1);
            partialCycleParity += (numTerms % 2) * ((k + 1) % 2); // These numbers can be large, so take parity early to avoid overflow!
            if (m != 0)
            {
                partialCycleParity += (((numTerms % 4) == 0) || ((numTerms % 4) == 1)) ? 0 : 1;
            }
            else
            {
                partialCycleParity += k;
            }
            parityM += partialCycleParity;
        }
        if ((parityM % 2) == 1)
            value |= twoToTheM;
    }
    return value;
}

int main()
{
    // God, I'm having a bad day - yet another complete misjudgement of difficulty, followed by not stepping back
    // and re-assessing!
    // Consider 
    //  A0^A1^A2^...^AL-1 ^ A0^A1^A2^...^AR.
    // Re-arranging terms, we have:
    //   (A0^A0)^(A1^A1)^...^(AL-1^AL-1)^AL^AL+1^...^AR =
    //   AL^AL+1^...^AR.
    // Thus, the problem reduces to computing A0^A1^...^Ak for arbitrary k; see computeA0XordToAk for details!
    int Q;
    cin >> Q;
    for (int q = 0; q < Q; q++)
    {
        uint64_t L, R;
        cin >> L >> R;

        const uint64_t result = computeA0XordToAk(L - 1) ^ computeA0XordToAk(R);
        cout << result << endl;
    }
}

