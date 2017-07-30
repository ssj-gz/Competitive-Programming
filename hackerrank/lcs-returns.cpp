// Simon St James (ssjgz) 2017-07-30 
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <string>
#include <vector>
#include <cassert>

using namespace std;

vector<vector<int>> findMaxCommonChildInFirstChars(const string& a, const string& b)
{
    // maxCommonChildInFirstChars[i][j] represents the largest common child in the prefix of a
    // of length i and the prefix of b of length j.
    // The "xxx.size() + 1" is because we must deal with suffixes of length 0, 1, ... xxx.size().
    // For i = 0, maxCommonChildInFirstChars[i][j] = 0 for all j;
    // For j = 0, maxCommonChildInFirstChars[i][j] = 0 for all i - since we set all maxCommonChildInFirstChars to
    // 0, we don't need to set these explicitly.
    // The recurrence relation is simple:
    // if a[i - 1] != b[j - 1], then ignore a[i - 1] and match the prefix of a (without a[i - 1]) against prefix of b,
    // or ignoring b[j - 1] and matching prefix of b (without b[j - 1]) against prefix of a, and pick the largest.
    // If a[i - 1] == a[j - 1], then do the above, but also match a[i - 1 - 1] and a[j] and (common child of length 1) and
    // try matching the prefixes (without a[i - 1] and a[j - 1]) of a and b.
    vector<vector<int>> maxCommonChildInFirstChars(a.size() + 1, vector<int>(b.size() + 1, 0));

    for (int i = 1; i <= a.size(); i++)
    {
        for (int j = 1; j <= b.size(); j++)
        {
            int maxCommonChildUpToHere = max(maxCommonChildInFirstChars[i - 1][j],
                    maxCommonChildInFirstChars[i][j - 1]);
            //cout << "i: " << i << " j: " << j << " maxCommonChildUpToHere: " << maxCommonChildUpToHere << endl;
            if (a[i - 1] == b[j - 1])
            {
                //cout << "i: " << i << " j: " << j << " match: " << maxCommonChildUpToHere << endl;
                maxCommonChildUpToHere = max(maxCommonChildUpToHere, 1 + maxCommonChildInFirstChars[i - 1][j - 1]);
            }
            maxCommonChildInFirstChars[i][j] = maxCommonChildUpToHere;
        }
    }
    return maxCommonChildInFirstChars;
}

int main()
{
    string a, b;
    cin >> a >> b;
    const auto maxCommonChildInFirstChars = findMaxCommonChildInFirstChars(a, b);
    const auto maxCommonChildInLastChars = findMaxCommonChildInFirstChars(string(a.rbegin(), a.rend()), string(b.rbegin(), b.rend()));
    const int maxCommonChildLength = maxCommonChildInLastChars[a.size()][b.size()];
    assert(maxCommonChildLength == maxCommonChildInFirstChars[a.size()][b.size()]);


    int numWaysOfAddingToIncreaseCommonChildLength = 0;
    const int numLetters = 26 + 10 + 26; // Alphanumeric - a-zA-Z0-9

    auto getLetterIndex = [numLetters](const char letter)
    {
        int letterIndex = -1;
        if (letter >= 'a' && letter <= 'z')
            letterIndex = letter - 'a';
        else if (letter >= '0' && letter <= '9')
            letterIndex = 26 + letter - '0';
        else if (letter >= 'A' && letter <= 'Z')
            letterIndex = 26 + 10 + letter - 'A';
        assert(letterIndex >= 0 && letterIndex < numLetters);
        return letterIndex;
    };

    for (int posToAddAt = 0; posToAddAt <= a.size(); posToAddAt++)
    {
        bool letterIndicesUsed[numLetters] = {};
        for (int bPosToMatchWith = 0; bPosToMatchWith < b.size(); bPosToMatchWith++)
        {
            const int letterToAddIndex = getLetterIndex(b[bPosToMatchWith]);
            if (letterIndicesUsed[letterToAddIndex])
                continue;
#ifndef NDEBUG
            const string withAdded = a.substr(0, posToAddAt) + b[bPosToMatchWith] + a.substr(posToAddAt);
            assert(withAdded.size() == a.size() + 1);
#endif
            if (maxCommonChildInFirstChars[posToAddAt][bPosToMatchWith] + maxCommonChildInLastChars[a.size() - posToAddAt][b.size() - 1 - bPosToMatchWith] == maxCommonChildLength)
            {
                numWaysOfAddingToIncreaseCommonChildLength++;
                assert(findMaxCommonChildInFirstChars(withAdded, b)[withAdded.size()][b.size()] == maxCommonChildLength + 1);
                letterIndicesUsed[letterToAddIndex] = true;
            }
        }
    }
    cout << numWaysOfAddingToIncreaseCommonChildLength << endl;
}
