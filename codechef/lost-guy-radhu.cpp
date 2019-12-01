// Simon St James (ssjgz) - 2019-12-01
// 
// Solution to: https://www.codechef.com/FLPAST01/problems/MAY19F1
//
#include <iostream>
#include <vector>
#include <algorithm>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

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
        const int T = 1;
        cout << T << endl;

        for (int t = 0; t < T; t++)
        {
            const int N = rand() % 10 + 1;
            const int Q = N;

            const int maxValue = rand() % 100 + 1;

            cout << N << " " << Q << endl;
            vector<int> queries;
            for (int i = 0; i < N; i++)
            {
                queries.push_back(i + 1);
            }
            random_shuffle(queries.begin(), queries.end());
            vector<int> scores;
            for (int i = 0; i < N; i++)
            {
                scores.push_back(rand() % (maxValue + 1));
            }

            for (int i = 0; i < N; i++)
            {
                cout << scores[i] ;
                if (i != N -1)
                    cout << " ";
            }
            cout << endl;
            for (int i = 0; i < Q; i++)
            {
                cout << queries[i] ;
                if (i != Q -1)
                    cout << " ";
            }
            cout << endl;

        }

        return 0;
    }
    
    // TODO - read in testcase.
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();
        const int Q = read<int>();

        vector<int> scores(N);
        for (auto& score : scores)
            score = read<int>();

        vector<int> queries(Q);
        for (auto& query : queries)
            query = read<int>() - 1;

        vector<int> maxScores;
        int maxScore = -1;
        for (const auto& score : scores)
        {
            maxScore = max(maxScore, score);
            maxScores.push_back(maxScore);
        }

        for (const auto& query : queries)
            cout << maxScores[query] << endl;

    }

    assert(cin);
}
