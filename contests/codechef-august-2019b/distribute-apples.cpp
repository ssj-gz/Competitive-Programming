// Simon St James (ssjgz) - 2019-08-03
#include <iostream>
#include <vector>
#include <algorithm>

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

bool calcCanBoxContentsDiffer(int64_t numApples, int64_t numBoxes)
{
    assert((numApples % numBoxes) == 0);
    assert(numApples >= numBoxes);
    // After all apples are packed by Candidate1, each box
    // will have the same number (numApples / numBoxes) of
    // apples in it.
    //
    // For candidate 2, things are a little trickier: after the first
    // minute, one of the boxes will have K (= numBoxes) apples in it; the remaining
    // K - 1 will not i.e. boxes have different amounts in them (assuming K > 1).
    // 
    // After the second minute, two boxes will each have K apples in them; the
    // remaining K - 2 will not - still uneven.
    //
    // After K minutes, all boxes will have the same number - K - of apples in them.
    //
    // In general, every K minutes we package K x K apples and end up with an 
    // equal number of apples in each box.  Thus, the boxes all contain
    // an equal number of apples if and only if we have packaged a multiple of K x K
    // apples.
    //
    // Thus, if numApples is not a multiple of numBoxes x numBoxes, then we will have an
    // uneven number of apples in the boxes and, since Candidate 1 packages them evenly,
    // Candidate 2's results are different from Candidate 1.
    //
    // If numApples *is* a multiple of numBoxes x numBoxes, then each box will have
    //
    //   numApples / (numBoxes x numBoxes) * numBoxes = numApples / numBoxes 
    //
    // apples in them - the same as Candidate 1! 
    // 
    // That is - Candidate 1 and Candidate 2 end up with different results if and only if
    // numApples is *not* divisible by numBoxes x numBoxes.
    if ((numApples / numBoxes) % numBoxes == 0) // Calculating numBoxes x numBoxes will likely overflow, so be a bit smarter :)
    {
        // Have an equal number of apples in each box - in fact,
        // numApples / numBoxes of them, just like Candidate1!
        // Box contents cannot differ.
        return false;
    }
    else
    {
        // Have an unequal number of apples in each box - must
        // differ from Candidate1, all of whose boxes contain
        // the same number of apples.
        return true;
    }
}

int main(int argc, char* argv[])
{
    // Easy - see calcCanBoxContentsDiffer for full details.
    const int T = read<int>();
    for (int t = 0; t < T; t++)
    {
        const int64_t N = read<int64_t>();
        const int64_t K = read<int64_t>();

        const auto canBoxContentsDiffer = calcCanBoxContentsDiffer(N, K);
        cout << (canBoxContentsDiffer ? "YES" : "NO") << endl;
    }
}

