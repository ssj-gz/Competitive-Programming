// Simon St James (ssjgz) - 2019-05-12
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

int gcd(int a, int b)
{
    while (true)
    {
        if (a > b)
            swap(a, b);
        if (a == 0)
            break;
        const int nextA = a;
        const int nextB = b - a;

        a = nextA;
        b = nextB;
    }
    return b;
}

int findNumIterations(const vector<int>& aOriginal)
{
    set<int> aOriginalSet(aOriginal.begin(), aOriginal.end());
    // Make a sorted and with no repetitions.  The sorting is not 
    // strictly necessary.
    vector<int> a(aOriginal.begin(), aOriginal.end());
    int numIterations = 0;
    const int numInBlock = 64;
    const int numBlocks = *std::max_element(a.begin(), a.end()) / numInBlock + 1;

    while (!a.empty())
    {
        const int N = a.size();
        if (N == *std::max_element(a.begin(), a.end()))
        {
            // A has converged to a solid block of consecutive numbers;
            // we can predict how many iterations remain, so do this and bail.
            numIterations += N;
            break;
        }
        
        // Deal with the special case where A = {x, y, y+x} - the next iteration
        // will be {x, y-x, y}, so (by induction) we can deduce the state of 
        // A after a given number of iterations.  Skip until the gap between
        // x and y is small.
        if (N == 3 && (a[2] - a[1] == a[0]))
        {
            const int skipIterations = ((a[1] - a[0]) / a[0]) - 1;
            if (skipIterations > 0)
            {
                a[1] -= skipIterations * a[0];
                a[2] -= skipIterations * a[0];
                numIterations += skipIterations;
            }
        }

        if (numIterations == 1)
        {
            if (N > 1)
            {
                // Compute A / gcd(A) - this will have the same number of iterations
                // remaining as the original A.
                // Note that (I believe) this situation, if it will occur at all, will
                // always be detectable after the first iteration, so no need to
                // check it every iteration.
                int gcdOfAllInA = a.front();
                for (int i = 1; i < N; i++)
                {
                    gcdOfAllInA = gcd(gcdOfAllInA, a[i]);
                }
                if (gcdOfAllInA != 1)
                {
                    for (auto& x : a)
                    {
                        x /= gcdOfAllInA;
                    }
                }
            }
        }

        vector<uint64_t> blocks(numBlocks);
        for (const auto x : a)
        {
            blocks[x / numInBlock] |= (static_cast<uint64_t>(1) << static_cast<uint64_t>(x % numInBlock));
        }

        // Create lookup table for the bit-vector representing A bit-shifted by 0, 1, ... numInBlock.
        vector<uint64_t> blocksShifted(numBlocks);
        vector<vector<uint64_t>> blocksShiftedBy;
        blocksShiftedBy.push_back(blocks);
        for (int i = 1; i < numInBlock; i++)
        {
            blocksShiftedBy.push_back(blocksShiftedBy.back());
            for (int blockNum = 0; blockNum < numBlocks; blockNum++)
            {
                blocksShiftedBy.back()[blockNum] >>= 1;
                if (blockNum + 1 < numBlocks)
                {
                    // Due to shifting, the first bit of the next block should become the last bit of the current block.
                    blocksShiftedBy.back()[blockNum] |= ((blocksShiftedBy.back()[blockNum + 1] & 1) << static_cast<uint64_t>(numInBlock - 1));
                }
            }
        }

        // For each x in A, "add" (A - x) (i.e. the set consisting of all elements of A but with x subtracted from them)
        //  to the bit-vector bBlocks representing B using shifts and logical or's.
        // The x's are grouped according to their remainder mod numInBlock (and so, which of the 
        // blocksShifted to use) for efficiency, though this is likely a premature optimisation :/
        vector<vector<int>> withRemainder(numInBlock);
        for (const auto x : a)
        {
            withRemainder[x % numInBlock].push_back(x);
        }
        vector<uint64_t> bBlocks(numBlocks + 1, 0);
        for (int i = 0; i < numInBlock; i++)
        {
            for (const auto x : withRemainder[i])
            {
                const auto numBlocksToShift = x / numInBlock;
                for (int j = 0; j + numBlocksToShift < numBlocks; j++)
                {
                    bBlocks[j] |= blocksShiftedBy[i][j + numBlocksToShift];
                }
            }
        }

        // Unpack the bit-vector representing B into an array of integers.
        vector<int> b;
        for (int j = 0; j < numBlocks; j++)
        {
            const uint64_t block = bBlocks[j];
            for (int k = 0; k < numInBlock; k++)
            {
                if (((block >> static_cast<uint64_t>(k)) & 1) == 1)
                {
                    b.push_back(k + j * numInBlock);
                }
            }
        }
        if (!b.empty())
        {
            // We'll have accidentally introduced a '0' to B, as our "compute B using bit-vectors"
            // approach does not obey the "x != y" part of the iteration: remove it.
            assert(b.front() == 0);
            b.erase(b.begin());
        }

        a = b;
        numIterations++;
    }

    return numIterations;
}

