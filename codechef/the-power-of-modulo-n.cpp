// Simon St James (ssjgz) - 2020-02-20
// 
// Solution to: https://www.codechef.com/problems/CGPOWN
//
#include <iostream>
#include <boost/multiprecision/cpp_int.hpp>

using namespace std;
using namespace boost::multiprecision;

template <typename T>
T read()
{
    T toRead;
    cin >> toRead;
    assert(cin);
    return toRead;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const auto a = read<cpp_int>();
        const auto b = read<cpp_int>();
        const auto n = read<cpp_int>();

        cout << powm(a, b, n) << endl;

    }

    assert(cin);
}
