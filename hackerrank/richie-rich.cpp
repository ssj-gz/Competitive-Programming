#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cassert>
using namespace std;


int main() {
    long n, k;
    cin >> n >> k;
    string s;
    cin >> s;
    long minChangesRequiredForPalindrome = 0;
    for (int i = 0; i < s.size() / 2; i++)
    {
        char& leftChar = s[i];
        char& rightChar = s[s.size() - i - 1];
        if (leftChar != rightChar)
            minChangesRequiredForPalindrome++;
    }
    if (minChangesRequiredForPalindrome > k)
    {
        cout << "-1";
        return 0;
    }
    long surplusChanges = k - minChangesRequiredForPalindrome;
    long numChangesMade = 0;
    // If we've got some surplus changes, then we might try
    // changing digits (plus their 'opposite' digits) to '9's, starting
    // from the outside and working our way in.
    // Only do this if *both* digit and opposite digit are not '9's already,
    //  though - the case where exactly one of them is is a "necessary"
    // change which we'll handle later.
    int i = 0;
    while (surplusChanges > 0 && i < s.size() / 2)
    {
        char& leftChar = s[i];
        char& rightChar = s[s.size() - i - 1];
        if (leftChar != '9' && rightChar != '9')
        {
            if (leftChar != rightChar) 
            {
                leftChar = '9';
                rightChar = '9';
                // We no longer have to worry about the (necessary) change of changing just one of these chars;
                // that's a bonus surplus change!
                surplusChanges++;
                // ... but we did need to change 2 characters.
                surplusChanges -= 2;
                numChangesMade += 2;
            }
            else
            {
                // Must change both - can't change just one.
                if (surplusChanges >= 2)
                {
                    leftChar = '9';
                    rightChar = '9';
                    surplusChanges -= 2;
                    numChangesMade += 2;
                }
            }
        }
        i++;
    }
    // Make the remaining *necessary* changes for a palindrome; for each
    // change, tranform the lowest digit into the highest digit e.g.
    // in s = 5435, we can either change the 4 into a 3 or the 3 into a 4 - 
    // pick the latter!
    for (int i = 0; i < s.size() / 2; i++)
    {
        char& leftChar = s[i];
        char& rightChar = s[s.size() - i - 1];
        if (leftChar != rightChar)
        {
            if (leftChar > rightChar)
                rightChar = leftChar;
            else
                leftChar = rightChar;
            numChangesMade++;
        }
    }
    if ((n & 1) == 1 && numChangesMade < k)
    {
        // We've done all the "greedy" and "necessary" changes, except one:
        // change the middle element (for odd n, of course) to a '9'.
        char& middleChar = s[s.size() / 2];
        if (middleChar != '9')
        {
            middleChar = '9';
            numChangesMade++;
        }
    }
    assert(numChangesMade <= k);
    assert(s == string(s.rbegin(), s.rend()));
    cout << s;
    return 0;
}

