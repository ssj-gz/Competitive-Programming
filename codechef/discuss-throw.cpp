// Simon St James (ssjgz) - 2022-03-31
// 
// Solution to: https://www.codechef.com/MARCH221D/problems/DISCUS
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

template <typename T>
vector<T> readVector(typename vector<T>::size_type numElements)
{
    vector<T> toRead(numElements);
    for (auto& element : toRead)
        element = read<T>();
    return toRead;
}

int main()
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const auto discussDistances = readVector<int>(3);
        cout << *max_element(discussDistances.begin(), discussDistances.end()) << endl;
    }

    assert(cin);
}
