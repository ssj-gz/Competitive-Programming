// Simon St James (ssjgz) - 2019-04-15
//#define SUBMISSION
#define BRUTE_FORCE
#ifdef SUBMISSION
#undef BRUTE_FORCE
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <sstream>
#include <cassert>

#include <sys/time.h>

using namespace std;

class Choice
{
    public:
        Choice(int numIndices)
            : m_indices(numIndices)
        {
        }
        Choice() = default;
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
    Choice choiceIndices;
    int64_t value = 0;
};


bool operator<(const Sum& lhs, const Sum& rhs)
{
    return lhs.value < rhs.value;
}

void findSums(const vector<int64_t>& a, Choice& choice, int indexToChange, int maxValueOfIndex, vector<Sum>& destSums)
{
    if (indexToChange == -1)
    {
        int64_t value = 0;
        for (int i = 0; i < choice.numIndices(); i++)
        {
            value += a[choice[i]];
        }
        Sum sum;
        sum.value = value;
        sum.choiceIndices = choice;
        destSums.push_back(sum);
        //cout << "--" << endl;
        //cout << "choice: " << endl;
        //for (int i = 0; i < choice.numIndices(); i++)
        //{
            //cout << choice[i] << " ";
        //}
        //cout << endl;
        //cout << "sum: " << value << endl;
        return;
    }

    for (int indexValue = 0; indexValue <= maxValueOfIndex; indexValue++)
    {
        choice[indexToChange] = indexValue;
        findSums(a, choice, indexToChange - 1, indexValue, destSums); 
    }
}

bool isSolutionCorrect(const vector<int64_t>& a, const vector<int64_t>& s, int k)
{
    Choice choice(k);
    vector<Sum> sums;
    findSums(a, choice, k - 1, a.size() - 1, sums);
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
        //const int n = 19;
        //const int k = 8;
        //const int maxValue = rand() % 100'000'000;
        const int maxValue = rand() % 100;

        vector<int64_t> a;
        for (int i = 0; i < n; i++)
        {
            a.push_back(rand() % (maxValue + 1));
        }
        sort(a.begin(), a.end());


        cout << n << " " << k << endl;

        Choice choice(k);
        vector<Sum> sums;
        findSums(a, choice, k - 1, a.size() - 1, sums);
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

        cout << "sums: (" << sums.size() << ")" << endl;
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

    string line;
    getline(cin, line);
    istringstream tStream(line);
    int T;
    tStream >> T;

    for (int t = 0; t < T; t++)
    {
        getline(cin, line);
        istringstream nkStream(line);
        //cout << "nkStream: " << line << endl;
        int N;
        nkStream >> N;
        int K;
        nkStream >> K;

        getline(cin, line);
        istringstream sStream(line);
        //cout << "sStream: " << line << endl;

        vector<int64_t> s;
        int64_t x;
        while (sStream >> x)
        {
            s.push_back(x);
        }


        Choice choice(K);
        vector<Sum> sums;
        vector<int64_t> dummyA(N);
        findSums(dummyA, choice, K - 1, dummyA.size() - 1, sums);
        vector<vector<Choice>> choicesUsingFirstElements(N);

        for (const auto& sum : sums)
        {
            choicesUsingFirstElements[sum.choiceIndices[K - 1]].push_back(sum.choiceIndices);
        }

        sort(s.begin(), s.end());
        assert((s.front() % K) == 0);
        vector<int64_t> a(N);
        a[0] = s.front() / K;
        std::priority_queue<int64_t, std::vector<int64_t>, std::greater<int64_t> >  expectedValuesUsingKnownElements;
        expectedValuesUsingKnownElements.push(K * a[0]);

        int numKnownElementsOfA = 1;
        for (const auto x : s)
        {
            if (expectedValuesUsingKnownElements.empty() || expectedValuesUsingKnownElements.top() != x)
            {
                const int64_t newNum = x - (K - 1) * a[0];
                a[numKnownElementsOfA] = newNum;
                for (const auto& choice : choicesUsingFirstElements[numKnownElementsOfA])
                {
                    int64_t newValueUsingKnownElements = 0;
                    for (int i = 0; i < K; i++)
                    {
                        assert(choice[i] <= numKnownElementsOfA);
                        newValueUsingKnownElements += a[choice[i]];
                    }
                    expectedValuesUsingKnownElements.push(newValueUsingKnownElements);
                }
                numKnownElementsOfA++;
                expectedValuesUsingKnownElements.pop();
                if (numKnownElementsOfA == N)
                    break;
            }
            else
            {
                expectedValuesUsingKnownElements.pop();
            }
        }
        for (const auto x : a)
        {
            cout << x << " ";
        }
        cout << endl;
#ifdef BRUTE_FORCE
        const bool correct = isSolutionCorrect(a, s, K);
        cout << "correct? " << correct << endl;
        assert(correct);
#endif
    }
}


