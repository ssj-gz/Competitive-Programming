// Simon St James (ssjgz) - 2019-08-03
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
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
    cout << "N: " << N << " K: " << K << endl;
    vector<int64_t> numInBoxCandidate1(K);
    vector<int64_t> numInBoxCandidate2(K);

    int64_t numApplesRemaining = N;

    int minutesElapsed = 0;
    while (numApplesRemaining > 0)
    {
        // Candidate1
        for (int i = 0; i < K; i++)
        {
            numInBoxCandidate1[i]++;
        }
        // Candidate2
        {
            const int64_t minInBox = *std::min_element(numInBoxCandidate2.begin(), numInBoxCandidate2.end());
            vector<int64_t> indicesWithMinInBox;
            for (int64_t i = 0; i < K; i++)
            {
                if (numInBoxCandidate2[i] == minInBox)
                {
                    indicesWithMinInBox.push_back(i);
                }
            }
            const int64_t randIndexWithMinInBox = indicesWithMinInBox[rand() % indicesWithMinInBox.size()];
            numInBoxCandidate2[randIndexWithMinInBox] += K;
        }

        minutesElapsed++;
        numApplesRemaining -= K;
        cout << "minutesElapsed: " << minutesElapsed << " numApplesRemaining: " << numApplesRemaining << endl;
        cout << " numInBoxCandidate1: " << endl;
        for (const auto x : numInBoxCandidate1)
        {
            cout << x << " ";
        }
        cout << endl << " numInBoxCandidate2: " << endl;
        for (const auto x : numInBoxCandidate2)
        {
            cout << x << " ";
        }
        cout << endl;
    }

    bool boxesEqual = true;
    for (int64_t i = 0; i < K; i++)
    {
        if (numInBoxCandidate1[i] != numInBoxCandidate2[i])
        {
            //cout << " found difference at index " << i << " - numInBoxCandidate1: " << numInBoxCandidate1[i] << " numInBoxCandidate2: " << numInBoxCandidate2[i] << endl;
            boxesEqual = false;
        }
    }

    return boxesEqual;
}

bool solveOptimised(int64_t numApples, int64_t numBoxes)
{
    assert((numApples % numBoxes) == 0);
    assert(numApples >= numBoxes);
    // After all apples are packed by Candidate1, each box
    // will have the same number (numApples / numBoxes) of
    // apples in it.
    //
    // For candidate 2, things are a little trickier.
    //
    if ((numApples / numBoxes) % numBoxes == 0)
    {
        // Have an equal number of apples in each box - in fact,
        // numApples / numBoxes of them, just like Candidate1!
        return true;
    }
    else
    {
        // Have an unequal number of apples in each box - must
        // differ from Candidate1, all of whose boxes contain
        // the same number of apples.
        return false;
    }
}

int main(int argc, char* argv[])
{
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        const int64_t K = rand() % 1000 + 1;
        const int64_t N = ((rand() % 1000) + 1) * K;
        cout << 1 << endl;
        cout << N << " " << K << endl;
        return 0;
    }

    const int T = read<int>();
    for (int t = 0; t < T; t++)
    {
        const int64_t N = read<int64_t>();
        const int64_t K = read<int64_t>();

#ifdef BRUTE_FORCE
        const auto solutionBruteForce = solveBruteForce(N, K);
        cout << "Can box contents differ? " << (solutionBruteForce ? "NO" : "YES") << endl;
        const auto solutionOptimised = solveOptimised(N, K);
        //cout << "solutionOptimised: " << (solutionOptimised ? "NO" : "YES") << endl;

        assert(solutionOptimised == solutionBruteForce);
#else
        const auto solutionOptimised = solveOptimised(N, K);
        cout << (solutionOptimised ? "NO" : "YES") << endl;
#endif
    }
}

