// Simon St James (ssjgz) - 2019-11-06
// 
// Solution to: https://www.codechef.com/NOV19B/problems/CAMC
//
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

#include <cassert>

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}


int64_t findMinValueDiffOfValidBoxChoices(int N, int M, const vector<int64_t>& a)
{
    struct ValueAndColour
    {
        int64_t value = -1; 
        int colour = -1; 
    };

    vector<ValueAndColour> sortedValuesWithColour;
    for (int i = 0; i < N; i++)
    {
        sortedValuesWithColour.push_back({a[i], i % M});
    }
    sort(sortedValuesWithColour.begin(), sortedValuesWithColour.end(), [](const auto& lhs, const auto& rhs) { return lhs.value < rhs.value; });

    // NB: from this point onwards, all references to "index"/ "indices" refer to indices in sortedValuesWithColour, not in the
    // original "a", which we no longer care about.

    int64_t result = std::numeric_limits<int64_t>::max();

    int rightIndex = 0;
    vector<int> numOfColourInRange(M, 0);
    numOfColourInRange[sortedValuesWithColour[0].colour]++;
    int numColoursInRange = 1;
    for (int leftIndex = 0; leftIndex < N; leftIndex++)
    {
        // Maintain the invariant that rightIndex is the smallest index >= leftIndex
        // such that the set { a[j].colour | leftIndex <= j <= rightIndex} is 
        // precisely the set of M colours.
        while (numColoursInRange < M && rightIndex < N)
        {
            rightIndex++;
            if (rightIndex < N)
            {
                numOfColourInRange[sortedValuesWithColour[rightIndex].colour]++;
                if (numOfColourInRange[sortedValuesWithColour[rightIndex].colour] == 1)
                    numColoursInRange++;
            }
        }
        if (numColoursInRange != M)
        {
            // Could not find all M colours in indices >= leftIndex; no point progressing.
            break;
        }
        assert(rightIndex > leftIndex);
        assert(sortedValuesWithColour[rightIndex].colour != sortedValuesWithColour[leftIndex].colour);

        // There is a choice of M indices all with different colours with
        // the min value = sortedValuesWithColour[leftIndex] and the max value sortedValuesWithColour[rightIndex].
        result = min(result, sortedValuesWithColour[rightIndex].value - sortedValuesWithColour[leftIndex].value);

        // Drop the left hand element of the range.
        numOfColourInRange[sortedValuesWithColour[leftIndex].colour]--;
        if (numOfColourInRange[sortedValuesWithColour[leftIndex].colour] == 0)
            numColoursInRange--;
    }

    return result;
}


