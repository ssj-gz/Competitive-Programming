// Simon St James (ssjgz) - 2019-08-14
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

bool isRainbow(const vector<int>& originalA)
{
    if (originalA != vector<int>(originalA.rbegin(), originalA.rend()))
    {
        // Rainbow's must be palindromes.
        return false;
    }

    vector<int> withAdjacentDupesRemoved = originalA;
    withAdjacentDupesRemoved.erase(std::unique(withAdjacentDupesRemoved.begin(), withAdjacentDupesRemoved.end()), withAdjacentDupesRemoved.end());

    if (withAdjacentDupesRemoved != vector<int>({1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1}))
    {
        return false;
    }

    return true;
}


int main()
{
    const int T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();

        vector<int> a(N);
        for(auto& x : a)
        {
            x = read<int>();
        }

        if (isRainbow(a))
            cout << "yes";
        else
            cout << "no";
        cout << endl;
    }
}

