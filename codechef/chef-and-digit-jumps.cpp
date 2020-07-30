// Simon St James (ssjgz) - 2020-07-30
// 
// Solution to: https://www.codechef.com/LRNDSA08/problems/DIGJUMP
//
#include <iostream>
#include <vector>

#include <cassert>

using namespace std;

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

int findMinJumpsFromFirstTolastIndex(const vector<int>& digits)
{
    const int digitsLength = digits.size();

    vector<int> minDistToDigitIndex(digitsLength, -1);
    // The "unvisitedIndicesWithDigit" mechanism allows us to shrink
    // the runtime to O(N).
    vector<vector<int>> unvisitedIndicesWithDigit(10);
    for (int index = 0; index < digitsLength; index++)
    {
        unvisitedIndicesWithDigit[digits[index]].push_back(index);
    }

    vector<int> indicesToExplore;
    indicesToExplore.push_back(0);
    minDistToDigitIndex[0] = 0;

    int numSteps = 1;

    while (!indicesToExplore.empty())
    {
        // A bit of thought tells us that this loop 
        // will only run a small number of times - this is because
        // we can't go more than two steps without visiting an index
        // with a so-far-unvisited digit, and there are only 10 possible 
        // digits.
        // The worst case I can think of occurs with the testcase:
        //
        //   00112233445566778899
        //
        // and this takes 19 steps.
        vector<int> nextIndicesToExplore;

        auto visitIndex = [&](int index)
        {
            if (index < 0 || index >= digitsLength)
                return;
            if (minDistToDigitIndex[index] != -1)
                return;
            minDistToDigitIndex[index] = numSteps;
            nextIndicesToExplore.push_back(index);
            // NB: doesn't update unvisitedIndicesWithDigit, but this
            // is harmless.
        };

        for (const auto index : indicesToExplore)
        {
            visitIndex(index - 1);
            visitIndex(index + 1);
            const auto digitAtIndex = digits[index];
            for (const auto unvisitedIndexWithThisDigit : unvisitedIndicesWithDigit[digitAtIndex])
            {
                visitIndex(unvisitedIndexWithThisDigit);
            }
            unvisitedIndicesWithDigit[digitAtIndex].clear(); // We've just visited them all! If we didn't clear this, 
                                                             // the algorithm would end up as O(N^2).
        }

        numSteps++;
        indicesToExplore = nextIndicesToExplore;
    }
    
    return minDistToDigitIndex[digitsLength - 1];
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        string digitString;
        const auto digitsLength = 1 + rand() % 100'000;
        for (int i = 0; i < digitsLength; i++)
        {
            digitString += '0' + (rand() % 10);
        }
        cout << digitString << endl;

        return 0;
    }


    const auto digitString = read<string>();
    vector<int> digits;
    for (const auto& digit : digitString)
    {
        digits.push_back(digit - '0');
    }
    cout << findMinJumpsFromFirstTolastIndex(digits) << endl;
    assert(cin);
}
