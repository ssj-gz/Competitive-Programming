// Simon St James (ssjgz) - 2019-10-31
// 
// Solution to: https://www.codechef.com/problems/SMPAIR
//
#include <iostream>
#include <vector>
#include <algorithm>

#include <cassert>

using namespace std;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

int sumOfTwoSmallest(vector<int> a)
{
    // Easiest way is to sort a and take two smallest elements,
    // but this approach is O(N) instead of O(N log2 N) :)
    auto minElementIter = std::min_element(a.begin(), a.end());
    const auto minElement = *minElementIter;

    a.erase(minElementIter);

    const auto secondMinElement = *std::min_element(a.begin(), a.end());

    return minElement + secondMinElement;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();

        vector<int> a(N);
        for (auto& aValue : a)
            aValue = read<int>();

        cout << sumOfTwoSmallest(a) << endl;
    }

    assert(cin);
}
