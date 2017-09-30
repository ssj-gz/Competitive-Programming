// Simon St James (ssjgz) 2017-09-30
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


    std::chrono::steady_clock::time_point inputBegin = std::chrono::steady_clock::now();
    int N;
    cin >> N;
    vector<int> A(N);

    for (int i = 0; i < N; i++)
    {
        cin >> A[i];
    }
    std::chrono::steady_clock::time_point inputEnd = std::chrono::steady_clock::now();
    std::cout << "Input took (ms) " << std::chrono::duration_cast<std::chrono::milliseconds>(inputEnd - inputBegin).count() <<std::endl;

    
    std::chrono::steady_clock::time_point sortBegin = std::chrono::steady_clock::now();
    sort(A.begin(), A.end());
    std::chrono::steady_clock::time_point sortEnd = std::chrono::steady_clock::now();
    std::cout << "sort took (ms) " << std::chrono::duration_cast<std::chrono::milliseconds>(sortEnd - sortBegin).count() <<std::endl;

    const int smallestElement = *min_element(A.begin(), A.end());
    const int largestElement = *max_element(A.begin(), A.end());
    cout << "smallestElement: " << smallestElement << " largestElement: " << largestElement << endl;

    std::chrono::steady_clock::time_point lookupBegin = std::chrono::steady_clock::now();
    // absoluteSumLookup[i] = absoluteSumBruteForce(A, i - largestElement).
    vector<int64_t> absoluteSumLookup(largestElement - smallestElement + 1);
    int64_t currentSum = 0;
    for (auto& value : A)
    {
        value -= largestElement;
        //cout << "abs(value) : " << abs(value) << endl;
        currentSum += abs(value);
    }
    //cout << "reduced A:" << endl;
    for (const auto& value : A)
    {
        //cout << value << " ";
    }
    cout  << endl;
    int firstPositiveIndex = N; // One-past-the-end i.e. there are no positive values.
    for (int i = 0; i <= largestElement - smallestElement; i++)
    {
        //assert(i + largestElement >= 0 && i + largestElement < absoluteSumLookup.size());
        absoluteSumLookup[i] = currentSum;
        //cout << "currentSum: " << currentSum << " bruteForce(A, i): " << bruteForce(A, i) << endl;
        assert(absoluteSumLookup[i] == bruteForce(A, i));
        while (firstPositiveIndex > 0 && A[firstPositiveIndex - 1] + i >= 0)
        {
            firstPositiveIndex--;
        }
        const int numPositive = N - firstPositiveIndex;
        currentSum += numPositive;
        currentSum -= (N - numPositive);
    }
    std::chrono::steady_clock::time_point lookupEnd = std::chrono::steady_clock::now();
    std::cout << "lookup took (ms) " << std::chrono::duration_cast<std::chrono::milliseconds>(lookupEnd - lookupBegin).count() <<std::endl;

    int Q;
    cin >> Q;

    int64_t totalX = 0; 
#if 1
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
            cout << "totalX > -smallestElement" << endl;
            absoluteSum = absoluteSumLookup.back() + N * (totalX - -smallestElement);
        }
        else if (totalX < -largestElement)
        {
            cout << "totalX < -largestElement" << endl;
            absoluteSum = absoluteSumLookup.front() + N * (-largestElement - totalX);
        }
        else
        {
            assert(false);
        }

        cout << absoluteSum << endl;
#define BRUTE_FORCE
#ifdef BRUTE_FORCE
        const auto absoluteSumBruteForce = bruteForce(A, totalX + largestElement);
        cout << "totalX: " << totalX << " absoluteSumBruteForce: " << absoluteSumBruteForce << " absoluteSum: " << absoluteSum << endl;
        assert(absoluteSum == absoluteSumBruteForce);
#endif
    }
}
