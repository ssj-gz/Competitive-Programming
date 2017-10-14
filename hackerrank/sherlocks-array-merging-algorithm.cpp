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

int64_t findNumWaysOfFillingRemainingStartingWithLayerSize(int numRemainingInArray, int layerSize)
{
    assert(numRemainingInArray >= 0 && numRemainingInArray < lookup.size());
    assert(layerSize >= 1 && layerSize < lookup[0].size());
    if (numRemainingInArray == 0)
        return 1;
    if (numRemainingInArray < layerSize)
        return 0; 

    auto& memoiseEntryRef = lookup[numRemainingInArray][layerSize];
    if (memoiseEntryRef != -1)
        return memoiseEntryRef;

    int64_t result = 0;
    auto layerIsInOrder = true;
    const auto posInArray = a.size() - numRemainingInArray;
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
        const auto isBottomLayer = (numRemainingInArray == a.size());
        const auto numPermutationsForThisLayer = isBottomLayer ? 1 : factorial(layerSize);
        if (numRemainingInArray != layerSize)
        {
            for (int nextLayerSize = layerSize; nextLayerSize >= 1; nextLayerSize--)
            {
                const auto contributionFromBottomLayer = nCr(layerSize, layerSize - nextLayerSize, ::modulus);
                const auto permutationFactor = (numPermutationsForThisLayer * contributionFromBottomLayer) % ::modulus;
                const auto nextLayerResult = findNumWaysOfFillingRemainingStartingWithLayerSize(numRemainingInArray - layerSize, nextLayerSize);
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
    // So: let V = [ v1, v2, .... vk ] for some k be a vector of vectors, and let SM(V) be the array
    // created by applying Sherlock's merging algorithm to V.  We need to find the number of 
    // *distinct* V such that SM(V) = a.  We're are given the rules under which V1 and V2 are 
    // equivalent and, as is often the case with equivalence relations, it's handy to pick a normalisation 
    // procedure such that the normalisations of two *equivalent* solutions A and B are *equal*.  
    // In this case, it's easy to see what procedure to use: take a solution; sort its vectors by size and then, 
    // if there is a run of vectors of the same size, ensure that they are ordered in order of their *first* 
    // (i.e. bottom-most) element.
    //
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
    //       XXXX
    //    XXXXXXX
    //    XXXXXXX
    //   XXXXXXXX
    //
    // Here, V = [ v1, v2, v2, ... v8 ], and |v1| = 1, |v2| = |v2| = |v3| =|v4| = 3 and
    //  |v5| = |v6| = |v7| = |v8| = 4.
    //
    // How many ways can we replace the X's with the elements 1 ... 26 in such a way that SM(V) = a?
    //
    // Note that we could describe the solution template as a vector containing the *lengths* of the vi
    // i.e. the template of V = [1, 3, 3, 3, 4, 4, 4, 4], but as we'll see, it's more useful to 
    // describe it via the lengths of the *layers*, starting at the bottom (length = 8) and ending at 
    // the top (length = 4).  Here, layerSize[0] = 8, layerSize[1] = 7, layerSize[2] = 7 and layerSize[3] = 4.
    //
    // Applying Sherlock's algorithm to V would place the elements in the bottom layer (layerSize[0] = 8 elements) into 
    // the array after sorting them.  If the first 8 elements of a are not in sorted order, then clearly there is 
    // no way of putting 1 ... 26 into V's template such that SM(V) = a.  If the first 8 elements of a are in
    // sorted order, then it looks like we can slot any of the 8! arrangements of the first 8 elements of a
    // into V: however, there's a subtlety here, affecting only the base layer, involving the fact that we only want to
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
    //
    // So, if the template of V has layer sizes (from bottom to top) layerSize[0], layerSize[1], ... , layerSize[l],
    // then for each i = 0 ... l, we need to check whether the elements of a from sum [ 0 <= j < i {layerSize[j]} to
    // sum [ 0 <= j < i {layerSize[j]} + layerSize[i] - 1 are sorted; if not, there are no solutions with this template.
    // Otherwise, for each layer i = 1 ... l (the bottom layer, i = 0, is deal with separately), we see that there
    // are layerSize[i]! ways of assigning numbers to that layer.  What about the bottom layer?
    //
    // As mentioned, we are only considering normalised V, so we may not add elements to the bottom layer in a way
    // that contravenes this i.e. in such a way that the first elements of vectors of the same size do not occur in
    // sorted order.  We see that there is a relation between the number of vectors of the same size and the layer
    // sizes:
    //
    //       XXXX
    //    XXXXXXX
    //    XXXXXXX
    //   ABBBCCCC
    //
    // to make it more obvious, the bottom elements of this template are marked with the same letter if they are the first
    // letter of vectors with the same length.  The correspondence should hopefully be clear: the points where vector
    // lengths change are the points where the sizes of consecutive layers change, and the size of the runs of same-length vectors matches
    // changes in layer size between consecutive layers.  If the first x1 vectors are the same size, then we must assign
    // elements of a to these x1 places so that they are in sorted order; there are layerSize[0] choose x1 ways to do this.
    // If the next x2 vectors are the same size, then there are layerSize[0] - x1 choose x2 ways of doing this. If the next x3
    // vectors have the same size, then there are layerSize[0] - (x1 + x2) choose x3 ways of doing  this; etc.
    //
    // In general, noting the correspondence between lengths of runs of vectors of the same size and the difference between
    // sizes of consecutive layers, and the fact that n choose 0 is always 1, we see that the number of ways
    // of assigning values to the bottom layer is:
    //
    // sum [ i = 1 ... l ] { (layerSize[0] - sum [ 1 <= j < i ] { layerSize[j] - layerSize[j - 1]}) choose (layerSize[i] - layerSize[i - 1]}.
    //
    // Putting this all together, for a V with a template with layer sizes layerSize[0], layerSize[1], ... , layerSize[l], the number of 
    // ways of filling the template, provided it is not determined to be 0 because a required range of a is not in sorted order, is:
    //
    // sum [ i = 1 ... l ] { (layerSize[0] - sum [ 1 <= j < i ] { layerSize[j] - layerSize[j - 1]}) choose (layerSize[i] - layerSize[i - 1]} * 
    // sum [ i = 1 ... l ] { layerSize[i]!}
    //
    // So one approach would be to generate all templates, then, once we've generated each template, use the process above (check whether there
    // are any ways at all of populating the template such that SM(V) = a, then use the formula above to find how many ways there are).  Generating
    // all possible templates layer-by-layer (starting with the base layer) is easy:
    //
    //  int64_t findNumWaysOfFillingRemainingStartingWithLayerSize(int numRemainingInArray, int layerSize, vector<int> layerSizesSoFar)
    //  {
    //     if (numRemainingInArray == 0)
    //     {
    //         // We have a full template that can accommodate all elements of the array. 
    //         return num ways of populating this template (the template is described by layerSizesSoFar), which could be 0 if any required range of 
    //                a is not in sorted order. 
    //
    //     }
    //     int64_t result;
    //     for (int nextLayerSize = layerSize; nextLayerSize >= 1; nextLayerSize--)
    //     {
    //        result += findNumWaysOfFillingRemainingStartingWithLayerSize(numRemainingInArray - layerSize, nextLayerSize, layerSizesSoFar << layerSize);
    //     }
    //     return result;
    //  }
    //
    // But the computation of the number of ways of filling a layer can also be computed layer-by-layer, so we can actually do them both in
    // tandem (including the check whether the required range of a is sorted).
    //
    // Hopefully, with all this background and the comments, the final code is clear :)
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

