// Simon St James (ssjgz) - 2019-04-06
#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>

using namespace std;

vector<int> findIndexOfNextGreaterThan(const vector<int>& a)
{
    std::stack<int> indices;
    const int n = a.size();

    vector<int> indexOfNextGreaterThan(n, -1);

    for (int index = 0; index < n; index++)
    {
        while (!indices.empty() && a[indices.top()] < a[index])
        {
            indexOfNextGreaterThan[indices.top()] = index;
            indices.pop();
        }
        indices.push(index);
    }

    return indexOfNextGreaterThan;
}
vector<int> findIndexOfPrevGreaterThan(const vector<int>& a)
{
    // Leave the heavy-lifting to findIndexOfNextGreaterThan after passing it
    // a reversed copy of a.
    const int n = a.size();
    const vector<int> reversedArr(a.rbegin(), a.rend());
    auto indexOfPrevGreaterThan = findIndexOfNextGreaterThan(reversedArr);

    // Correct for the fact that we gave findIndexOfNextGreaterThan a reversed version 
    // of a: Reverse order indexOfPrevGreaterThan, and also "reverse" (flip from 
    // left to right) each index.
    for (auto& index : indexOfPrevGreaterThan)
    {
        if (index != -1)
            index = (n - 1) - index;
    }
    reverse(indexOfPrevGreaterThan.begin(), indexOfPrevGreaterThan.end());

    return indexOfPrevGreaterThan;
}


int64_t findMaxIndexProduct(const vector<int>& a)
{
    const int n = a.size();

    const auto indexOfNextGreaterThan = findIndexOfNextGreaterThan(a);
    const auto indexOfPrevGreaterThan = findIndexOfPrevGreaterThan(a);

    int64_t maxIndexProduct = 0;

    for (int i = 0; i < n; i++)
    {
        const int64_t left_i = indexOfPrevGreaterThan[i] + 1;
        const int64_t right_i = indexOfNextGreaterThan[i] + 1;

        const int64_t indexProduct_i = left_i * right_i;

        maxIndexProduct = max(maxIndexProduct, indexProduct_i);
    }

    return maxIndexProduct;
}

int main()
{
    // Trivially easy, if you've done Min Max Riddle (the heavy lifting
    // is done by findIndexOfNextGreaterThan, which is copy-n-pasted,
    // with minor modifications, from there!), of which this is a minor
    // variant.
    int n;
    cin >> n;

    vector<int> a(n);

    for (int i = 0; i < n; i++)
    {
        cin >> a[i];
    }

    cout << findMaxIndexProduct(a) << endl;
}


