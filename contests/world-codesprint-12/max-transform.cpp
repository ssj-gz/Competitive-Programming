#define VERIFY
//#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#undef VERIFY
#endif
#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>

using namespace std;

constexpr int64_t mod = 1'000'000'007UL;

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

class ModNum
{
    public:
        ModNum(int64_t n)
            : m_n{n}
        {
        }
        ModNum& operator+=(const ModNum& other)
        {
            m_n = (m_n + other.m_n) % ::mod;
            return *this;
        }
        ModNum& operator-=(const ModNum& other)
        {
            m_n = (::mod + m_n - other.m_n) % ::mod;
            return *this;
        }
        ModNum& operator*=(const ModNum& other)
        {
            m_n = (m_n * other.m_n) % ::mod;
            return *this;
        }
        int64_t value() const { return m_n; };
    private:
        int64_t m_n;
};

ModNum operator+(const ModNum& lhs, const ModNum& rhs)
{
    ModNum result(lhs);
    result += rhs;
    return result;
}

ModNum operator-(const ModNum& lhs, const ModNum& rhs)
{
    ModNum result(lhs);
    result -= rhs;
    return result;
}

ModNum operator*(const ModNum& lhs, const ModNum& rhs)
{
    ModNum result(lhs);
    result *= rhs;
    return result;
}

ostream& operator<<(ostream& os, const ModNum& toPrint)
{
    os << toPrint.value();
    return os;
}

int64_t quickPower(int64_t n, int64_t k, int64_t mod)
{
    int64_t result = 1;
    int64_t power = 0;
    while (k > 0)
    {
        if (k & 1)
        {
            int64_t subResult = n;
            for (int64_t i = 0; i < power; i++)
            {
                subResult = (subResult * subResult) % mod;
            }
            result = (result * subResult) % mod;
        }
        k >>= 1;
        power++;
    }
    return result;
}


int64_t sumUpTo(int64_t n, int64_t mod)
{
    const int64_t oneHalfModMod = quickPower(2, mod - 2, mod);
    assert((oneHalfModMod * 2) % mod == 1);
    int64_t result = n + 1;
    result %= mod;
    result = (result * (n)) % mod;
    result = (result * oneHalfModMod) % mod;
    return result;
}

vector<int> computeLeftCBruteForce(const vector<int>& A)
{
    vector<int> results(A.size());
    for (int i = 0; i < A.size(); i++)
    {
        int leftCLTPos = -1;
        for (int j = 0; j < i; j++)
        {
            if (A[j] > A[i])
            {
                leftCLTPos = j + 1;
            }
        }
        if (leftCLTPos != -1)
            results[i] = i - leftCLTPos;
        else
            results[i] = -1;
    }
    return results;
}

vector<int> computeLeftCLTBruteForce(const vector<int>& A)
{
    vector<int> results(A.size());
    for (int i = 0; i < A.size(); i++)
    {
        int leftCLTPos = -1;
        for (int j = 0; j < i; j++)
        {
            if (A[j] >= A[i])
            {
                leftCLTPos = j + 1;
            }
        }
        if (leftCLTPos != -1)
            results[i] = i - leftCLTPos;
        else
            results[i] = -1;
    }
    return results;
}

vector<int> computeRightMostCLEBruteForce(const vector<int>& A)
{
    vector<int> results(A.size());
    for (int i = 0; i < A.size(); i++)
    {
        int rightMostCLEPos = -1;
        for (int j = A.size() - 1;  j >=0; j--)
        {
            if (A[j] >= A[i])
            {
                rightMostCLEPos = j;
                break;
            }
        }
        assert(rightMostCLEPos != -1);
        results[i] = rightMostCLEPos;
        cout << "rightMostCLEPos for i: " << i << " = " << rightMostCLEPos << endl;
    }
    return results;
}

vector<int> computeRightCBruteForce(const vector<int>& A)
{
    vector<int> results(A.size());
    for (int i = 0; i < A.size(); i++)
    {
        int rightCLEPos = -1;
        for (int j = A.size() - 1; j > i; j--)
        {
            if (A[j] > A[i])
            {
                rightCLEPos = j - 1;
            }
        }
        if (rightCLEPos != -1)
            results[i] = rightCLEPos - i;
        else
            results[i] = -1;
    }
    return results;
}

int64_t findNumOccurrencesBruteForce2(int64_t l, int64_t m, int64_t r)
{
    assert(l <= r);
#if 0
    if (r < l)
    {
        assert(false);
        swap(l, r);
    }
#endif
    l %= mod;
    r %= mod;
    ModNum numOccurrences = 0;
    numOccurrences += ModNum(l + 1) * m + sumUpTo(l, mod);
    numOccurrences += ModNum(r - l) * (l + m);
    numOccurrences += ModNum(l + m) * (ModNum(l + m + r));
    numOccurrences -= ModNum(sumUpTo(l + m + r, mod)) - ModNum(sumUpTo(r, mod));
#ifdef VERIFY
    {
        int64_t dbgNumOccurrences = 0;
        for (int k = 0; k < 1000; k++)
        {
            int64_t blee = 0;
            if (k <= l)
            {
                blee = k + m;
            }
            else if (k >= l && k <= r)
            {
                blee = l + m;
            }
            else if (k > r && k <= l + m + r)
                blee = l + m - (k - r);
            else if (k > l + m + r)
                blee = 0;

            cout << "  findNumOccurrencesBruteForce2 l: " << l << " m: " << m << " r: " << r << " k: " << k << " = " << blee << endl;

            dbgNumOccurrences += blee;

            if (blee == 0)
                break;
        }
        cout << " findNumOccurrencesBruteForce2 l : " << l << " m: " << m << " r: " << r << " numOccurrences: " << numOccurrences << " dbgNumOccurrences: " << dbgNumOccurrences << endl;
        assert(dbgNumOccurrences == numOccurrences.value());
    }
#endif
    return numOccurrences.value();
}

