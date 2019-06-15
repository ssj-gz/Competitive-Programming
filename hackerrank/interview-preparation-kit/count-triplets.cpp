#include <iostream>
#include <vector>
#include <map>
#include <cassert>

using namespace std;

int64_t solveBruteForce(const vector<int64_t>& a, int64_t r)
{
    const int n = a.size();
    int64_t numTriplets = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            for (int k = j + 1; k < n; k++)
            {
                if (((a[k] % a[j]) == 0) &&
                    (a[k] / a[j] == r) &&
                   (((a[j] % a[i]) == 0) &&
                    (a[j] / a[i] == r)))
                {
                    cout << "Triple i j k: " << i << ", " << j << ", " << k << " (" << a[i] << ", " << a[j] << ", " << a[k] << ")" << endl;
                    numTriplets++;
                }
            }
        }
    }
    return numTriplets;
}

int main(int argc, char* argv[])
{
    int n;
    cin >> n;
    int64_t r;
    cin >> r;

    vector<int64_t> a(n);
    for (int i = 0; i < n; i++)
    {
        cin >> a[i];
    }
    assert(cin);

    const auto solutionBruteForce = solveBruteForce(a, r);
    cout << "solutionBruteForce: " << solutionBruteForce << endl;
}

