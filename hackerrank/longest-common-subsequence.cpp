// Simon St James (ssjgz) 2017-07-30 10:22
//#define SUBMISSION
#ifdef SUBMISSION
#undef NDEBUG
#endif
#include <iostream>
#include <string>
#include <vector>
#include <cassert>

using namespace std;

struct CommonSubsequenceInfo
{
    int longest = 0;
    bool isPartOfCommonSubsequence = false;
    int next_i = -1;
    int next_j = -1;
};

int main()
{
    int n, m;
    cin >> n >> m;
    vector<int> a(n), b(m);
    for (int i = 0; i < n; i++)
    {
        cin >> a[i];
    }
    for (int j = 0; j < m; j++)
    {
        cin >> b[j];
    }
    // commonSubsequenceInLast[i][j] represents the info about the largest common subsequence in the suffix of a
    // of length i and the suffix of b of length j.
    // The "xxx.size() + 1" is because we must deal with suffixes of length 0, 1, ... xxx.size().
    // For i = a.size(), commonSubsequenceInLast[i][j] = 0 for all j;
    // For j = b.size(), commonSubsequenceInLast[i][j] = 0 for all i - since we set all commonSubsequenceInLast to
    // 0, we don't need to set these explicitly.
    // The recurrence relation is simple:
    // if a[i] != b[j], then try skipping next letter of a and matching remainder of it against the remainder of b,
    // or skipping next letter of b and matching remainder of b against remainder of a, and pick the largest.
    // If a[i] == a[j], then do the above, but also match a[i] and a[j] and (common subsequence of length 1) and
    // try matching the remainder (after a[i] and a[j]) of a and b.
    // We make a note of which of the choices (next_i, next_j) we made and whether we matched the current pair (isPartOfCommonSubsequence).  
    // We then read this off at the end in order to find
    vector<vector<CommonSubsequenceInfo>> commonSubsequenceInLast(a.size() + 1, vector<CommonSubsequenceInfo>(b.size() + 1));

    for (int i = a.size() - 1; i >= 0; i--)
    {
        for (int j = b.size() - 1; j >= 0; j--)
        {
            //cout << "i: " << i << " a[i]: " << a[i] << " j: " << j << " b[j]: " << b[j] << endl;
            const int longestIfSkipA = commonSubsequenceInLast[i + 1][j].longest;
            CommonSubsequenceInfo commonSubsequenceInfo;
            commonSubsequenceInfo.longest = longestIfSkipA;
            commonSubsequenceInfo.next_i = i + 1;
            commonSubsequenceInfo.next_j = j;

            const int longestIfSkipB = commonSubsequenceInLast[i][j + 1].longest;
            if (longestIfSkipB > commonSubsequenceInfo.longest)
            {
                commonSubsequenceInfo.longest = longestIfSkipB;
                commonSubsequenceInfo.next_i = i;
                commonSubsequenceInfo.next_j = j + 1;
                
            }
            if (a[i] == b[j])
            {
                // Match a[i] and b[j] (increasing common subsequence by 1), skip them both, then try and match remainders of a and b.
                const int longestIfMatchAndSkipAAndB = 1 + commonSubsequenceInLast[i + 1][j + 1].longest;
                if (longestIfMatchAndSkipAAndB > commonSubsequenceInfo.longest)
                {
                    commonSubsequenceInfo.longest = longestIfMatchAndSkipAAndB;
                    commonSubsequenceInfo.isPartOfCommonSubsequence = true;
                    commonSubsequenceInfo.next_i = i + 1;
                    commonSubsequenceInfo.next_j = j + 1;
                }

            }
            commonSubsequenceInLast[i][j] = commonSubsequenceInfo;
        }
    }

    // Print subsequence.
    int i = 0, j = 0;
    int numInSubsequence = 0;
    while (i != -1 && j != -1)
    {
        if (commonSubsequenceInLast[i][j].isPartOfCommonSubsequence)
        {
            assert(a[i] == b[j]);
            cout << a[i] << " ";
            numInSubsequence++;
        }
        const int next_i = commonSubsequenceInLast[i][j].next_i;
        const int next_j = commonSubsequenceInLast[i][j].next_j;

        i = next_i;
        j = next_j;
    }
    assert(numInSubsequence == commonSubsequenceInLast[0][0].longest);
}
