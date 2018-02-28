// Simon St James (ssjgz) - 2018-02-28
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;

const int numLetters = 26;

string reversed(const string& s)
{
    string reversedS(s);
    reverse(reversedS.begin(), reversedS.end());
    return reversedS;
}

int main(int argc, char* argv[])
{
    // Fairly easy one.  Reverse the given string s to give sReversed; then we're essentially asked to
    // decompose sReversed into two subsequences: one will be A; the other will be some shuffling of A (shuffleA)
    // i.e. we need to decompose sReversed into two subsequences that are anagrams of each other and,
    // over the set of all such decompositions, give the one with the smallest A.  Recall that two
    // strings are anagrams of each other if and only if they have the same letter histogram.
    //
    // Now, we want the first letter of A to be as small as possible so, intuitively, if the first letter
    // of sReversed is "big" (e.g. 'z') then we don't want that to be part of the A-subsequence of sReversed,
    // so it must be put into the shuffleA-subsequence.  If the next letter is also "big" ('y', say), then we also
    // want to put this letter into shuffleA.  If it is small, however ('a', say), then we want that to be part of
    // the A-subsequence.  So when choosing the first letter of A, we want to scan along sReversed, putting "big" letters
    // into shuffleA and hopefully choosing the smallest letter in sReversed for our A-subsequence.
    //
    // Of course, we can't just keep bumping undesirable letters into shuffleA as the eventual decomposition into A-subsequence
    // and shuffleA-subsequence must have identical histograms; so, for example, given:
    //
    //  s = aabbbbyzyz
    //
    // then 
    //
    //  sReversed = zyzybbbbaa ( = originalSReversed)
    //
    // We may put the first 'z' into shuffleA-subsequence and even the first 'y', but the next 'z' we encounter *cannot* be put into
    // shuffleA - this is because any decomposition must have one 'z', one 'y', two 'b's and one 'a' in each of A and shuffleA.  So
    // we can't skip all the big 'z's and 'y's to get  a nice 'b' or 'a' for our first letter of A (as it happens, the best A we can
    // get is 'yzbba'!).  So what we are forced to do is scan from right to left, adding "big" letters to shuffleA only when we are able
    // to i.e. only when the number of a given letter x we've added to shuffleA does not exceed letterHistogram[x] / 2.
    //
    // So: the procedure for finding the first letter in A is as follows: set bestNextLetter (what will be the first letter in A) to 
    // some value that is higher than all other letters, and commence scanning sReversed from left to right.  If the current letter x in
    // our scan is < bestNextLetter, then this is a better choice; otherwise, we have to add x to shuffleA if we can (i.e. if the number
    // of times we've added x to shuffleA so far is <= letterHistogram[x] / 2), and proceed onwards.  If we *can't* add x to shuffleA, then
    // we pick the current bestNextLetter as our first letter of A.
    //
    // In the example above, the first letter we encounter is z; this is better than our current bestNextLetter, so we set bestNextLetter to z.
    // The next letter we encounter is y; this is even better than z, so we now add the previously-chosen z to shuffleA and set bestNextLetter = y.
    // Note that we've now added z to shuffleA once, and letterHistogram[z] = 2, so we can add it to shuffleA a maximum of letterHistogram[z] / 2 = 1
    // times.
    //
    // The next letter we encounter is another z: this is not better than y, but we can't add it to shuffleA as we'd exceed our "z's in shuffleA" limit
    // of 1, so the search for the best first letter of A stops here (before we even reach the a's or b's!) with bestNextLetter = y.
    //
    // So we've found our first letter of A - the y as index 1. Since the next letter of A must occur after this index, we must delete the prefix of sReversed
    // up to and including this y, so that we must find the 2nd letter of A in the string:
    //
    //  sReversed = zybbbbaa
    //
    // Now, note that we begin our search having added 1 z to shuffleA; in fact in general, for each letter x, we must have added x to shuffleA:
    //
    //  number of occurrences of x in the prefix we've removed from originalSReversed, minus the number of times x was added to A (*)
    //
    // so we need to keep this in mind as we scan along the remaining portion of originalSReversed, sReversed.
    //
    // The first letter we encounter in sReversed in our search for the third letter of A is z.  This is better than our initial bestNextLetter (which,
    // as stated, is chosen to be higher than all letters) so we set bestNextLetter = z.  The next letter we encounter is y; this is better than our
    // bestNextLetter (== z), so it would be nice to choose this as our new bestNextLetter.  However, we can't do this: we've already added one y to
    // A, and there are only two y's in total; thus, this y must, regrettably, be added to shuffleA instead.  In general, the number of the letter x
    // we add to A cannot exceed letterHistogram[x] / 2.
    //
    // So we continue onwards, with bestNextLetter still == z, and eventually reach b.  Hooray - this is less than z, and we haven't added any b's to A
    // yet, so surely we can set bestNextLetter to b? Unfortunately, no we can't - if we did this, then we would be forced to add our previous bestNextLetter
    // to shuffleA, and we have already reached our quota of 1 for the number of z's in shuffleA.  Similarly, the remaining b's and a's are disqualified
    // from becoming the bestNextLetter for the exact same reasons: thus, when we finish our scan, we still have bestNextLetter == z, so the next
    // letter of A is z (A = yz, so far).
    //
    // So we knock off the prefix up to and including this first z we encountered; we now have
    //
    //  originalSReversed = zyzybbbbaa
    //  sReversed =            ybbbbaa
    //
    // and the number of z's added to A is 1; the number of y's added to A is 1 and so from (*) the number of z's added to shuffleA is 1.
    //
    // What about the third letter of A? The first letter we encounter is y which, as we saw above, is disqualifed from being bestNextLetter due to 
    // exceeding the number of y's we can add to A, so we add it to shuffleA instead  The next letter is 'b' - this is fine: there's no reason
    // to exclude it from being bestNextLetter - no b's have been added to A, yet.  We next encounter another 'b' - this is no better than bestNextLetter,
    // so we add b to shuffleA.  The next letter is again b, and this is no better; we must again add it to shuffleA.  We next encounter yet another
    // b, but this time, we can't add it to shuffleA - we've already added two b's, and the maximum number of times we can add a b to shuffleA
    // is letterHistogram[b] / 2 = 4 / 2 = 2.   Thus, we must abort our search for the third letter, and use our current bestNextLetter, b.
    //
    // Then we have:
    //
    //  originalSReversed = zyzybbbbaa
    //  sReversed =              bbbaa
    //
    // and so on; eventually (in O(|s|^2)), we'll have filled out all letters in A.  And that's about it: there's a few minor differences between
    // the code and the description above: the code, instead of checking whether the number of a given letter we've added to A/ shuffleA exceeds
    // the quota, instead uses countdowns (numOfLetterCanAddToA/ numOfLetterCanAddToShuffleA, respectively), and rather than setting the initial 
    // bestNextLetter to some value that is higher than all letters, I introduce haveCandidateBestNextLetter instead.
    string s;
    cin >> s;

    int letterHistogram[numLetters] = {};
    for (const auto letter : s)
    {
        letterHistogram[letter - 'a']++;
    }

    assert(s.size() % 2 == 0);
    const int numLettersInA = s.size() / 2;

    int numOfLetterCanAddToA[numLetters] = {};
    for (int letterIndex = 0; letterIndex < numLetters; letterIndex++)
    {
        assert(letterHistogram[letterIndex] % 2 == 0);
        numOfLetterCanAddToA[letterIndex] = letterHistogram[letterIndex] / 2;
    }

    string sReversed = reversed(s);
    const string originalSReversed(sReversed);

    string A;
    while (A.size() < numLettersInA)
    {
        // Update numOfLetterCanAddToShuffleA, according to the logic in (*): every letter in the prefix we've removed so far
        // must be treated as "added to shuffle", *except* for those that have been added to A.
        int numOfLetterCanAddToShuffleA[numLetters] = {};
        for (int letterIndex = 0; letterIndex < numLetters; letterIndex++)
        {
            const int numOfLetterInPrefixAddedToA = (letterHistogram[letterIndex] - numOfLetterCanAddToA[letterIndex]);
            numOfLetterCanAddToShuffleA[letterIndex] = numOfLetterInPrefixAddedToA;
        }
        const int sizeOfRemovedPrefix = originalSReversed.size() - sReversed.size();
        for (int i = 0; i < sizeOfRemovedPrefix; i++)
        {
            const int letterIndex = originalSReversed[i] - 'a';
            numOfLetterCanAddToShuffleA[letterIndex]--;
        }

        // Find the next best character for A.
        int bestNextLetterIndex = -1;
        bool haveCandidateBestNextLetter = false;
        for (const auto letter : sReversed)
        {
            const int letterIndex = letter - 'a';
            const bool isBetterCandidate = ((!haveCandidateBestNextLetter || letterIndex < bestNextLetterIndex) && numOfLetterCanAddToA[letterIndex] > 0);
            const bool canAddPreviousBestToShuffleA = (!haveCandidateBestNextLetter || numOfLetterCanAddToShuffleA[bestNextLetterIndex] > 0);
            if (isBetterCandidate && canAddPreviousBestToShuffleA)
            {                                          
                if (haveCandidateBestNextLetter)
                    numOfLetterCanAddToShuffleA[bestNextLetterIndex]--;

                bestNextLetterIndex = letterIndex;
                haveCandidateBestNextLetter = true;
            }  
            else
            {  
                // Add to shuffleA.
                numOfLetterCanAddToShuffleA[letterIndex]--;
                const bool hitInvalidState = (numOfLetterCanAddToShuffleA[letterIndex] < 0);
                if (hitInvalidState)         
                    break;                   
            }
        }

        assert(haveCandidateBestNextLetter);

        const char bestNextLetter = 'a' + bestNextLetterIndex;
        A.push_back(bestNextLetter);
        // Remove the prefix up to and including the letter we just added (the first occurrence of bestNextLetter).
        sReversed.erase(sReversed.begin(),  sReversed.begin() + sReversed.find(bestNextLetter) + 1);

        numOfLetterCanAddToA[bestNextLetterIndex]--;
    }

    const auto result = A;
    cout << result << endl;
}
