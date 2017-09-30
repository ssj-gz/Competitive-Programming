// Simon St James (ssjgz) 2017-09-30 13:15
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#define NDEBUG
#undef BRUTE_FORCE
#endif
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <cassert>

using namespace std;

int64_t bruteForce(const vector<int>& A, int totalX)
{
    int64_t absoluteSum = 0;
    for (const auto value : A)
    {
        absoluteSum += abs(value + totalX);
    }
    return absoluteSum;
}

int main()
{

    if (false)
    {
        // Generate a huge testcase.
        const int N = 500000;
        const int Q = 500000;

        cout << N << endl;
        for (int i = 0; i < N; i++)
        {
            cout << (rand() % 4000 - 2000) << " ";
        }
        cout << endl;
        cout << Q << endl;

        for (int i = 0; i < Q; i++)
        {
            cout << (rand() % 4000 - 2000) << " ";
        }
        return 0;
    }


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

    vector<int64_t> absoluteSumLookup(largestElement - smallestElement + 1);
    int64_t currentSum = 0;
    for (auto& value : A)
    {
        value -= largestElement;
        currentSum += abs(value);
    }

    int firstNonNegativeIndex = N - 1; // The last element is 0.
    for (int i = 0; i <= largestElement - smallestElement; i++)
    {
        absoluteSumLookup[i] = currentSum;
        assert(absoluteSumLookup[i] == bruteForce(A, i));
        while (firstNonNegativeIndex > 0 && A[firstNonNegativeIndex - 1] + i >= 0)
        {
            firstNonNegativeIndex--;
        }
        const int numNonNegative = N - firstNonNegativeIndex;
        currentSum += numNonNegative;
        const int numNegative = N - numNonNegative;
        currentSum -= numNegative;
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
        if (totalX >= -largestElement && totalX <= -smallestElement)
        {
            absoluteSum = absoluteSumLookup[totalX + largestElement];
        }
        else if (totalX > -smallestElement)
        {
            absoluteSum = absoluteSumLookup.back() + N * (totalX - -smallestElement);
        }
        else if (totalX < -largestElement)
        {
            absoluteSum = absoluteSumLookup.front() + N * (-largestElement - totalX);
        }
        else
        {
            assert(false);
        }

        cout << absoluteSum << endl;
#ifdef BRUTE_FORCE
        const auto absoluteSumBruteForce = bruteForce(A, totalX + largestElement);
        cout << "totalX: " << totalX << " absoluteSumBruteForce: " << absoluteSumBruteForce << " absoluteSum: " << absoluteSum << endl;
        assert(absoluteSum == absoluteSumBruteForce);
#endif
    }
}
