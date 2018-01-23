// Simon St James (ssjgz) 2018-01-23
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

int main()
{
    // Fairly tricky one, I thought - very surprised by the high rate of completion 
    // (71% at the time of writing, with over 4000 people solving it correctly!)
    //
    // Anyway, it's partly a trick question: the rule for updating the energy appears to 
    // be conditional on the current height, but is actually the same either way; we are 
    // told:
    //
    //  if h_k+1 > e_old, then 
    //    e_new = e_old - (h_k+1 - e_old) = 2 * e_old - h_k+1
    //  else
    //    e_new = e_old + (e_old - h_k+1) = 2 * e_old - h_k+1
    //
    // i.e. in either case, e_new = 2 * e_old - h_k+1.  Whatever!
    //
    // Iterating this a couple of times, we have, if e_start is the initial energy and e_i the energy
    // after i iterations:
    //
    //  e1 = 2 * e_start - h_1
    //  e2 = 2 * e1 - h_2 = 2 * (2 * e_start - h_1) - h_2 = 2^2 * e_start - 2^1 * h_1 + 2^0 * h_2
    //  ..
    //
    // and by induction, it's easy to see that
    //
    //  e_N = 2^N * e_start + sum [i = 0, 1, ... , N-1] { 2^(N - i) * h_i) }
    //
    // Let e_reduction = sum [i = 0, 1, ... , N-1] { 2^(N - i) * h_i); then we can easily find 
    // e_start by trial and error: we need only find the smallest e_start such that 2^N * e_start >= e_reduction.
    //
    // However, N can be up to 100'000, and 2^N is far too large to compute (as is e_reduction).  We could probably use 
    // a BigNum implementation, but this would probably also be too slow.  We need to rethink!
    //
    // Imagine if h[0] = 2m for some m, and all other height were 0; then e_reduction = 2 * m * 2 ^ N - 1 = m * 2^N,
    // and setting e_start = m is plainly the solution.  Can we somehow massage h so that it has the same
    // value of e_reduction, but in a form where h[0] is known and all other h's are 0? Not quite, but it turns out that 
    // we *can* get it into a form where all other h's are 0 or 1.  In this case, h_reduction is of the form
    //
    //  h[0] * 2^N-1 + sum [ i = 1, 2, ... N-1 ] { (0 or 1) * 2^(N - i) }
    //
    // The sum [blah] {blee} is < 2^N-1 by basic properties of powers of 2; then we see that setting 
    // e_start to either h[0]/2 or h[0]/2 + 1 if this is not large enough (i.e. if any of the other h's are non-zero).
    // h[0]/2 + 2 would be too high due to the inequality of the sum above.
    //
    // Now, if we just note that, if h[i] (i > 1) is greater than 2, then incrementing h[i - 1] and decrementing h[i] by two
    // gives us an h with exactly the same e_reduce.  So: if we start from the last element of h, decrement it by 2 and increment
    // the next-last element of h by 1 until we can't do this any more (i.e. the last element of h is either 0 or 1), then repeat
    // for the next-last element, then the next-next-last element etc, we end up with a height array of the required form (h[i] = 0 or 1
    // for i != 0) which has the same e_reduce as the original height array.
    //
    // Then, we just set e_start to h[0] / 2, and add the 1 if this isn't big enough i.e. if any of the remaining h's are non-zero.  Ta-da!
    //
    // UPDATE: Ooooh - the Editorial is much simpler.  Clever!
    int N;
    cin >> N;

    vector<int> heights(N);
    for (int i = 0; i < N; i++)
    {
        cin >> heights[i];
    }

    int startEnergy = 0;
    // Re-work the array to one with an identical e_reduce but where heights[i] = 0 or 1 whenever i != 0.
    for (int i = N - 1; i >= 0; i--)
    {
        if (i >= 1)
        {
            heights[i - 1] += heights[i] / 2;
        }
        else
        {
            // i.e. startEnergy = h[0] / 2, roughly.
            startEnergy = heights[i] / 2;
        }
        heights[i] %= 2;
    }

    bool hasNonZeroHeight = false;
    for (const auto h : heights)
    {
        if (h != 0)
        {
            hasNonZeroHeight = true;
            break;
        }
    }
    if (hasNonZeroHeight)
        startEnergy = startEnergy + 1;

    cout << startEnergy << endl;
}
