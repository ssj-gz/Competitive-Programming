// Simon St James (ssjgz) - 2019-11-01
// 
// Solution to: https://www.codechef.com/problems/TICKETS5
//
#include <iostream>

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

bool isLuckyTicket(const string& ticket)
{
    if (ticket[0] == ticket[1])
        return false;
    for (int i = 2; i < ticket.size(); i++)
    {
        if (ticket[i] != ticket[i - 2])
            return false;
    }
    return true;
}

int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();

    for (int t = 0; t < T; t++)
    {
        const auto ticket = read<string>();

        cout << (isLuckyTicket(ticket) ? "YES" : "NO") << endl;
    }

    assert(cin);
}
