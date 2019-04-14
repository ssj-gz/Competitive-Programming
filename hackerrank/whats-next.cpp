// Simon St James (ssjgz) - 2019-04-14 19:07
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#define NDEBUG
#undef BRUTE_FORCE
#endif
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
        // Need to transform e.g.
        //
        //   XXXX01111111000
        //
        // to
        //
        //   XXXX10000111111
        //
        // or, if if a.size() == 2 e.g.:
        //
        //   1111111000
        //
        // to
        //
        //   10000111111
        // 
        // In either case, the last two blocks are replaced with 
        // a string of the same length, this time beginning with 0's
        // instead of 1's, and with one less 1 than in the original
        // last two blocks.
        // The digit before the last two blocks must become a 1.
        assert(a.size() > 1);
        const auto originalNumTrailing0s = a[a.size() - 1];
        const auto originalNum1sBefore = a[a.size() - 2];
        const auto totalInLastTwoBlocks = originalNum1sBefore + originalNumTrailing0s;
        // Pop off last two blocks (the 1's and the 0's).
        result.pop_back(); 
        result.pop_back();
        if (!result.empty())
        {
            result.back()--; // Make a note that one of the 0's has become a 1 (the "digit before the last two blocks").
        }
        result.push_back(1); // Add a single 1 (the "digit before the last two blocks" )
        const auto newNumTrailing1s = originalNum1sBefore - 1; // Need 1 less 1, due to the one we've just added.
        const auto newNum0s = totalInLastTwoBlocks - newNumTrailing1s;
        result.push_back(newNum0s); // Add the block of 0's.
        result.push_back(newNumTrailing1s); // Add the block of 1's.
    }
    else
    {
        // Easier case: transform e.g.
        // 
        //   XXXXX0001111111
        //
        // to
        // 
        //   XXXXX0010111111
        //
        // i.e. last block (all 1's) removed; digit that was to left (if any:
        // insert one if there wasn't one there originally) turned into a 1; 
        // a single "0" inserted; and one less 1 than in the original last block
        // appended.
        const auto originalNumTrailing1s = a[a.size() - 1];
        result.pop_back();
        if (!result.empty())
        {
            result.back()--; // One of the 0's has become a 1.
        }
        result.push_back(1); // Add the single 1.
        result.push_back(1); // Add the single 0.
        const auto newNumTrailing1s = originalNumTrailing1s - 1; // Need one less 1, due to the one (tee-hee) we've just added.
        result.push_back(newNumTrailing1s); // Add remaining block of 1's.
    }
    // Prepare output to be in correct format - the correct answer
    // can be expressed in many different ways, but only one is 
    // accepted!
    // No trailing 0's in the compressed version.
    while (!result.empty() && result.back() == 0)
    {
        result.pop_back();
    }
    // Also, no 0's anywhere except the first place (if the number
    // begins with 0's) - though I'm not sure if that particular input
    // can occur.
    vector<int64_t> cleanedResult;
    cleanedResult.push_back(result.front());
    for (int i = 1; i < result.size(); i++)
    {
        if (result[i] != 0)
        {
            cleanedResult.push_back(result[i]);
        }
        else
        {
            i++;
            assert(!cleanedResult.empty());
            cleanedResult.back() += result[i];
        }
    }
    return cleanedResult;
}

int main(int argc, char* argv[])
{

    if (argc == 2)
    {
        struct timeval time; 
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        cout << 1 << endl;
        string binaryString;
        while (true)
        {
            binaryString.clear();
            const int numDigits = rand() % 30 + 1;
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
#ifdef BRUTE_FORCE
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
#else
        const auto optimizedResult = optimized(a);
        cout << optimizedResult.size() << endl;
        for (const auto x : optimizedResult)
        {
            cout << x << " ";
        }
        cout << endl;

#endif
    }
}
