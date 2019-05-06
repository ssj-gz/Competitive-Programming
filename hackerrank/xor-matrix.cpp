// Simon St James (ssjgz) 2019-05-06
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

vector<int64_t> calcRowOfMatrix(const vector<int64_t>& originalA, int n, uint64_t m)
{
    m--; // Already know the 0th row.
    vector<int64_t> currentRow(originalA);

    uint64_t powerOf2 = static_cast<uint64_t>(1) << static_cast<uint64_t>(63);
    while (powerOf2 != 0)
    {
        if (m >= powerOf2)
        {
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

    const auto solution = calcRowOfMatrix(a, n, m);
    for (int i = 0; i < n; i++)
    {
        cout << solution[i] << " ";
    }
    cout << endl;
}


