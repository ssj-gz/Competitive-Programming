// Simon St James (ssjgz) - 2019-04-07
#include <iostream>
#include <vector>
#include <limits>

#include <cassert>

using namespace std;

struct Blah
{
    enum Action { SubtractOne, SetEqualToMaxProdand };

    Action action = SubtractOne;

    int prodand = -1;
    int minMovesToZero = -1;
};

int findMinDownToZero(int N, vector<Blah>& minDownToZeroLookup, const vector<vector<int>>& factorsLookup)
{
    if (minDownToZeroLookup[N].minMovesToZero != -1)
    {
        return minDownToZeroLookup[N].minMovesToZero;
    }

    // The "subtract 1 from N" case.
    int minDownToZero = 1 + findMinDownToZero(N - 1, minDownToZeroLookup, factorsLookup);
    Blah action;
    // The "reduce to largest of prodands in product equalling N" case.
    for (const auto factor : factorsLookup[N])
    {
        assert(N / factor == max(N / factor, factor));
        const int result = 1 + findMinDownToZero(N / factor, minDownToZeroLookup, factorsLookup);
        if (result < minDownToZero)
        {
            action.action = Blah::SetEqualToMaxProdand;
            action.prodand = N / factor;
            minDownToZero = result;
        }
    }

    action.minMovesToZero = minDownToZero;

    minDownToZeroLookup[N] = action;

    return minDownToZero;
}

int main()
{
    // I was a bit over-confident with this one XD
    // It looks like a simple Dynamic Programming problem, but you need to be wary:
    // the recursion step in findMinDownToZero includes the case where we
    // subtract 1 from N and recurse, which for large N is prone to causing a stack
    // overflow (and is very slow besides).  
    // Instead, pre-compute all minDownToZeroLookup from the bottom-up
    // which eliminates this possibility and makes things much more efficient.
    //
    // Not much else besides that - hopefully the comments explain the rest!
    int Q;
    cin >> Q;

    const int maxN = 1'000'000;
    vector<Blah> minDownToZeroLookup(maxN + 1);
    minDownToZeroLookup[0].minMovesToZero = 0;

    // Compute list of factors for all N =  1 ... maxN using Sieve of 
    // Erastophenes.
    // We only care about the factors <= sqrt(N), as the
    // others are just "mirrors" of these (i.e. if F is a factor of
    // N and F > sqrt(N), then N / F is a factor of N and is <= sqrt(N)).
    vector<vector<int>> factorsLookup(maxN);
    for (int64_t factor = 2; factor <= maxN; factor++)
    {
        for (int64_t multiple = factor; multiple <= maxN; multiple += factor)
        {
            if (factor * factor <= multiple)
            {
                factorsLookup[multiple].push_back(factor);
            }
        }
    }

    // Precompute minDownToZeroLookup from bottom-up.
    for (int i = 1; i <= maxN; i++)
    {
        findMinDownToZero(i, minDownToZeroLookup, factorsLookup);
    }

    for (int q = 0; q < Q; q++)
    {
        int N;
        cin >> N;
        assert(cin);

        assert(N <= maxN);

        cout << minDownToZeroLookup[N].minMovesToZero << endl;

        int movesMade = 0;
        while (true)
        {
            cout << "N is now: " << N << " after making " << movesMade << " moves" << endl;
            if (N == 0)
            {
                cout << "Done!" << endl;
                break;
            }
            const auto thing = minDownToZeroLookup[N];
            if (thing.action == Blah::SubtractOne)
            {
                cout << " subtract one" << endl;
                N--;
            }
            else
            {
                cout << " N = " << thing.prodand << " x " << N / thing.prodand << " - set N to max of these (" << thing.prodand << ")" << endl;
                N = thing.prodand;
            }
            movesMade++;

        }
    }
}
