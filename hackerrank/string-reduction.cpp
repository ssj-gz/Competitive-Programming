#include <iostream>
#include <string>
#include <limits>
#include <cassert>

using namespace std;

const int maxStringLength = 100;
const int numLetters = 3;

// i.e. "minLetterCountInRange[i][j][x]": in the substring s[i..j], 
// what is the smallest number of  letter x we can reduce the substring to?
// NB: will be 0 if we can't produce any runs of that letter at all.
int minLetterCountInRange[maxStringLength][maxStringLength][numLetters];

int calcMinLetterCountInRange(const string& s, int rangeBegin, int rangeEnd, int letter)
{
    const int memoisedResult = minLetterCountInRange[rangeBegin][rangeEnd][letter];
    if (memoisedResult != -1)
    {
        return memoisedResult;
    }

    if (rangeBegin == rangeEnd)
    {
        return (s[rangeBegin] == letter) ? 1 : 0;
    }
    bool allSameLetter = true;
    for (int i = rangeBegin + 1; i <= rangeEnd; i++)
    {
        if (s[i] != s[i - 1])
        {
            allSameLetter = false;
            break;
        }
    }
    if (allSameLetter)
    {
        return s[rangeBegin] == letter ? rangeEnd - rangeBegin + 1 : 0;
    }

    int minOfLetter = std::numeric_limits<int>::max();
    for (int i = rangeBegin + 1; i <= rangeEnd; i++)
    {
        for (int leftLetter = 0; leftLetter < numLetters; leftLetter++)
        {
            for (int rightLetter = 0; rightLetter < numLetters; rightLetter++)
            {
                const int leftRangeBegin = rangeBegin;
                const int leftRangeEnd = i - 1;
                const int minLeftLetterCountOnLeft = calcMinLetterCountInRange(s, leftRangeBegin, leftRangeEnd, leftLetter);
                // Memo-ise.
                minLetterCountInRange[leftRangeBegin][leftRangeEnd][leftLetter] = minLeftLetterCountOnLeft;
                if (minLeftLetterCountOnLeft == 0)
                    continue;

                const int rightRangeBegin = i;
                const int rightRangeEnd = rangeEnd;
                const int minRightLetterCountOnRight = calcMinLetterCountInRange(s, rightRangeBegin, rightRangeEnd, rightLetter);
                // Memo-ise.
                minLetterCountInRange[rightRangeBegin][rightRangeEnd][rightLetter] = minRightLetterCountOnRight;
                if (minRightLetterCountOnRight == 0)
                    continue;

                if (leftLetter == rightLetter)
                {
                    // i.e. XXXXXXXXXXXXX, which is irreducible.
                    if (leftLetter == letter && rightLetter == letter)
                        minOfLetter = min(minOfLetter, minLeftLetterCountOnLeft + minRightLetterCountOnRight);
                }
                else 
                {
                    // i.e. XXX...XYY...Y.  Let's say there are n X's and m Y's.  Let Z be not X nor Y.
                    // It can be shown (probably by induction) that:
                    //  - if both n and m are even, then we can produce XX and YY - no smaller runs are possible,
                    //    and we can't end up with any Z's.
                    // - if both m and n are odd, we can end up with precisely one Z, but no X's or Y's.
                    // - if precisely one is odd (n, say), then we can end up with precisely one X (or if m is 
                    //  the odd one, precisely one Y).  We can't end up with any Z's.
                    const bool leftLetterRunIsOdd = ((minLeftLetterCountOnLeft % 2) == 1);
                    const bool rightLetterRunIsOdd = (minRightLetterCountOnRight % 2) == 1;
                    if (!leftLetterRunIsOdd && !rightLetterRunIsOdd)
                    {
                        if (leftLetter == letter || rightLetter == letter)
                        {
                            minOfLetter = min(minOfLetter, 2);
                        }
                    }
                    else if (leftLetterRunIsOdd && rightLetterRunIsOdd)
                    {
                        const int otherLetter = 3 - (leftLetter + rightLetter);
                        if (otherLetter == letter)
                        {
                            minOfLetter = min(minOfLetter, 1);
                        }
                    }
                    // Precisely one is odd.
                    else if (leftLetter == letter && leftLetterRunIsOdd)
                    {
                        minOfLetter = min(minOfLetter, 1);
                    }
                    else if (rightLetter == letter && rightLetterRunIsOdd)
                    {
                        minOfLetter = min(minOfLetter, 1);
                    }
                }
            }
        }
    }
    // Memo-ise.
    if (minOfLetter == std::numeric_limits<int>::max())
    {
        // Couldn't produce any of the required letter; answer is 0.
        minOfLetter = 0;
    }
    minLetterCountInRange[rangeBegin][rangeEnd][letter] = minOfLetter;
    return minOfLetter;
}

int main()
{
    // Fairly tricky.  Firstly, note the obvious fact that a string cannot be 
    // reduced further if and only if that string consists entirely of one letter.
    // For each way of splitting the string into a prefix and suffix, find, for each letter,
    // the minimum of that letter producable by the prefix, and the minimum of that
    // letter producable by the suffix.  By combining the results across all prefix+suffixes,
    // and then all pairs of letters, we can deduce the smallest length
    // our original string can be reduced to; more details are given in calcMinLetterCountInRange.
    // Finding the "minimum of that letter producable by the prefix/ suffix" of course
    // requires recursion.  Note that in the recursion step, we will have to process
    // suffixes of the prefix and prefixes of the suffix i.e. we end up dealing with not
    // just prefixes and suffixes of the original string, but arbitrary *ranges*
    // of the original string.

    int T;
    cin >> T;
    for (int t = 0; t < T; t++)
    {
        string s;
        cin >> s;
        // Reduce the characters of the string to 0 .. numLetters - 1 : 
        // makes life a little easier :) 
        for (auto& character : s)
        {
            character -= 'a';
        }
        // Initialise.
        for (int i = 0; i < maxStringLength; i++)
        {
            for (int j = 0; j < maxStringLength; j++)
            {
                for (int letter = 0; letter < numLetters; letter++)
                {
                    minLetterCountInRange[i][j][letter] = -1;
                }
            }
        }
        // Compute.
        int minimumLength = std::numeric_limits<int>::max();
        for (int letter = 0; letter < numLetters; letter++)
        {
            const int minOfLetter = calcMinLetterCountInRange(s, 0, s.size() - 1, letter);
            if (minOfLetter == 0)
                continue;
            minimumLength = min(minimumLength, minOfLetter);
        }
        cout << minimumLength << endl;
    }
}



