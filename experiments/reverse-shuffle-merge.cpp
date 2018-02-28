// Simon St James (ssjgz) - 2018-02-28
#define BRUTE_FORCE
//#define SUBMISSION
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <sys/time.h>


using namespace std;

const int numLetters = 26;

string sorted(const string& s)
{
    string sortedS(s);
    sort(sortedS.begin(), sortedS.end());
    return sortedS;
}

string reversed(const string& s)
{
    string reversedS(s);
    reverse(reversedS.begin(), reversedS.end());
    return reversedS;
}

string bruteForce(const string& s)
{
    cout << "s: " << s << endl;
    string best;
    vector<bool> putLetterInRevA(s.size());

    int blah = 0;
    while (true)
    {
        int nextLetterChoiceIndex = 0;
        while (putLetterInRevA[nextLetterChoiceIndex] && nextLetterChoiceIndex < putLetterInRevA.size())
        {
            putLetterInRevA[nextLetterChoiceIndex] = false;
            nextLetterChoiceIndex++;
        }
        
        if (nextLetterChoiceIndex == putLetterInRevA.size())
            break;

        putLetterInRevA[nextLetterChoiceIndex] = true;

        string revA;
        string shuffleA;
        for (int i = 0; i < s.size(); i++)
        {
            if (putLetterInRevA[i])
            {
                revA.push_back(s[i]);
            }
            else
            {
                shuffleA.push_back(s[i]);
            }
        }
        //cout << "revA: " << revA << endl;
        if (sorted(revA) == sorted(shuffleA))
        {
            const string A = reversed(revA);
            if (best.empty() || A < best)
            {
                best = A;
            }
        }
        blah++;
    }
    cout << "blah: " << blah << endl;
    return best;
}

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int numLettersInA = rand() % 10 + 1;
        const int maxLetter = rand() % numLetters + 1;
        string A;
        for (int i = 0; i < numLettersInA; i++)
        {
            A.push_back('a' + rand() % maxLetter);
        }
        string scrambed = A + A;
        random_shuffle(scrambed.begin(), scrambed.end());
        cout << scrambed << endl;
        return 0;
    }

    string s;
    cin >> s;

    int letterHistogram[numLetters] = {};
    for (const auto letter : s)
    {
        letterHistogram[letter - 'a']++;
    }

    assert(s.size() % 2 == 0);
    const int numLettersInA = s.size() / 2;


    int numOfLetterCanAddToReverseA[numLetters] = {};
    for (int letterIndex = 0; letterIndex < numLetters; letterIndex++)
    {
        assert(letterHistogram[letterIndex] % 2 == 0);
        numOfLetterCanAddToReverseA[letterIndex] = letterHistogram[letterIndex] / 2;
    }

    string sReversed = reversed(s);
    const string originalSReversed(sReversed);

    string A;
    while (A.size() < numLettersInA)
    {
        // Update numOfLetterCanAddToShuffle: every letter in the prefix we've removed so far
        // must be treated as "added to shuffle", *except* for those that have been added to A.
        int numOfLetterCanAddToShuffle[numLetters] = {};
        const int sizeOfRemovedPrefix = originalSReversed.size() - sReversed.size();
        for (int letterIndex = 0; letterIndex < numLetters; letterIndex++)
        {
            const int numOfLetterInPrefixNotAddedToShuffle = (letterHistogram[letterIndex] - numOfLetterCanAddToReverseA[letterIndex]);
            numOfLetterCanAddToShuffle[letterIndex] = numOfLetterInPrefixNotAddedToShuffle;
        }
        for (int i = 0; i < sizeOfRemovedPrefix; i++)
        {
            const int letterIndex = originalSReversed[i] - 'a';
            numOfLetterCanAddToShuffle[letterIndex]--;
        }

        // Find the next best character for A.
        char bestNextLetterIndex = numLetters;
        for (const auto letter : sReversed)
        {
            const int letterIndex = letter - 'a';
            if (numOfLetterCanAddToReverseA[letterIndex] > 0 && letterIndex < bestNextLetterIndex)
            {
                if (bestNextLetterIndex == numLetters || numOfLetterCanAddToShuffle[bestNextLetterIndex] > 0)
                {
                    if (bestNextLetterIndex != numLetters)
                        numOfLetterCanAddToShuffle[bestNextLetterIndex]--;

                    bestNextLetterIndex = letterIndex;
                }
            }
            else
            {
                numOfLetterCanAddToShuffle[letterIndex]--;
                const bool hitInvalidState = (numOfLetterCanAddToShuffle[letterIndex] < 0);
                if (hitInvalidState)
                    break;
            }
        }

        assert(bestNextLetterIndex != numLetters);
        const char bestNextLetter = 'a' + bestNextLetterIndex;
        A.push_back(bestNextLetter);
        // Remove the prefix up to and including the letter we just added (the first occurrence of bestNextLetter).
        sReversed.erase(sReversed.begin(),  sReversed.begin() + sReversed.find(bestNextLetter) + 1);

        numOfLetterCanAddToReverseA[bestNextLetterIndex]--;
    }

    const auto result = A;
    cout << result << endl;
#ifdef BRUTE_FORCE
    const auto resultBruteForce = bruteForce(s);
    cout << "result: " << result << " resultBruteForce: " << resultBruteForce << endl;
    assert(result == resultBruteForce);
#endif
}
