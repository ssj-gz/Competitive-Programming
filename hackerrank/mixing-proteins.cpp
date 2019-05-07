// Simon St James (ssjgz) - 2019-05-07
#include <iostream>
#include <vector>

using namespace std;

string calcRowOfMatrix(const string& originalProteinString, int n, uint64_t m)
{
    // Adapted from Xor Matrix.
    auto transform = [](char a, char b)
    {
        static const char transformTable[][4] =
        {
            {'A', 'B', 'C', 'D' },
            {'B', 'A', 'D', 'C' },
            {'C', 'D', 'A', 'B' },
            {'D', 'C', 'B', 'A' }
        };
        return transformTable[a - 'A'][b - 'A'];

    };

    string currentRow(originalProteinString);

    uint64_t powerOf2 = static_cast<uint64_t>(1) << static_cast<uint64_t>(63);
    while (powerOf2 != 0)
    {
        if (m >= powerOf2)
        {
            // Set the row to be all the identity element : we'll update it by 
            // transforming each element.
            string currentPlusPowerOf2thRow(n, 'A');

            for (int j = 0; j < n; j++)
            {
                // From the Xor Matrix result, currentRow[j] and currentRow[(j + powerOf2) % n]
                // are the only elements that occur an odd number of times for column j
                // in currentPlusPowerOf2thRow.
                //
                // From the other analysis - which says we don't care about A's and we only
                // care about the parity of B's, C's, and D's, we can easily find the resulting
                // letter at currentPlusPowerOf2thRow[j].
                currentPlusPowerOf2thRow[j] = transform(currentPlusPowerOf2thRow[j], currentRow[j]);
                currentPlusPowerOf2thRow[j] = transform(currentPlusPowerOf2thRow[j], currentRow[(j + powerOf2) % n]);
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
    // A weird one, as it's basically a (slightly) more complex version of Xor
    // Matrix, which apparently pre-dates it!
    //
    // Anyway, I'll refer you to Xor Matrix for the full details, but one result
    // we need to mention from there is as follows:
    //
    //   If i is a power of 2, and we recursively expand the recurrence relation
    //   for a[i][j], then in resulting the x-sum (where x is the transform operator e.g.
    //   A x B = B), the elements a[0][i] and a[0][(i + j) % N are the only ones
    //   that appear an odd number of times in the x-sum. (*)
    //
    // Why on earth does this help? Well, note that x is associative and commutative, so
    // we can re-order the terms of the x-sum in any way we wish.  For example, we
    // might want to group all the A's together, followed by all the B's, then the C's
    // etc.
    //
    // Note further that A is the "identity"  element for this transform that is A x X = 
    // X x A = X for any X in {A, B, C, D}. 
    //
    // Note even further that X ^ X = A for any X in {A, B, C, D}.
    //
    // The upshot of this is that, for any x-sum:
    //
    //   * We only care about the parity of the number of each of A, B, C, D.
    //   * Actually, we don't care about the number of A's, at all - being identity for x 
    //     the their presence within the x-sum contributes nothing!
    //
    //  So, for example, imagine our x-sum was something like B x C x C x A x B x D x A x C.
    //  We can re-order:
    //
    //     A x B x B x C x C x C x D
    //
    //  B x B = A:
    //
    //     A x A x C x C x C x D
    //
    //  C x C = A:
    //
    //     A x A x A x C x D
    //
    //  C X D = B:
    //
    //     A x A x A x B
    //
    //  A's are irrelevant:
    //
    //     B
    //
    //  This is exactly the same as just ignoring A's and only adding a single copy of each 
    //  element that occurs an odd number of times (that is, C and D) in the x-sum to a simplified x-sum:
    //
    //    C x D
    //
    //  which again is equal to B.
    //
    //  Thus, for any a[i][j], if we could just calculate the parity of the number of 
    //  B's, C's and D's in the corresponding x-sum, we'd be able to easily calculate
    //  a[i][j].  But this is precisely what (*) allows us to do - starting from the
    //  first row, just "jump up" the largest power of 2 at a time that we can,
    //  computing the contents of the row from (*) and the analysis above, until
    //  we reach the desired row m.
    //  
    //  And that's it!
    int N, K;
    cin >> N >> K;

    string proteinString;
    cin >> proteinString;

    const auto solution = calcRowOfMatrix(proteinString, N, K);
    cout << solution << endl;
}
