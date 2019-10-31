// Simon St James (ssjgz) 2017-7-15 9:50
#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

int64_t numWaysOfFormingAmountWithCoinTypes(int targetAmount, int numCoinTypes, vector<vector<int64_t>>& lookup, const vector<int>& coinValues)
{
    if (targetAmount == 0)
        return 1; // Use no coins.
    if (numCoinTypes == 0)
        return 0;
    const int64_t lookedUpValue = lookup[targetAmount][numCoinTypes];
    if (lookedUpValue != -1)
        return lookedUpValue;

    int64_t numWays = 0;
    int numOfLastCoin = 0;
    while (numOfLastCoin * coinValues[numCoinTypes - 1] <= targetAmount)
    {
        numWays += numWaysOfFormingAmountWithCoinTypes(targetAmount - numOfLastCoin * coinValues[numCoinTypes - 1], numCoinTypes - 1, lookup, coinValues);
        numOfLastCoin++;
    }

    lookup[targetAmount][numCoinTypes] = numWays;

    return numWays;
}


int main() {
    // Fairly well known algorithm; not going to bother with an explanation :)
    int targetAmount, numCoinTypes;
    cin >> targetAmount >> numCoinTypes;
    vector<int> coinValues(numCoinTypes);
    for (int i = 0; i < numCoinTypes; i++)
        cin >> coinValues[i];

    vector<vector<int64_t>> lookup(vector<vector<int64_t>>(targetAmount + 1, vector<int64_t>(numCoinTypes + 1, -1)) );

    cout << numWaysOfFormingAmountWithCoinTypes(targetAmount, numCoinTypes, lookup, coinValues);

    return 0;
}

