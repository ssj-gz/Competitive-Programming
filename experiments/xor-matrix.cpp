// Simon St James (ssjgz) 2019-05-06
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

vector<int64_t> solutionBruteForce(const vector<int64_t>& originalA, int n, uint64_t m)
{
    vector<int64_t> currentRow(originalA);
    uint64_t row = 1;
    while (row != m)
    {

        vector<int64_t> nextRow(n);
        for (int i = 0; i < n; i++)
        {
            nextRow[i] = currentRow[i] ^ currentRow[(i + 1) % n];
        }

        row++;
        currentRow = nextRow;
    }
    return currentRow;
}

vector<int64_t> solution(const vector<int64_t>& originalA, int n, uint64_t m)
{
    m--; // Already know the 0th row.
    vector<int64_t> currentRow(originalA);

    uint64_t powerOf2 = static_cast<uint64_t>(1) << static_cast<uint64_t>(63);
    while (powerOf2 != 0)
    {
        //cout << "powerOf2: " << powerOf2 << " m: " << m << endl;
        if (m >= powerOf2)
        {
            //cout << "power of 2 is greater >= m" << std::endl;
            vector<int64_t> currentPlusPowerOf2thRow(n);
            for (int i = 0; i < n; i++)
            {
                currentPlusPowerOf2thRow[i] ^= currentRow[i];
                currentPlusPowerOf2thRow[i] ^= currentRow[(i + powerOf2) % n];
            }
            currentRow = currentPlusPowerOf2thRow;


            m -= powerOf2;
        }
        powerOf2 >>= 1;
    }
    return currentRow;
}

int main(int argc, char* argv[])
{
    int n;
    cin >> n;
    uint64_t m;
    cin >> m;

    vector<int64_t> a(n);
    for (int i = 0; i < n ; i++)
    {
        cin >> a[i];
    }

#ifdef BRUTE_FORCE
    const auto bruteForceSolution = solutionBruteForce(a, n, m);
    cout << "bruteForceSolution: " << endl; 
    for (int i = 0; i < n; i++)
    {
        cout << bruteForceSolution[i] << " ";
    }
    cout << endl;
#endif

    const auto optimisedSolution = solution(a, n, m);
#ifdef BRUTE_FORCE
    cout << "optimisedSolution: " << endl; 
#endif
    for (int i = 0; i < n; i++)
    {
        cout << optimisedSolution[i] << " ";
    }
    cout << endl;
    assert(bruteForceSolution == optimisedSolution);
}


