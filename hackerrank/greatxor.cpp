#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

string asBinary(long x)
{
    unsigned long highestPowerOf2InX = 1UL << 63UL;
    string binaryString;
    for (; highestPowerOf2InX >= 1; highestPowerOf2InX >>= 1)
    {
        if ((x & highestPowerOf2InX) != 0)
        {
            break;
        }
    }
    while (highestPowerOf2InX >= 1)
    {
        if ((x & highestPowerOf2InX) != 0)
        {
            x -= highestPowerOf2InX;
            binaryString += "1";
        }
        else
            binaryString += "0";
        highestPowerOf2InX >>= 1;
    }
    return binaryString;
}
int main() {
    long q;
    cin >> q;
    // Fairly easy one: the pattern is fairly clear if you just do a brute-force
    // implementation and examine the results, but I prefer a more analytical solution :)
    // Express x as a binary number with no leading 0's, and let m be the number of binary digits;
    // then 2^(m-1) is the highest power of 2 that is no greater than x.
    // We'll construct from this a's that satisfy the requirements ([1] 0 < a < x; [2] a xor x > x).
    // Since a is less than x, it must be that the highest power of 2 occurring in a is
    // at most 2^(m-1), so a has at most as many binary digits as x.  
    //
    //  x = 1XXXX....X
    //  a = YYYYY....Y
    //
    // But for a xor x to be greater than x, 2^(m-1) cannot be the highest power of 2 no greater
    // than a i.e. the first digit of a cannot be "1":
    //
    //  x = 1XXXX....X
    //  a = 0YYYY....Y
    //
    // We need to fill out the remaining Y's.  Note that any remaining Y's will satisfy [1], so
    // we need only focus on making sure the Y's satisfy [2].
    // Assume the 2nd digit of x is 1; then the corresponding digit of a cannot also be 1, as this
    // would lead to a ^ x < x, so it must be 0:
    //
    //  x = 11XXX....X
    //  a = 00YYY....Y
    //
    // On the other hand, imagine if the 2nd digit of x were 0: then the second digit of a could be
    // either 0 or 1; neither would lead to a violation of [2], so we need to examine further
    // digits.  But there's a shortcut if the 2nd digit of a were 1:
    //
    //  x = 10XXX....X
    //  a = 01YYY....Y
    //
    // Then the remaining values could have *any* values and satisfy [2] (as the first 2 digits of 
    // a xor x would be 11, and the first two digits of x are 10), so we can simply add the number of
    // such values for the Y's, of which there are 2^(m-2).
    //
    // So hopefully the pattern is clear: go through each binary digit of x, and let d be the number
    // of digits encountered so far: if it is a 1, add 2^(m - d); else do nothing.

    for (int i = 0; i < q; i++)
    {
        unsigned long x;
        cin >> x;

        const string xAsBinary = asBinary(x);
        const unsigned long numBinaryDigits = xAsBinary.size();
        unsigned long currentPowerOf2 = (1UL << (numBinaryDigits - 1));

        unsigned long numA = 0;

        for (const char binaryDigit : xAsBinary)
        {
            if (binaryDigit == '0')
                numA += currentPowerOf2;

            currentPowerOf2 >>= 1;
        }

        cout << numA << endl;
    }
    return 0;
}
