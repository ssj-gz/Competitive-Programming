// Simon St James (ssjgz) - 2020-08-08
// 
// Solution to: https://www.codechef.com/problems/BFTT/
//
#include <iostream>
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

int main()
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const auto N = read<int>();
        auto candidateAnswer = N + 1;
        auto numDigit3s = [](const auto& numberAsString)
        {
            return count(numberAsString.begin(), numberAsString.end(), '3');
        };
        while (numDigit3s(to_string(candidateAnswer)) < 3)
            candidateAnswer++;
        cout << candidateAnswer << endl;
    }

    assert(cin);
}
