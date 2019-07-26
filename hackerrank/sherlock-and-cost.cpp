// Simon St James (ssjgz) 2017-7-19 10:35
#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char* argv[])
{
    // Easy one if you trust intuition; a bit trickier if you want a more formal analysis :)
    // So, intuitive part: it seems that the maximum would be obtained if each choice of a[i]
    // differed from the previous as much as possible: that is, where each A[i] is either
    // B[i] or 1, the two possible extremes for A[i].
    // 
    // Let's see if we can come up with some kind of argument for this.   Firstly, let's 
    // get some kind of recurrence relation going.
    // Imagine we have found the largest value that can be obtained using the first
    // m values of B, where the last value chosen is exactly a (1 <= a <= B[m - 1]).
    // What can we say about the relation to the first m - 1 choices?
    // Let's say that the m - 1th choice was x (1 <= x <= B[m - 2]), and gave a total value V.  
    // Then the value of our m-choice is V + |a - x|.  Imagine there
    // had been another choice of the first m - 1 which ended on x but gave a larger
    // value V' - choosing a as the mth choice would give a value V' + |a - x| > V + |a - x|
    // contradicting the optimality of our choice.  Thus, the m - 1 choice ending on a
    // must be the largest value that can be obtained using the first m - 1 and ending on a.
    // Likewise, consider another choice of m - 1 which ends on b (!= a), say, and has value 
    // V''.  If V'' + |a - b| > V + |a - x|, then this would contradict the optimality of our
    // choice.  Putting these observations together, we see that, if largestUsingFirstMEndingOnA[m][a]
    // represents the largest value obtained using the first m choices the last choice is a, that:
    // 
    //  largestUsingFirstMEndingOnA[m][a] = max over all x <= 1 <= B[m-2] {largestUsingFirstMEndingOnA[m - 1][x] }.
    //
    // Since each B is limited to 100, and m <= 10^5, we could easily compute this recurrence relation for a few
    // examples, but for T = 20 of them, we might start running into trouble!  So let's work out how to
    // reduce this.  Our intuition above suggested that in an optimal choice, each a[i] would likely be either 1 or B[i].  We can
    // easily prove that it is the case for a[0].  Let C = (a[0], a[1], a[2], ... a[n - 1]) be an optimal choice (n > 1).
    // The value of V is then sum over i = 1 to n - 2 of { |a[i] - a[i+1]|} + |a[1] - a[0]|.  (*)
    // If a[0] is either 1 or B[0] then we're done, so assume it is not: then a[0] can be increased or decreased by 1.
    // Now, if a[0] >= a[1], then increasing a[0] increases the value of |a[1] - a[0]| by 1; plugging this into
    // (*) shows that the modified (and still valid) C gives a value of V + 1, contradicting the optimality of C.
    // Likewise, if a[0] < a[1], then decreasing a[0] by 1 increases the value of |a[1] - a[0]|, again giving
    // another valid choice with value > V.  Hence result.
    // What about for a[i], i != 0?
    // Again, pick a C = (a[0], a[1], a[2], ... a[n-1]) to be optimal.  If each a[i] is either 1 or B[i], we're done,
    // so assume there is an a[i] that is not and and let k be the first i.e. a[k] is neither 1 nor B[k].  So a[k]
    // can be increased or decreased by (at least) 1.
    // From above, we know that k != 0 (else we could pick a better choice of C where it was, from above).
    // The value of C is 
    //
    //   sum over i = 0 to k - 2 {|a[i] - a[i + 1]} + sum over i = k + 1 to n - 2 {a|i| + a|i + 1} + (|a[k - 1] - a[k]| + |a[k] - a[k + 1]).
    //
    // In particular, note that varying a[k] can only alter the final bracketed term, and that if we can change a[k] so that it is 1 or b[k]
    // without changing the value of that bracketed term (or even better, increasing it) then we're done.
    // Here things get boring, so I'm going to leave it as an exercise for the reader ;) Do note though that if a[k] < min(a[k - 1], a[k+1])
    // then decreasing a[k] will increase the bracketed term.  Likewise, if a[k] > max(a[k - 1], a[k + 1]), then increasing it will
    // also increase the bracketed term.  If a[k] >= min(a[k - 1], a[k + 1]) and a[k] <= max(a[k - 1], a[k + 1]), then changing a[k]
    // will leave the bracketed term unchanged.  In any case, we see that we can move a[k] to either 1 or b[k] (depending on 
    // where it sits in relation to its neighbours) and obtain a choice that is at least as good as C.
    //
    // So, what this means is that in our original recurrence relation, we don't have to range over all x <= 1 <= B[m - 2]; instead
    // x can be assumed to be either 1 or B[m - 2].  Thus, we only need to maintain two values: the current best choice using
    // the first m where the last choice is 1, and the current best choice using the first me where the last choice is B[m - 1].
    // The final, simple recurrence relation is shown in the code below.
    //
    // As a sidenote, you might think that letting A[i] "alternate" between two extremes would maximise the cost e.g. that
    // the optimal A would be of the form:
    // 
    //   1 B[1] 1 B[3] 1 ... etc
    //
    // or 
    //
    //   B[0] 1 B[2] 1 B[4] etc
    //
    //
    // But the case where B = [5 2 2 5 2] is a counter-example to this.
    int T;
    cin >> T;
    for (int t = 0; t < T; t++)
    {
        int N;
        cin >> N;
        vector<int> B(N);
        for (int i = 0; i < N; i++)
        {
            cin >> B[i];
        }
        int64_t maxValueEndingAtTop = 0;
        int64_t maxValueEndingAtBottom = 0;

        for (int i = 1; i < N; i++)
        {
            const int64_t newMaxValueEndingAtTop = max(maxValueEndingAtBottom + abs(B[i] - 1),
                                                 maxValueEndingAtTop + abs(B[i] - B[i - 1]));
            const int64_t newMaxValueEndingAtBottom = max(maxValueEndingAtBottom + abs(1 - 1),
                                                 maxValueEndingAtTop + abs(1 - B[i - 1]));

            maxValueEndingAtTop = newMaxValueEndingAtTop;
            maxValueEndingAtBottom = newMaxValueEndingAtBottom;
        }

        cout << max(maxValueEndingAtTop, maxValueEndingAtBottom) << endl;

    }
}


