#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <sys/time.h>

using namespace std;

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

vector<int64_t> optimized(const vector<int64_t>& a)
{
    vector<int64_t> result = a;
    const bool lastDigitsAre0s = ((a.size() % 2) == 0);
    if (lastDigitsAre0s)
    {
        assert(a.size() > 1);
        const int num0s = a[a.size() - 1];
        const int num1sBefore = a[a.size() - 2];
        result.pop_back();
        result.pop_back();
        if (!result.empty())
        {
            result.back()--; // One of the 0's has become a 1.
            result.push_back(1); // Add a single 1.
            result.push_back(num0s + num1sBefore - (num1sBefore - 1));
            result.push_back(num1sBefore - 1);
        }
        else
        {
            result.insert(result.begin(), 1); // Add a single 1.
            result.push_back(num0s + num1sBefore - (num1sBefore - 1));
            result.push_back(num1sBefore - 1);
        }
        return result;
    }
    else
    {
        return bruteForce(a);
    }
}

int main(int argc, char* argv[])
{

    struct timeval time; 
    gettimeofday(&time,NULL);
    srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

    if (argc == 2)
    {
        cout << 1 << endl;
        string binaryString;
        while (true)
        {
            binaryString.clear();
            const int numDigits = rand() % 5 + 1;
            for (int i = 0; i < numDigits; i++)
            {
                binaryString += '0' + rand() % 2;
            }
            if (countBits(binaryString) != 0)
                break;
        }
        const auto compressed = compressBinaryString(binaryString);
        cout << compressed.size() << endl;
        for (const auto x : compressed)
        {
            cout << x << " ";
        }
        cout << endl;
        return 0;
    }
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
        cout << "Elements: " << uncompressToBinaryString(a)<< endl;
        for (const auto x : a)
        {
            cout << x << " ";
        }
        cout << endl;
        const auto bruteForceResult = bruteForce(a);
        cout << "bruteForceResult: " << uncompressToBinaryString(bruteForceResult) << endl;
        cout << bruteForceResult.size() << endl;
        for (const auto x : bruteForceResult)
        {
            cout << x << " ";
        }
        cout << endl;
        const auto optimizedResult = optimized(a);
        cout << "optimizedResult: " << uncompressToBinaryString(optimizedResult) << endl;
        cout << optimizedResult.size() << endl;
        for (const auto x : optimizedResult)
        {
            cout << x << " ";
        }
        cout << endl;
        assert(withLeadingZerosTrimmed(uncompressToBinaryString(optimizedResult)) == withLeadingZerosTrimmed(uncompressToBinaryString(bruteForceResult)));
    }
}
