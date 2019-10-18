// Simon St James (ssjgz) - 2019-10-18
// 
// Solution to: https://www.codechef.com/problems/SPAMCLAS
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

std::pair<int, int> findNumNonSpammersAndSpammers(const vector<int>& weights, const vector<int>& bias, int minX, int maxX)
{
    const int N = weights.size();

    const int numInRange = maxX - minX + 1;

    auto parityAfterApplyingNet = [N, &weights, &bias](const int id)
    {
        int netResultParity = id % 2;
        for (int layer = 0; layer < N; layer++)
        {
            netResultParity = (weights[layer] * netResultParity + bias[layer]) % 2;
        }
        return netResultParity;
    };

    const int affectOnEven = parityAfterApplyingNet(0);
    const int affectOnOdd = parityAfterApplyingNet(1);

    int numEvenInRange = -1;
    if (numInRange % 2 == 0)
    {
        numEvenInRange = numInRange / 2;
    }
    else
    {
        if ((minX % 2) == 0)
            numEvenInRange = numInRange / 2 + 1;
        else
            numEvenInRange = numInRange / 2;
    }

    int numNonSpammers = 0;
    int numSpammers = 0;
    const auto numOddInRange = numInRange - numEvenInRange;

    if (affectOnEven % 2 == 0)
    {
        numNonSpammers += numEvenInRange;
    }
    else
    {
        numSpammers += numEvenInRange;
    }


    if (affectOnOdd % 2 == 0)
    {
        numNonSpammers += numOddInRange;
    }
    else
    {
        numSpammers += numOddInRange;
    }

    return {numNonSpammers, numSpammers};
}


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();
        const int minX = read<int>();
        const int maxX = read<int>();

        vector<int> weights(N);
        vector<int> bias(N);

        for (int i = 0; i < N; i++)
        {
            cin >> weights[i];
            cin >> bias[i];
        }


        const auto solution = findNumNonSpammersAndSpammers(weights, bias, minX, maxX);
        cout << solution.first << " " << solution.second << endl;
    }

    assert(cin);
}
