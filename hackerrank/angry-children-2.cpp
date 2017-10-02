// Simon St James (ssjgz) 2017-10-02
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <cassert>

using namespace std;

int64_t calcUnfairness(const vector<int64_t>& numCandiesInPacket, int indexOfLastPacket, int K)
{
    int64_t dbgUnfairness = 0;
    for (int j = indexOfLastPacket - K + 1; j <= indexOfLastPacket; j++)
    {
        for (int l = j + 1; l <= indexOfLastPacket; l++)
        {
            dbgUnfairness += numCandiesInPacket[l] - numCandiesInPacket[j];
        }
    }
    return dbgUnfairness;
}

int main()
{
    int N, K;
    cin >> N >> K;

    vector<int64_t> numCandiesInPacket(N);

    for (int i = 0; i < N; i++)
    {
        cin >> numCandiesInPacket[i];
    }

    sort(numCandiesInPacket.begin(), numCandiesInPacket.end());

    int64_t totalKSum = 0;
    int64_t totalUnfairness = 0;
    auto minUnfairness = numeric_limits<int64_t>::max();

    for (int i = 0; i < K; i++)
    {
        totalUnfairness += min(i, K) * numCandiesInPacket[i] - totalKSum;
        totalKSum += numCandiesInPacket[i];
    }
    minUnfairness = min(minUnfairness, totalUnfairness);

    assert(totalUnfairness == calcUnfairness(numCandiesInPacket, K - 1, K));

    for (int i = K; i < N; i++)
    {
        const int64_t amountGained = (numCandiesInPacket[i] * (K - 1) - (totalKSum - 1 * numCandiesInPacket[i - K]));
        const int64_t amountLost = totalKSum - numCandiesInPacket[i - K] - (K - 1) * numCandiesInPacket[i - K];
        totalUnfairness = totalUnfairness + amountGained - amountLost;
        totalKSum -= numCandiesInPacket[i - K];
        totalKSum += numCandiesInPacket[i];

        assert(totalUnfairness == calcUnfairness(numCandiesInPacket, i, K));

        minUnfairness = min(minUnfairness, totalUnfairness);
    }
    cout << minUnfairness << endl;
}
