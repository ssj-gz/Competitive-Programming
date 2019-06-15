// Simon St James (ssjgz) - 2019-06-15
#define SUBMISSION
#define VERIFY
#ifdef SUBMISSION
#undef VERIFY
#define NDEBUG
#endif
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

int main()
{
    // Another easy one: it's hopefully obvious that the optimal algorithm
    // simply sweeps from left to right: if the element at the current index
    // is not the expected one (i.e. not equal to index + 1) then we need 
    // to swap the element at this index with the expected element; otherwise
    // we do nothing.
    //
    // The indexOf array - which we need to maintain in the face of swaps - 
    // enables us to find the index of the expected element in O(1).
    int n;
    cin >> n;

    vector<int> a(n);
    for (int i = 0; i < n; i++)
    {
        cin >> a[i];
    }

    vector<int> indexOf(n + 1);
    for (int i = 0; i < n; i++)
    {
        indexOf[a[i]] = i;
    }

    int numSwaps = 0;
    for (int i = 0; i < n; i++)
    {
        const int expectedNumForIndex = i + 1;
        if (a[i] != expectedNumForIndex)
        {
            // Need to swap.
            const int indexOfExpectedNum = indexOf[expectedNumForIndex];

            indexOf[a[i]] = indexOfExpectedNum;
            indexOf[expectedNumForIndex] = i;
            swap(a[i], a[indexOfExpectedNum]);
            numSwaps++;

#ifdef VERIFY
            for (int j = 0; j <= i; j++)
            {
                assert(a[j] == j + 1);
                assert(indexOf[a[j]] == j);
            }
#endif
        }
    }
    cout << numSwaps << endl;
}

