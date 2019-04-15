#include <iostream>
#include <string>

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
        cout << "Trying " << binaryString << endl;

        if (encode(binaryString, k) == s)
        {
            return binaryString;
        }
    }
    return "";
}

int main()
{
    int n;
    cin >> n;
    int k;
    cin >> k;

    string s;
    cin >> s;

    const auto bruteForceResult = bruteForce(s, n, k);
    cout << bruteForceResult << endl;

      
}
