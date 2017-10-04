// Simon St James (ssjgz) 2017-09-30 13:15
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;

int main()
{
    // Hmmm ... fairly easy one, I thought, and am quite surprised at the low completion rate
    // (~24.32% at the time of writing), especially as I completed Toll Cost Digits yesterday
    // which had a completion rate of ~44% and absolutely kicked my ass :)
    //
    // Anyway, let's have a bit of terminology.  For an int i, let A + i be the array A with
    // i added to all its values.  For any array X, let ABS(X) be the sum of the absolute
    // values of all elements of X.  The problem gives us Q values x1 ... xQ and at each step
    // we need to perform the following:
    //
    //  A = A + xi
    //  print ABS(A).
    //
    // Note first of all that ((A + x1) + x2) + ... ) + xk is simply A + (x1 + x2 + ... + xk), so
    // we can reduce this to:
    //
    // totalX = 0
    // For each i = 1 ... Q
    //   totalX = totalX + xi
    //   print ABS(A + totalX).
    //
    // So the problem reduces to computing ABS(A + totalX) for some totalX.
    //
    // Now, imagine what would happen if A consisted entirely of non-negative numbers: then A + 1
    // would simply increase the absolute value of each element, so that ABS(A + 1) = ABS(A) + N,
    // and in general, by induction, ABS(A + i) = ABS(A) + i * N for all i >= 0.
    // Similarly, imagine that if A consisted entirely of negative numbers; then A - 1 would
    // also increase the absolute value of each element, so ABS(A - 1) = ABS(A) + N and again
    // in general, ABS(A - i) = ABS(A) + i * N for i >= 0.
    //
    // Now, if i >= -smallestElement, then A + i would give an array where all values are non-negative.
    // So let i >= -smallestElement, j = i - -smallestElement (so j >=0); then 
    //
    //  ABS(A + i) = ABS(A + -smallestElement + j) = ABS((A + -smallestElement) + j) = 
    //  ABS(A + -smallestElement) + j * N = ABS(A + -smallestElement + (i - -smallestElement) * N
    //
    // If we know ABS(A + -smallestElement) - which we just need to compute once and then lookup - then we can 
    // compute ABS(A + i) instantly.
    //
    // Similarly, if i <= -largestElement and j = (-largestElement - i) (so j >= 0), then
    //
    //  ABS(A + i) = ABS(A + -largestElement - j) = ABS((A + -largestElement) - j) = ABS(A + -largestElement) + j * N
    //   = ABS(A + -largestElement) + (-largestElement - i) * N
    //
    // So, if i >= -smallestElement or i <= -largestElement, then we can instantly find ABS(A + i).
    //
    // For the remaining values (i = -largestElement .. -smallestElement), we can build a lookup table by just computing ABS(A + i)
    // by brute force. Unfortunately, -largestElement .. -smallestElement can cover 4'001 values, and N can be up to 500'000:
    // 4'001 * 500'000 is (just a little) too untractable to be computed :/

    // However, we can compute this lookup table more efficiently by first sorting A, then subtracting largestElement from 
    // each value in A.  We then have an A where all elements are non-positive (i.e. <= 0) and where elements are in ascending
    // order.  Find the currentAbsoluteSum of this new A (easy).  Now: what would happen to the absolute sum of A if we added 1 to A?
    // We could do this manually but as mentioned above, doing this 4'001 times would be too intractable.  Observe that if we added 1 then
    // all non-negative values in A would have their absolute values increased by 1, and all negative values would have 
    // their absolute value reduced by 1; then ABS(A + 1) = ABS(A) + num non-negative values in A - num negative values in A.
    // i.e. ABS(A + 1) = ABS(A) + numNonNegative - (N - numNonNegative) = ABS(A) + 2 * numNonNegative - N.
    // So if, instead of adding 1 for each of the 4'001 values of i, we could just find the number of non-negative values of 
    // A + i, we'd be done.  But this is very easy: consider the index of the first non-negative number in A + i, firstNonNegativeIndex.
    // As i increases, this moves exclusively to the left (or not at all :)), and we can easily find firstNonNegativeIndex for A + i
    // by decreasing firstNonNegativeIndex until A[firstNonNegativeIndex - 1] + i < 0.  Then the numNonNegative is simply N - firstNonNegativeIndex.
    // This way, we can construct our absoluteSumLookup very quickly.
    //
    // And that's about it!

    int N;
    cin >> N;
    vector<int> A(N);

    for (int i = 0; i < N; i++)
    {
        cin >> A[i];
    }
    
    sort(A.begin(), A.end());

    const int smallestElement = A.front();
    const int largestElement = A.back();

    // Subtract largestElement from all elements in A; A now has
    // all non-positive values in ascending order.  Compute currentAbsoluteSum 
    // while we're at it!
    int64_t currentAbsoluteSum = 0;
    for (auto& value : A)
    {
        value -= largestElement;
        currentAbsoluteSum += abs(value);
    }

    // absoluteSumLookup[i] = absolute sum of (original A + (i - largestElement)).
    vector<int64_t> absoluteSumLookup(largestElement - smallestElement + 1);
    int firstNonNegativeIndex = N - 1; // The last element is 0.
    for (int i = 0; i <= largestElement - smallestElement; i++)
    {
        absoluteSumLookup[i] = currentAbsoluteSum;

        // "Pretend" that we've added i to A, and find firstNonNegativeIndex in this imaginary array!
        // Note that firstNonNegativeIndex will always only move leftwards, and will move at most
        // N places in the entire construction of the lookup table.
        while (firstNonNegativeIndex > 0 && A[firstNonNegativeIndex - 1] + i >= 0) 
        {
            firstNonNegativeIndex--;
        }
        // As i increases, numbers that would be non-negative in A + i give currentAbsoluteSum an extra 1;
        // numbers that would be negative would subtract 1.
        const int numNonNegative = N - firstNonNegativeIndex;
        currentAbsoluteSum += numNonNegative;
        const int numNegative = N - numNonNegative;
        currentAbsoluteSum -= numNegative;
    }

    int Q;
    cin >> Q;

    int64_t totalX = 0; 
    for (int i = 0; i < Q; i++)
    {
        int x;
        cin >> x;

        totalX += x;

        int64_t absoluteSum = 0;
        if (totalX > -smallestElement)
        {
            // All elements are positive.  The last element in absoluteSumLookup is ABS(original A + -smallestElement).
            absoluteSum = absoluteSumLookup.back() + N * (totalX - -smallestElement);
        }
        else if (totalX < -largestElement)
        {
            // All elements are negative.
            absoluteSum = absoluteSumLookup.front() + N * (-largestElement - totalX);
        }
        else
        {
            // Elements are a mix of positive and negative; use our lookup.
            absoluteSum = absoluteSumLookup[totalX + largestElement];
        }

        cout << absoluteSum << endl;
    }
}
