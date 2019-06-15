#include <iostream>
#include <string>
#include <vector>

#include <sys/time.h>

using namespace std;

int64_t solveBruteForce(const string& a)
{
    const int n = a.size();
    int64_t numSpecialSubstrings = 0;
    for (int i = 0; i < a.size(); i++)
    {
        for (int length = 1; i + length <= n; length++)
        {
            const auto numLetters = 26;
            const auto substring = a.substr(i, length);
            if (length == 1)
            {
                numSpecialSubstrings++;
                continue;
            }
            else
            {
                int letterHistogram[numLetters] = {};
                for (const auto letter : substring)
                {
                    letterHistogram[letter - 'a']++;
                }
                bool allSameLetter = false;
                bool allSameButOne = false;
                for (const auto letterCount : letterHistogram)
                {
                    if (letterCount == length)
                    {
                        allSameLetter = true;
                        break;
                    }
                    if (letterCount == length - 1)
                    {
                        allSameButOne = true;
                        break;
                    }
                }
                if (allSameLetter)
                {
                    numSpecialSubstrings++;
                    continue;
                }
                if (allSameButOne && ((length % 2) == 1))
                {
                    if (substring[length / 2] != substring[0])
                    {
                        numSpecialSubstrings++;
                        continue;
                    }

                }
            }
        }
    }
    return numSpecialSubstrings;
}

int64_t solveOptimised(const string& a)
{
    const int n = a.size();
    int64_t numSpecialSubstrings = 0;

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
    for (int j = n - 1; j >= 0 && j >= n - 1 - (numInCurrentRun - 1); j--)
    {
        numInRunContainingIndex[j] = numInCurrentRun;
    }
    for (int i = 0; i < n; i++)
    {
        cout << "i: " << i << " numInRunContainingIndex: " << numInRunContainingIndex[i] << endl;
    }

    return numSpecialSubstrings;
}

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int n = rand() % 100 + 1;
        const int maxLetterIndex = rand() % 26 + 1;

        cout << n << endl;
        for (int i = 0; i < n; i++)
        {
            cout << static_cast<char>('a' + rand() % maxLetterIndex);
        }
        cout << endl;
        return 0;
    }

    int n;
    cin >> n;

    string a;
    cin >> a;

    const auto solutionBruteForce = solveBruteForce(a);
    cout << "solutionBruteForce: " << solutionBruteForce << endl;

    const auto optimisedSolution = solveOptimised(a);
    cout << "optimisedSolution: " << optimisedSolution << endl;

}
