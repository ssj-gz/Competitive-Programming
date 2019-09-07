// Simon St James (ssjgz) - 2019-09-07
// 
// Solution to: https://www.codechef.com/SEPT19B/problems/FIBEASY
//
#include <iostream>
#include <vector>

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


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int64_t N = read<int64_t>(); 

        int64_t fn_2 = 0;
        int64_t fn_1 = 1;
        cout << (fn_2 % 10) << endl;
        cout << (fn_1 % 10) << endl;

        int64_t fn_2_mod10 = 0;
        int64_t fn_1_mod10 = 1;

        for (int i = 0; i < N - 2; i++)
        {
            int64_t fn = fn_2 + fn_1;
            cout << "fn: " <<  fn << " mod 10: " << (fn % 10) << endl;
            int64_t fn_mod10 = (fn_2_mod10 + fn_1_mod10) % 10;
            cout << "fn_mod10: " << fn_mod10 << endl;

            assert(fn_mod10 == (fn % 10));

            fn_2 = fn_1;
            fn_1 = fn;

            fn_2_mod10 = fn_1_mod10;
            fn_1_mod10 = fn_mod10;
        }
    }

    assert(cin);
}
