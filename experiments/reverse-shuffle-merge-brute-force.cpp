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

        cout << "trying with reverse(A) = " << revA << " and possible shuffle(A) = " << shuffleA << endl;
        if (sorted(revA) == sorted(shuffleA))
        {
            const string A = reversed(revA);
            cout << "found a solution: " << A << endl;
            if (best.empty() || A < best)
            {
                cout << " ** " << A << " is the new best solution so far!" << endl;
                best = A;
            }
        }
        else
        {
            cout << "The candidate solution A = " << reversed(revA) << " will not work, as the letters in S - reversed(A) ( = " << shuffleA << ") are not a permutation of A" << endl;
        }
    }
    cout << "The best found was " << best << ".  We tried " << numCandidateRevATested << " candidates for reverse(A) i.e. " << numCandidateRevATested << " subsequences of S" << endl;
    return best;
}

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int numLettersInA = rand() % 10'000 + 1;
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


#ifdef BRUTE_FORCE
    const auto resultBruteForce = bruteForce(s);
    cout << "resultBruteForce: " << resultBruteForce << endl;
#endif
}
