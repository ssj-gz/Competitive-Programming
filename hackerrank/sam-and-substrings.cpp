// Simon St James (ssjgz) 2017-7-20 10:04
#include <iostream>

using namespace std;

int main()
{
    // A nice, easy one :)
    // Consider all strings ending at the ith digit (0-relative, counting from the left) of N.  For example, if
    // N = 1234, then the set of strings ending at the 2nd digit is:
    //
    //  123
    //   23
    //    3
    //
    // In general, there will obviously be i + 1 of these.  List them in order of increasing length:
    //
    //  s1
    //  s2
    //  ..
    //  si+1
    //
    // Let N[i] be the ith digit of N (in our example, the N[2] would be 3).  Then this list can be re-written as:
    //  
    //   N[i]
    //   t1.N[i]
    //   t2.N[i]
    //   ..
    //   ti.N[i]
    // 
    // where the "." denotes concatenation and where the ti's are precisely the i substrings of N ending at the i-1th digit.
    //
    // Sum the list: s1 + s2 + ... + si+1 =
    //
    //   N[i] + t1.N[i] + t2.N[i] + ... + ti.N[i].
    //
    // Now, the decimal value of tj.N[i] is (tj * 10) + N[i] (rather confusingly, I'm using tj as either a string or the
    // decimal value of that string according to context - sorry!), so this becomes
    //
    //   N[i] + ((t1 * 10) + N[i]) + ((t2 * 10) + N[i]) + ... + ((ti * 10) + N[i]).
    //
    // Re-arranging, we get
    //
    //   N[i] + N[i] + ... + N[i] + (t1 * 10) + (t2 * 10) + ... + (ti * 10)
    //
    // (i + 1 lots of N[i]) which equals:
    //
    //  (i + 1) * N[i] + 10 * (t1 + t2 + ... + ti)
    //
    // i.e. 
    //
    //   sum of all substrings ending at ith digit is equal to (i + 1) * N[i] + 10 * sum of all substrings ending at i - 1.
    //
    // We can then use this simple recurrence relation to find the sum of all substrings of N.

    string N;
    cin >> N;
    int64_t totalCandies = 0;
    const int64_t modulus = 1'000'000'007ULL;

    int64_t sumOfEndingAtPreviousDigit = 0;
    for (int digitIndex = 0; digitIndex < N.size(); digitIndex++)
    {
        const int digit = N[digitIndex] - '0';
        const int numEndingAtDigit = digitIndex + 1;

        const int sumOfEndingAtDigit = (10 * sumOfEndingAtPreviousDigit + numEndingAtDigit * digit) % modulus;
        totalCandies += sumOfEndingAtDigit;
        totalCandies %= modulus;

        sumOfEndingAtPreviousDigit = sumOfEndingAtDigit;
    }
    cout << totalCandies << endl;
}
