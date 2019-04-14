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

void solve(const string& A, const string& B, const string& C, int K)
{
    string AAsBinary = hexToBinary(A);
    string BAsBinary = hexToBinary(B);
    string CAsBinary = hexToBinary(C);
    cout << "A: " << A << " AAsBinary: " << AAsBinary << " binaryToHex(AAsBinary): " << binaryToHex(AAsBinary) << endl;
    cout << "B: " << B << " BAsBinary: " << BAsBinary << " binaryToHex(BAsBinary): " << binaryToHex(BAsBinary) << endl;
    cout << "C: " << C << " CAsBinary: " << CAsBinary << " binaryToHex(CAsBinary): " << binaryToHex(CAsBinary) << endl;
    assert(A == binaryToHex(AAsBinary));
    assert(B == binaryToHex(BAsBinary));
    assert(C == binaryToHex(CAsBinary));
    const int binaryLength = AAsBinary.size();

    for (string::size_type i = 0; i < binaryLength; i++)
    {
        const int aDigit = AAsBinary[i] - '0';
        const int bDigit = BAsBinary[i] - '0';
        const int cDigit = CAsBinary[i] - '0';

        if ((aDigit ^ bDigit) != cDigit)
        {
            // Must change this digit.
            if (K == 0)
            {
                // ... but no changes left :(
                cout << -1 << endl;
                return;
            }
            if (aDigit == 1)
            {
                AAsBinary[i] = '0';
            }
            else
            {
                BAsBinary[i] = '0' + (1 - bDigit);
            }
            K--;
        }
    }
    // Do we have any changes left? If so, made the leftmost "1" in A a "0" (and toggle
    // corresponding digit in B) until we can no longer do so.
    for (string::size_type i = 0; i < binaryLength; i++)
    {
        if (AAsBinary[i] == '1')
        {
            AAsBinary[i] = '0';
            BAsBinary[i] = '0' + ('1' - BAsBinary[i]);
            K -= 2;;
            if (K < 2)
                break;
        }
    }
    for (string::size_type i = 0; i < binaryLength; i++)
    {
        const int aDigit = AAsBinary[i] - '0';
        const int bDigit = BAsBinary[i] - '0';
        const int cDigit = CAsBinary[i] - '0';

        assert ((aDigit ^ bDigit) == cDigit);
    }
    cout << binaryToHex(AAsBinary) << endl;
    cout << binaryToHex(BAsBinary) << endl;
    cout << "A' as binary: " << AAsBinary << endl;
    cout << "B' as binary: " << BAsBinary << endl;
    cout << "A' as decimal: " << hexToDecimal(binaryToHex(AAsBinary)) << endl;
    cout << "B' as decimal: " << hexToDecimal(binaryToHex(BAsBinary)) << endl;
    cout << "C as decimal: " << hexToDecimal(binaryToHex(CAsBinary)) << endl;
    cout << "A' ^ B' == " << (hexToDecimal(binaryToHex(AAsBinary)) ^ hexToDecimal(binaryToHex(BAsBinary))) << endl;
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

        cout << "A: " << hexToDecimal(A) << " B: " << hexToDecimal(B) << " C: " << hexToDecimal(C) << endl;

        solve(A, B, C, K);

    }
}
