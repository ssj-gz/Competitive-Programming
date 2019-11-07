// Simon St James (ssjgz) - 2019-11-06
// 
// Solution to: https://www.codechef.com/NOV19B/problems/CAMC
//
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
//#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}


#if 1
int64_t solveBruteForce(int N, int M, const vector<int64_t>& a)
{
    int64_t minDistance = std::numeric_limits<int64_t>::max();
    vector<int> boxIndices(M);
    for (int choiceIndex = 0; choiceIndex < M; choiceIndex++)
    {
        boxIndices[choiceIndex] = choiceIndex;
    }
    while (true)
    {
        // Test this set of chosen indices.
        vector<bool> isColourUsed(M, false);
        bool isValidChoice = true;
        for (int i = 0; i < M; i++)
        {
            if (isColourUsed[boxIndices[i] % M])
                isValidChoice = false;

            isColourUsed[boxIndices[i] % M] = true;
        }
#if 0
        string s(N, '.');
        for (int i = 0; i < M; i++)
        {
            s[boxIndices[i]] = 'X';
        }
        cout << s << " " << isValidChoice << endl;
#endif

        if (isValidChoice)
        {
            int64_t largestElement = -1;
            int64_t smallestElement = std::numeric_limits<int64_t>::max();

            for (int i = 0; i < M; i++)
            {
                const auto chosenValue = a[boxIndices[i]];
                if (chosenValue > largestElement)
                    largestElement = chosenValue;
                if (chosenValue < smallestElement)
                    smallestElement = chosenValue;
            }
            //cout << "largestElement: " << largestElement << " smallestElement: " << smallestElement << endl;

            minDistance = min(minDistance, largestElement - smallestElement);
        }

        // Next choice of M boxes.
        int index = M - 1;
        while (index >= 0 && boxIndices[index] == N - 1 - (M - 1 - index))
        {
            index--;
        }
        if (index < 0)
            break;
        int nextChoice = boxIndices[index] + 1;
        for (int i = index; i < M; i++)
        {
            boxIndices[i] = nextChoice;
            nextChoice++;
        }

    }


    return minDistance;
}
#endif



