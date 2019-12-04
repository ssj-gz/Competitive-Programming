// Simon St James (ssjgz) - 2019-11-23
// 
// Solution to: https://www.codechef.com/problems/TRAINSET
//
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <map>
#include <algorithm>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!
#include <vector>

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        // TODO - generate randomised test.
        //const int T = rand() % 100 + 1;
        const int T = 10;
        cout << T << endl;

        for (int t = 0; t < T; t++)
        {
            const int N = rand() % 100 + 1;
            const int maxLetter = 1 + rand() % 26;
            const int maxWordLength = rand() % 5 + 1;
            struct WordClassification
            {
                string word;
                bool isSpam = false;
            };
            vector<WordClassification> wordClassifications;
            int i = 0;
            while (i != N)
            {
                const int wordLength = 1 + rand() % maxWordLength;
                string word;
                for (int j = 0; j < wordLength; j++)
                {
                    const char letter = 'a' + rand() % maxLetter;
                    word += letter;
                }
                const int numTimesAsSpam = rand() % (N - i + 1);
                i += numTimesAsSpam;
                const int numTimesAsNonSpam = rand() % (N - i + 1);
                i += numTimesAsNonSpam;
                for (int j = 0; j < numTimesAsSpam; j++)
                {
                    wordClassifications.push_back({word, true});
                }
                for (int j = 0; j < numTimesAsNonSpam; j++)
                {
                    wordClassifications.push_back({word, false});
                }
            }
            random_shuffle(wordClassifications.begin(), wordClassifications.end());

            cout << N << endl;
            for (const auto wordClassification : wordClassifications)
            {
                cout << wordClassification.word << " " << static_cast<int>(wordClassification.isSpam) << endl;
            }

        }

        return 0;
    }
    
    // TODO - read in testcase.
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();

        struct WordClassification
        {
            int numTimesMarkedAsSpam = 0;
            int numTimesMarkedAsNotSpam = 0;
        };

        map<string, WordClassification> wordClassifications;

        for (int i = 0; i < N; i++)
        {
            const string word = read<string>();
            const bool isSpam = (read<int>() == 1);
            if (isSpam)
            {
                wordClassifications[word].numTimesMarkedAsSpam++;
            }
            else
            {
                wordClassifications[word].numTimesMarkedAsNotSpam++;
            }
        }

        int trainingSetSize = 0;
        for (const auto& wordClassification : wordClassifications)
        {
            trainingSetSize += max(wordClassification.second.numTimesMarkedAsSpam, wordClassification.second.numTimesMarkedAsNotSpam);
        }

        cout << trainingSetSize << endl;
    }

    assert(cin);
}
