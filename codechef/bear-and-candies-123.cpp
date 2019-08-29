// Simon St James (ssjgz) - 2019-08-29
//
// Problem link: https://www.codechef.com/problems/CANDY123
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

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    
    const int T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int maxCandiesLimark = read<int>();
        const int maxCandiesBob = read<int>();

        int numCandiesToEat = 1;

        int numCandiesEatenByLimark = 0;
        int numCandiesEatenByBob = 0;

        bool bobIsWinner = true;
        while (true)
        {
            numCandiesEatenByLimark += numCandiesToEat;
            if (numCandiesEatenByLimark > maxCandiesLimark)
            {
                bobIsWinner = true;
                break;
            }

            numCandiesToEat++;

            numCandiesEatenByBob += numCandiesToEat;
            if (numCandiesEatenByBob > maxCandiesBob)
            {
                bobIsWinner = false;
                break;
            }

            numCandiesToEat++;
        }

        if (bobIsWinner)
            cout << "Bob";
        else
            cout << "Limak";
        cout << endl;


    }

    assert(cin);
}
