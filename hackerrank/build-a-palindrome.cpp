#include <iostream>
#include <utility>

using namespace std;

string findLongestPalindrome(const string&a, const string& b)
{
    return "";
}

string findLongestPalindromeBruteForce(const string&a, const string& b)
{
    string result;
    for (int aSuffix = 0; aSuffix < a.size(); aSuffix++)
    {
        for (int aLength = 1; aSuffix + aLength <= a.size(); aLength++)
        {
            const auto sA = a.substr(aSuffix, aLength);
            for (int bSuffix = 0; bSuffix < b.size(); bSuffix++)
            {
                for (int bLength = 1; bSuffix + bLength <= b.size(); bLength++)
                {
                    const auto sB = b.substr(bSuffix, bLength);
                    const auto sAB = sA + sB;
                    if (sAB == string(sAB.rbegin(), sAB.rend()) && sAB.length() >= result.length())
                    {
                        if (sAB.length() > result.length())
                            result = sAB;
                        result = min(result, sAB);
                    }
                }
            }
        }
    }
    if (result.empty())
        result = "-1";
    return result;
}


int main()
{
    int q;
    cin >> q;

    for (int i = 0; i < q; i++)
    {
        string a;
        string b;

        cin >> a >> b;

        cout << "a: " << a << " b: " << b << endl;

        const auto result = findLongestPalindrome(a, b);
#define BRUTE_FORCE
#ifdef BRUTE_FORCE
        const auto resultBruteForce = findLongestPalindromeBruteForce(a, b);
        cout << "result: " << result << " resultBruteForce: " << resultBruteForce << endl; 
#endif
    }
}
