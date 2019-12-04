// Simon St James (ssjgz) - 2019-11-23
// 
// Solution to: https://www.codechef.com/problems/TRAINSET
//
#include <iostream>
#include <map>

#include <cassert>

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

        // Everything read in; now just compute the result.
        int trainingSetSize = 0;
        for (const auto& wordClassification : wordClassifications)
        {
            trainingSetSize += max(wordClassification.second.numTimesMarkedAsSpam, wordClassification.second.numTimesMarkedAsNotSpam);
        }

        cout << trainingSetSize << endl;
    }

    assert(cin);
}
