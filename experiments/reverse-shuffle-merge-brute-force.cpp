// Simon St James (ssjgz) - 2018-02-28
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <sys/time.h>

//#define VERY_VERBOSE

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
    // Warning: this is O(2 ** N x N), so will explode only strings larger than twenty characters, say!
    // There's no way it can deal with |S| = 1'000 as in the original problem, so it is useful for
    // teaching only!
    //
    // Basic approach: find all subsequences of s, and for each one, treat it
    // as if it is reverse(A).
    //
    // Then see if the remaining letters are a permutation of A i.e. if they can
    // be the result of shuffle(A).
    cout << "s: " << s << endl;
    string best;
    vector<bool> putLetterInRevA(s.size());

    int numCandidateRevATested = 0;
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
        numCandidateRevATested++;

#ifdef VERY_VERBOSE
        cout << "trying with reverse(A) = " << revA << " and possible shuffle(A) = " << shuffleA << endl;
#endif
        if (sorted(revA) == sorted(shuffleA))
        {
            const string A = reversed(revA);
            cout << endl;
            cout << "Found a solution: " << A << endl;
            cout << "Pick " << shuffleA << " as shuffle(A), a permutation of " << A << endl;
            cout << "reverse(A) is " << revA << endl << endl;
            cout << "Merge as follows: " << endl << endl;
            cout << " reverse(A): ";
            for (int i = 0; i < putLetterInRevA.size(); i++)
            {
                if (putLetterInRevA[i])
                    cout << s[i];
                else
                    cout << " ";
            }
            cout << endl;
            cout << " shuffle(A): ";
            for (int i = 0; i < putLetterInRevA.size(); i++)
            {
                if (putLetterInRevA[i])
                    cout << " ";
                else
                    cout << s[i];
            }
            cout << endl;
            cout << "             " << string(s.size(), '=') << endl;
            cout << "             " << s << ", the original string S " << endl << endl;
            if (best.empty() || A < best)
            {
                cout << " ** " << A << " is the new best solution so far!" << endl;
                best = A;
            }
        }
        else
        {
#ifdef VERY_VERBOSE
            cout << "The candidate solution A = " << reversed(revA) << " will not work, as the letters in S - reversed(A) ( = " << shuffleA << ") are not a permutation of A" << endl;
#endif
        }
    }
    cout << "The best found was " << best << ".  We tried " << numCandidateRevATested << " candidates for reverse(A) i.e. " << numCandidateRevATested << " subsequences of S" << endl;
    return best;
}

int main(int argc, char* argv[])
{
    if (argc == 2 && string(argv[1]) == "--test")
    {
        // Generate a small random testcase - no more than twenty letters,
        // as this is about the most our brute force generator can
        // comfortably deal with!
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
        string scrambled = A + A;
        random_shuffle(scrambled.begin(), scrambled.end());
        cout << scrambled << endl;
        return 0;
    }

    string s;
    cin >> s;


    const auto resultBruteForce = bruteForce(s);
    cout << "resultBruteForce: " << resultBruteForce << endl;
}
