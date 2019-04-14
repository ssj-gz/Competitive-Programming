// Simon St James (ssjgz) - 2019-04-14 11:29
#include <iostream>
#include <vector>
#include <string>
#include <cassert>

using namespace std;

string hexToBinary(const string& hexString)
{
    // One hex digit == exactly 4 binary digits.
    string asBinary;
    for (const auto& hexDigit : hexString)
    {
        int digitValue = 0;
        if ( hexDigit >= '0' && hexDigit <= '9')
        {
            digitValue = hexDigit - '0';
        }
        else
        {
            assert(hexDigit >= 'A' && hexDigit <= 'F');
            digitValue = 10 + hexDigit - 'A';
        }
        int powerOf2 = 8;
        while (powerOf2 >= 1)
        {
            if (digitValue >= powerOf2)
            {
                asBinary += '1';
                digitValue -= powerOf2;
            }
            else
            {
                asBinary += '0';
            }
            powerOf2 >>= 1;
        }
    }
    return asBinary;
}

string binaryToHex(const string& binaryString)
{
    
    string asHex;
    int powerOf2 = 8;
    int hexDigitValue = 0;
    for (string::size_type i = 0; i < binaryString.size(); i++)
    {
        hexDigitValue += powerOf2 * (binaryString[i] - '0');
        powerOf2 >>= 1;
        if (powerOf2 == 0)
        {
            // We've processed 4 binary digits, which is one 1 hex digit.
            string hexDigit;
            if (hexDigitValue <= 9)
            {
                hexDigit = '0' + hexDigitValue;
            }
            else
            {
                hexDigit = 'A' + (hexDigitValue - 10);
            }

            asHex += hexDigit;
            powerOf2 = 8;
            hexDigitValue = 0;
        }
    }
    return asHex;
}

string withLeadingZerosTrimmed(const string& str)
{
    string::size_type i = 0;
    while (str[i] == '0' && i < str.size())
    {
        i++;
    }
    if (i == str.size())
        return "0";
    return str.substr(i);
}

void solve(const string& A, const string& B, const string& C, int K)
{
    string AAsBinary = hexToBinary(A);
    string BAsBinary = hexToBinary(B);
    string CAsBinary = hexToBinary(C);
    const int binaryLength = AAsBinary.size();

    // Make all *necessary* changes, but do so
    // in a way that favours making A smaller.
    // Note: We can make A smaller greedily without jeopardising our
    // chances at later minimising B.
    for (string::size_type i = 0; i < binaryLength; i++)
    {
        const int aDigit = AAsBinary[i] - '0';
        const int bDigit = BAsBinary[i] - '0';
        const int cDigit = CAsBinary[i] - '0';

        if ((aDigit | bDigit) != cDigit)
        {
            if (cDigit == 0)
            {
                // Need to change some bits: this digit in *both*
                // A and B must be 0.
                if (aDigit == 1 && bDigit == 1)
                {
                    // Must change both.
                    AAsBinary[i] = '0';
                    BAsBinary[i] = '0';
                    K -= 2;
                }
                else if (aDigit == 1)
                {
                    AAsBinary[i] = '0';
                    K--;
                }
                else
                {
                    BAsBinary[i] = '0';
                    K--;
                }
            }
            else
            {
                assert(cDigit == 1);
                assert(aDigit == 0 && bDigit == 0);
                // One of A or B must be changed into a 1; obviously, do it to B 
                // instead of A!
                BAsBinary[i] = '1';
                K--;
            }
        }
    }
    if (K < 0)
    {
        // We exceeded our allotted number of changes for the *necessary* changes - 
        // no solution exists.
        cout << -1 << endl;
        return;
    }
    // At this point, we've changed at most K bits and have A | B == C.

    // Do we have any changes left? If so, make the leftmost (and so, highest-valued)
    // "1" in A into a "0" if we can (i.e. without changing the value of A | B) 
    // until we can no longer do so.
    // We can be greedy, here: if we can make a leftmost-er digit into a 0, then
    // it doesn't matter if this reduces the chances to change digits to the right:
    // changing a leftmost-er digit trumps changing any number of digits to the right.
    for (string::size_type i = 0; i < binaryLength; i++)
    {
        if (AAsBinary[i] == '1' && BAsBinary[i] == '1')
        {
            // Don't need this digit of A to be 1: B is handling it already!
            assert(CAsBinary[i] == '1');
            if ( K > 0)
            {
                AAsBinary[i] = '0';
                K--;
            }
        }
        else if (AAsBinary[i] == '1' && BAsBinary[i] == '0')
        {
            // B is not handling making this digit 1, but perhaps it could do:
            // check, and if it can, arrange this instead.
            assert(CAsBinary[i] == '1');
            if ( K >= 2)
            {
                AAsBinary[i] = '0';
                BAsBinary[i] = '1';
                K -= 2;
            }
        }
    }
    // Note: At this point, there are no changes we can make to shrink B any smaller than it
    // is already: changing any digit of B from a 1 to a 0 will always
    // make (A | B) != C as it is guaranteed that the corresponding digit of A will be 0 at 
    // this point i.e. it will change the corresponding digit of (A | B) from a (correct) 1
    // to an (incorrect) 0.
    cout << withLeadingZerosTrimmed(binaryToHex(AAsBinary)) << endl;
    cout << withLeadingZerosTrimmed(binaryToHex(BAsBinary)) << endl;
}

int main()
{
    // Pretty easy one, though lots of schoolboy errors on the way - chief amongst them,
    // completely misreading the question as one involving "xor", not "or" XD
    // Hopefully the inline comments above explain everything satisfactorily :)
    int Q;
    cin >> Q;

    for (int q = 0; q < Q; q++)
    {
        int K;
        cin >> K;

        string A, B, C;
        cin >> A;
        cin >> B;
        cin >> C;

        solve(A, B, C, K);

    }
}