int main(int argc, char* argv[])
{
    // Expert??
    //
    // Fundamentally very easy one - I made a few schoolboy errors due to trying to be a bit too
    // clever with optimisations, but there's a simple (though slightly slower) approach that would
    // have worked well enough and side-stepped these issues altogether.   A few helpful observations:
    //
    //    1) We can assume A and B do not contain repeated elements (i.e. we can remove repeated elements);
    //       a repeat of an element makes no difference to the next iteration.
    //    2) Neither A nor B will ever contain the element 0.
    //    3) If A has max element X, then the maximum element of B will be strictly less than x.
    //    4) A will nearly always saturate to become a set that consisting solely of a run of consecutive
    //       numbers starting at 1, and when this happens, it happens "exponentially".
    //    5) The sole exceptions to this - and this falls under the "a bit too clever" part mentioned above,
    //       occur when:
    //       i) A has one or two elements; and 
    //       ii) A has precisely three elements and is of the form A = {x, y, y+x} for some x and y (x != y).
    //       iii) A consists of elements all of whom are divisible by some K > 1.
    //    6) For a set of integers S, let S / K represent the set obtained by dividing each element of S by K, 
    //       where K is a factor of each element of S.   Then if iter(A, x) represents the result of iterating A x times:
    //   
    //          iter(A / K, x) = iter(A, x) / K.
    //
    //    7) If A consists solely of the consecutive numbers 1, 2, ... m, then B will consist soley of the run of 
    //       consecutive numbers 1, 2, ... m - 1. 
    //    8) Once repetitions have been disallowed, the maximum number of elements in A or B is 50'000.
    //
    // Nearly all of these are trivially obvious, and I can't be bothered to prove them XD.  5ii) is not so obvious, and
    // in fact I actually only stumbled upon it accidentally (I'd deduced the special case where A = {1, y, y+1}, but 
    // missed the general case) - it's largely irrelevant, though, as as mentioned, there's a simpler (but slightly slower) approach
    // that means we don't have to worry about it, though I didn't take this approach in this problem (the approach is
    // simple: if |A| < sqrt(N), then just compute B from it naively - it will be acceptably fast (O(N))).
    //
    // Note that 3) ensures that the iteration *always* terminates, so the "-1 if iteration never terminates" in the 
    // problem description is a red herring :)
    //
    // To see the "exponential" part of 4), consider what happens when A consists of a run of length X at the beginning 
    // and a run of length Y at the end: you'll see that a couple of iterations gives us a run of length at least X + Y 
    // and the beginning and X + Y at the end, causing A to rapidly saturate "from the ends".  Having elements in
    // between these book-ending runs just hastens the saturation.
    //
    // 7) allows us to instantly predict the number of iterations as soon as A has converged to a solid block of 1's.
    //
    // 5iii) poses a small problem, but just dividing A by gcd(A) (i.e. the greatest common divisor of all elements of A)
    // deals with this nicely - the resulting gcd(A)/A will (assuming it's not now of the form 5i) nor 5ii)!) now
    // converge to a solid block of 1's exponentially, and from 6), A / gcd(A) has the same number of iterations as the
    // original A.
    //
    // So: we can tackle all of 5i), 5ii) and 5iii) very efficiently (see the code), which leaves the case where A has
    // up to 50'000 elements.  How can we efficiently compute B from such a large A?
    //
    // The answer is simple, but not very satisfactory (IMO) as it doesn't give any asymptotic improvements: simply
    // use 64-bit bit arithmetic to perform the iteration in approx |A| * |A| / 64 steps instead of |A| * |A| - since
    // the number of iterations will be small (exponential progress towards a state where we can instantly deduce
    // the number of iterations remaining), this turns out to be good enough.
    //
    // Imagine we have a bit-vector A' representing A (so A'[x] is true if and only if x is in A).  Then to compute 
    // a bit-vector representing B, we'd simply have to, for each x in A:
    //
    //   i) bit shift A leftwards by x.
    //   ii) Set the bit-vector B to be itself or'd with this shifted version of A.
    //
    // My using a 64-bit type as the underlying type of the bit vector, we can make this process very efficient - see
    // the code for more details.
    //
    // That's about it - kind of a weird problem, and not really Expert-rated, IMO, especially if you skip some of the
    // more awkward logic and just use the "if |A| <= sqrt(N), compute B naively" approach.
    //
    // Kind of surprised at the (at the time of writing) < 10% completion rate.
    int n;
    cin >> n;

    vector<int> a(n);
    for (int i = 0; i < n; i++)
    {
        cin >> a[i];
        assert(a[i] != 0);
    }

    cout << findNumIterations(a) << endl;
}
