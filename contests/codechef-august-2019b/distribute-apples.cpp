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
    // For candidate 2, things are a little trickier.
    //
    if ((numApples / numBoxes) % numBoxes == 0)
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
    const int T = read<int>();
    for (int t = 0; t < T; t++)
    {
        const int64_t N = read<int64_t>();
        const int64_t K = read<int64_t>();

        const auto canBoxContentsDiffer = calcCanBoxContentsDiffer(N, K);
        cout << (canBoxContentsDiffer ? "YES" : "NO") << endl;
    }
}

