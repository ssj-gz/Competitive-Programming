// Simon St James (ssjgz) 2017-7-20 11:17
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main()
{
    // Fairly easy one - it's fundamentally a "matching subsequence" question, where we are sometimes forced to match specific portions of A and B
    // together, and sometimes given a choice as to make a change to A to improve our chances of matching.
    // Imagine we have a pair of suffixes of A and B, A' and B'.  For example, if A = aBcaAdz and B = BCAAD, we might
    // have A' = aAdz and B' = AD.
    // Let 0 <= i <= A.size(), and 0 <= b <= B.size() be indices, and let A' and B' be the suffixes of A and B starting at i and j respectively - 
    // note that we treat the suffix of a string of length n beginning at n as the empty string.
    // Let canMatchSuffixes[i][j] be true if and only if A' and B' can be matched - that is, if there are some letters in A' that can be capitalised so 
    // that, after removing all lowercase letters in A', it is equal to B'.
    // There are a few cases to consider:
    //  - The first letter of A' is uppercase e.g. A' = Adz and B' = AD
    //  Note that if the first letter of A' is uppercase, we're stuck with it i.e. we can't remove it in order to exactly match
    // the suffix of B.  Thus, if the first letter of A' is uppercase and the first letters of A' and B' match, then
    // we *must* match them - we can then match A' and B' if and only if we canMatchSuffixes[i + 1][i + j].
    // Conversely, if the first letter of A' is uppercase and is *not* equal to the first letter of B', then we *cannot* match A' and B' (e.g.
    // A' = Adz and B' = BCAAD can't be matched - removing lowercase letters in A' always gives us 'A' as first letter, while B' begins with 'B').
    //  - The first letter of A' is lowercase.
    //   - ... and, when made uppercase is equal to the first letter of B'
    //     We have two options for matching, here: we can capitalise the first letter of A' in which case we *must* match it with the
    //     first letter of B', or we can keep it lowercase and see if we can match the remainder of A' with B'.
    //   - ... and, when made uppercase is *not* equal to the first letter of B'.  
    //     Here, we can only try to match the remainder of A' with B'.
    //  - A' is empty.
    //   - ... and so is B'.  This is a match.
    //   - ... and B' is non-empty.  There can be no match.
    //  - B' is empty, and A' is not.
    //   There is a match if and only if the remainder of A' has no (unremovable) uppercase letters.
    // That's about it - the rules above are used to form the canMatchSuffixes recurrence relation, which we just crunch through from end
    // to beginning. 


    int Q;
    cin >> Q;
    for (int q = 0; q < Q; q++)
    {
        string A, B;
        cin >> A >> B;
        vector<vector<bool>> canMatchSuffixes(A.size() + 1, vector<bool>(B.size() + 1, false));

        for (int i = A.size(); i >= 0; i--)
        {
            if (isupper(A[i]))
                break;
            // Can match all-lowercase suffix of A (or empty suffix of A) against empty B.
            canMatchSuffixes[i][B.size()] = true;
        }
        
        for (int aSuffixBegin = A.size() - 1; aSuffixBegin >= 0; aSuffixBegin--)
        {
            for (int bSuffixBegin = B.size() - 1; bSuffixBegin >= 0; bSuffixBegin--)
            {
                bool canMatch = true;
                const char aSuffixFirstChar = A[aSuffixBegin];
                const char bSuffixFirstChar = B[bSuffixBegin];

                if (isupper(aSuffixFirstChar))
                {
                    if (aSuffixFirstChar != bSuffixFirstChar)
                        canMatch = false;
                    else
                        canMatch = canMatchSuffixes[aSuffixBegin + 1][bSuffixBegin + 1]; 
                }
                else
                {
                    if (toupper(aSuffixFirstChar) == bSuffixFirstChar)
                    {
                        canMatch = canMatchSuffixes[aSuffixBegin + 1][bSuffixBegin + 1] || // Capitalise first letter of suffix of A; match remainders of both.
                                   canMatchSuffixes[aSuffixBegin + 1][bSuffixBegin];       // Leave first letter of suffix of A; match remainder of suffix of A against suffix of B.
                    }
                    else
                    {
                        // Leave first letter of suffix of A unchanged; try to match remainder of suffix of A against suffix of B.
                        canMatch = canMatchSuffixes[aSuffixBegin + 1][bSuffixBegin];
                    }
                }
                canMatchSuffixes[aSuffixBegin][bSuffixBegin] = canMatch;
            }
        }
        if (canMatchSuffixes[0][0])
            cout << "YES";
        else
            cout << "NO";

        cout << endl;
    }

}
