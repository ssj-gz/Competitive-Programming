// Simon St James (ssjgz) 2018-02-23
#include <iostream>

using namespace std;

int main()
{
    int p, d, m, s;
    cin >> p >> d >> m >> s;

    int numItemsBought = 0;
    int currentItemPrice = p;
    while (s > 0)
    {
        if (currentItemPrice <= m)
            currentItemPrice = m;

        s -= currentItemPrice;
        currentItemPrice -= d;

        if (s >= 0)
            numItemsBought++;
    }

    cout << numItemsBought << endl;
}
