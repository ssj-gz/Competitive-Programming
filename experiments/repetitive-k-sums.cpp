#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <cassert>

#include <sys/time.h>

using namespace std;

class ChoiceIndices
{
    public:
        ChoiceIndices(int numIndices)
            : m_indices(numIndices)
        {
        }
        ChoiceIndices() = default;
        int& operator[](int i)
        {
            return m_indices[i];
        }
        const int& operator[](int i) const
        {
            return m_indices[i];
        }
        int numIndices() const
        {
            return m_indices.size();
        }
    private:
        vector<int> m_indices;
};
struct Sum
{
    ChoiceIndices choiceIndices;
    int64_t value = 0;
};


bool operator<(const Sum& lhs, const Sum& rhs)
{
    return lhs.value < rhs.value;
}

void findSums(const vector<int64_t>& a, ChoiceIndices& indices, int indexToChange, int maxValueOfIndex, vector<Sum>& destSums)
{
    if (indexToChange == -1)
    {
        int64_t value = 0;
        for (int i = 0; i < indices.numIndices(); i++)
        {
            value += a[indices[i]];
        }
        Sum sum;
        sum.value = value;
        sum.choiceIndices = indices;
        destSums.push_back(sum);
        //cout << "--" << endl;
        //cout << "indices: " << endl;
        //for (int i = 0; i < indices.numIndices(); i++)
        //{
            //cout << indices[i] << " ";
        //}
        //cout << endl;
        //cout << "sum: " << value << endl;
        return;
    }

    for (int indexValue = 0; indexValue <= maxValueOfIndex; indexValue++)
    {
        indices[indexToChange] = indexValue;
        findSums(a, indices, indexToChange - 1, indexValue, destSums); 
    }
}

bool isSolutionCorrect(const vector<int64_t>& a, const vector<int64_t>& s, int k)
{
    ChoiceIndices indices(k);
    vector<Sum> sums;
    findSums(a, indices, k - 1, a.size() - 1, sums);
    sort(sums.begin(), sums.end());
    vector<int64_t> sortedSums;
    for (const auto& sum : sums)
    {
        sortedSums.push_back(sum.value);
    }
    vector<int64_t> sortedS(s);
    sort(sortedS.begin(), sortedS.end());
    return (sortedS == sortedSums);

}

int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        cout << 1 << endl;
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        const int n = rand() % 10 + 1;
        const int k = rand() % n + 1;
        const int maxValue = rand() % 100;

        vector<int64_t> a;
        for (int i = 0; i < n; i++)
        {
            a.push_back(rand() % (maxValue + 1));
        }
        sort(a.begin(), a.end());


        cout << n << " " << k << endl;

        ChoiceIndices indices(k);
        vector<Sum> sums;
        findSums(a, indices, k - 1, a.size() - 1, sums);
        stable_sort(sums.begin(), sums.end());

        for (const auto& x : sums)
        {
            cout << x.value << " ";
        }
        cout << endl;
        cout << "a: ";
        for (const auto x : a)
        {
            cout << x << " ";
        }
        cout << endl;

        cout << "sums: " << endl;
        for (const auto& x : sums)
        {
            cout << x.value << " ";

            cout << "(";
            for (int i = 0; i < x.choiceIndices.numIndices(); i++)
            {
                cout << x.choiceIndices[i] << " ";
            }
            cout << ") "; 
            cout << "(";
            string indices(a.size(), '.');
            for (int i = 0; i < x.choiceIndices.numIndices(); i++)
            {
                indices[x.choiceIndices[i]] = 'X';
            }
            cout << indices;
            cout << ")" << endl;
        }
        cout << endl;

        return 0;
    }
#if 0
    sort(a.begin(), a.end());

    a = {1, 20, 21, 22, 23, 24, 25000 };

    cout << "n: " << a.size() << " k: " << k << endl;
    cout << "a:" << endl;
    for (const auto x : a)
    {
        cout << x << " ";
    }
    cout << endl;

    ChoiceIndices indices(k);
    vector<Sum> sums;
    findSums(a, indices, k - 1, a.size() - 1, sums);
    stable_sort(sums.begin(), sums.end());


    cout << "sums: " << endl;
    for (const auto& x : sums)
    {
        cout << x.value << " ";

        cout << "(";
        for (int i = 0; i < x.choiceIndices.numIndices(); i++)
        {
            cout << x.choiceIndices[i] << " ";
        }
        cout << ") "; 
        cout << "(";
        string indices(a.size(), '.');
        for (int i = 0; i < x.choiceIndices.numIndices(); i++)
        {
            indices[x.choiceIndices[i]] = 'X';
        }
        cout << indices;
        cout << ")" << endl;
    }
    cout << endl;
    //assert(is_sorted(sums.begin(), sums.end()));
#endif
    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        int N;
        cin >> N;
        int K;
        cin >> K;

        int64_t x;
        vector<int64_t> s;
        while (cin >> x)
        {
            s.push_back(x);
        }


        ChoiceIndices indices(K);
        vector<Sum> sums;
        vector<int64_t> dummyA(N);
        findSums(dummyA, indices, K - 1, dummyA.size() - 1, sums);
        vector<vector<ChoiceIndices>> choiceIndicesWithForLastIndex(N);

        for (const auto& sum : sums)
        {
            choiceIndicesWithForLastIndex[sum.choiceIndices[K - 1]].push_back(sum.choiceIndices);
        }


        sort(s.begin(), s.end());
        assert((s.front() % K) == 0);
        vector<int64_t> a(N);
        a[0] = s.front() / K;
        std::priority_queue<int64_t, std::vector<int64_t>, std::greater<int64_t> >  blah;
        blah.push(K * a[0]);
        cout << "First num: " << a[0] << endl;
        int numKnownElementsOfA = 1;
        for (const auto x : s)
        {
            cout << "x: " << x << " top: " << (blah.empty() ? -1 : blah.top()) << endl;
            if (blah.empty() || blah.top() != x)
            {
                const int64_t newNum = x - (K - 1) * a[0];
                cout << "New!" << newNum << " x: " << x << endl;
                a[numKnownElementsOfA] = newNum;
                for (const auto& blee : choiceIndicesWithForLastIndex[numKnownElementsOfA])
                {
                    int64_t value = 0;
                    for (int i = 0; i < K; i++)
                    {
                        assert(blee[i] <= numKnownElementsOfA);
                        value += a[blee[i]];
                    }
                    //cout << " adding " << value << endl;
                    blah.push(value);
                }
                numKnownElementsOfA++;
                blah.pop();
                if (numKnownElementsOfA == N)
                    break;
            }
            else
            {
                blah.pop();
            }
        }
        cout << "A:" << endl;
        for (const auto x : a)
        {
            cout << x << " ";
        }
        cout << endl;
        cout << "correct? " << isSolutionCorrect(a, s, K) << endl;
    }
}


