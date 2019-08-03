// Simon St James (ssjgz) - 2019-08-03
#include <iostream>
#include <vector>
#include <algorithm>

#include <cassert>

#include <sys/time.h>

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

bool solveBruteForce(int64_t N, int64_t K)
{
    vector<int64_t> numInBoxEmployee1(K);
    vector<int64_t> numInBoxEmployee2(K);

    int64_t numApplesRemaining = N;

    while (numApplesRemaining > 0)
    {
        // Employee1
        for (int i = 0; i < K; i++)
        {
            numInBoxEmployee1[i]++;
        }
        // Employee2
        {
            const int64_t minInBox = *std::min_element(numInBoxEmployee2.begin(), numInBoxEmployee2.end());
            vector<int64_t> indicesWithMinInBox;
            for (int64_t i = 0; i < K; i++)
            {
                if (numInBoxEmployee2[i] == minInBox)
                {
                    indicesWithMinInBox.push_back(i);
                }
            }
            const int64_t randIndexWithMinInBox = indicesWithMinInBox[rand() % indicesWithMinInBox.size()];
            numInBoxEmployee2[randIndexWithMinInBox] += K;
        }

        numApplesRemaining -= K;
    }

    cout << "N: " << N << " K: " << K << endl;
    cout << "numInBoxEmployee1: " << endl;
    for (const auto x : numInBoxEmployee1)
    {
        cout << x << " ";
    }
    cout << endl << "numInBoxEmployee2: " << endl;
    for (const auto x : numInBoxEmployee2)
    {
        cout << x << " ";
    }
    bool boxesEqual = true;
    for (int64_t i = 0; i < K; i++)
    {
        if (numInBoxEmployee1[i] != numInBoxEmployee2[i])
        {
            cout << " found difference at index " << i << " - numInBoxEmployee1: " << numInBoxEmployee1[i] << " numInBoxEmployee2: " << numInBoxEmployee2[i] << endl;
            boxesEqual = false;
        }
    }

    return boxesEqual;
}

int main(int argc, char* argv[])
{
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        // TODO - generate randomised test.
        return 0;
    }

    const int T = read<int>();
    for (int t = 0; t < T; t++)
    {
        const int64_t N = read<int64_t>();
        const int64_t K = read<int64_t>();

        const auto solutionBruteForce = solveBruteForce(N, K);
        cout << "solutionBruteForce: " << (solutionBruteForce ? "NO" : "YES") << endl;
    }
}

