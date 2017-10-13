// Simon St James (ssjgz) - 2017-10-11 15:10
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <cassert>

using namespace std;

const int64_t modulus = 1'000'000'007ULL;

namespace 
{
    vector<int64_t> factorialLookup;
    vector<int64_t> factorialInverseLookup;

    int64_t factorial(int64_t n)
    {
        assert(n >= 0 && n < factorialLookup.size());
        return factorialLookup[n];
    }

    int64_t nCr(int64_t n, int64_t r, int64_t modulus)
    {
        assert(n >= 0 && r >= 0);
        assert(n >= r);
        int64_t result = factorial(n);
        assert(r < factorialInverseLookup.size());
        assert(n - r < factorialInverseLookup.size());
        result = (result * factorialInverseLookup[n - r]) % modulus;
        result = (result * factorialInverseLookup[r] ) % modulus;
        return result;
    }

    int64_t quickPower(int64_t base, int64_t exponent, int64_t modulus)
    {
        // Raise base to the exponent mod modulus using as few multiplications as 
        // we can e.g. base ^ 8 ==  (((base^2)^2)^2).
        int64_t result = 1;
        int64_t power = 0;
        while (exponent > 0)
        {
            if (exponent & 1)
            {
                int64_t subResult = base;
                for (auto i = 0; i < power; i++)
                {
                    subResult = (subResult * subResult) % modulus;
                }
                result = (result * subResult) % modulus;
            }
            exponent >>= 1;
            power++;
        }
        return result;
    }

    void buildFactorialLookups(int maxN)
    {
        factorialLookup.resize(maxN + 1);
        factorialLookup[0] = 1;
        factorialInverseLookup.resize(maxN + 1);
        factorialInverseLookup[0] = 1;
        int64_t factorial = 1;
        for (auto i = 1; i <= maxN; i++)
        {
            factorial = (factorial * i) % ::modulus;
            factorialLookup[i] = factorial;
            const auto factorialInverse = quickPower(factorial, ::modulus - 2, ::modulus);
            assert((factorial * factorialInverse) % ::modulus == 1);
            factorialInverseLookup[i] = factorialInverse;
        }

    }

    vector<vector<int64_t>> lookup;
    vector<int> a;

}

int64_t findNumWaysOfFillingRemainingStartingWithLayerSize(int numRemaining, int layerSize)
{
    assert(numRemaining >= 0 && numRemaining < lookup.size());
    assert(layerSize >= 1 && layerSize < lookup[0].size());
    if (numRemaining == 0)
        return 1;
    if (numRemaining < layerSize)
        return 0; 

    auto& memoiseEntryRef = lookup[numRemaining][layerSize];
    if (memoiseEntryRef != -1)
        return memoiseEntryRef;

    int64_t result = 0;
    auto layerIsInOrder = true;
    const auto posInArray = a.size() - numRemaining;
    for (auto i = posInArray + 1; i < posInArray + layerSize; i++)
    {
        assert(i < a.size());
        assert(i - 1 >= 0);
        if (a[i] < a[i - 1])
        {
            layerIsInOrder = false;
            break;
        }
    }
    if (layerIsInOrder)
    {
        const auto isBottomLayer = (numRemaining == a.size());
        const auto numPermutationsForThisLayer = isBottomLayer ? 1 : factorial(layerSize);
        if (numRemaining != layerSize)
        {
            for (int nextLayerSize = layerSize; nextLayerSize >= 1; nextLayerSize--)
            {
                const auto contributionFromBottomLayer = nCr(layerSize, layerSize - nextLayerSize, ::modulus);
                const auto permutationFactor = (numPermutationsForThisLayer * contributionFromBottomLayer) % ::modulus;
                const auto nextLayerResult = findNumWaysOfFillingRemainingStartingWithLayerSize(numRemaining - layerSize, nextLayerSize);
                const auto adjustedNextLayerResult = (permutationFactor * nextLayerResult) % ::modulus;
                result = (result + adjustedNextLayerResult) % ::modulus;
            }
        }
        else
        {
            // Not enough elements left for further layers!
            result = numPermutationsForThisLayer;
        }
    }

    assert(result >= 0 && result < ::modulus);
    assert(memoiseEntryRef == -1);
    memoiseEntryRef = result;
    return result;

}

