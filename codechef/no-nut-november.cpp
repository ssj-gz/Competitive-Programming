// Simon St James (ssjgz) - 2019-11-30
// 
// Solution to: https://www.codechef.com/problems/NNN
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

int main(int argc, char* argv[])
{
    // Easy one, though with an abysmal Problem Statement: rephrased,
    //   i) Each squirrel can carry at most one nut;
    //  ii) No squirrel can carry a nut that is more than K units away.
    //
    // The strategy is to sweep from left to right and have each squirrel
    // carry the leftmost remaining nut that is <= K units away from it.
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int K = read<int>();
        const int N = read<int>();

        string nutsAndSquirrels;
        for (int i = 0; i < N; i++)
        {
            nutsAndSquirrels.push_back(read<char>());
        }

        int numNutsFound = 0;

        for (int i = 0; i < N; i++)
        {
            if (nutsAndSquirrels[i] == 'S')
            {
                for (int j = max(0, i - K); j <= min(i + K, N); j++)
                {
                    if (nutsAndSquirrels[j] == 'N')
                    {
                        nutsAndSquirrels[j] = ' '; // Remove this nut.
                        numNutsFound++;
                        break;
                    }
                }
            }
        }
        cout << numNutsFound << endl;
    }

    assert(cin);
}
