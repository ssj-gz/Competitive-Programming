// Simon St James (ssjgz) - 2019-06-15
#include <iostream>
#include <vector>
#include <map>
#include <cassert>

using namespace std;

int64_t solveOptimised(const vector<int64_t>& a, int64_t r)
{
    // Compute (and back up) numRemainingOccurrencesOf.
    map<int64_t, int64_t> numRemainingOccurrencesOf;
    for (const auto x : a)
    {
        numRemainingOccurrencesOf[x]++;
    }
    const auto numRemainingOccurrencesOfOriginal = numRemainingOccurrencesOf;

    // Compute numRemainingPairsBeginningWith.
    map<int64_t, int64_t> numRemainingPairsBeginningWith;
    for (const auto x : a)
    {
        // The order of updates is important, here: x * r == x if r == 1.
        numRemainingOccurrencesOf[x]--;
        numRemainingPairsBeginningWith[x] += numRemainingOccurrencesOf[x * r];

    }

    // Restore numRemainingOccurrencesOf.
    numRemainingOccurrencesOf = numRemainingOccurrencesOfOriginal;
    int64_t numTriplets = 0;
    // Now that we have numRemainingPairsBeginningWith and numRemainingOccurrencesOf up-to-date,
    // sweep through the array and do the counting of triplets, and maintain both of these maps.
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
    // Fundamentally easy: sweep through the array from left to right.
    // At index i, we need to find the number of *pairs* in a geometric
    // progression after index i + 1 where the first element in the pair
    // is a[i] * r (call this numRemainingPairsBeginningWith) i.e. the number of pairs
    // (a[i] * r, a[i] * r * r).  Each such pair, when a[i] is prepended to the front,
    // gives rise to one triplet.
    //
    // We can compute numRemainingPairsBeginningWith for the initial array
    // quite easily: hopefully the code is self-explanatory :)
    //
    // We can then keep it up to date as we sweep through the array quite easily - again,
    // hopefully the code is self-explanatory :)
    //
    // There is some trickiness for the r == 1 case as x, x * r, and x * r * r
    // are all equal to x and so we need to update numRemainingOccurrencesOf, numRemainingPairsBeginningWith,
    // and numTriplets in the correct order, but this is not too difficult.
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

