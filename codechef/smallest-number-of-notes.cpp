// Simon St James (ssjgz) - 2019-12-21
// 
// Solution to: https://www.codechef.com/problems/FLOW005
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

int findMinCoinsWithValue(const vector<int>& denomimationsDecreasing, int targetValue)
{
    int minNumCoins = 0;

    for (const auto& denomimation : denomimationsDecreasing)
    {
        minNumCoins += targetValue / denomimation;
        targetValue = targetValue % denomimation;
    }
    
    return minNumCoins;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    const vector<int> denomimationsDecreasing = { 100, 50, 10, 5, 2, 1};

    for (int t = 0; t < T; t++)
    {
        const int targetValue = read<int>();

        cout << findMinCoinsWithValue(denomimationsDecreasing, targetValue) << endl;
    }

    assert(cin);
}
