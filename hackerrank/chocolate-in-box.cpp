// Simon St James (ssjgz) - 2017-12-06
#include <iostream>
#include <vector>

using namespace std;

int main()
{
    int N;
    cin >> N;

    int nimSum = 0;
    vector<int> a(N);
    for (int i = 0; i < N; i++)
    {
        cin >> a[i];
        nimSum ^= a[i];
    }

    const int originalNimSum = nimSum;
    int numWinningFirstMoves = 0;
    for (int i = 0; i < N; i++)
    {
        const int nimSumWithoutContainer = originalNimSum ^ a[i];
        if (nimSumWithoutContainer < a[i])
        {
            numWinningFirstMoves++;
        }
    }
    cout << numWinningFirstMoves << endl;
}
