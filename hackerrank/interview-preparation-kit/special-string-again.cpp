// Simon St James (ssjgz) - 2019-06-15
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int64_t countNumSpecialSubstrings(const string& a)
{
    const int n = a.size();
    int64_t numSpecialSubstrings = 0;

    // Compute numInRunContainingIndex - this is O(N), despite the O(N) loop
    // with an O(N) loop - every i either does nothing, or fills in a numInRunContainingIndex
    // that has never been filled in, and each numInRunContainingIndex is only filled in once,
    // so the whole process is bounded by O(N).
    vector<int> numInRunContainingIndex(n, -1);
    char previousLetter = '\0';
    int numInCurrentRun = 0;
    for (int i = 0; i < n; i++)
    {
        if (a[i] != previousLetter)
        {
            for (int j = i - 1; j >= 0 && j >= i - numInCurrentRun; j--)
            {
                numInRunContainingIndex[j] = numInCurrentRun;
            }
            numInCurrentRun = 1;
        }
        else
        {
            numInCurrentRun++;
        }

        previousLetter = a[i];
    }
    // Finish off the remaining elements (those in the current run when we
    // hit the end).
    for (int j = n - 1; j >= 0 && j >= n - 1 - (numInCurrentRun - 1); j--)
    {
        numInRunContainingIndex[j] = numInCurrentRun;
    }

    auto numSubstrings = [](const int stringLength)
    {
        int64_t result = stringLength;
        result *= stringLength + 1;
        result /= 2;
        return result;
    };

    for (int i = 0; i < n;)
    {
        const auto numFromRun = numSubstrings(numInRunContainingIndex[i]);
        numSpecialSubstrings += numFromRun;

        if (i >= 2)
        {
            if (a[i - 2] == a[i] && a[i - 1] != a[i])
            {
                // Compute how many allSameButOne Special substrings a portion of this
                // run can be the end of (centred around a[i - 1]).
                const auto numFromAllButOne = min(numInRunContainingIndex[i - 2], numInRunContainingIndex[i]);
                numSpecialSubstrings += numFromAllButOne;
            }
        }

        // Skip to next run, if any.
        i += numInRunContainingIndex[i];
    }

    return numSpecialSubstrings;
}

int main(int argc, char* argv[])
{
    // Easy one.  It seems obvious here that we're interested in runs of the same
    // character.
    //
    // Consider the following string:
    //
    //   aabaaaaa
    //
    // This can be separated into 3 runs: one run of 2 a's; then one run of a single b;
    // then a run of 5 a's.
    //
    // All substrings of a run of length x will be Special, and there are numSubstrings = x * (x + 1) / 2
    // such substrings.  Thus, the runs give rise to, respectively, 2 * (2 + 1) / 2 = 3, 1 * (1 + 1) / 2 = 1 and
    // 5 * (5 + 1) = 6 Special substrings.
    //
    // That deals nicely with all Special substrings consisting of all the same letter; what about the allSameButOne ones?
    // These are also easy: assume we reach the beginning of a run of x occurences of a letter L at index i. 
    // Since this is the beginning of a run, either i == 0 (which we're not interested in) or a[i - 1] != a[i]. Assume further that 
    // a[i - 2] is the tail of a run (this requires that i >= 2, obviously!), and that this run is also comprised of the
    // letter L.  Then we'll be able to generate some number of allSameButOne, where some part of the run ending at a[i - 2] is the 
    // first part; a[i - 1] is the middle (different letter) part; and some part of the run beginning at a[i] is the end part.
    // We then just need to count the number of ways of forming such a Special substring centred around a[i-1], and
    // this is hopefully obviously min(length of run ending at a[i - 2], length of run beginning at a[i]).
    //
    // For example, at our string above, i = 3 begins a run of 5 a's, and i - 2 = 1 ends a run of 2 a's (a[i-1] = b).
    // We can form the following Special allSameButOne substrings centred around b:
    //
    //    aba
    //    aabaa
    //
    // and that's all - just two, as two = min(2, 5).
    //
    // And that's it - basically, just process each run of the same letter in turn, and see how many allSameLetter Special substrings
    // can be formed from that run, and how many allSameButOne substrings it can form the tail end of surrounding the letter
    // before the beginning of this run.
    int n;
    cin >> n;

    string a;
    cin >> a;

    cout << countNumSpecialSubstrings(a) << endl;
}
