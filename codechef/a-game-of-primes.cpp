// Simon St James (ssjgz) - 2019-07-19
#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <functional>
#include <cmath>


#include <cassert>

#include <sys/time.h>

using namespace std;

struct Query
{
    char queryType;
    int l = -1;
    int r = -1;
    int value = -1;
};

vector<int> solveBruteForce(const vector<Query>& queries, int64_t K, const vector<int>& primesThatDivideK)
{
    const int maxRangeEnd = 100'000;
    vector<int64_t> values(maxRangeEnd + 1, -1);
    vector<int> queryResults;
    for (const auto& query : queries)
    {
        if (query.queryType == '!')
        {
            for (int i = query.l; i <= query.r; i++)
            {
                values[i] = query.value;
            }
        }
        else if (query.queryType == '?')
        {
            int num = 0;
            for (int primeFactorOfKIndex = 0; primeFactorOfKIndex < primesThatDivideK.size(); primeFactorOfKIndex++)
            {
                bool hasFactor = false;
                for (int i = query.l; i <= query.r; i++)
                {
                    if (values[i] == -1)
                        continue;
                    if ((values[i] % primesThatDivideK[primeFactorOfKIndex]) == 0)
                    {
                        hasFactor = true;
                        break;
                    }
                }
                if (hasFactor)
                {
                    num++;
                }
            }
            queryResults.push_back(num);
        }
    }
    return queryResults;
}

class SegmentTree2
{
    public:
        SegmentTree2(int maxRangeEnd)
            : maxRangeEnd(maxRangeEnd),
              tree(3 * maxRangeEnd, 0),
              lazy(3 * maxRangeEnd, 0)
        {
            build(1, 0, maxRangeEnd - 1);
        };
        void setAllInRangeA(int left, int right, bool setOn)
        {
            range_update(1, 0, maxRangeEnd - 1, left, right, (setOn ? 2 : 1));
        }
        int getSumOfRange(int left, int right)
        {
            return query(1, 0, maxRangeEnd - 1, left, right);
        }
    private:
        int maxRangeEnd = -1;
        vector <int> tree;
        vector <int> lazy;
        void combine(int &v, const int &v1, const int &v2)
        {
            v = v1 + v2;
        }

        void build(int where, int left, int right)
        {
            if ( left > right ) return;
            if ( left == right ) {
                tree[where] = 0;
                return;
            }
            int mid = (left+right)>>1;
            build((where<<1), left, mid);
            build((where<<1)+1, mid+1, right);
            combine(tree[where], tree[(where<<1)], tree[(where<<1)+1]);
        }

        void push_down(int where, int left, int right)
        {
            if ( lazy[where] ) {
                tree[where] = (lazy[where] == 1 ? 0 : (right - left + 1));
                if ( left != right ) {
                    lazy[(where<<1)] = lazy[where];
                    lazy[(where<<1)+1] = lazy[where];
                }
                lazy[where] = 0;
            }
            return;
        }

        void range_update(int where, int left, int right ,int i, int j, int val)
        {
            push_down(where, left, right);
            if ( left > right || left > j || right < i ) return;
            if ( left >= i && right <= j ) {
                lazy[where] = val;
                push_down(where, left, right);
                return;
            }
            int mid = (left+right)>>1;
            range_update((where<<1), left, mid, i, j, val);
            range_update((where<<1)+1, mid+1, right, i, j, val);
            combine(tree[where], tree[(where<<1)], tree[(where<<1)+1]);
        }

