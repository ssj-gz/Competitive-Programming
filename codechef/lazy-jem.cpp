// Simon St James (ssjgz) - 2019-10-29
// 
// Solution to: https://www.codechef.com/problems/TALAZY
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

int64_t findTimeTaken(int64_t numProblemsToSolve, int64_t breakLength, int64_t timeToSolveOneProblem)
{
    int64_t timeTaken = 0;

    while (true)
    {
        const auto numToSolveBeforeBreak = ((numProblemsToSolve % 2) == 0) ? numProblemsToSolve / 2 : numProblemsToSolve / 2 + 1;
        timeTaken += numToSolveBeforeBreak * timeToSolveOneProblem;

        numProblemsToSolve -= numToSolveBeforeBreak;

        if (numProblemsToSolve == 0)
            break;

        timeTaken += breakLength;

        timeToSolveOneProblem *= 2;
    }
     
    return timeTaken;
}

int main(int argc, char* argv[])
{
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const auto N = read<int64_t>();
        const auto B = read<int64_t>();
        const auto M = read<int64_t>();

        cout << findTimeTaken(N, B, M) << endl;
    }

    assert(cin);
}
