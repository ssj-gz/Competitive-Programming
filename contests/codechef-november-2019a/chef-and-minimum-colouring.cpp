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


int64_t solveOptimised(int N, int M, const vector<int64_t>& a)
{
    // Ok - if c1, c2, ... , cm are the colours, then any
    // valid colouring is of the form:
    //
    // π(c1)π(c2)...π(cm)π(c1)π(c2)...π(cm)...π(c1)π(c2)...
    //
    // where π is a permutation of c1, c2, ... , cm i.e.
    // is it some permutation of c1, c2, ... , cm.
    //
    // Similarly, a choice is valid for such a π if and only if
    // it is valid for the colouring:
    //
    // c1c2...cmc1c2...cm...c1c2...
    //
    // The colours at i and j are different if and only if i != j mod m,
    // so we merely need to find, for each k ... 1, 2, ..., m, the 
    // maximum a[i] such that i mod m = k and the minimum a[j] such that
    // j mod m != k.
    // TODO - rest of documentation, etc.

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
    //
    // Anyway:
    //
    // QUICK EXPLANATION
    //
    // Let c_0, c_1, ... c_{m-1} be the M different colours.  Once valid colouring
    // would be to simple colour the box i with the colour c_{i mod M} (where i is
    // 0-relative) - call this the Main Colouring.  In fact, this Main Colouring
    // is "essentially" the only colouring in that all other colourings are just
    // the Main Colouring with the colours permuted.  In particular, for any colouring,
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
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();
        const int M = read<int>();

        vector<int64_t> a(N);
        for (auto& x : a)
            x = read<int64_t>();

        const auto solutionOptimised = solveOptimised(N, M, a);
        cout << solutionOptimised << endl;
    }

    assert(cin);
}
