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

    int Q;
    cin >> Q;

    int totalX = 0; 
    for (int i = 0; i < Q; i++)
    {
        int x;
        cin >> x;

        totalX += x;
#define BRUTE_FORCE
#ifdef BRUTE_FORCE
        const auto absoluteSumBruteForce = bruteForce(A, totalX);
        cout << absoluteSumBruteForce << endl;
#endif
    }
}
