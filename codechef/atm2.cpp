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

int main()
{
    const int T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();
        const int K = read<int>();

        vector<int> moneyToWithdraw(N);
        for (auto& amount : moneyToWithdraw)
        {
            amount = read<int>();
        }

        int amountInMachine = K;

        for (const auto& amountToWithdraw : moneyToWithdraw)
        {
            if (amountInMachine >= amountToWithdraw)
            {
                cout << "1";
                amountInMachine -= amountToWithdraw;
            } 
            else
            {
                cout << "0";
            }
        }

        cout << endl;
        
    }



}
