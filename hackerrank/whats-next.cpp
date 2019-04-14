// Simon St James (ssjgz) - 2019-04-14 19:07
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#define NDEBUG
#undef BRUTE_FORCE
#endif
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;

vector<int64_t> solve(const vector<int64_t>& a)
{
    vector<int64_t> result = a;
    const bool lastDigitsAre0s = ((a.size() % 2) == 0);
    if (lastDigitsAre0s)
    {
        // Need to transform e.g.
        //
        //   XXXX01111111000
        //
        // to
        //
        //   XXXX10000111111
        //
        // or, if if a.size() == 2 e.g.:
        //
        //   1111111000
        //
        // to
        //
        //   10000111111
        // 
        // In either case, the last two blocks are replaced with 
        // a string of the same length, this time beginning with 0's
        // instead of 1's, and with one less 1 than in the original
        // last two blocks.
        // The digit before the last two blocks must become a 1.
        assert(a.size() > 1);
        const auto originalNumTrailing0s = a[a.size() - 1];
        const auto originalNum1sBefore = a[a.size() - 2];
        const auto totalInLastTwoBlocks = originalNum1sBefore + originalNumTrailing0s;
        // Pop off last two blocks (the 1's and the 0's).
        result.pop_back(); 
        result.pop_back();
        if (!result.empty())
        {
            result.back()--; // Make a note that one of the 0's has become a 1 (the "digit before the last two blocks").
        }
        result.push_back(1); // Add a single 1 (the "digit before the last two blocks" )
        const auto newNumTrailing1s = originalNum1sBefore - 1; // Need 1 less 1, due to the one we've just added.
        const auto newNum0s = totalInLastTwoBlocks - newNumTrailing1s;
        result.push_back(newNum0s); // Add the block of 0's.
        result.push_back(newNumTrailing1s); // Add the block of 1's.
    }
    else
    {
        // Easier case: transform e.g.
        // 
        //   XXXXX0001111111
        //
        // to
        // 
        //   XXXXX0010111111
        //
        // i.e. last block (all 1's) removed; digit that was to left (if any:
        // insert one if there wasn't one there originally) turned into a 1; 
        // a single "0" inserted; and one less 1 than in the original last block
        // appended.
        // The case where we only had one block originally is not much harder.
        const auto originalNumTrailing1s = a[a.size() - 1];
        result.pop_back();
        if (!result.empty())
        {
            result.back()--; // One of the 0's before what was the last block of 1's has become a 1.
        }
        result.push_back(1); // Add the single 1.
        result.push_back(1); // Add the single 0.
        const auto newNumTrailing1s = originalNumTrailing1s - 1; // Need one less 1, due to the one (tee-hee) we've just added.
        result.push_back(newNumTrailing1s); // Add remaining block of 1's.
    }
    // Prepare output to be in correct format - the correct answer
    // can be expressed in many different ways, but only one is 
    // accepted!
    // No trailing 0's in the compressed version.
    while (!result.empty() && result.back() == 0)
    {
        result.pop_back();
    }
    // Also, no 0's anywhere except the first place (if the number
    // begins with 0's) - though I'm not sure if that particular input
    // can occur.
    vector<int64_t> cleanedResult;
    cleanedResult.push_back(result.front());
    for (int i = 1; i < result.size(); i++)
    {
        if (result[i] != 0)
        {
            cleanedResult.push_back(result[i]);
        }
        else
        {
            i++;
            assert(!cleanedResult.empty());
            cleanedResult.back() += result[i];
        }
    }
    return cleanedResult;
}

int main(int argc, char* argv[])
{
    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        int n;
        cin >> n;

        vector<int64_t> a(n);
        for (int i = 0; i < n; i++)
        {
            cin >> a[i];
        }
        const auto result = solve(a);
        cout << result.size() << endl;
        for (const auto x : result)
        {
            cout << x << " ";
        }
        cout << endl;
    }
}
