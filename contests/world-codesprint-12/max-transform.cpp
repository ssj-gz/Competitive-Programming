#define VERIFY
#define BRUTE_FORCE
//#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#undef VERIFY
#undef BRUTE_FORCE
#endif
#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <sys/time.h>
#include <cassert>

using namespace std; 
constexpr int64_t mod = 1'000'000'007UL;

vector<int> maxTransform(const vector<int>& A)
{
    vector<int> result;

    for (int k = 0; k < A.size(); k++)
    {
        //cout << " |(" << k << ") ";
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
#if 0
            cout << maxInRange << 
                "[" << indexOfMaxInRange << "]"  <<
                " ";
#endif
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
    if (n == 0)
        return 0;
    //cout << " sumUpTo: " << n << endl;
    const static int64_t oneHalfModMod = quickPower(2, mod - 2, mod);
    assert((oneHalfModMod * 2) % mod == 1);
    int64_t result = n + 1;
    result %= mod;
    result = (result * n) % mod;
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
        //cout << "rightMostCLEPos for i: " << i << " = " << rightMostCLEPos << endl;
    }
    return results;
}

vector<int> computeLeftMostCGTBruteForce(const vector<int>& A)
{
    vector<int> results(A.size());
    for (int i = 0; i < A.size(); i++)
    {
        int leftMostCGTPos = -1;
        for (int j = 0; j < A.size(); j++)
        {
            if (A[j] > A[i])
            {
                leftMostCGTPos = j;
                break;
            }
        }
        //assert(leftMostCGTPos != -1);
        results[i] = leftMostCGTPos;
        //cout << "leftMostCGTPos for i: " << i << " = " << leftMostCGTPos << endl;
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
    return results; } 
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
    m %= mod;
    ModNum numOccurrences = 0;
    numOccurrences += ModNum(l + 1) * m + sumUpTo(l, mod);
    numOccurrences += ModNum(r - l) * (l + m);
    numOccurrences += ModNum(l + m) * (ModNum(l + m + r));
    numOccurrences -= ModNum(sumUpTo(l + m + r, mod)) - ModNum(sumUpTo(r, mod));
#ifdef VERIFY
    {
        ModNum dbgNumOccurrences = 0;
        for (int k = 0; true ; k++)
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

            //cout << "  findNumOccurrencesBruteForce2 l: " << l << " m: " << m << " r: " << r << " k: " << k << " = " << blee << endl;

            dbgNumOccurrences += blee;

            if (blee == 0)
                break;
        }
        //cout << " findNumOccurrencesBruteForce2 l : " << l << " m: " << m << " r: " << r << " numOccurrences: " << numOccurrences << " dbgNumOccurrences: " << dbgNumOccurrences << endl;
        assert(dbgNumOccurrences.value() == numOccurrences.value());
    }
#endif
    return numOccurrences.value();
}

int64_t sizeOfTransformA = -1;

