// Simon St James (ssjgz) - 2019-11-20
// 
// Solution to: https://www.codechef.com/problems/NNN
//
#include <iostream>
#include <vector>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

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
