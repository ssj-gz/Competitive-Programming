// Simon St James (ssjgz) - 2019-04-14 19:07
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
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
            assert(i < result.size()); // There are no trailing 0's in result at this point, remember!
            cleanedResult.back() += result[i];
        }
    }
    return cleanedResult;
}

int main(int argc, char* argv[])
{
    // Fairly easy one.  Note that the size of the uncompressed binary strings
    // are too large to deal with, so we must instead manipulate the compressed
    // representations, though as we'll see, this is not too difficult.
    //
    // Imagine that we did uncompress the binary string, and counted the number
    // of bits in it.  Then imagine we incremented this binary number by 1
    // until we had a number with the same number of set bits in it.  How many
    // times would we have to do this, and what would the resultant binary string
    // look like?
    //
    // Imagine that the binary string happens to end in a string of X 0's.  If we
    // increment once, then we have a 1 at the end of this string of 0's, and the
    // number of set bits is one more than it was originally.  Here's an example with
    // X = 6, after we've incremented once: the original number of bits was 4.
    //
    //   01111000001 
    //
    // Increment again, and we *still* have strictly more set bits than we had 
    // originally.  And again.  And again.
    //
    //   01111000010 
    //   01111000011 
    //
    // In fact, we will always have strictly more set bits than we had originally until
    // we "overflow" the 0's (and the 1's immediately to the left of these 0's), 
    // 2 ** x increments to the original number later:
    //
    //   10000000000 
    //
    // We now have fewer set bits than we originally had.  What's the smallest number
    // of increments we can add to restore the original number of set bits (4)?
    // This is the number of increments we need to achieve in order to get the
    // rightmost 4 - 1 = 3 bits all set to 1, which is (2 ** 3) - 1:
    //
    //   10000000111
    //
    // and this is the number we want.
    //
    // Note that no digits to the left of the initial leading 0 will have changed: in general,
    // answering the question where we have trailing 0's will only alter the last 3
    // compressed elements, leaving the rest of the compressed representation completely
    // unchanged.  With a bit of jiggery-pokery, we can then easily answer the question
    // with only minor changes to the compressed representation of the input.
    //
    // And that's the basic gist of it: the case where the number ends in 1's is similar.
    // Hopefully the inline comments inside solve(...) explain the rest!
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