        int query(int where, int left, int right, int i, int j)
        {
            push_down(where, left, right);
            if ( left > right || left > j || right < i ) return 0;
            if ( left >= i && right <= j ) 
            {
                return tree[where];
            }
            int mid = (left+right)>>1;
            int ans = 0;
            int ans1 = query((where<<1), left, mid, i, j);
            int ans2 = query((where<<1)+1, mid+1, right, i, j);
            combine(ans, ans1, ans2);
            return ans;
        }

};
vector<int> solveOptimised(const vector<Query>& queries, int64_t K, const vector<int>& primesThatDivideK)
{
    vector<int> queryResults;
    const int maxRangeEnd = 100'000;

    vector<SegmentTree2> dbgNumWithPrimeFactorOfKTracker;

    for (int i = 0; i < primesThatDivideK.size(); i++)
    {
        dbgNumWithPrimeFactorOfKTracker.emplace_back(maxRangeEnd);
    }
    for (const auto& query : queries)
    {
        if (query.queryType == '!')
        {
            for (int primeFactorOfKIndex = 0; primeFactorOfKIndex < primesThatDivideK.size(); primeFactorOfKIndex++)
            {
                if ((query.value % primesThatDivideK[primeFactorOfKIndex]) == 0)
                {
                    //numWithPrimeFactorOfKTracker[primeFactorOfKIndex].applyOperatorToAllInRange(query.l, query.r, true);
                    dbgNumWithPrimeFactorOfKTracker[primeFactorOfKIndex].setAllInRangeA(query.l, query.r, true);
                }
                else
                {
                    //numWithPrimeFactorOfKTracker[primeFactorOfKIndex].applyOperatorToAllInRange(query.l, query.r, false);
                    dbgNumWithPrimeFactorOfKTracker[primeFactorOfKIndex].setAllInRangeA(query.l, query.r, false);
                }
            }
        }
        else if (query.queryType == '?')
        {
            int num = 0;
            int dbgNum = 0;
            for (int primeFactorOfKIndex = 0; primeFactorOfKIndex < primesThatDivideK.size(); primeFactorOfKIndex++)
            {
#if 0
                const int numInRange = numWithPrimeFactorOfKTracker[primeFactorOfKIndex].combinedValuesInRange(query.l, query.r);
                if (numInRange > 0)
                    num++;
#endif
                const int dbgNumInRange = dbgNumWithPrimeFactorOfKTracker[primeFactorOfKIndex].getSumOfRange(query.l, query.r);
                if (dbgNumInRange > 0)
                    dbgNum++;
                //cout << "  numInRange: " << numInRange << " dbgNumInRange: " << dbgNumInRange << endl;
                //assert(numInRange == dbgNumInRange);
            }
            queryResults.push_back(dbgNum);
        }
        else
        {
            assert(false);
        }
    }
    return queryResults;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);

#if 0
    if (true)
    {
        const int maxRangeEnd = 100'000;
        auto combineSetValue = [](const int64_t& newValueToSetTo, const int64_t& earlierValueToSetTo)
        {
            return newValueToSetTo;
        };

        auto combineValues = [](const int& lhsValue, const int& rhsValue)
        {
            return lhsValue + rhsValue;
        };

        auto applySetValue = [](int64_t valueToSetTo, int& value)
        {
            value = valueToSetTo;
        };

        using NumPrimesTracker = SegmentTree<int, int64_t>;

        NumPrimesTracker numPrimesTracker(maxRangeEnd, combineValues, applySetValue, combineSetValue);
        numPrimesTracker.setInitialValues(vector<int>(maxRangeEnd, 0));
        build(1, 0, maxRangeEnd - 1);


        {
            const int setL = 3;
            const int setR = 100;
            range_update(1, 0, maxRangeEnd - 1, setL, setR, 2);
            numPrimesTracker.applyOperatorToAllInRange(setL, setR, true);

            const int qL = 30;
            const int qR = 50;
            const int blee = ::query(1, 0, maxRangeEnd - 1, qL, qR);
            const int blaa = numPrimesTracker.combinedValuesInRange(qL, qR);
            cout << "Blee: " <<  blee << endl;
            cout << "Blaa: " << blaa << endl;
        }
{
            const int setL = 56;
            const int setR = 63;
            range_update(1, 0, maxRangeEnd - 1, setL, setR, 1);
            numPrimesTracker.applyOperatorToAllInRange(setL, setR, false);

            const int qL = 23;
            const int qR = 80;
            const int blee = ::query(1, 0, maxRangeEnd - 1, qL, qR);
            const int blaa = numPrimesTracker.combinedValuesInRange(qL, qR);
            cout << "Blee: " <<  blee << endl;
            cout << "Blaa: " << blaa << endl;
        }

        return 0;
    }
