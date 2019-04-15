#include <iostream>
#include <string>
#include <cassert>

#include <sys/time.h>

using namespace std;

string encode(const string& toEncode, int k)
{
    string encoded(toEncode.size() + k - 1, '0');
    for (int shift = 0; shift < k; shift++)
    {
        for (int i = 0; i < toEncode.size(); i++)
        {
            const int currentEncodedDigit = encoded[i + shift] - '0';
            const int toEncodeDigit = toEncode[i] - '0';
            encoded[i + shift] = '0' + (currentEncodedDigit ^ toEncodeDigit);
        }
    }
    return encoded;
}

string bruteForce(const string& s, int n, int k)
{
    string binaryString(n, '0');
    string solution;
    while (true)
    {
        string::reverse_iterator r = binaryString.rbegin();
        while (r != binaryString.rend() && *r == '1')
        {
            *r = '0';
            r++;
        }
        if (r != binaryString.rend())
        {
            *r = '1';
        }
        else
        {
            break;
        }
        //cout << "Trying " << binaryString << endl;

        if (encode(binaryString, k) == s)
        {
            if (!solution.empty())
                return "";
            solution = binaryString;
        }
    }
    return solution;
}

string optimised(const string& s, int n, int k)
{
    string result;
    int xorOfLastKDigitsOfResult = 0;
    for (int i = 0; i < s.size(); i++)
    {
        if (i >= k)
        {
            // We've got the xor of the last k+1 digits of result, not just k:
            // trim off the first of these k+1 digits.
            xorOfLastKDigitsOfResult ^= (result[i - k] - '0');
        }
        const int encodedBinDigit = s[i] - '0';
        const int decodedNextDigit = encodedBinDigit ^ xorOfLastKDigitsOfResult;
        result.push_back('0' + decodedNextDigit);
        xorOfLastKDigitsOfResult ^= decodedNextDigit;

        if (result.size() == n)
            return result;
    }
    return "";
}

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        while (true)
        {
            const int n = rand() % 12 + 1;
            const int k = rand() % 20 + 1;

            string binaryString;
            for (int i = 0; i < n; i++)
            {
                binaryString += ('0' + rand() % 2);
            }
            if (!bruteForce(encode(binaryString, k), n, k).empty())
            {
                cout << n << endl;
                cout << k << endl;
                cout << encode(binaryString, k) << endl;
                return 0;
            }
            else
            {
            }
        }
    }
    int n;
    cin >> n;
    int k;
    cin >> k;

    string s;
    cin >> s;

    const auto bruteForceResult = bruteForce(s, n, k);
    cout << "bruteForceResult: " << bruteForceResult << endl;
    const auto optimisedResult = optimised(s, n, k);
    cout << "optimisedResult : " << optimisedResult << endl;
    assert(bruteForceResult == optimisedResult);
}
