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
#if 0
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
#if 0
    cout << "maxTransformMaxTransformA: " << endl;
    for (const auto x : maxTransformMaxTransformA)
    {
        cout << x << " ";
    }
    cout << endl;
#endif


    cout << sum << endl;

}

