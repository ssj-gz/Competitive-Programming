// Simon St James (ssjgz) - 2019-04-14
#include <iostream>
#include <vector>
#include <string>
#include <cassert>

using namespace std;

string hexToBinary(const string& hexString)
{
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
            string hexDigit;
            if (hexDigitValue <= 9)
            {
                hexDigit = '0' + hexDigitValue;
            }
            else
            {
                hexDigit = 'A' + (hexDigitValue - 10);
            }
            powerOf2 = 8;
            asHex += hexDigit;
            hexDigitValue = 0;
        }
    }
    return asHex;
}
int hexToDecimal(const string& hexString)
{
    int asDecimal = 0;
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
        asDecimal = 16 * asDecimal + digitValue;
    }
    return asDecimal;
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
    assert(A == binaryToHex(AAsBinary));
    assert(B == binaryToHex(BAsBinary));
    assert(C == binaryToHex(CAsBinary));
    const int binaryLength = AAsBinary.size();

    for (string::size_type i = 0; i < binaryLength; i++)
    {
        const int aDigit = AAsBinary[i] - '0';
        const int bDigit = BAsBinary[i] - '0';
        const int cDigit = CAsBinary[i] - '0';

        if ((aDigit | bDigit) != cDigit)
        {
            if (cDigit == 0)
            {
                if (aDigit == 1 && bDigit == 1)
                {
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
                BAsBinary[i] = '1';
                K--;
            }
        }
    }
    if (K < 0)
    {
        cout << -1 << endl;
        return;
    }
    // At this point, we've changed at most K bits and have A | B == C.

    // Do we have any changes left? If so, made the leftmost "1" in A a "0" if we can
    // until we can no longer do so.
    for (string::size_type i = 0; i < binaryLength; i++)
    {
        if (AAsBinary[i] == '1' && CAsBinary[i] == '0')
        {
            if ( K > 0)
            {
                AAsBinary[i] = '0';
                K--;
            }
        }
        else if (AAsBinary[i] == '1' && BAsBinary[i] == '1')
        {
            assert(CAsBinary[i] == '1');
            if ( K > 0)
            {
                AAsBinary[i] = '0';
                K--;
            }
        }
        else if (AAsBinary[i] == '1' && BAsBinary[i] == '0')
        {
            assert(CAsBinary[i] == '1');
            if ( K >= 2)
            {
                AAsBinary[i] = '0';
                BAsBinary[i] = '1';
                K -= 2;
            }
        }
    }
    // Now do the same for B.
    for (string::size_type i = 0; i < binaryLength; i++)
    {
        if (BAsBinary[i] == '1' && CAsBinary[i] == '0')
        {
            if (K > 0)
            {
                BAsBinary[i] = '0';
                K--;
            }
        }
        else if (BAsBinary[i] == '1' && AAsBinary[i] == '1')
        {
            assert(CAsBinary[i] == '1');
            if (K > 0)
            {
                BAsBinary[i] = '0';
                K--;
            }
        }
    }
    cout << withLeadingZerosTrimmed(binaryToHex(AAsBinary)) << endl;
    cout << withLeadingZerosTrimmed(binaryToHex(BAsBinary)) << endl;
}

int main()
{
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

        //cout << "A: " << hexToDecimal(A) << " B: " << hexToDecimal(B) << " C: " << hexToDecimal(C) << endl;

        solve(A, B, C, K);

    }
}
