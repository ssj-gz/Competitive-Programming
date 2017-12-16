#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>

using namespace std;

vector<int> maxTransform(const vector<int>& A)
{
    vector<int> result;

    for (int k = 0; k < A.size(); k++)
    {
        cout << " |(" << k << ") ";
        for (int i = 0; i < A.size() - k; i++)
        {
            const int j = i + k;
            int maxInRange = 0;
            int indexOfMaxInRange = -1;
            for (int l = i; l <= j; l++)
            {
                if (A[l] > maxInRange)
                {
                    maxInRange = A[l];
                    indexOfMaxInRange = l;
                }
            }
            result.push_back(maxInRange);
            cout << maxInRange << 
#if 0
                "[" << indexOfMaxInRange << "]"  <<
#endif
                 " ";
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

int64_t findNumOccurrencesBruteForce2(int64_t l, int64_t m, int64_t r)
{
    if (r < l)
        swap(l, r);
    int64_t numOccurrences = 0;
    for (int k = 0; k < 1000; k++)
    {
        int64_t blee = 0;
        if (k <= l)
            blee = k + m;
        else if (k >= l && k <= r)
            blee = l + m;
        else if (k > r && k <= l + m + r)
            blee = l + m - (k - r);
        else if (k > l + m + r)
            blee = 0;

        cout << "  findNumOccurrencesBruteForce2 l: " << l << " m: " << m << " r: " << r << " k: " << k << " = " << blee << endl;

        numOccurrences += blee;

        if (blee == 0)
            break;
    }
    return numOccurrences;
}

int64_t findNumOccurrencesBruteForce(int index, const vector<int>& A, const vector<int>& leftC, const vector<int>& rightC)
{
    cout << "numOccurrences for index: " << index << endl;
    int l = leftC[index];
    int r = rightC[index];
    bool swappedLR = false;
    if (r < l)
    {
        swappedLR = true;
        swap(l, r);
    }
    int64_t numOccurrences = 0;
    for (int k = 0; k < A.size(); k++)
    {
        int leftTransformA = -1;
        int rightTransformA = -1;
        int numInA = -1;
        if (k <= l)
            numInA = k + 1;
        else if (k >= l && k <= r)
            numInA = l + 1;
        else if (k > r && k <= l + 1 + r)
            numInA = l + 1 - (k - r);
        else if (k > l + 1 + r)
            numInA = 0;

        leftTransformA = max(l - k, 0);
        rightTransformA = max(r - k, 0);
        cout << " k: " << k << " leftTransformA: " << leftTransformA << " rightTransformA: " << rightTransformA << " numInA: " << numInA << " swappedLR: " << swappedLR << endl;
        const auto blee = findNumOccurrencesBruteForce2(leftTransformA, numInA, rightTransformA);

        numOccurrences += blee;
        cout << " numOccurrences in maxTransformMaxTransformA for index " << index << " k: " << k << " = " << blee << endl;

    }
    cout << "Total occurrences for index " << index << " = " << numOccurrences << endl;
    return numOccurrences;
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
    cout << endl;
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

    const int trackNumber = 2;

    int indexOf2 = -1;
    for (int i = 0; i < A.size(); i++)
    {
        if (A[i] == trackNumber)
        {
            findNumOccurrencesBruteForce(i, A, leftC, rightC);
            break;
        }
    }
    int realOccurrences = 0;
    for (const auto x : maxTransformMaxTransformA)
    {
        if (x == trackNumber)
            realOccurrences++;
    }
    cout << "real occurrences of " << trackNumber << " in maxTransformMaxTransformA: " << realOccurrences << endl;

    for (int k = 0; k < A.size(); k++)
    {
        cout << "k: " << k << endl;
        //for (int i = 0; i < A.size(); i++)
        const int i = indexOf2;
        {
            //const auto blee = min(k, rightC[i] - leftC[i]) - min(leftC[i] - 1, k);
            //const auto blee = min(k, rightC[i]) - max(-k, -leftC[i]);
            //const auto blee = min(k, rightC[i] - k) + min(k, leftC[i]);
            const auto blee = max(0, min(0, rightC[i] - k) + min(k, leftC[i]) + 1);
            //const auto bloo = min(leftC[i], rightC[i]) + min(k, abs(leftC[i] - rightC[i]));
            int bloo = 0;
            int l = leftC[i];
            int r = rightC[i];
            if (r < l)
                swap(l, r);
            if (k <= l)
                bloo = k + 1;
            else if (k >= l && k <= r)
                bloo = l + 1;
            else if (k > r && k <= l + 1 + r)
                bloo = l + 1 - (k - r);
            else if (k > l + 1 + r)
                bloo = 0;
            //if (bloo < 0)
                //bloo = 0;
            assert(bloo == blee);
            cout << " i: " << i << " A[i]: " << A[i] << " leftC[i]: " << leftC[i] << " rightC[i]: " << rightC[i] << " blee: " << blee << " bloo: " << bloo << endl;
        }
    }


}

