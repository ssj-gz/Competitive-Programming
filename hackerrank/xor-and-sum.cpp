// Simon St James (ssjgz) 2017-10-02
#include <iostream>
#include <vector>

using namespace std;


int main()
{
    // Fairly easy one: we compute the sum bitwise for all possible bits that could
    // end up being set in the sum, which is max(num digits in a, num digits in b) + numSums
    // i.e. we compute the number of times a '1' appears at each digit index
    // if we expanded the sum in binary, then convert to decimal in the normal way
    // (adding powers of 2).
    // Hopefully the inline comments below explain it sufficiently :)

    string a, b;
    cin >> a >> b;

    const int64_t numSums = 314159 + 1;

    const string aReversed(a.rbegin(), a.rend());
    const string bReversed(b.rbegin(), b.rend());

    const int64_t modulus = 1'000'000'007UL;


    auto digit = [](const string& binaryString, int digitNum)
    {
        if (digitNum < binaryString.size())
            return binaryString[digitNum] - '0';
        else
            return 0;
    };

    // In a binary string B, define the digitNum'th digit of B as B[B.size() - digitNum] i.e.
    // we count from the right, 0-relative.
    // e.g. Given the binary string 10010, the 0th digit is the end of the string (0); the 1st digit is 1;
    // the 2nd digit is 0, the third digit is 0 and the fourth digit (the beginning of the string)
    // is 1.
    int64_t powerOf2 = 1;
    // At digit index digitNum, num1DigitsOfBInSums will be the number of times that (b << i)
    // has a '1' at digit index digitNum for i = 0 ... numSums - 1.
    int num1DigitsOfBInSums = 0; 
    int64_t sum = 0;
    const int numDigitsInSum = max(a.size(), b.size()) + numSums;
    for (int digitNum = 0; digitNum < numDigitsInSum; digitNum++)
    {
        const int digitA = digit(aReversed, digitNum);
        const int digitB = digit(bReversed, digitNum);

        if (digitB == 1)
        {
            num1DigitsOfBInSums++;
        }
        if (digitNum - numSums >= 0)
        {
            if (digit(bReversed, digitNum - numSums) == 1)
            {
                // The previously encountered '1' at index (digitNum - numSums) in B
                // will not appear at index digitNum during the numSums sums: it's "too far away".
                num1DigitsOfBInSums--;
            }
                
        }

        if (digitA == 1)
        {
            // This will contribute a '1' at digitNum for every time we have a 0 at digitNum in (b << i)
            // for i = 0 .... numSums - 1 i.e. throughout the numSums sums,
            // it will only *not* contribute '1' num1DigitsOfBInSums times.
            sum = (sum + (powerOf2 * (numSums - num1DigitsOfBInSums))) % modulus;
        }
        else
        {
            // This will contribute a '1' at digitNum for every time '1' appears 
            // at digitNum in (b << i) for i = 0... numSums - 1.
            sum = (sum + (powerOf2 * num1DigitsOfBInSums)) % modulus;
        }

        powerOf2 = (2 * powerOf2) % modulus;
    }

    cout << sum << endl;
}