#endif

    if (argc == 2)
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int K = (rand() % 100) + 1;
        const int Q = (rand() % 100) + 1;
        const int maxR = (rand() % 100) + 1;
        const int maxX = rand() % 100 + 1;

        cout << K << " " << Q << endl;

        for (int q = 0; q < Q; q++)
        {
            if (rand() % 2 == 0)
            {
                cout << "! ";
                int l = rand() % maxR + 1;
                int r = rand() % maxR + 1;
                if (r < l)
                    swap(l ,r);
                cout << l << " " << r << " " << (rand() % maxX + 1) << endl;
            }
            else
            {
                cout << "? ";
                int l = rand() % maxR + 1;
                int r = rand() % maxR + 1;
                if (r < l)
                    swap(l ,r);
                cout << l << " " << r << endl;
            }
        }
        return 0;
    }

    const int64_t maxXorK = 1'000'000'000ULL;
    const int sqrtMaxXorK = sqrt(maxXorK);

    vector<int> primesUpToSqrtMaxXorK;
    vector<bool> isPrime(sqrtMaxXorK + 1, true);

    for (int factor = 2; factor <= sqrtMaxXorK; factor++)
    {
        if (isPrime[factor])
        {
            primesUpToSqrtMaxXorK.push_back(factor);
            for (int multiple = factor; multiple < isPrime.size(); multiple += factor)
            {
                isPrime[multiple] = false;
            }
        }
    }


    int64_t K;
    cin >> K;

    int Q;
    cin >> Q;

    vector<int> primesThatDivideK;
    for (const auto prime : primesUpToSqrtMaxXorK)
    {
        bool addedAlready = false;
        while ((K % prime) == 0)
        {
            K /= prime;
            if (!addedAlready)
            {
                primesThatDivideK.push_back(prime);
                addedAlready = true;
            }
        }
    }

    if (K != 1)
    {
        // If value of K after being divided by all its prime divisors <= sqrt(K)
        // is not 1, then the remaining value of K is prime (otherwise: it would have
        // at least two prime factors, P and Q.   But P, Q must be > sqrt(K) else
        // they would have been divided out, and so P x Q > K, a contradiction).
        primesThatDivideK.push_back(K);
    }


    vector<Query> queries(Q);

    for (int q = 0; q < Q; q++)
    {
        cin >> queries[q].queryType;

        if (queries[q].queryType == '!')
        {
            cin >> queries[q].l;
            cin >> queries[q].r;
            queries[q].l--;
            queries[q].r--;
            cin >> queries[q].value;
        }
        else if (queries[q].queryType == '?')
        {
            cin >> queries[q].l;
            cin >> queries[q].r;
            queries[q].l--;
            queries[q].r--;
        }
        else
        {
            assert(false);
        }
    }

#ifdef BRUTE_FORCE
    const auto solutionBruteForce = solveBruteForce(queries, K, primesThatDivideK);
    cout << " solutionBruteForce: ";
    for (const auto x : solutionBruteForce)
    {
        cout << x << " ";
    }
    cout << endl;
    const auto solutionOptimised = solveOptimised(queries, K, primesThatDivideK);

    cout << " solutionOptimised:  ";
    for (const auto x : solutionOptimised)
    {
        cout << x << " ";
    }
    cout << endl;
    assert(solutionOptimised == solutionBruteForce);
#else
    const auto solutionOptimised = solveOptimised(queries, K, primesThatDivideK);

    for (const auto x : solutionOptimised)
    {
        cout << x << " ";
    }
    cout << endl;
#endif


}
