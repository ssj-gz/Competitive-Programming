// Simon St James (ssjgz) - 2019-12-19
// 
// Solution to: https://www.codechef.com/problems/TRICOIN
//
#include <iostream>

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

int findMaxTriangleHeight(const int64_t numCoins)
{
    int maxHeight = 0;
    int64_t numCoinsUsed = 0;
    int64_t numCoinsInLastRow = 0;
    while (numCoinsUsed < numCoins)
    {
        const int numCoinsThisRow = numCoinsInLastRow + 1;
        if (numCoinsUsed + numCoinsThisRow > numCoins)
        {
            break;
        }

        maxHeight++;
        numCoinsUsed += numCoinsThisRow;
        numCoinsInLastRow = numCoinsThisRow;
    }
    
    return maxHeight;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int64_t N = read<int64_t>();

        cout << findMaxTriangleHeight(N) << endl;
    }

    assert(cin);
}
