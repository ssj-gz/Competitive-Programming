#include <iostream>
#include <vector>
#include <limits>

using namespace std;

void solve(int indexToChange, vector<int>& permutationSoFar, int& largestXorSum)
{
    if (indexToChange == permutationSoFar.size())
    {
        cout << "Got permutation: " << endl;
        for (const auto x : permutationSoFar)
        {
            cout << " " << x;
        }
        cout << endl;
        int xorSum = std::numeric_limits<int>::max();
        for (int i = 0; i < permutationSoFar.size() - 1; i++)
        {
            xorSum = min(xorSum, permutationSoFar[i] ^ permutationSoFar[i + 1]);
        }
        xorSum = min(xorSum, permutationSoFar.front() ^ permutationSoFar.back());
        cout << " xorSum: " << xorSum << endl;
        if (xorSum > largestXorSum)
        {
            largestXorSum = xorSum;
        }
        return;
    }

    for (int i = indexToChange; i < permutationSoFar.size(); i++)
    {
        std::swap(permutationSoFar[indexToChange], permutationSoFar[i]);
        solve(indexToChange + 1, permutationSoFar, largestXorSum);
        std::swap(permutationSoFar[indexToChange], permutationSoFar[i]);
    }
}

int solutionBruteForce(const vector<int>& a)
{
    int largestXorSum = -1;
    vector<int> permutationSoFar(a);
    solve(0, permutationSoFar, largestXorSum);
    cout << "largestXorSum: " << largestXorSum << endl;
    int solution = -1;
    for (int k = 0; largestXorSum >= (1 << k); k++)
    {
        solution = k;
    }
    return solution;
}

int main(int argc, char* argv[])
{
    int N;
    cin >> N;

    vector<int> a(N);
    for (int i = 0; i < N; i++)
    {
        cin >> a[i];
    }

    const auto bruteForceSolution = solutionBruteForce(a); 
    cout << "bruteForceSolution: " << bruteForceSolution << endl;
}