int main(int argc, char* argv[])
{
    // Quite a neat little problem; I'd rather have had this than the much more
    // straightforward "PHCUL" as the replacement Div 1 problem, but oh well :)
    // Managed to mess it up on the first attempt by - somehow - completely 
    // mis-interpreting the question, showing that I can still make stupid
    // schoolboy errors after decades of practice XD
    //
    // Anyway:
    //
    // QUICK EXPLANATION
    //
    // Let c_0, c_1, ... c_{m-1} be the M different colours.  One valid colouring
    // would be to simple colour the box i with the colour c_{i % M} (where i is
    // 0-relative) - call this the Main Colouring.  In fact, this Main Colouring
    // is "essentially" the only valid colouring in that all other valid colourings are just
    // the Main Colouring with the colours permuted.  In particular, for any valid colouring,
    // a choice of M boxes will have different colours in that colouring if and only
    // if they have different colours in the Main Colouring, so we can just assume
    // that the Main Colouring is always used by Chef.
    //
    // Sort the boxes in order of value, taking care to not forget each box's 
    // colour in the Main Colouring.  Then we can use the Two Pointer Technique to
    // find, for each leftIndex = 0, 1, ... , N - 1, the smallest rightIndex >=
    // leftIndex such that the boxes at indices in the range [leftIndex, rightIndex]
    // in our sorted list have, between them, the full set of M colours.  It's then
    // easy to find the set of all choices of M boxes that give the smallest 
    // difference between max and min values of the boxes.
    //
    // LONGER EXPLANATION
    //
    // Let's deal with the valid colourings, first.
    //
    // Lemma
    //
    // Let the boxes be coloured with a valid colouring.  Then for any consecutive block
    // of M m boxes, the colours of the boxes, in order, are a permutation of 
    // {c_0, c_1, ... c_{m-1}}.
    //
    // Proof
    //
    // There are M boxes, each of a colour in {c_0, c_1, ... c_{m-1}}, and each a different
    // colour - pretty much by definition, the box colours in order must be a permutation 
    // of this set.
    //
    // Corollary
    //
    // For any valid colouring, there is a permutation π of {c_0, c_1, ... c_{m-1}}
    // such that the first M boxes are coloured π(c_0), π(c_1), ... , π(c_{m-1}),
    // respectively.
    //
    // Lemma 
    // Let the boxes have a valid colouring.  Then there is a permutation π of 
    // {c_0, c_1, ... c_{m-1}} such that the box i is coloured π(c_{i % M}).
    //
    // Proof
    //
    // From the Corollary above, this holds for i = 0, 1, ... M - 1, so that forms
    // a base case for induction on i >= M.
    //
    // Assume it holds true for i >= M - 1; what can we deduce about the colour C of the
    // (i+1)th box?
    //
    // By induction hypothesis, the boxes with indices i - m + 1, i - m + 2, ... , i - 1, i
    // are coloured c_{(i - m + 1) % M}, c_{(i - m + 2) % M}, ... , c_{(i - 1) % M}, c_{i % M}.
    // These indices are a block of M consecutive indices, so because we have a valid colouring,
    // we must have:
    //
    //   {c_{(i - m + 1) % M}, c_{(i - m + 2) % M}, ... , c_{(i - 1) % M}, c_{i % M}} =
    //   {c_0, c_1, ... c_{M - 1}}
    //
    // Drop the first index (i - m + 1): then we have M - 1 consecutive indices, and:
    //
    //   {c_{(i - m + 2) % M}, ... , c_{(i - 1) % M}, c_{i % M}} =
    //   {c_0, c_1, ... c_{M - 1}} - c_{(i - m + 1) % M
    //
    // Now add the next index, i+1 - then we have M consecutive indices again, and:
    //
    //   {c_{(i - m + 2) % M}, ... , c_{(i - 1) % M}, c_{i % M}, C} =
    //   ({c_0, c_1, ... c_{M - 1}} - c_{(i - m + 1) % M) ⋃ C                       (1)
    //
    // But because it is a valid colouring, the boxes colours at these M consecutive indices 
    // must be precisely {c_0, c_1, ..., c_{M - 1}} in some order; i.e.
    //
    //   {c_{(i - m + 2) % M}, ... , c_{(i - 1) % M}, c_{i % M}, C} =
    //   {c_0, c_1, ... c_{M - 1}}                                                  (2)
    //
    // Substituting (2) into (1) gives:
    //   {c_0, c_1, ... c_{M - 1}} =
    //   ({c_0, c_1, ... c_{M - 1}} - c_{(i - m + 1) % M) ⋃ C
    //
    // which can only be the case if C = c_{(i - m + 1) % M}.  Hence result.
    //
    // Lemma
    //
    // Given a valid colouring of boxes, the boxes with indices i and j have a different colouring
    // if and only if i and j have a different colouring in the Main Colouring.
    //
    // Proof 
    //
    // The colours of the boxes i and j are, from the Lemma above, π(c_{i % M}) and π(c_{j % M}) respectively, for
    // some permutation π of {c_0, c_1, ... c_{M - 1}}.  Since π is a permutation, it has an 
    // inverse, π^-1.  Thus:
    //
    //     the boxes i and j have the same colour <=>
    //     π(c_{i % M}) = π(c_{j % M}) <=>
    //     π^-1(π(c_{i % M})) = π^-1(π(c_{j % M})) <=>
    //     c_{i % M} = c_{j % M}
    //     colour in Main colouring of i = colour in Main colouring of i.
    //
    // Corollary
    //
    // We can just assume that we are using the Main Colouring i.e. that the colour of box i is c_{i % M}.
    //
    // That's the issue of box colouring dealt with; now the issue of actually choosing boxes! As mentioned
    // in the Quick Explanation, we now sort the boxes by value, taking care to keep track of each box's
    // colour from the Main Colouring (see ValueAndColour).  From henceforth, this sorted vector is the only
    // one we care about (we ignore the original array, "a"), and any mention of "box indices" etc refers to
    // a box's index in this ValueAndColour array.
    //
    // Let leftIndex range from 0 to N - 1, and for each leftIndex, find the smallest index rightIndex, 
    // rightIndex >= leftIndex, such that the set { a[j].colour | leftIndex <= j <= rightIndex} is 
    // precisely the set of M colours.  This is pretty easy to do using standard Two-Pointer technique; the code 
    // hopefully speaks for itself.  It could have perhaps been implemented more simply using a multiset to store
    // the colours in the range [leftIndex, rightIndex], but the approach used is faster :)
    //
    // Note that by the minimality of rightIndex, ValueAndColour[leftIndex].colour != ValueAndColour[rightIndex].colour:
    // if we had ValueAndColour[leftIndex].colour != ValueAndColour[rightIndex].colour, then rightIndex we contribue
    // no additional colours to the set of colours in the range [leftIndex, rightIndex], and we would be able
    // to reduce it by at least 1, contradicting its minimality.
    //
    // Note that all indices j in the range [leftIndex, rightIndex] satisfy 
    //
    //     ValueAndColour[leftIndex].value <= ValueAndColour[j].value <= ValueAndColour[rightIndex].value
    //
    // i.e. leftIndex and rightIndex have the min and max values in the range, respectively.
    //
    // Now, we can use our leftIndex and rightIndex to form a set of M indices where the difference between
    // the max and min values of indexed boxes is minimal for the given leftIndex and is equal to
    // ValueAndColour[rightIndex].value - ValueAndColour[leftIndex].value and where all indices have
    // different coloured boxes: just take leftIndex, rightIndex, and M - 2 indices in the range
    // [leftIndex + 1, rightIndex - 1] that make up the remaining M - 2 colours that are other than those
    // of leftIndex and rightIndex.
    //
    // Finding the minimum difference of max element - min element over all leftIndex = 0, 1, ... , N -1
    // gives us our result.
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();
        const int M = read<int>();

        vector<int64_t> a(N);
        for (auto& x : a)
            x = read<int64_t>();

        cout << findMinValueDiffOfValidBoxChoices(N, M, a) << endl;
    }

    assert(cin);
}