int64_t findNumOccurrencesBruteForce(int index, const vector<int>& A, const vector<int>& leftCLT, const vector<int>& rightCLE, const vector<int>& rightMostCLTPos)
{
    cout << "numOccurrences for index: " << index << endl;
    int64_t numOccurrences = 0;
    if (leftCLT[index] != -1 && rightCLE[index] != -1)
    {
        int l = leftCLT[index];
        int r = rightCLE[index];
        bool swappedLR = false;
        if (r < l)
        {
            swappedLR = true;
            swap(l, r);
        }
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
    }
    else if (leftCLT[index] == -1 && rightCLE[index] != -1)
    {
        // TODO - k == 0
        for (int64_t k = 1; k < A.size(); k++)
        {
            //const int clearToLeftThisK = index - k;
            const int clearToLeftThisK = max(int64_t(0), index - k);
            //const int rightmostToEndLastK = max(int64_t(0), int64_t(A.size()) - rightMostCLTPos[index] - (k));
            const int rightmostToEndLastK = A.size() - rightMostCLTPos[index] - k;
            //const int left = (clearToLeftThisK >= 0 ? clearToLeftThisK + rightmostToEndLastK : 0);
            const int left = clearToLeftThisK + rightmostToEndLastK;
            int numInA = -1;
            int l = max(0, index) ;
            int r = rightCLE[index];
            if (r < l)
                swap(l, r);
            if (k <= l)
                numInA = k + 1;
            else if (k >= l && k <= r)
                numInA = l + 1;
            else if (k > r && k <= l + 1 + r)
                numInA = l + 1 - (k - r);
            else if (k > l + 1 + r)
                numInA = 0;

            int leftTransformA = max(l - k + max(rightmostToEndLastK, 0), int64_t(0));
            int rightTransformA = max(r - k, int64_t(0));
            if (rightTransformA < leftTransformA)
                swap(leftTransformA, rightTransformA);
            const auto blee = findNumOccurrencesBruteForce2(leftTransformA, numInA, rightTransformA);
            cout << " k: " << k << " index: " << index << " left: " << left << " clearToLeftThisK: " << clearToLeftThisK << " rightmostToEndLastK: " << rightmostToEndLastK << " numInA: " << numInA << endl;

            numOccurrences += blee;
            cout << " numOccurrences in maxTransformMaxTransformA for index " << index << " k: " << k << " = " << blee << endl;
        }
        //assert(false && "Unhandled");
    }
    else if (leftCLT[index] != -1 && rightCLE[index] == -1)
    {
        assert(false && "Unhandled");
    }
    else if (leftCLT[index] == -1 && rightCLE[index] == -1)
    {
        assert(false && "Unhandled");
    }
    else
    {
        assert(false);
    }
    cout << "Total occurrences for index " << index << " = " << numOccurrences << endl;
    return numOccurrences;
}


int main()
{
#if 0
    {
        while (true)
        {
            const int m = rand() % 20 + 1;
            int l = rand() % 20;
            int r = rand() % 20;
            if (r < l)
                swap(r, l);
            findNumOccurrencesBruteForce2(l, m, r);
        }


    }
#endif
    cout << "sumUpTo 5: " << sumUpTo(5, mod) << endl;
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

    const auto leftCLT = computeLeftCLTBruteForce(A);
    for (int i = 0; i < A.size(); i++)
    {
        cout << "i: " << i << " A[i]: " << A[i] << " leftCLT[i]: " << leftCLT[i] << endl;
    }
    const auto rightCLE = computeRightCBruteForce(A);
    for (int i = 0; i < A.size(); i++)
    {
        cout << "i: " << i << " A[i]: " << A[i] << " rightCLE[i]: " << rightCLE[i] << endl;
    }
    const auto rightMostCLEPos = computeRightMostCLEBruteForce(A);


    cout << sum << endl;

    const int trackNumber = 2;

    int indexOf2 = -1;
    int64_t occurrencesOpt = 0;
    for (int i = 0; i < A.size(); i++)
    {
        if (A[i] == trackNumber)
        {
            occurrencesOpt += findNumOccurrencesBruteForce(i, A, leftCLT, rightCLE, rightMostCLEPos);
        }
    }
    int realOccurrences = 0;
    for (const auto x : maxTransformMaxTransformA)
    {
        if (x == trackNumber)
            realOccurrences++;
    }
    cout << "real occurrences of " << trackNumber << " in maxTransformMaxTransformA: " << realOccurrences << " occurrencesOpt: " << occurrencesOpt << endl;

    for (int k = 0; k < A.size(); k++)
    {
        cout << "k: " << k << endl;
        //for (int i = 0; i < A.size(); i++)
        const int i = indexOf2;
        {
            //const auto blee = min(k, rightCLE[i] - leftCLT[i]) - min(leftCLT[i] - 1, k);
            //const auto blee = min(k, rightCLE[i]) - max(-k, -leftCLT[i]);
            //const auto blee = min(k, rightCLE[i] - k) + min(k, leftCLT[i]);
            const auto blee = max(0, min(0, rightCLE[i] - k) + min(k, leftCLT[i]) + 1);
            //const auto bloo = min(leftCLT[i], rightCLE[i]) + min(k, abs(leftCLT[i] - rightCLE[i]));
            int bloo = 0;
            int l = leftCLT[i];
            int r = rightCLE[i];
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
            cout << " i: " << i << " A[i]: " << A[i] << " leftCLT[i]: " << leftCLT[i] << " rightCLE[i]: " << rightCLE[i] << " blee: " << blee << " bloo: " << bloo << endl;
        }
    }


}

