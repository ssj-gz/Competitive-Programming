// Simon St James (ssjgz) - 2022-03-31
// 
// Solution to: https://www.codechef.com/MARCH221D/problems/BATH
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

int main()
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        auto geyserLitresRemaining = read<int>();
        const auto bucketLitres = read<int>();

        int numBathsFilled = 0;
        while (geyserLitresRemaining >= 2 * bucketLitres)
        {
            numBathsFilled++;
            geyserLitresRemaining -= 2 * bucketLitres;
        }
        cout << numBathsFilled << endl;
    }

    assert(cin);
}
