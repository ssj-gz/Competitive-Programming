// Simon St James (ssjgz) - 2020-01-21
// 
// Solution to: https://www.codechef.com/problems/ANKTRAIN
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

string findBerthPartner(int berthNumber)
{
    static const string berthNames[] = 
    {
        "LB", // 1
        "MB", // 2
        "UB", // 3
        "LB", // 4
        "MB", // 5,
        "UB", // 6
        "SL", // 7
        "SU", // 8
    };

    static const int berthPartner[] = 
    {
        4, // 1
        5, // 2
        6, // 3
        1, // 4
        2, // 5
        3, // 6
        8, // 7
        7, // 8
    };

    const int carriageNum = (berthNumber - 1) / 8;
    const int berthNumberReduced = ((berthNumber - 1) % 8); // 0-relative.
    const int berthPartnerIndex = berthPartner[berthNumberReduced] - 1; // 0-relative.
    const int berthPartnerNumber = carriageNum * 8 + berthPartnerIndex + 1;
    const string berthPartnerName = berthNames[berthPartnerIndex];
    
    return to_string(berthPartnerNumber) + berthPartnerName;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const int berthNumber = read<int>();
        cout << findBerthPartner(berthNumber) << endl;
    }

    assert(cin);
}
