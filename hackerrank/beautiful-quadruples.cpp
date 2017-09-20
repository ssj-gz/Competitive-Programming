// Simon St James (ssjgz) - 2017-09-19 13:41
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <array>
#include <vector>
#include <map>
#include <algorithm>
#include <cassert>

using namespace std;

constexpr int numComponents = 4;

int computeMaxXor(const array<int, numComponents>& maxComponentValues)
{
    const auto largestMaxComponentValue = *std::max_element(maxComponentValues.begin(), maxComponentValues.end());
    // Given the constraints, it is guaranteed that 0 <= W^X <= maxXor and 0 <= Y^Z <= maxXor
    // (NB: we don't bother finding an optimal bound).
    int maxXor = 0;
    for (int i = 0; i <= largestMaxComponentValue; i++)
    {
        maxXor |= i;
    }

    return maxXor;
}

uint64_t findNumBeautifulQuadruples(const array<int, numComponents>& maxComponentValues)
{
    const int maxXor = computeMaxXor(maxComponentValues);

    uint64_t numUnbeautiful = 0;
    uint64_t numDistinct = 0;
    uint64_t numDistinctWX = 0; // Total number of pairs W, X (W <= X, W <= maxComponentValues[0], X <= maxComponentValues[1]) we've seen so far.
    vector<int> numWXXorsWithValue(maxXor + 1, 0);
    for (int Y = 1; Y <= maxComponentValues[2]; Y++)
    {
        uint64_t numDistinctYZForThisY = 0; // Num distinct pairs Y, Z for this Y.
        vector<int> numYZXorsWithValueForThisY(maxXor + 1, 0);
        // Generate Y ^ Z for this Y.
        for (int Z = Y; Z <= maxComponentValues[3]; Z++)
        {
            numDistinctYZForThisY++;
            numYZXorsWithValueForThisY[Y ^ Z]++;
        }
        if (maxComponentValues[1] >= Y)
        {
            // Bump X up to Y; xor it with all W satisfying W <= maxComponentValues[0] and W <= X (== Y) to incrementally update
            // numWXXorsWithValue.
            const int X = Y;
            for (int W = 1; W <= min(maxComponentValues[0], X); W++)
            {
                numWXXorsWithValue[W ^ X]++;
                numDistinctWX++;
            } 
        }
        uint64_t numUnbeautifulForThisY = 0;
        for (int value = 0; value <= maxXor; value++)
        {
            numUnbeautifulForThisY += numWXXorsWithValue[value] * numYZXorsWithValueForThisY[value];
        }

        numUnbeautiful += numUnbeautifulForThisY;
        numDistinct += (numDistinctWX * numDistinctYZForThisY);
    }
    return numDistinct - numUnbeautiful;
}

int main()
{
    // Fairly tricky one.  Firstly, note that xor is commutative, so we can re-order W,X,Y and Z in any
    // way we want  and get the same xor i.e. let ᴨ be a permutation of W,X, Y and Z; 
    // then ᴨ(W)^ᴨ(X)^ᴨ(Y)^ᴨ(Z) = W^X^Y^Z.
    //
    // So let A', B', C', D' be A, B, C, D in sorted order.  Let (W, X, Y, Z) be a quadruple with
    // W <= A, X <= B, Y <= C, Z <= D, and let ᴨ be a permutation that sorts W, X, Y, Z (i.e. ᴨ(W)<=ᴨ(X)<=ᴨ(Y)<=ᴨ); 
    // then it can be shown that ᴨ(W) ^ ᴨ(X) ^  ᴨ(Y) ^ ᴨ(Z)) =  W ^ X ^ Y ^ Z and ᴨ(W) <= A', ᴨ(X) <= B', ᴨ(Y) <= C', ᴨ(Z) <= D' i.e..
    // (ᴨ(W), ᴨ(X), ᴨ(Y), ᴨ(Z)) is beautiful using the sorted A', B', C', D' as the component max bounds.
    //
    // Now, (W1, X1, Y1, Z1) is equivalent to (W2, X2, Y2, Z2) if and only if they are equal once you sort
    // the components i.e. (1, 5, 7, 3) is equivalent to (7, 5, 3, 1) since sorting the components of the 
    // first gives (1, 3, 5, 7) and sorting the components of the 2nd also gives (1, 3, 5, 7). Thus, if we consider
    // only distinct (W1, X1, Y1, Z1) subject to W1<=X1<=Y1<=Z1, we'll never generate equivalent quadruples.
    //
    // So, to re-cap: to solve the problem, we need to sort A, B, C, D to give A',B',C',D', and then find
    // all (W,X,Y,Z) with W^X^Y^Z != 0 and W<=X<=Y<=Z and W<=A', X<=B', Y<=C' and Z<=D'.
    // If we can find all such (W,X,Y,Z) with W^X^Y^Z == 0 and subtract that from the number of such (W,X,Y,Z),
    // then we're done.
    //
    // Now, W^X^Y^Z == 0 if and only if W^X == Y^Z, so for each Y = 1, 2, ... , C', find, for all possible values of v (0 ... maxXor),
    // the number of ways of picking W,X with W^X = v and W<=A', X<=B', W<=X, X<=Y) (numWXXorsWithValue) and the number of ways of picking
    // Z (we've "fixed" Y) with Z>=Y and Z<=D' such that Y^Z == value (numYZXorsWithValueForThisY): then the number of all quadruples (W', X', Y, Z')
    // with W', X', Y, Z' subject to the standard constraints and which are not beautiful for this Y is
    // numWXXorsWithValue * numYZXorsWithValueForThisY.
    //
    // This process be performed very efficiently: for each of the C' Y, we do the following:
    //
    //  - find values that can be obtained by Y^Z and how many times that value can be obtained for Z >= Y, Z<=D' (O(D'));
    //  - find values that can be obtained by W^X and how many times that value can be obtained for W<=X, X<=Y, W<= A', X<=B' (O(A' * B')).
    //
    // In total, this is O(C' * (D' + A' * B')), which is too large (can be ~ 3000 * 3000 * 3000).
    // However, the latter step can be computed efficiently: if we've computed all such values from W, X where X <= Y-1,
    // we can use this to compute the values for X <= Y in O(A').
    // And that's about it!
     
    array<int, numComponents> maxComponentValues;
    for (int i = 0; i < numComponents; i++)
    {
        cin >> maxComponentValues[i];
    }
    sort(maxComponentValues.begin(), maxComponentValues.end());

    cout << findNumBeautifulQuadruples(maxComponentValues) << endl;
}
