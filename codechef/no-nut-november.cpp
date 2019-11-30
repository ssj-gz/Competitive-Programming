// Simon St James (ssjgz) - 2019-11-20
// 
// Solution to: https://www.codechef.com/problems/NNN
//
#include <iostream>
#include <vector>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!
#include <algorithm> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

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
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        // TODO - generate randomised test.
        //const int T = rand() % 100 + 1;
        const int T = 1;
        cout << T << endl;

        for (int t = 0; t < T; t++)
        {
            const int numSquirrels = rand() % 20 + 1;
            const int numNuts = rand() % 20 + 1;
            const int N = numNuts + numSquirrels;

            string nutsAndSquirrels;
            for (int i = 0; i < numNuts; i++)
            {
                nutsAndSquirrels.push_back('N');
            }
            for (int i = 0; i < numSquirrels; i++)
            {
                nutsAndSquirrels.push_back('S');
            }
            random_shuffle(nutsAndSquirrels.begin(), nutsAndSquirrels.end());
            const int K = 1 + rand() % (N - 1);
            assert(1 <= K && K < N);

            cout << K << " " << N << endl;
            for (int i = 0; i < N; i++)
            {
                cout << nutsAndSquirrels[i];
                if (i != N - 1)
                    cout << " ";
            }
            cout << endl;
        }

        return 0;
    }
    
    // TODO - read in testcase.
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
                        nutsAndSquirrels[j] = ' ';
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
