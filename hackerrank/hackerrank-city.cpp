#include <iostream>
#include <vector>

using namespace std;

int main()
{
    const int64_t modulus = 1000'000'007ULL;
    int N;
    cin >> N;

    vector<int> A(N);
    for (int i = 0; i < N; i++)
    {
        cin >> A[i];
    }

    int64_t numNodes = 1;
    int64_t sumOfAllPaths = 0;
    int64_t sumOfPathsToCorner = 0;
    int64_t sumCornerToCorner = 0;


    for (int i = 0; i < N; i++)
    {
        cout << "numNodes: " << numNodes << endl;
        cout << "sumOfAllPaths: " << sumOfAllPaths << endl;
        cout << "sumOfPathsToCorner: " << sumOfPathsToCorner << endl;
        cout << "sumCornerToCorner: " << sumCornerToCorner << endl;
        int64_t newNumNodes = 4 * numNodes + 2;
        int64_t newSumOfAllPaths = 4 * sumOfAllPaths + 
            (3 * numNodes + 2 * sumOfPathsToCorner) * 4 + 6 * numNodes * sumOfPathsToCorner + 
            (3 + 2 + 3 + 3 + 2 + 3) * (A[i] * numNodes * numNodes) +
            A[i];
        newSumOfAllPaths %= modulus;
        int64_t newSumOfPathsToCorner = 3 * sumOfPathsToCorner + (8 * A[i] + sumCornerToCorner) * numNodes + 3 * A[i] + 2 * sumCornerToCorner;
        int64_t newSumCornerToCorner = 2 * sumCornerToCorner + 3 * A[i];

        cout << "newNumNodes: " << newNumNodes << endl;
        cout << "newSumOfAllPaths: " << newSumOfAllPaths << endl;
        cout << "newSumOfPathsToCorner: " << newSumOfPathsToCorner << endl;
        cout << "newSumCornerToCorner: " << newSumCornerToCorner << endl;


        numNodes = newNumNodes;
        sumOfAllPaths = newSumOfAllPaths;
        sumOfPathsToCorner = newSumOfPathsToCorner;
        sumCornerToCorner = newSumCornerToCorner;
    }

    cout << sumOfAllPaths << endl; 

}
