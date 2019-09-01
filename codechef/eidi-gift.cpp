// Simon St James (ssjgz) - 2019-09-01
//
// Solution to https://www.codechef.com/problems/EID2/
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
    
    const int T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int numChildren = 3;
        vector<int> ages(numChildren);
        for (auto& age : ages)
        {
            age = read<int>();
        }
        vector<int> moneyGiven(numChildren);
        for (auto& money : moneyGiven)
        {
            money = read<int>();
        }

        bool fair = true;
        for (int index1 = 0; index1 < numChildren; index1++)
        {
            for (int index2 = 0; index2 < numChildren; index2++)
            {
                if (ages[index1] < ages[index2] && !(moneyGiven[index1] < moneyGiven[index2]))
                {
                    fair = false;
                }
                if (ages[index1] == ages[index2] && !(moneyGiven[index1] == moneyGiven[index2]))
                {
                    fair = false;
                }
            }
        }

        cout << ( fair ? "FAIR" : "NOT FAIR") << endl;
    }

    assert(cin);
}
