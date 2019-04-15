#include <iostream>
#include <string>

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

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        while (true)
        {
            const int n = rand() % 20 + 1;
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
        }
    }
    int n;
    cin >> n;
    int k;
    cin >> k;

    string s;
    cin >> s;

    const auto bruteForceResult = bruteForce(s, n, k);
    cout << "Result: " << bruteForceResult << endl;
}
