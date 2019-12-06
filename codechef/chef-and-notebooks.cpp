// Simon St James (ssjgz) - 2019-12-06
// 
// Solution to: https://www.codechef.com/problems/CNOTE
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
        const int numPagesToWrite = read<int>();
        const int numPagesInNoteBook = read<int>();

        const int moneyLeft = read<int>();
        const int numNoteBooks = read<int>();

        const int numNewPagesRequired = numPagesToWrite - numPagesInNoteBook;
        assert(numPagesInNoteBook > 0);

        bool foundNoteBook = false;
        for (int i = 0;  i < numNoteBooks; i++)
        {
            const int numNewPages = read<int>();
            const int cost = read<int>();

            if (numNewPages >= numNewPagesRequired && cost <= moneyLeft)
                foundNoteBook = true;
        }

        cout << (foundNoteBook ? "LuckyChef" : "UnluckyChef") << endl;
            
    }

    assert(cin);
}
