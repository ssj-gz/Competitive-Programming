// Simon St James (ssjgz) - 2019-12-20
// 
// Solution to: https://www.codechef.com/problems/CMPRSS/
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

string compress(const vector<int>& a)
{
    string compressed;
    for (int rangeBeginIndex = 0; rangeBeginIndex < a.size();)
    {
        // As with STL iterators, rangeEndIndex is the index *one past* the range
        // of consecutive numbers beginning at index rangeBeginIndex.
        int rangeEndIndex = rangeBeginIndex + 1;
        for (; rangeEndIndex < a.size(); rangeEndIndex++)
        {
            if (a[rangeEndIndex] != a[rangeBeginIndex] + (rangeEndIndex - rangeBeginIndex))
                break;
        }
        if (rangeEndIndex - rangeBeginIndex > 2)
        {
            compressed += to_string(a[rangeBeginIndex]) + "..." + to_string(a[rangeEndIndex - 1]);
        }
        else if (rangeEndIndex - rangeBeginIndex > 1)
        {
            compressed += to_string(a[rangeBeginIndex]) + "," + to_string(a[rangeEndIndex - 1]);
        }
        else
        {
            compressed += to_string(a[rangeBeginIndex]);
        }
        rangeBeginIndex = rangeEndIndex;
        if (rangeBeginIndex != a.size())
            compressed += ",";
    }
    
    return compressed;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();
        vector<int> a(N);
        for (auto& x : a)
            x = read<int>();

        cout << compress(a) << endl;
    }

    assert(cin);
}
