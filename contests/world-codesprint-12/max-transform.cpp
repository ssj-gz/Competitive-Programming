#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

vector<int> maxTransform(const vector<int>& A)
{
    vector<int> result;

    for (int k = 0; k < A.size(); k++)
    {
        for (int i = 0; i < A.size() - k; i++)
        {
            const int j = i + k;
            int maxInRange = 0;
            for (int l = i; l <= j; l++)
            {
                maxInRange = max(maxInRange, A[l]);
            }
            result.push_back(maxInRange);
        }

    }

    return result;
}

vector<int> computeLeftCBruteForce(const vector<int>& A)
{
    vector<int> results(A.size());
    for (int i = 0; i < A.size(); i++)
    {
        int leftCPos = -1;
        for (int j = 0; j < i; j++)
        {
            if (A[j] > A[i])
            {
                leftCPos = j + 1;
            }
        }
        if (leftCPos != -1)
            results[i] = i - leftCPos;
        else
            results[i] = -1;
    }
    return results;
}

vector<int> computeRightCBruteForce(const vector<int>& A)
{
    vector<int> results(A.size());
    for (int i = 0; i < A.size(); i++)
    {
        int rightCPos = -1;
        for (int j = A.size() - 1; j > i; j--)
        {
            if (A[j] > A[i])
            {
                rightCPos = j - 1;
            }
        }
        if (rightCPos != -1)
            results[i] = rightCPos - i;
        else
            results[i] = -1;
    }
    return results;
}


int main()
{
    int n;
    cin >> n;

    vector<int> A(n);

    for (int i = 0; i < n; i++)
    {
        cin >> A[i];
    }

    const auto maxTransformA = maxTransform(A);
#if 1
    cout << "maxTransformA: " << endl;
    for (const auto x : maxTransformA)
    {
        cout << x << " ";
    }
    cout << endl;
#endif

    const auto maxTransformMaxTransformA = maxTransform(maxTransformA);
    int64_t sum = 0;
    for (const auto x : maxTransformMaxTransformA)
    {
        sum += x;
    }
#if 1
    cout << "maxTransformMaxTransformA: " << endl;
    for (const auto x : maxTransformMaxTransformA)
    {
        cout << x << " ";
    }
    cout << endl;
#endif

    const auto leftC = computeLeftCBruteForce(A);
    for (int i = 0; i < A.size(); i++)
    {
        cout << "i: " << i << " A[i]: " << A[i] << " leftC[i]: " << leftC[i] << endl;
    }
    const auto rightC = computeRightCBruteForce(A);
    for (int i = 0; i < A.size(); i++)
    {
        cout << "i: " << i << " A[i]: " << A[i] << " rightC[i]: " << rightC[i] << endl;
    }


    cout << sum << endl;

}