int64_t findNumOccurrencesBruteForce(int index, const vector<int>& A, const vector<int>& leftCLT, const vector<int>& rightCLE, const vector<int>& rightMostCLEPos, const vector<int>& leftMostCGTPos)
{
    //cout << "numOccurrences for index: " << index << endl;
    ModNum numOccurrences = 0;
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
            //cout << " k: " << k << " leftTransformA: " << leftTransformA << " rightTransformA: " << rightTransformA << " numInA: " << numInA << " swappedLR: " << swappedLR << endl;
            const auto blee = findNumOccurrencesBruteForce2(leftTransformA, numInA, rightTransformA);

            numOccurrences += blee;
            //cout << " numOccurrences in maxTransformMaxTransformA for index " << index << " k: " << k << " = " << blee << endl;

        }
    }
    else if (leftCLT[index] == -1 && rightCLE[index] != -1)
    {
        int l = index;
        int r = rightCLE[index];
        if (r < l)
            swap(l, r);
        // k == 0 case.
        numOccurrences += findNumOccurrencesBruteForce2(l, 1, r);
        //cout << " after k = 0, numOccurrences: " << numOccurrences << endl;
        for (int64_t k = 1; k < A.size(); k++)
        {
            //const int clearToLeftThisK = index - k;
            const int clearToLeftThisK = max(int64_t(0), index - k);
            //const int rightmostToEndLastK = max(int64_t(0), int64_t(A.size()) - rightMostCLEPos[index] - (k));
            const int rightmostToEndLastK = A.size() - rightMostCLEPos[index] - k;
            //const int left = (clearToLeftThisK >= 0 ? clearToLeftThisK + rightmostToEndLastK : 0);
            const int left = clearToLeftThisK + rightmostToEndLastK;
            int numInA = -1;
            if (k <= l)
                numInA = k + 1;
            else if (k >= l && k <= r)
                numInA = l + 1;
            else if (k > r && k <= l + 1 + r)
                numInA = l + 1 - (k - r);
            else if (k > l + 1 + r)
                numInA = 0;

            int leftTransformA = max(max(index - k, int64_t(0)) + max(rightmostToEndLastK, 0), int64_t(0));
            int rightTransformA = max(rightCLE[index] - k, int64_t(0));
            //cout << " k: " << k << " leftTransformA: " << leftTransformA << " rightTransformA: " << rightTransformA << endl;
            if (rightTransformA < leftTransformA)
                swap(leftTransformA, rightTransformA);
            const auto blee = findNumOccurrencesBruteForce2(leftTransformA, numInA, rightTransformA);
            //cout << " k: " << k << " index: " << index << " left: " << left << " clearToLeftThisK: " << clearToLeftThisK << " rightmostToEndLastK: " << rightmostToEndLastK << " numInA: " << numInA << " lesser side transform: " << leftTransformA << " greater side transform: " << rightTransformA  << endl;

            numOccurrences += blee;
            //cout << " numOccurrences in maxTransformMaxTransformA for index " << index << " k: " << k << " = " << blee << endl;
        }
        //assert(false && "Unhandled");
    }
    else if (leftCLT[index] != -1 && rightCLE[index] == -1)
    {
        int l = leftCLT[index];
        int r = A.size() - index - 1;
        if (r < l)
            swap(l, r);
        // k == 0 case.
        //numOccurrences += findNumOccurrencesBruteForce2(l, 1, r);
        //cout << " after k = 0, numOccurrences: " << numOccurrences << endl;
        if (sizeOfTransformA == -1)
        {
            sizeOfTransformA = 0;
            for (int k = 0; k <= A.size(); k++)
            {
                sizeOfTransformA += k;
            }
        }
        //cout << "sizeOfTransformA: " << sizeOfTransformA << endl;
        auto remainingAfterK = sizeOfTransformA;
        for (int64_t k = 0; k < A.size(); k++)
        {
            const int kBlockSize = A.size() - k;
            remainingAfterK -= kBlockSize;
            assert(remainingAfterK >= 0);
            //const int clearToRightThisK = index - k;
            const int clearToRightThisK = max(int64_t(0), int64_t(A.size()) -  index - 1 - k);
            //const int leftmostToBeginNextK = max(int64_t(0), int64_t(A.size()) - rightMostCLEPos[index] - (k));
            //assert(leftMostCGTPos[index] != -1);
            //const int left = (clearToRightThisK >= 0 ? clearToRightThisK + leftmostToBeginNextK : 0);
            int numInA = -1;
            if (k <= l)
                numInA = k + 1;
            else if (k >= l && k <= r)
                numInA = l + 1;
            else if (k > r && k <= l + 1 + r)
                numInA = l + 1 - (k - r);
            else if (k > l + 1 + r)
                numInA = 0;

            int spaceToRight = 0;
            if (leftMostCGTPos[index] == -1)
            {
#if 0
                const int clearToLeftThisK = max(int64_t(0), leftCLT[index] - k);
                //const int indexInKBlock = max(index - leftCLT[index] - k, int64_t(0));
                int requiredBeginOfThing = 0;
                if (k < leftCLT[index])
                {
                    requiredBeginOfThing = index - k;
                }
                else
                {
                    requiredBeginOfThing = index - leftCLT[index];
                }
                assert(requiredBeginOfThing >= 0);
                const int indexInKBlock = (numInA != 0 ? requiredBeginOfThing : 0);

                cout << " k: " << k << " clearToLeftThisK: " << clearToLeftThisK << " indexInKBlock: " << indexInKBlock << endl;
                spaceToRight = max(remainingAfterK + (kBlockSize - numInA - indexInKBlock), int64_t(0));
                if (numInA == 0)
                    spaceToRight = 0;
#endif
                //spaceToRight =  max(remainingAfterK + (kBlockSize - numInA - indexInKBlock), int64_t(0));
                spaceToRight =  remainingAfterK;
            }
            else
            {
                spaceToRight = (k < A.size() - 1 ? leftMostCGTPos[index] - (k + 1) : 0);
            }

            int leftTransformA = max(leftCLT[index] - k, int64_t(0));
            //int rightTransformA = max(clearToRightThisK + max(spaceToRight, 0), 0);
            int rightTransformA = max(clearToRightThisK + max(spaceToRight, 0), 0);
            //cout << " k: " << k << " leftTransformA: " << leftTransformA << " rightTransformA: " << rightTransformA << endl;
            if (rightTransformA < leftTransformA)
                swap(leftTransformA, rightTransformA);
            const auto blee = findNumOccurrencesBruteForce2(leftTransformA, numInA, rightTransformA);
            //cout << " k: " << k << " index: " << index << " left: " << left << " clearToRightThisK: " << clearToRightThisK << " numInA: " << numInA << " spaceToRight: " << spaceToRight << " lesser side transform: " << leftTransformA << " greater side transform: " << rightTransformA  << endl;

            numOccurrences += blee;
            //cout << " numOccurrences in maxTransformMaxTransformA for index " << index << " k: " << k << " = " << blee << endl;
        }
        //assert(false && "Unhandled");
    }
    else if (leftCLT[index] == -1 && rightCLE[index] == -1)
    {
        int l = index;
        int r = A.size() - index - 1;
        if (r < l)
            swap(l, r);
        // k == 0 case.
        //numOccurrences += findNumOccurrencesBruteForce2(l, 1, r);
        //cout << " after k = 0, numOccurrences: " << numOccurrences << endl;
        if (sizeOfTransformA == -1)
        {
            sizeOfTransformA = 0;
            for (int k = 0; k <= A.size(); k++)
            {
                sizeOfTransformA += k;
            }
        }

        //cout << "sizeOfTransformA: " << sizeOfTransformA << endl;
        auto remainingAfterK = sizeOfTransformA;
        for (int64_t k = 0; k < A.size(); k++)
        {
            const int kBlockSize = A.size() - k;
            remainingAfterK -= kBlockSize;
            assert(remainingAfterK >= 0);
            //const int clearToRightThisK = index - k;
            const int clearToRightThisK = max(int64_t(0), int64_t(A.size()) -  index - 1 - k);
            const int clearToLeftThisK = max(int64_t(0), index - k);
            //const int leftmostToBeginNextK = max(int64_t(0), int64_t(A.size()) - rightMostCLEPos[index] - (k));
            const int leftmostToBeginNextK = (k < A.size() - 1 ? leftMostCGTPos[index] - (k + 1) : 0);
            const int rightmostToEndLastK = (k != 0 ? A.size() - rightMostCLEPos[index] - k : 0);
            //const int left = (clearToRightThisK >= 0 ? clearToRightThisK + leftmostToBeginNextK : 0);
            int numInA = -1;
            if (k <= l)
                numInA = k + 1;
            else if (k >= l && k <= r)
                numInA = l + 1;
            else if (k > r && k <= l + 1 + r)
                numInA = l + 1 - (k - r);
            else if (k > l + 1 + r)
                numInA = 0;

            int64_t leftTransformA = max(clearToLeftThisK + max(rightmostToEndLastK, 0), 0);
            //int rightTransformA = max(clearToRightThisK + max(leftmostToBeginNextK, 0), 0);
            //cout << " k: " << k << " blee: " << int64_t(A.size()) - numInA - clearToLeftThisK << endl;
            //int64_t rightTransformA = max(remainingAfterK + (kBlockSize - numInA - clearToLeftThisK), int64_t(0));
            int64_t rightTransformA = clearToRightThisK + remainingAfterK;

            //cout << " k: " << k << " leftTransformA: " << leftTransformA << " rightTransformA: " << rightTransformA << endl;
            if (rightTransformA < leftTransformA)
                swap(leftTransformA, rightTransformA);
            const auto blee = findNumOccurrencesBruteForce2(leftTransformA, numInA, rightTransformA);
            //cout << " k: " << k << " index: " << index << " left: " << left << " clearToRightThisK: " << clearToRightThisK << " clearToLeftThisK: " << clearToLeftThisK << " leftmostToBeginNextK: " << leftmostToBeginNextK << " rightmostToEndLastK: " << rightmostToEndLastK << " numInA: " << numInA << " lesser side transform: " << leftTransformA << " greater side transform: " << rightTransformA << " remainingAfterK: " << remainingAfterK << endl;

            numOccurrences += blee;
            //cout << " numOccurrences in maxTransformMaxTransformA for index " << index << " k: " << k << " = " << blee << endl;
        }
        //assert(false && "Unhandled");
    }
    else
    {
        assert(false);
    }
    //cout << "Total occurrences for index " << index << " = " << numOccurrences << endl;
    return numOccurrences.value();
}


