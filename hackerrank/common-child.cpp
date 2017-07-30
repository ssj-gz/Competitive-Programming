// Simon St James (ssjgz) 2017-07-30 9:45
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main()
{
    string a, b;
    cin >> a >> b;
    // maxCommonChildInLastChars[i][j] represents the largest common child in the suffix of a
    // of length i and the suffix of b of length j.
    // The "xxx.size() + 1" is because we must deal with suffixes of length 0, 1, ... xxx.size().
    // For i = a.size(), maxCommonChildInLastChars[i][j] = 0 for all j;
    // For j = b.size(), maxCommonChildInLastChars[i][j] = 0 for all i - since we set all maxCommonChildInLastChars to
    // 0, we don't need to set these explicitly.
    // The recurrence relation is simple:
    // if a[i] != b[j], then try skipping next letter of a and matching remainder of it against the remainder of b,
    // or skipping next letter of b and matching remainder of b against remainder of a, and pick the largest.
    // If a[i] == a[j], then do the above, but also match a[i] and a[j] and (common child of length 1) and
    // try matching the remainder (after a[i] and a[j]) of a and b.
    vector<vector<int>> maxCommonChildInLastChars(a.size() + 1, vector<int>(b.size() + 1, 0));

    for (int i = a.size() - 1; i >= 0; i--)
    {
        for (int j = b.size() - 1; j >= 0; j--)
        {
            int maxCommonChildFromHere = max(maxCommonChildInLastChars[i + 1][j], // Skip a[i], match remainder of a against remainder of b.
                    maxCommonChildInLastChars[i][j + 1]); // Skip b[j], match remainder of b against remainder of a.
            if (a[i] == b[j])
            {
                // Match a[i] and b[j] (increasing common child by 1), skip them both, then try and match remainders of a and b.
                maxCommonChildFromHere = max(maxCommonChildFromHere, 1 + maxCommonChildInLastChars[i + 1][j + 1]);
            }
            maxCommonChildInLastChars[i][j] = maxCommonChildFromHere;
        }
    }

    cout << maxCommonChildInLastChars[0][0] << endl;
}
