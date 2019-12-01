// Simon St James (ssjgz) - 2019-12-01
// 
// Solution to: https://www.codechef.com/FLPAST01/problems/MAY19F1
//
#include <iostream>
#include <vector>

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
