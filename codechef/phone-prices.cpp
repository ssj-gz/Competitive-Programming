// Simon St James (ssjgz) - 2019-10-16
// 
// Solution to: https://www.codechef.com/problems/S10E
//
//#define DIAGNOSTICS // Uncomment to have Diagnostic output - showing which individual days are Good or not, plus the reason why.
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

int findNumGoodDays(const vector<int>& priceForDay)
{
    int numGoodDays = 0;
    for (int day = 0; day < priceForDay.size(); day++)
    {
        const int currentPrice = priceForDay[day];
        bool isGood = true;
#ifdef DIAGNOSTICS
        cout << "Day: " << (day + 1) << " out of " << priceForDay.size() << " ";
#endif
        for (int previousDay = day - 1; day - previousDay <= 5 && previousDay >= 0; previousDay--)
        {
            if (priceForDay[previousDay] <= currentPrice)
            {
                isGood = false;
#ifdef DIAGNOSTICS
                cout << "Not good - price is " << priceForDay[previousDay] << " on day " << (previousDay + 1) << " which is >= " << currentPrice << endl;
#endif
                break;
            }

        }

        if (isGood)
        {
#ifdef DIAGNOSTICS
            cout << "Good" << endl;
#endif
            numGoodDays++;
        }

    }
    
    return numGoodDays;
}


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    
    const auto T = read<int>();
    assert(1 <= T && T <= 100);

    for (int t = 0; t < T; t++)
    {
        const int N = read<int>();
        assert(N >= 7 && N <= 100);

        vector<int> priceForDay(N);

        for (auto& price : priceForDay)
        {
            price = read<int>();
            assert(350 <= price && price <= 750);
        }

        cout << findNumGoodDays(priceForDay) << endl;
    }

    assert(cin);
}
