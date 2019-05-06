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

template<typename T>
using IncreasingPriorityQueue = std::priority_queue<T, std::vector<T>, std::greater<T>>;

int main(int argc, char* argv[])
{
    // Another one that "took me ages", in that I started thinking about it years ago and
    // then recently re-tried it, and figured it out fairly quickly.  At least part of 
    // the delay was due to going down a rabbit hole and trying to find rule/ pattern behind generating
    // the sequence of choices in non-decreasing order, which is entirely unnecessary.
    //
    // Can't believe it's worth 150 points(!)
    //
    // So: how do we begin to untangle this list s of choices and get the original set A that the
    // choices were drawn from? First, note that the smallest item in s must be the choice where
    // we choose the smallest item in A, k times - from this, we can deduce A[0] (let's assume
    // for convenience that A is sorted).
    //
    // What about the *next* smallest item in s - does this tell us anything? Yes: we can't choose the
    // smallest item in A k times any more, so the next smallest choice must be choosing the smallest
    // choice in A (i.e. A[0]) k-1 times, then the *next* smallest choice in A: that is - 
    //
    //  the second-smallest element in s is equal to (k-1) * A[0] + A[1]
    //
    // from which we can easily deduce the next smallest item in A, A[1].
    //
    // There's already a strategy beginning to form: sort s into ascending order, and see if we can 
    // use successive values in this list to deduce successive values of A.  How can we formalise this?
    //
    // Assume s is sorted (sort it if it is not!).  For any j, 0 <= j <= k-1, there must be an index 
    // in our sorted list where A[j] is chosen for the first time.  Call this index first(j).
    // What can we deduce about first(j)? 
    //
    // Firstly, by definition, all the values of s before first(j) are formed by choices drawn from
    // A[0], A[1], .... A[j - 1].
    //
    // Secondly, the value first(j) is the smallest choice that can be formed using A[j] (again, by
    // definition), and so it must be of the form:
    //
    //  s[first(j)] = (k - 1) * A[0] + A[j]                (*)
    //
    // from which we can easily deduce A[j].
    //
    // But how can we find the first(j)'s? The clue comes from the first part: imagine we generated 
    // all choices using A[0], A[1], ... A[j-1], and sorted them - call this sortedChoices(j-1).  We'd be able to pair off,
    // starting from the top, items in this list vs items in our sorted s.  What would happen at 
    // first(j)? We're suddenly incorporating A[j], but why? There are only two possible reasons:
    // either we've run out of elements of sortedChoices(j-1), or the next element in sortedChoices(j-1)
    // would be greater than the first choice than can be formed using A[j], either of which is 
    // easily detectable and so we can easily find first(j).
    //
    // So to recap: if we know A[0], A[1], ... A[j-1], we can find A[j] by generating sortedChoices(j-2) (that is,
    // all choices using only A[0] ... A[j-1], in sorted order); pairing them off one by one with s; and designating
    // the first mismatch (where either we run out of sortedChoices(j-2), or the next element of sortedChoices(j-2)
    // does not match the next element of s) as first(j), then deduce A[j] using the formula (*).
    //
    // Obviously, generating all of sortedChoices(0), sortedChoices(1), etc and iterating over this and s
    // each time we want to find the next first(j) is very inefficient, but we can do it all incrementally:
    // sortedChoices(j) is just sortedChoices(j-1) plus all choices where i_k == j merged in and re-sorted.
    // We can model this with a set, but actually, since we're pairing off elements of sortedChoices(j) with
    // elements of s, a priority_queue is easier: we can discard elements of sortedChoices(j) that
    // have already been paired off elements of s, which helps with the book-keeping.
    //
    // And that's about it! In the code, the sortedChoices array(s) is referred to as expectedValuesUsingKnownElements.

    int T;
    cin >> T;

    for (int t = 0; t < T; t++)
    {
        int N;
        cin >> N;
        int K;
        cin >> K;

        // Skip to next line, so we can read in s.
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        // Read in and sort s.
        string sLine;
        getline(cin, sLine);
        istringstream sStream(sLine);
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

        IncreasingPriorityQueue<int64_t> expectedValuesUsingKnownElements;
        expectedValuesUsingKnownElements.push(K * a[0]);

        for (const auto x : s)
        {
            if (expectedValuesUsingKnownElements.empty() || expectedValuesUsingKnownElements.top() != x)
            {
                // This element x of s is not expected: therefore, it must be using the next unknown value of a,
                // which we can now deduce.
                const int64_t nextElementOfA = x - (K - 1) * a[0];
                a[numKnownElementsOfA] = nextElementOfA;
                // We now need to ensure that expectedValuesUsingKnownElements now contains precisely the set of 
                // elements where the last chosen index (i.e. i_k) is *at most* numKnownElementsOfA.
                // It is already (from previous passes) precisely the set of elements where the last chosen index 
                // is *at most* (numKnownElementsOfA - 1), which is a subset of what we want.
                // So, to avoid re-adding duplicates of values added in previous passes, we add precisely the values
                // where the last chosen index (i.e. i_k) *is equal to* numKnownElementsOfA.
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
                if (numKnownElementsOfA == N)
                    break;
            }

            // We matched this expected element against x (possibly after just adding it!): discard it.
            expectedValuesUsingKnownElements.pop();
        }
        for (const auto x : a)
        {
            cout << x << " ";
        }
        cout << endl;
    }
}

