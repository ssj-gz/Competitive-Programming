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
            if (a[i - 1] == b[j - 1])
            {
                maxCommonChildUpToHere = max(maxCommonChildUpToHere, 1 + maxCommonChildInFirstChars[i - 1][j - 1]);
            }
            maxCommonChildInFirstChars[i][j] = maxCommonChildUpToHere;
        }
    }
    return maxCommonChildInFirstChars;
}

int main()
{
    // Fairly easy one, I thought, though a couple of silly mis-steps along the way! Surprised at the comparatively
    // low completion rate (about 28% at the time of writing).
    // So: imagine we add some letter x at index posToAddAt in A, and it increases the LCS of A & B by one.
    // It's easy to prove the following by contradiction:
    //   - The added letter x is part of the new, longer LCS; and
    //   - Adding x can't increase the LCS by more than 1.
    //
    // Since x is part of the new, longer LCS, it must be "matched" with some letter in B; the one at
    // index bPosToMatchWith, say.  Thus, x must be b[bPosToMatchWith]. Therefore, we could just
    // simply try all |A| + 1 values of posToAddAt combined with all |B| values of bPosToMatchWith and
    // see the LCS of all resulting strings (taking care not to count adding the same value of x at the
    // same position more than once), but this would of course be rather inefficient :)
    // 
    // Consider the case where we've added the letter c at the ^'d position in A (originallly abdef), and matched it against
    // the *'d letter c in B:
    //
    //     abcdef
    //       ^
    //     bcdf
    //      *
    // Can we use information about the original LCS of A and B to quickly deduce the LCS of the new A and B? Yes :)
    //
    //     [ab]c[def]
    //         ^
    //     [b]c[df]
    //        *
    // It's hopefully easily seen that the LCS increases by 1 if and only if the LCS of the first chars in A and B before the ^ and *
    // (that is, ab from A, b from B) and the LCS of the last chars in A and B after the ^ and * (that is, def from A and df from B)
    // sum to the original LCS: combined with the newly-matched c in A & B, this would give a new LCS one greater than the original.
    // And that's about it: computing the matches for the first i chars in A and j chars in B is pretty typically for LCS
    // calculations, and computing the matches for the *last* chars in A and B is exactly the same, except that we operate on reversed
    // versions of A and B.


    auto reversed = [](const string& s)
    {
        return string(s.rbegin(), s.rend());
    };

    string a, b;
    cin >> a >> b;
    const auto maxCommonChildInFirstChars = findMaxCommonChildInFirstChars(a, b);
    const auto maxCommonChildInLastChars = findMaxCommonChildInFirstChars(reversed(a), reversed(b));
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
        bool letterIndexAlreadyAddedAtThisPos[numLetters] = {};
        for (int bPosToMatchWith = 0; bPosToMatchWith < b.size(); bPosToMatchWith++)
        {
            const int letterToAddIndex = getLetterIndex(b[bPosToMatchWith]);
            if (letterIndexAlreadyAddedAtThisPos[letterToAddIndex])
                continue;
#ifndef NDEBUG
            const string withAdded = a.substr(0, posToAddAt) + b[bPosToMatchWith] + a.substr(posToAddAt);
            assert(withAdded.size() == a.size() + 1);
#endif
            const int maxCommonChildBeforeNewlyAddedAndMatched = maxCommonChildInFirstChars[posToAddAt][bPosToMatchWith];
            const int maxCommonChildAfterNewlyAddedAndMatched = maxCommonChildInLastChars[a.size() - posToAddAt][b.size() - 1 - bPosToMatchWith];
            if (maxCommonChildBeforeNewlyAddedAndMatched + maxCommonChildAfterNewlyAddedAndMatched == maxCommonChildLength)
            {
                numWaysOfAddingToIncreaseCommonChildLength++;
                assert(findMaxCommonChildInFirstChars(withAdded, b)[withAdded.size()][b.size()] == maxCommonChildLength + 1);
                letterIndexAlreadyAddedAtThisPos[letterToAddIndex] = true;
            }
        }
    }
    cout << numWaysOfAddingToIncreaseCommonChildLength << endl;
}

