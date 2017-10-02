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

    // Calculate totalKSum/ totalUnfairness for initial (leftmost) block of K.
    int64_t totalKSum = 0;
    int64_t totalUnfairness = 0;
    for (int i = 0; i < K; i++)
    {
        totalUnfairness += min(i, K) * numCandiesInPacket[i] - totalKSum;
        totalKSum += numCandiesInPacket[i];
    }
    auto minUnfairness = totalUnfairness;

    // Calculate unfairness for remaining blocks of K.
    for (int i = K; i < N; i++)
    {
        const int64_t numInPacketLost = numCandiesInPacket[i - K];
        const int64_t amountGained = (numCandiesInPacket[i] * (K - 1) - (totalKSum - numInPacketLost));
        const int64_t amountLost = totalKSum - numInPacketLost - (K - 1) * numInPacketLost;
        totalUnfairness = totalUnfairness + amountGained - amountLost;
        totalKSum -= numCandiesInPacket[i - K];
        totalKSum += numCandiesInPacket[i];

        minUnfairness = min(minUnfairness, totalUnfairness);
    }
    cout << minUnfairness << endl;
}
