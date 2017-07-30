#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <cassert>

using namespace std;

const int numLetters = 26;

int findLargestPalindromicSubsequence(const string&s, int startIndex, int endIndex, vector<vector<int>>& largestPalindromicSubsequenceWithBoundsLookup, const vector<int>(&firstOccurrenceOfLetterAfterOrAt)[numLetters], const vector<int> (&lastOccurrenceOfLetterBeforeOrAt)[numLetters])
{
    if (startIndex > endIndex)
        return 0;
    if (startIndex == endIndex)
        return 1;
    if (endIndex == startIndex + 1)
        return (s[startIndex] == s[endIndex] ? 2 : 1);

    int largestPalindromicSubstring = 0;
    for (int letterIndex = 0; letterIndex < numLetters; letterIndex++)
    {
        const int outerPalindromeStart = firstOccurrenceOfLetterAfterOrAt[letterIndex][startIndex];
        const int outerPalindromeEnd = lastOccurrenceOfLetterBeforeOrAt[letterIndex][endIndex];
        if (outerPalindromeStart == -1 || outerPalindromeEnd == -1 || outerPalindromeStart > outerPalindromeEnd )
            continue;
        assert(s[outerPalindromeStart] == s[outerPalindromeEnd]);
        int largestPalindromicSubstringWithThisOuter = 0;
        if (outerPalindromeStart == outerPalindromeEnd)
        {
            largestPalindromicSubstringWithThisOuter = 1;
        }
        else if (largestPalindromicSubsequenceWithBoundsLookup[outerPalindromeStart][outerPalindromeEnd] == -1)
        {
            const int largestInnerPalindromicSubstring = findLargestPalindromicSubsequence(s, outerPalindromeStart + 1, outerPalindromeEnd - 1, largestPalindromicSubsequenceWithBoundsLookup, firstOccurrenceOfLetterAfterOrAt, lastOccurrenceOfLetterBeforeOrAt);
            largestPalindromicSubstringWithThisOuter = largestInnerPalindromicSubstring + 2;
            largestPalindromicSubsequenceWithBoundsLookup[outerPalindromeStart][outerPalindromeEnd] = largestPalindromicSubstringWithThisOuter;
        }
        else
        {
            largestPalindromicSubstringWithThisOuter = largestPalindromicSubsequenceWithBoundsLookup[outerPalindromeStart][outerPalindromeEnd];
        }
        largestPalindromicSubstring = max(largestPalindromicSubstring, largestPalindromicSubstringWithThisOuter);
    }

    return largestPalindromicSubstring;
}

vector<int> findLargestPalindromicSubstringInFirstN(const string& s)
{
    vector<int> firstOccurrenceOfLetterAfterOrAt[numLetters];
    vector<int> lastOccurrenceOfLetterBeforeOrAt[numLetters];
    for (auto& firstOccurence : firstOccurrenceOfLetterAfterOrAt)
    {
        firstOccurence.resize(s.size());
    }
    for (auto& lastOccurence : lastOccurrenceOfLetterBeforeOrAt)
    {
        lastOccurence.resize(s.size());
    }
    for (int letterIndex = 0; letterIndex < numLetters; letterIndex++)
    {
        int firstOccurenceOfLetter = -1;
        for (int i = s.size() - 1; i >= 0; i--)
        {
            if (s[i] - 'a' == letterIndex)
            {
                firstOccurenceOfLetter = i;
            }
            firstOccurrenceOfLetterAfterOrAt[letterIndex][i] = firstOccurenceOfLetter;
        }
        int lastOccurenceOfLetter = -1;
        for (int i = 0; i < s.size(); i++)
        {
            if (s[i] - 'a' == letterIndex)
            {
                lastOccurenceOfLetter = i;
            }
            lastOccurrenceOfLetterBeforeOrAt[letterIndex][i] = lastOccurenceOfLetter;
        }
    }

    // largestPalindromicSubsequenceWithBoundsLookup[a][b] is used to memo-ise the length of the largest palindromic subsequence
    // which starts precisely at a and ends precisely at b.
    vector<vector<int>> largestPalindromicSubsequenceWithBoundsLookup(vector<vector<int>>(s.size(),  vector<int>(s.size(), -1)));

    vector<int> largestPalindromicSubstringInFirstN;
    largestPalindromicSubstringInFirstN.reserve(s.size());
    int largest = 0;
    for (int i = 0; i < s.size(); i++)
    {
        largestPalindromicSubstringInFirstN.push_back(largest);
        const int outerPalindromeEnd = i;
        const int outerPalindromeStart = firstOccurrenceOfLetterAfterOrAt[s[i] - 'a'][0];

        //cout << "i: " << i << " letter: " << s[i] << " outerPalindromeStart: " << outerPalindromeStart << " outerPalindromeEnd: " << outerPalindromeEnd << endl;
        if (outerPalindromeStart == -1)
            continue;
        int largestPalindromeEndingHere = 1;
        if (outerPalindromeStart != outerPalindromeEnd)
        {
            const int largestInnerPalindrome = findLargestPalindromicSubsequence(s, outerPalindromeStart + 1, outerPalindromeEnd - 1, largestPalindromicSubsequenceWithBoundsLookup, firstOccurrenceOfLetterAfterOrAt, lastOccurrenceOfLetterBeforeOrAt);
            largestPalindromeEndingHere = largestInnerPalindrome + 2;
        }
        //cout << "largest palindrome ending at " << outerPalindromeEnd << " has length: " << largestPalindromeEndingHere << endl;
        largest = max(largest, largestPalindromeEndingHere);
    }
    largestPalindromicSubstringInFirstN.push_back(largest);

    return largestPalindromicSubstringInFirstN;
}

int main()
{
    string s;
    cin >> s;

    //cout << "Largest: " << findLargestPalindromicSubstringInFirstN(s).back() << endl;
    const auto largestPalindromicSubstringInFirstN = findLargestPalindromicSubstringInFirstN(s);
    const string sReversed(s.rbegin(), s.rend());
    const auto largestPalindromicSubstringInLastN = findLargestPalindromicSubstringInFirstN(sReversed);
    int bestValue = 0;
    for (int firstPieceLength = 0; firstPieceLength < s.size(); firstPieceLength++)
    {
        const int secondPieceLength = s.size() - firstPieceLength;
        const int value = largestPalindromicSubstringInFirstN[firstPieceLength] * largestPalindromicSubstringInLastN[secondPieceLength];
        //cout << "firstPieceLength: " << firstPieceLength << " secondPieceLength: " << secondPieceLength << " largestPalindromicSubstringInFirstN: " << largestPalindromicSubstringInFirstN[firstPieceLength] << " largestPalindromicSubstringInLastN: " << largestPalindromicSubstringInLastN[secondPieceLength] << endl;
        bestValue = max(bestValue, value);
    }
    cout << bestValue << endl;
}
