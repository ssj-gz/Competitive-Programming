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
        for (int childIndex1 = 0; childIndex1 < numChildren; childIndex1++)
        {
            for (int childIndex2 = 0; childIndex2 < numChildren; childIndex2++)
            {
                if (ages[childIndex1] < ages[childIndex2] && !(moneyGiven[childIndex1] < moneyGiven[childIndex2]))
                {
                    fair = false;
                }
                if (ages[childIndex1] == ages[childIndex2] && !(moneyGiven[childIndex1] == moneyGiven[childIndex2]))
                {
                    fair = false;
                }
            }
        }

        cout << ( fair ? "FAIR" : "NOT FAIR") << endl;
    }

    assert(cin);
}
