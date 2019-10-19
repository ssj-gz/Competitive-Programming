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

    const int affectOfNetOnEven = parityAfterApplyingNet(0);
    const int affectOfNetOnOdd = parityAfterApplyingNet(1);

    const int numEvenInRange = (numInRange / 2) + (((numInRange % 2) == 1 && (minX % 2 == 0)) ? 1 : 0); // Easy but boring to figure out :)
    const auto numOddInRange = numInRange - numEvenInRange;

    int numNonSpammers = 0;
    int numSpammers = 0;

    if (affectOfNetOnEven == 0)
    {
        numNonSpammers += numEvenInRange;
    }
    else
    {
        numSpammers += numEvenInRange;
    }


    if (affectOfNetOnOdd == 0)
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
    // Easy one - hopefully it's clear that for each layer, the parity of the result of applying 
    // the layer depends only on the parity of the input of the layer, since
    //
    //    (weight_i * layerInput + bias_i) % 2 = (weight_i * (layerInput % 2) + bias_i) % 2.
    //
    // Thus, by induction, the parity of the output of the whole network depends only on the parity of the 
    // input, so it makes sense to compute affectOfNetOnOdd and affectOfNetOnEven.
    //
    // We can then use these affectOfNetOnEven/Odd values, along with the number of odd and even ids in the 
    // range [minX, maxX], to find the number of ids in the range that give even parity (non-spammers) and 
    // odd parity (spammers) respectively.
    //
    // Hopefully the code is clear :)
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
