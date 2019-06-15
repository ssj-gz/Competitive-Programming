// Simon St James (ssjgz) - 2019-06-15
#include <iostream>
#include <vector>
#include <map>
#include <cassert>


using namespace std;

int64_t solveOptimised(const vector<int64_t>& a, int64_t r)
{
    int64_t numTriplets = 0;
    map<int64_t, int64_t> numRemainingOccurrencesOf;
    for (const auto x : a)
    {
        numRemainingOccurrencesOf[x]++;
    }
    const auto numRemainingOccurrencesOfOriginal = numRemainingOccurrencesOf;

    map<int64_t, int64_t> numRemainingPairsBeginningWith;
    for (const auto x : a)
    {
        // The order of updates is important, here: x * r == x if r == 1.
        numRemainingOccurrencesOf[x]--;
        numRemainingPairsBeginningWith[x] += numRemainingOccurrencesOf[x * r];

    }

    numRemainingOccurrencesOf = numRemainingOccurrencesOfOriginal;
    for (const auto x : a)
    {
        // The order of updates is important, here: x * r == x if r == 1.
        numRemainingOccurrencesOf[x]--;
        numRemainingPairsBeginningWith[x] -= numRemainingOccurrencesOf[x * r];

        numTriplets += numRemainingPairsBeginningWith[x * r];


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

    cout << solveOptimised(a, r) << endl;
}

