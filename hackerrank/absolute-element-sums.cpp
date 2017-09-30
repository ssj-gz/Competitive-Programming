#include <iostream>
#include <vector>

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
    int N;
    cin >> N;
    vector<int> A(N);

    for (int i = 0; i < N; i++)
    {
        cin >> A[i];
    }

    const int smallestElement = *min_element(A.begin(), A.end());
    const int largestElement = *max_element(A.begin(), A.end());
    cout << "smallestElement: " << smallestElement << " largestElement: " << largestElement << endl;

    vector<int> absoluteSumLookup(largestElement - smallestElement + 1);
    for (int i = -largestElement; i <= -smallestElement; i++)
    {
        assert(i + largestElement >= 0 && i + largestElement < absoluteSumLookup.size());
        absoluteSumLookup[i + largestElement] = bruteForce(A, i);
    }

    int Q;
    cin >> Q;

    int totalX = 0; 
#if 0
    for (int i = 0; i < Q; i++)
    {
        int x;
        cin >> x;

        totalX += x;
#else
    for (int i = -10000; i <= 10000; i++)
    {
        totalX = i;
#endif

        int64_t absoluteSum = 0;
        if (totalX >= -largestElement && totalX <= -smallestElement)
        {
            cout << "lookup" << endl;
            absoluteSum = absoluteSumLookup[totalX + largestElement];
        }
        else if (totalX > -smallestElement)
        {
            cout << "bigger than -smallestElement" << endl;
            absoluteSum = absoluteSumLookup.back() + N * (totalX - -smallestElement);
        }
        else if (totalX < -largestElement)
        {
            cout << "smaller than -largestElement" << endl;
            absoluteSum = absoluteSumLookup.front() + N * (-largestElement - totalX);
        }
        else
        {
            assert(false);
        }


        cout << absoluteSum << endl;
#define BRUTE_FORCE
#ifdef BRUTE_FORCE
        const auto absoluteSumBruteForce = bruteForce(A, totalX);
        cout << "absoluteSumBruteForce: " << absoluteSumBruteForce << " absoluteSum: " << absoluteSum << endl;
        assert(absoluteSum == absoluteSumBruteForce);
#endif
    }
}
