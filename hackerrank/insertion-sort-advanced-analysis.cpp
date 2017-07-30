#include <iostream>
#include <vector>
#include <set>
#include <cstdlib>
#include <cassert>
#include <utility>
#include <chrono>

using namespace std;

struct Result
{
    Result(long numOutOfOrder, const vector<long>& sorted)
        : numOutOfOrder(numOutOfOrder), sorted(sorted)
    {
    }
    long numOutOfOrder;
    vector<long> sorted;
};

Result findNumOutOfOrder(const vector<long>& a)
{
    long numOutOfOrder = 0;
    if (a.empty())
    {
        return Result(0, a);
    }
    if (a.size() == 1)
    {
        return Result(0, a);
    }
    if (a.size() == 2)
    {
        vector<long> aCopy(a);
        if (aCopy[0] > aCopy[1])
        {
            swap(aCopy[0], aCopy[1]);
            numOutOfOrder = 1;
        }
        return Result(numOutOfOrder, aCopy);
    }

    vector<long> leftHalf(a.begin(), a.begin() + a.size() / 2);
    vector<long> rightHalf(a.begin() + a.size() / 2, a.end());
    assert(leftHalf.size() + rightHalf.size() == a.size());

    auto processedLeftHalf = findNumOutOfOrder(leftHalf);
    const long outOfOrderLeftHalf = processedLeftHalf.numOutOfOrder;
    const vector<long>& sortedLeftHalf = processedLeftHalf.sorted;

    numOutOfOrder += outOfOrderLeftHalf;

    auto processedRightHalf = findNumOutOfOrder(rightHalf);
    const long outOfOrderRightHalf = processedRightHalf.numOutOfOrder;
    const vector<long>& sortedRightHalf = processedRightHalf.sorted;

    numOutOfOrder += outOfOrderRightHalf;

    vector<long> merged;
    merged.reserve(a.size());
    long leftHalfIndex = 0;
    long rightHalfIndex = 0;

    while (merged.size() != a.size())
    {
        bool pickLeft = true;
        if (leftHalfIndex >= sortedLeftHalf.size())
        {
            // Run out of left's.
            pickLeft = false;
        }
        else if (rightHalfIndex < sortedRightHalf.size() && sortedLeftHalf[leftHalfIndex] > sortedRightHalf[rightHalfIndex])
        {
            // Right is lower or equal to left.
            pickLeft = false;
        }
        if (pickLeft)
        {
            assert(leftHalfIndex <= sortedLeftHalf.size());
            merged.push_back(sortedLeftHalf[leftHalfIndex]);
            leftHalfIndex++;
        }
        else
        {
            assert(rightHalfIndex <= sortedRightHalf.size());
            merged.push_back(sortedRightHalf[rightHalfIndex]);
            rightHalfIndex++;
            const long numUnmergedInLhs = sortedLeftHalf.size() - leftHalfIndex; 
            numOutOfOrder += numUnmergedInLhs;
        }
    }

    return Result(numOutOfOrder, merged);
}

int main()
{
    // *sigh* - yet again, I think of a much simpler solution immediately after pressing submit.  Worse,
    // my original solution actually incorporated the entirety of this new solution as a substep!
    // Consider performing a partial insertion sort of an array a up to the point where we have to place x.
    // How many places must x move?
    // This is actually very easy: all elements currently preceding x in our partially-sorted a precede x 
    // in the original a. x must move left each time the element to its left is greater than x.
    // Putting this together: the number of places x must move is the number of elements in a that
    // precede x in a but are greater than x.  The total number of moves will be the total number of places
    // x must move, for all x in a.
    // How can we calculate this? A modified merge sort will do!
    // Take any array a, and split it in half.  Imagine we have a function called findNumOutOfOrder that computes
    //
    //   sum over x in a of (number of elements in  preceding x in a but greater than x) 
    //
    // and which also gives us a sorted a.
    // 
    // Split a down the middle, and apply findNumOutOfOrder to the lhs.  For example, with the original array a =
    //  [5 3 6 1] [4 7 8 2]
    // we'd have:
    //  [1 3 5 6] [4 7 8 2]
    // The contribution of everything in the lhs to total number of out of order is simply findOutOfOrder(lhs).
    // What about the contributions of the right-hand side? Well, for each element x in the rhs, we need to find
    //  1) all elements in lhs that are > x (NB: any element in lhs precedes any element in rhs in original a); and
    //  2) all elements in rhs that are > x and precede x in rhs (and thus precede x in original a).
    // 2) is easy to deal with just by calling findNumOutOfOrder(rhs).
    // For 1), we note that if we sorted rhs, and then did a merge of lhs and rhs, when we get to the point where
    // we are about to merge x in rhs, the set of elements in lhs that are > x is precisely the set of unmerged elements
    // from lhs.  Putting 1) and 2) together allows us to easily compute the total number of out of order elements,
    // and hence the total number of moves!

    long T;
    cin >> T;
    for (int t = 0; t < T; t++)
    {
        long N;
        cin >> N;
        vector<long> a(N);
        for (int i = 0; i < N; i++)
        {
            cin >> a[i];
        }
        Result processed = findNumOutOfOrder(a);
        cout << processed.numOutOfOrder << endl;
    }
}
