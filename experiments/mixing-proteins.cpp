// Simon St James (ssjgz) - 2019-05-07
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <cassert>

#include <sys/time.h>

using namespace std;

string calcRowOfMatrix(const string& originalProteinString, int n, uint64_t m)
{
    auto transform = [](char a, char b)
    {
        static const char transformTable[][4] =
        {
            {'A', 'B', 'C', 'D' },
            {'B', 'A', 'D', 'C' },
            {'C', 'D', 'A', 'B' },
            {'D', 'C', 'B', 'A' }
        };
        return transformTable[a - 'A'][b - 'A'];

    };

    string currentRow(originalProteinString);

    uint64_t powerOf2 = static_cast<uint64_t>(1) << static_cast<uint64_t>(63);
    while (powerOf2 != 0)
    {
        if (m >= powerOf2)
        {
            // Set the row to be all the identity element : we'll update it by 
            // transforming each element.
            string currentPlusPowerOf2thRow(n, 'A');

            for (int i = 0; i < n; i++)
            {
                currentPlusPowerOf2thRow[i] = transform(currentPlusPowerOf2thRow[i],  currentRow[i]);
                currentPlusPowerOf2thRow[i] = transform(currentPlusPowerOf2thRow[i], currentRow[(i + powerOf2) % n]);
            }
            currentRow = currentPlusPowerOf2thRow;

            m -= powerOf2;
        }
        powerOf2 >>= 1;
    }
    return currentRow;
}


string solveBruteForce(const string& originalProteinString, int N, int K)
{
    auto proteinString = originalProteinString;
    for (int second = 0; second < K; second++)
    {
        cout << proteinString << endl;
        string nextProteinString(N, '\0');
        auto transform = [](char a, char b)
        {
            static const char transformTable[][4] =
            {
                {'A', 'B', 'C', 'D' },
                {'B', 'A', 'D', 'C' },
                {'C', 'D', 'A', 'B' },
                {'D', 'C', 'B', 'A' }
            };
            return transformTable[a - 'A'][b - 'A'];

        };
        for (int i = 0; i < N; i++)
        {
            nextProteinString[i] = transform(proteinString[i], proteinString[(i + 1) % N]);
        }
        proteinString = nextProteinString;
    }
    return proteinString;
}

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int N = rand() % 100 + 1;
        const int K = rand() % 1000 + 1;
        cout << N << " " << K << endl;
        for (int i = 0; i < N; i++)
        {
            cout << static_cast<char>('A' + rand() % 4);
        }
        cout << endl;
        return 0;

    }
    int N, K;
    cin >> N >> K;

    string proteinString;
    cin >> proteinString;

#ifdef BRUTE_FORCE
    const auto bruteForceSolution = solveBruteForce(proteinString, N, K);
    cout << "bruteForceSolution: " << bruteForceSolution << endl;
#endif
    const auto optimisedSolution = calcRowOfMatrix(proteinString, N, K);
#ifdef BRUTE_FORCE
    cout << "optimisedSolution: " << optimisedSolution << endl;
    assert(bruteForceSolution == optimisedSolution);
#else
    cout << optimisedSolution << endl;
#endif

}
