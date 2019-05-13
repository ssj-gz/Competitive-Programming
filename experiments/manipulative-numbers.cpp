#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>

#include <sys/time.h>

using namespace std;

string asBinary(int n)
{
    string binaryString;
    while (n > 0)
    {
        const char digit = '0' + (n & 1);
        binaryString.push_back(digit);
        n >>= 1;
    }
    return binaryString;
}

void solve(int indexToChange, vector<uint64_t>& permutationSoFar, int& solution)
{
    if (indexToChange == permutationSoFar.size())
    {
        cout << "Got permutation: " << endl;
        for (const auto x : permutationSoFar)
        {
            cout << " " << x;
        }
        cout << endl;
        for (const auto x : permutationSoFar)
        {
            cout << asBinary(x) << endl;
        }
        uint64_t xorSum = std::numeric_limits<uint64_t>::max();
        for (int i = 0; i < permutationSoFar.size() - 1; i++)
        {
            xorSum = min(xorSum, permutationSoFar[i] ^ permutationSoFar[i + 1]);
        }
        xorSum = min(xorSum, permutationSoFar.front() ^ permutationSoFar.back());
        cout << " xorSum: " << xorSum << endl;
        //if (xorSum > largestXorSum)
        //{
            //cout << " new largestXorSum!" << endl;
            //largestXorSum = xorSum;
        //}
        int highestK = -1;
        for (int k = 0; xorSum >= (static_cast<uint64_t>(1) << k); k++)
        {
            highestK = k;
        }
        if (highestK > solution)
        {
            cout << " new best solution: " << highestK << endl;
            solution = highestK;
        }
        return;
    }

    for (int i = indexToChange; i < permutationSoFar.size(); i++)
    {
        std::swap(permutationSoFar[indexToChange], permutationSoFar[i]);
        solve(indexToChange + 1, permutationSoFar, solution);
        std::swap(permutationSoFar[indexToChange], permutationSoFar[i]);
    }
}

int solutionBruteForce(const vector<uint64_t>& a)
{
    int solution = -1;
    vector<uint64_t> permutationSoFar(a);
    solve(0, permutationSoFar, solution);
    return solution;
}

int solutionOptimised(const vector<uint64_t>& a)
{
    int k = 0;
    int solution = -1;
    while (true)
    {
        const auto powerOf2 = (static_cast<uint64_t>(1) << k);
        cout << "k: " << k << " powerOf2: " << powerOf2 << endl;
        if (*std::max_element(a.begin(), a.end()) < powerOf2)
            break;

        int numElementsWithPowerOf2 = 0;
        for (const auto x : a)
        {
            if ((x & powerOf2) != 0)
                numElementsWithPowerOf2++;
        }
        cout << " numElementsWithPowerOf2: " << numElementsWithPowerOf2 << endl;
        //if (a.size() % 2 == 0)
        {
            if (numElementsWithPowerOf2 == a.size() / 2)
            {
                solution = k;
                cout << " solution updated to " << k << endl;
            }
        }
        //else
        {
#if 0
            if (numElementsWithPowerOf2 == a.size() / 2)
            {
                solution = k;
            }
#endif
        }
        k++;
    }
    return solution;
}
int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int n = rand() % 10 + 2;
        //const int maxA = rand() % 10'000'000'000 + 1;
        const int maxA = rand() % 256 + 1;

        cout << n << endl;
        for (int i = 0; i < n; i++)
        {
            cout << (rand() % maxA) << endl;
        }
        return 0;
    }
    int N;
    cin >> N;

    vector<uint64_t> a(N);
    for (int i = 0; i < N; i++)
    {
        cin >> a[i];
    }

    const auto bruteForceSolution = solutionBruteForce(a); 
    cout << "bruteForceSolution: " << bruteForceSolution << endl;
    const auto optimisedSolution = solutionOptimised(a); 
    cout << "optimisedSolution: " << optimisedSolution << endl;
}