int main(int argc, char** argv)
{
    if (argc == 2)
    {
        struct timeval time; 
        gettimeofday(&time,NULL);

        // microsecond has 1 000 000
        // Assuming you did not need quite that accuracy
        // Also do not assume the system clock has that accuracy.
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        const int N = rand() % 50 + 1;
        cout << N << endl;
        for (int i = 0; i < N; i++)
        {
            cout << ((rand() % 10) + 1) << " ";
        }
        cout << endl;
        return 0;
    }

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
    //cout << "sumUpTo 5: " << sumUpTo(5, mod) << endl;
    int n;
    cin >> n;

    vector<int> A(n);

    bool allSameNumber = true;
    for (int i = 0; i < n; i++)
    {
        cin >> A[i];
        if (i > 0 && A[i] != A[i - 1])
            allSameNumber = false;
    }


#if 0
    cout << "maxTransformA: " << endl;
    for (const auto x : maxTransformA)
    {
        cout << x << " ";
    }
    cout << endl;
    cout << "size: " << maxTransformA.size() << endl;
#endif

#if 0
    int64_t sum = 0;
    for (const auto x : maxTransformMaxTransformA)
    {
        sum += x;
    }
    cout << endl;
#endif
#if 0
    cout << "maxTransformMaxTransformA: " << endl;
    for (const auto x : maxTransformMaxTransformA)
    {
        cout << x << " ";
    }
    cout << endl;
#endif

    const auto leftCLT = computeLeftCLTBruteForce(A);
    const auto rightCLE = computeRightCBruteForce(A);
    const auto rightMostCLEPos = computeRightMostCLEBruteForce(A);
    const auto leftMostCGTPos = computeLeftMostCGTBruteForce(A);


    ModNum sumOpt = 0;
    if (allSameNumber)
    {
        int64_t sizeOfTransformA = sumUpTo(A.size(), mod);
        int64_t sizeOfTransformTransformA = sumUpTo(sizeOfTransformA, mod);
        sumOpt = sizeOfTransformTransformA * A[0];
    }
    else
    {

        for (int i = 0; i < A.size(); i++)
        {
            sumOpt += ModNum(A[i]) * findNumOccurrencesBruteForce(i, A, leftCLT, rightCLE, rightMostCLEPos, leftMostCGTPos);
        }
    }
    cout << sumOpt << endl;
#ifdef BRUTE_FORCE
    const auto maxTransformA = maxTransform(A);
    cout << endl;
    const auto maxTransformMaxTransformA = maxTransform(maxTransformA);
    cout << endl;
    int realOccurrences = 0;
    ModNum realSum = 0;
    for (const auto x : maxTransformMaxTransformA)
    {
        //if (x == trackNumber)
            //realOccurrences++;
        realSum += x;
    }
    //cout << "real occurrences of " << trackNumber << " in maxTransformMaxTransformA: " << realOccurrences << " occurrencesOpt: " << occurrencesOpt << endl;
    cout << "real sum: " << realSum << " sumOpt: " << sumOpt << endl;
    assert(realSum.value() == sumOpt.value());
#endif

#if 0
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
#endif


}

