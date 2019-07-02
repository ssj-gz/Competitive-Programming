// Simon St James (ssjgz) 2019-05-06
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

vector<int64_t> calcRowOfMatrix(const vector<int64_t>& firstRow, int n, uint64_t m)
{
    m--; // Already know the 0th row.
    vector<int64_t> currentRow(firstRow);

    uint64_t powerOf2 = static_cast<uint64_t>(1) << static_cast<uint64_t>(63);
    while (powerOf2 != 0)
    {
        if (m >= powerOf2)
        {
            vector<int64_t> currentPlusPowerOf2thRow(n);
            for (int j = 0; j < n; j++)
            {
                currentPlusPowerOf2thRow[j] = currentRow[j] ^ currentRow[(j + powerOf2) % n];
            }
            currentRow = currentPlusPowerOf2thRow;


            m -= powerOf2;
        }
        powerOf2 >>= 1;
    }
    return currentRow;
}

int main(int argc, char* argv[])
{
    // A bit of a disappointment, this one, as I was unable to Logic
    // my way to a solution, and instead had to pattern-recognise :(
    //
    // After playing around with a brute force solution, I was unable
    // to recognise any usable patterns (though looking at the output
    // reveals fairly rad Sierpinski's triangle-type patterns :)),
    // so I tried expanding the recurrence relation to see if I could
    // glean anything from that.
    //
    // We have a[i][j] = a[i-1][j] ^ a[i-1][(j + 1) % n].  We can keep
    // expanding this to see that every a[i][j] is a xor-sum of elements
    // from the first row of A.  Since Xor is commutative, we can re-arrange
    // this xor sum however we please, and since x ^ x = 0, we see that
    // the contribution of a first-row element a[0][j] is 0 if it occurs 
    // an even number of times in this xor sum, and odd otherwise i.e.
    // we only care about the parity of the number of occurrences in
    // the xor sum expanded from a[i][j].
    //
    // Note that there is rotation symmetry, here, so a[0][k] contributes
    // to a[i][j] if and only if a[0][(k + 1) % n] contributes to a[i][(j + 1) % n].
    //
    // I was hoping to find a nice exploitable pattern, here - i.e. perhaps the 
    // a[0][i]s for odd i are entirely absent from every other row, or something,
    // allowing us to somehow find a nice, closed-form solution for each row - 
    // but nothing like that surfaced.  I tried to logic my way to it - there's a strong
    // resemblance to Pascal's Triangle, here - but failed to do so.
    //
    // Worse, it seems that an a[0][k] quite often contributes to O(n) elements of
    // a given row, so attempting to capitalise on such a pattern would be O(n ^ 2)
    // (n a[0][k]'s each contributing (by the o"rotational symmetry" argument above)
    // to O(n) a[i][j]'s), so this seemed like it might be a dead end.
    //
    // Out of desperation, though, I tried to find the rows where a[k]'s contribute to the
    // minimum number in that row - perhaps sometimes it would contribute to O(1) of them,
    // instead of O(n)? - and indeed that turned out to be the case.
    //
    // By pure pattern recognition, we can observe the following: (*)
    //
    //   if i is a power of 2, then the 0th column of that row has
    //   contributions from a[0][0] and a[0][i % n] i.e. the two first row elements 
    //   a[0][0] and a[0][i % n], and only these two first row elements, occur an odd 
    //   number of times in the xor sum of such a a[i][0].
    //
    // Again from the rotational symmetry argument above, this means that
    //
    //   if i is a power of 2, then the jth column of that row has
    //   contributions from a[0][j] and a[0][(j + 1) % n] and no other contributions i.e.
    //   a[i][j] = a[0] ^ a[(i + j) % n] for such an i.
    //
    // and so on.
    //
    // This means that, if we know a given row, we can compute some row that is a 
    // power of 2 larger than the current row in O(n)  operations.  It's then a 
    // simple matter to jump up "the largest power of 2 at a time" rows until we 
    // reach the desired mth row, giving up an O(n log2 m) solution.
    //
    // And that's it!
    // 
    // (*) Although I couldn't logic my way to solution, this is not complete magic and
    // can be relatively easily proven by induction: Let i = 1 << k, and assume
    // true for k - 1.  From our (1 << (k - 1))th row - which has, from induction hypothesis,
    // two columns that have contributions from a[0][0] and a[0][i], respectively - we
    // can jump down (1 << (k - 2)) rows, and this will give us - by induction hypothesis -
    // *four* columns that have contributions from the first row.  Which ones precisely
    // is left as an exercise for the reader :)
    //
    // Jump down a further (1 << (k - 2) rows, and (exercise for the reader, again!), we see
    // that - taking our known ((1 << (k - 1)) + (1 << (k - 2))) row with the four columns with
    // contributions and again using the induction hypothesis - we end up with just two
    // columns with contributions from the first row, and these are at exactly the places
    // expected by the induction hypothesis for row (1 << k).  But this *is* row (1 << k) - 
    // we started at (1 << (k - 1)) and took two steps, each of size (1 << (k - 2)), ending up 
    // at row 
    //
    //    (1 << (k - 1)) + (1 << (k - 2)) + (1 << (k - 2))
    //    = (1 << (k - 1)) + (1 << (k - 1))
    //    = (1 << k)
    //
    // Thus proving the induction hypothesis!
    int n;
    cin >> n;
    uint64_t m;
    cin >> m;

    vector<int64_t> a(n);
    for (int i = 0; i < n ; i++)
    {
        cin >> a[i];
    }

    const auto solution = calcRowOfMatrix(a, n, m);
    for (int i = 0; i < n; i++)
    {
        cout << solution[i] << " ";
    }
    cout << endl;
}