#if 1
int64_t solveOptimised(int N, int M, const vector<int64_t>& a)
{
    // Ok - if c1, c2, ... , cm are the colours, then any
    // valid colouring is of the form:
    //
    // π(c1)π(c2)...π(cm)π(c1)π(c2)...π(cm)...π(c1)π(c2)...
    //
    // where π is a permutation of c1, c2, ... , cm i.e.
    // is it some permutation of c1, c2, ... , cm.
    //
    // Similarly, a choice is valid for such a π if and only if
    // it is valid for the colouring:
    //
    // c1c2...cmc1c2...cm...c1c2...
    //
    // The colours at i and j are different if and only if i != j mod m,
    // so we merely need to find, for each k ... 1, 2, ..., m, the 
    // maximum a[i] such that i mod m = k and the minimum a[j] such that
    // j mod m != k.
    // TODO - rest of documentation, etc.

    struct ValueAndColour
    {
        int64_t value = -1; 
        int colour = -1; 
        int indexOfNextDifferentColour = -1;
    };

    vector<ValueAndColour> sortedValuesWithColour;
    for (int i = 0; i < N; i++)
    {
        sortedValuesWithColour.push_back({a[i], i % M});
    }
    sort(sortedValuesWithColour.begin(), sortedValuesWithColour.end(), [](const auto& lhs, const auto& rhs) { return lhs.value < rhs.value; });

    int previousColour = -1;
    int numInRun = 0;
    for (int i = N - 1; i >= 0; i--)
    {
        if (sortedValuesWithColour[i].colour != previousColour)
        {
            numInRun = 0;
            previousColour = sortedValuesWithColour[i].colour;
        }
        numInRun++;
        sortedValuesWithColour[i].indexOfNextDifferentColour = (i + numInRun == N ? -1 : i + numInRun);

    }

#if 0
    cout << "sortedValuesWithColour: " << endl;
    for (const auto x : sortedValuesWithColour)
    {
        cout << "{" << x.value << ", " << x.colour <<  ", " << x.indexOfNextDifferentColour << " } ";
    }
    cout << endl;
#endif

    //result = min(result, minThing(sortedValuesWithColour, M));

    int64_t result = std::numeric_limits<int64_t>::max();

    int rightIndex = 0;
    vector<int> numOfColourInRange(M, 0);
    numOfColourInRange[sortedValuesWithColour[0].colour]++;
    int numColoursInRange = 1;
    for (int leftIndex = 0; leftIndex < N; leftIndex++)
    {
        //cout << "Begin loop; leftIndex: " << leftIndex << " numColoursInRange: " << numColoursInRange << endl;
        //numOfColourInRange[sortedValuesWithColour[leftIndex].colour]++;
        //if (numOfColourInRange[sortedValuesWithColour[leftIndex].colour] == 1)
        //numColoursInRange++;
        //cout << "updated leftIndex colour:  numColoursInRange: " << numColoursInRange << endl;
#if 0
        cout << "numOfColourInRange: " << endl;
        for (const auto x : numOfColourInRange)
        {
            cout << " " << x;
        }
        cout << endl;
#endif

        while (numColoursInRange < M && rightIndex < N)
        {
            rightIndex++;
            if (rightIndex < N)
            {
                numOfColourInRange[sortedValuesWithColour[rightIndex].colour]++;
                if (numOfColourInRange[sortedValuesWithColour[rightIndex].colour] == 1)
                    numColoursInRange++;
            }
            //cout << " rightIndex: " << rightIndex << " numColoursInRange: " << numColoursInRange << endl;
        }

        //cout << "leftIndex: " << leftIndex << " rightIndex: " << rightIndex << endl;
        //cout << "numOfColourInRange: " << endl;
#if 0
        for (const auto x : numOfColourInRange)
        {
            cout << " " << x;
        }
        cout << endl;
#endif

        if (numColoursInRange != M)
            break;
        assert(rightIndex > leftIndex);

        int newRightIndex = rightIndex;
        while (newRightIndex < N && sortedValuesWithColour[newRightIndex].colour == sortedValuesWithColour[leftIndex].colour)
        {
            newRightIndex++;
        }

        if (newRightIndex != N)
        {
            assert(newRightIndex == rightIndex ||  sortedValuesWithColour[rightIndex].indexOfNextDifferentColour == newRightIndex);
            //cout << "Best for leftIndex: " << sortedValuesWithColour[newRightIndex].value - sortedValuesWithColour[leftIndex].value << endl;
            result = min(result, sortedValuesWithColour[newRightIndex].value - sortedValuesWithColour[leftIndex].value);
        }
        else
        {
            assert(newRightIndex == rightIndex ||sortedValuesWithColour[rightIndex].indexOfNextDifferentColour == -1);
        }


        numOfColourInRange[sortedValuesWithColour[leftIndex].colour]--;
        if (numOfColourInRange[sortedValuesWithColour[leftIndex].colour] == 0)
            numColoursInRange--;
#if 0
        cout << "Dropped leftIndex: " << leftIndex << " numColoursInRange: " << numColoursInRange << endl;
        cout << "numOfColourInRange: " << endl;
        for (const auto x : numOfColourInRange)
        {
            cout << " " << x;
        }
        cout << endl;
#endif

    }



    return result;
}
#endif


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        // TODO - generate randomised test.
        //const int T = rand() % 100 + 1;
        const int T = 1;
        cout << T << endl;

        for (int t = 0; t < T; t++)
        {
            const int N = 2 + rand() % 25;
            const int M = 2 + rand() % (N - 1);
            assert(M <= N);
            const int maxA = 1 + rand() % 100;

            cout << N << " " << M << endl;

            for (int i = 0; i < N; i++)
            {
                cout << (1 + rand() % maxA) << " ";
            }
        }

        return 0;
    }
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();
        const int M = read<int>();

        vector<int64_t> a(N);
        for (auto& x : a)
            x = read<int64_t>();

#ifdef BRUTE_FORCE
#if 1
        const auto solutionBruteForce = solveBruteForce(N, M, a);
        cout << "solutionBruteForce: " << solutionBruteForce << endl;
#endif
#if 1
        const auto solutionOptimised = solveOptimised(N, M, a);
        cout << "solutionOptimised:  " << solutionOptimised << endl;

        assert(solutionOptimised == solutionBruteForce);
#endif
#else
        const auto solutionOptimised = solveOptimised(N, M, a);
        cout << solutionOptimised << endl;
#endif
    }

    assert(cin);
}
