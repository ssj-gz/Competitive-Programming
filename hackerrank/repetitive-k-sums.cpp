// Simon St James (ssjgz) - 2019-04-15 21:45
#define SUBMISSION
#ifdef SUBMISSION
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <sstream>
#include <cassert>

using namespace std;

class Choice
{
    public:
        Choice(int numIndices)
            : m_indices(numIndices)
        {
        }
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

void findChoices(Choice& choice, int indexToChange, int maxValueOfIndex, vector<Choice>& destChoices)
{
    if (indexToChange == -1)
    {
        destChoices.push_back(choice);
        return;
    }

    for (int indexValue = 0; indexValue <= maxValueOfIndex; indexValue++)
    {
        choice[indexToChange] = indexValue;
        findChoices(choice, indexToChange - 1, indexValue, destChoices); 
    }
}

int main(int argc, char* argv[])
{
    string line;
    getline(cin, line);
    istringstream tStream(line);
    int T;
    tStream >> T;

    for (int t = 0; t < T; t++)
    {
        getline(cin, line);
        istringstream nkStream(line);
        int N;
        nkStream >> N;
        int K;
        nkStream >> K;

        getline(cin, line);
        istringstream sStream(line);

        // Read in and sort s.
        vector<int64_t> s;
        int64_t x;
        while (sStream >> x)
        {
            s.push_back(x);
        }
        sort(s.begin(), s.end());

        // Build up choicesWithLastIndexEqualTo.
        Choice choice(K);
        vector<Choice> choices;
        findChoices(choice, K - 1, N - 1, choices);
        vector<vector<Choice>> choicesWithLastIndexEqualTo(N);

        for (const auto& choice : choices)
        {
            choicesWithLastIndexEqualTo[choice[K - 1]].push_back(choice);
        }

        // Extract first element.
        assert((s.front() % K) == 0);
        vector<int64_t> a(N);
        a[0] = s.front() / K;
        int numKnownElementsOfA = 1;

        std::priority_queue<int64_t, std::vector<int64_t>, std::greater<int64_t>> expectedValuesUsingKnownElements;
        expectedValuesUsingKnownElements.push(K * a[0]);

        for (const auto x : s)
        {
            if (expectedValuesUsingKnownElements.empty() || expectedValuesUsingKnownElements.top() != x)
            {
                // This element x of s is not expected: therefore, it must be using the next unknown value of a,
                // which we can now deduce.
                const int64_t newNum = x - (K - 1) * a[0];
                a[numKnownElementsOfA] = newNum;
                // Add values where the last chosen index (i.e. i_k) equals numKnownElementsOfA,
                // thus keeping expectedValuesUsingKnownElements up-to-date with our new known element of a.
                for (const auto& choice : choicesWithLastIndexEqualTo[numKnownElementsOfA])
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
                // This element x of s was what we expected; discard it, so we don't expect it again :)
                expectedValuesUsingKnownElements.pop();
            }
        }
        for (const auto x : a)
        {
            cout << x << " ";
        }
        cout << endl;
    }
}