int main()
{
    // *Ouch* - severely misjudged this one - I somehow came away with the idea that the contents
    // of the array were irrelevant and a red-herring (yes, really!) despite one of the sample
    // testcases pretty much dispelling this idea right off the bat XD
    // 
    // Then I attempted a version which mixed iteration and recursion in just the right way
    // to ensure that (subsets of) the iteration would be performed many times in such a way
    // that memo-isation of the recursion results wouldn't work!
    //
    // *Then* I spent ages trying to figure out why I wasn't getting the right answer, when it
    // turned out that I simply wasn't taking the final result mod modulus XD
    //
    // Overall, a bit of a disaster - fun problem, though :)
    //
    // So: let V = [ v1, v2, .... vk ] for some k be a vector vectors, and let SM(V) be the array
    // created by applying Sherlock's merging algorithm to V.  We need to find the number of 
    // *distinct* V such that SM(V) = a.  We're are given the rules under which V1 and V2 are 
    // equivalent and, as is often the case with equivalence relations, it's handy to pick a normalisation 
    // procedure such that the normalisation
    // of two *equivalent* solutions A and B are *equal*.  In this case, it's easy to see what procedure
    // to use; take a solution; sort its vectors by size and then, if there is a run of vectors of the 
    // same size, then ensure that they are ordered in order of their *first* (i.e. bottom-most) element.
    // A normalised vector V = [ v1, v2, ... vk ] will satisfy |vi+1| >= |vi| for i = 1, ... k-1 and
    // whenever |vi+1| = |vi|, we have that vi+1[0] >= vi[0].  
    //
    // So for example, given the candidate solution [ [3, 4], [2, 1] [7, 6] [5] ], we'd normalise it to
    // [ [5], [2, 1], [3, 4], [7, 6] ].
    //
    // We now "just" have to count all normalised V such that SM(V) = a, and we're done :)
    //
    // A candidate solution can be expressed as a template defining the lengths of the vectors into
    // which we can place the elements 1 ... M.  For example, if M = 26, a candidate solution
    // might look like this:
    //
    //
    //       XXXX
    //    XXXXXXX
    //    XXXXXXX
    //   XXXXXXXX
    //
    //
    // Here, V = [ v1, v2, v2, ... v8 ], and |v1| = 1, |v2| = |v2| = |v3| =|v4| = 3 and
    //  |v5| = |v6| = |v7| = |v8| = 4.
    //
    // How many ways can we replace the X's with the elements 1 ... 26 in such a way that SM(V) = a?
    //
    // Note that we could describe the solution template as a vector containing the *lengths* of the vi
    // i.e. the template of V = [1, 3, 3, 3, 4, 4, 4, 4], but as we'll see, it's more useful to 
    // describe it via the lengths of the *layers*, starting at the bottom (length = 8) and ending at 
    // the top (length = 4).
    //
    // Applying Sherlock's algorithm to V would place the bottom layer (8 elements) into the array after sorting
    // the bottom layer.  If the first 8 elements of a are not in sorted order, then clearly there is 
    // no way of putting 1 ... 26 into V's template such that SM(V) = a.  If the first 8 elements of a are in
    // sorted order, then it looks like we can slot any of the 8! arrangements of the first 8 elements of a
    // into V: however, there's a subtlety here , affecting only the base layer, involving the fact that we only want to
    // generate normalised V that restricts this: let's skip it for now and come back to it later.
    //
    // Assume that the number of ways of adding the first 8 elements of a to the base layer of V's template is K.
    // What about the next layer? Applying Sherlock's algorithm will sort this next layer (of 7 elements) and place 
    // these 7 elements after the base layer's contributions to the output array i.e. in (0-relative) indices 8, 9, .... 14.
    // If the elements a[8], a[9], .... a[14] are not in sorted order then, again, there are 0 ways of adding 1 ... 26 to
    // V's template to give SM(V) = a, and we can give up.  If there are then, as long as we've arranged the base layer
    // to ensure that V is normalised (again, we'll deal with this later :)) then inserting *any* of the 7! permutations 
    // of a[8], a[9], .... , a[14] to the 2nd layer of V's template will give SM(V) = a (at least up to the first 8 + 7
    // elements of a!).
    //
    // Let's move up to the next layer, also of length 7: these will be placed by Sherlock's algorithm into indices
    // 15, 16, .... 21 of the output array; again, if a[15] , ... a[21] is not in sorted order, then there's no way
    // of adding elements to V to give SM(V) = a and we can stop; otherwise, there are 7! ways of adding
    // a[15], ... a[21] to this layer of V's template that give SM(V) = a to the first 8 + 7 + 7 elements of a.
    //
    // Finally, we reach the final layer (length 4): these will be placed in indices 22, 23, .... 25 of the output array,
    // and if a[22], a[23], .... a[25] are not in sorted order then get can't get SM(V) = a, else there are 
    // 4! ways of adding a[22], a[23], .... a[25] to the last layer of V.
    int M;
    cin >> M;

    a.resize(M);
    for (int i = 0; i < M; i++)
    {
        cin >> a[i];
    }

    lookup.resize(M + 1, vector<int64_t>(M + 1, -1));

    buildFactorialLookups(M);

    int64_t result = 0;
    for (int bottomLayerSize = 1; bottomLayerSize <= M; bottomLayerSize++)
    {
        result = (result + findNumWaysOfFillingRemainingStartingWithLayerSize(M, bottomLayerSize)) % ::modulus;
    }
    cout << result << endl;
}

