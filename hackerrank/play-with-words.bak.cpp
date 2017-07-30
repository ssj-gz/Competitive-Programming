#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <cassert>

using namespace std;

struct PalindromicSubsequence
{
    int size = -1;
    int firstIndex = -1;
};

const int numLetters = 26;

int findLargestPalindromicSubsequence(const string&s, int startIndex, int endIndex, map<pair<int, int>, int>& lookup, const vector<int>(&firstOccurrenceOfLetterAfterOrAt)[numLetters], const vector<int> (&lastOccurrenceOfLetterBeforeOrAt)[numLetters])
{
    if (startIndex > endIndex)
        return 0;
    if (startIndex == endIndex)
        return 1;
    if (lookup[make_pair(startIndex, endIndex)] != 0)
        return lookup[make_pair(startIndex, endIndex)];
    int largestPalindromicSubstring = 0;
    for (int letterIndex = 0; letterIndex < numLetters; letterIndex++)
    {
        const int outerPalindromeStart = firstOccurrenceOfLetterAfterOrAt[letterIndex][startIndex];
        const int outerPalindromeEnd = lastOccurrenceOfLetterBeforeOrAt[letterIndex][endIndex];
        if (outerPalindromeStart == -1 || outerPalindromeEnd == -1)
            continue;
        //cout << "outerPalindromeStart: " << outerPalindromeStart << " outerPalindromeEnd: " << outerPalindromeEnd << endl;
        assert(s[outerPalindromeStart] == s[outerPalindromeEnd]);
        int largestPalindromicSubstringWithThisOuter = 0;
        if (lookup[make_pair(outerPalindromeStart, outerPalindromeEnd)] == 0)
        {
            const int largestInnerPalindromicSubstring = findLargestPalindromicSubsequence(s, outerPalindromeStart + 1, outerPalindromeEnd - 1, lookup, firstOccurrenceOfLetterAfterOrAt, lastOccurrenceOfLetterBeforeOrAt);
            largestPalindromicSubstringWithThisOuter = 2 + largestInnerPalindromicSubstring;
            lookup[make_pair(outerPalindromeStart, outerPalindromeEnd)] = largestPalindromicSubstringWithThisOuter;
            //cout << "Added: " << outerPalindromeStart << "," << outerPalindromeEnd << " with value: " << largestPalindromicSubstringWithThisOuter << " to lookup; size: " << lookup.size() << endl; 
            if (lookup.size() % 10000 == 0)
                cout << "Lookup size: " << lookup.size() << endl;
        }
        else
        {
            largestPalindromicSubstringWithThisOuter = lookup[make_pair(outerPalindromeStart, outerPalindromeEnd)];
        }
        largestPalindromicSubstring = max(largestPalindromicSubstring, largestPalindromicSubstringWithThisOuter);
    }

    return largestPalindromicSubstring;
}

int main()
{
    string s;
    cin >> s;

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
    //vector<int> lookup(s.size(), -1);
    map<pair<int, int>, int> lookup;
    const int largest = findLargestPalindromicSubsequence(s, 0, s.size() - 1, lookup, firstOccurrenceOfLetterAfterOrAt, lastOccurrenceOfLetterBeforeOrAt);
    cout << "largest: " << largest << endl;
#if 0
    vector<PalindromicSubsequence> bestPalindromicSubsequenceEndingAt(s.size());
    
    for (int i = 0; i < s.size(); i++)
    {
        const int currentLetterIndex = s[i] - 'a';

        // A single letter is a palindromic subsequence.
        int longestPalindromicSequenceEndingHere = 1;
        int firstIndexOfPalindromicSequenceEndingHere = i;

        for (int earlierSubsequenceEnd = 0; earlierSubsequenceEnd < i; earlierSubsequenceEnd++)
        {
            const auto& earlierBestPalindromicSubsequence = bestPalindromicSubsequenceEndingAt[earlierSubsequenceEnd];
            const int newPalindromicSubsequenceFirstIndex = lastOccurrenceOfLetterIndexBefore[currentLetterIndex][earlierBestPalindromicSubsequence.firstIndex];
            if (newPalindromicSubsequenceFirstIndex == -1)
                continue;

            assert(s[newPalindromicSubsequenceFirstIndex] == s[i]);
            const int newPalindromicSubsequenceSize = earlierBestPalindromicSubsequence.size + 2;
            if (newPalindromicSubsequenceSize > longestPalindromicSequenceEndingHere)
            {
                longestPalindromicSequenceEndingHere = newPalindromicSubsequenceSize;
                firstIndexOfPalindromicSequenceEndingHere = newPalindromicSubsequenceFirstIndex;
            }
            else if (newPalindromicSubsequenceSize > longestPalindromicSequenceEndingHere)
            {
                if (firstIndexOfPalindromicSequenceEndingHere < newPalindromicSubsequenceFirstIndex)
                    firstIndexOfPalindromicSequenceEndingHere = newPalindromicSubsequenceFirstIndex;
            }

        }

        bestPalindromicSubsequenceEndingAt[i].size = longestPalindromicSequenceEndingHere;
        bestPalindromicSubsequenceEndingAt[i].firstIndex = firstIndexOfPalindromicSequenceEndingHere;

        cout << "i: " << i << " bestPalindromicSubsequenceEndingAt[i].size: " << bestPalindromicSubsequenceEndingAt[i].size << " bestPalindromicSubsequenceEndingAt[i].firstIndex: " << bestPalindromicSubsequenceEndingAt[i].firstIndex << endl;
    }
#endif
}
