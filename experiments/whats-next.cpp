#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

string asBinary(int64_t x)
{
    uint64_t highestPowerOf2InX = 1UL << 63UL;
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

int countBits(const string& binaryString)
{
    return std::count(binaryString.begin(), binaryString.end(), '1');
}

string uncompressToBinaryString(const vector<int64_t>& a)
{
    string binaryString;
    for (int i = 0; i < a.size(); i++)
    {
        if ((i % 2) == 0)
        {
            binaryString += string(a[i], '1');
        }
        else
        {
            binaryString += string(a[i], '0');
        }
    }
    return binaryString;
}

vector<int64_t> compressBinaryString(const string& binaryString)
{
    vector<int64_t> compressed;
    int digit = 1;
    int numInRun = 0;
    for (int i = 0; i < binaryString.size(); i++)
    {
        if (binaryString[i] - '0' != digit)
        {
            compressed.push_back(numInRun);
            numInRun = 1;
            digit = 1 - digit;
        }
        else
        {
            numInRun++;
        }
    }
    if (numInRun != 0)
    {
        compressed.push_back(numInRun);
    }
    return compressed;
}

int64_t binaryToDecimal(const string& binaryString)
{
    int64_t decimal = 0;
    for ( const auto binDigit : binaryString)
    {
        if (binDigit == '1')
        {
            decimal = 2 * decimal + 1;
        }
        else
        {
            decimal = 2 * decimal;
        }
    }
    return decimal;
}



vector<int64_t> bruteForce(const vector<int64_t>& a)
{
    const string aAsBinary = uncompressToBinaryString(a);
    const int64_t asDecimal = binaryToDecimal(aAsBinary);
    const int64_t numBits = countBits(aAsBinary);

    int64_t candidate = asDecimal + 1;
    while (countBits(asBinary(candidate)) != numBits)
    {
        candidate++;
    }
    return compressBinaryString(asBinary(candidate));
}


int main()
{
    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        int n;
        cin >> n;

        vector<int64_t> a(n);
        for (int i = 0; i < n; i++)
        {
            cin >> a[i];
        }
        const auto bruteForceResult = bruteForce(a);
        cout << "bruteForceResult: " << endl;
        cout << bruteForceResult.size() << endl;
        for (const auto x : bruteForceResult)
        {
            cout << x << " ";
        }
    }
}
