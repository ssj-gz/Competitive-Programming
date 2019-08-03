// Simon St James (ssjgz) - 2019-08-03
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
    const int T = read<int>();
    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();
        vector<int> a(N);
        for (auto& x : a)
        {
            x = read<int>();
        }
        vector<int> b(N);
        for (auto& x : b)
        {
            x = read<int>();
        }

        int maxScore = -1;
        for (int i = 0; i < N; i++)
        {
            int score = 20 * a[i] - 10 * b[i];
            score = max(score, 0);
            maxScore = max(maxScore, score);
        }
        cout << maxScore << endl;
    }
}

