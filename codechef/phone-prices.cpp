// Simon St James (ssjgz) - 2019-10-16
// 
// Solution to: https://www.codechef.com/problems/S10E
//
#define DIAGNOSTICS
#include <iostream>
#include <vector>

#include <cassert>

#include <sys/time.h> // TODO - this is only for random testcase generation.  Remove it when you don't need new random testcases!

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
    int result = 0;
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
            result++;
        }

    }
    
    return result;
}


int main(int argc, char* argv[])
{
    ios::sync_with_stdio(false);
    if (argc == 2 && string(argv[1]) == "--test")
    {
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
        // TODO - generate randomised test.
        //const int T = rand() % 100 + 1;
        const int T = 1;
        cout << T << endl;

        for (int t = 0; t < T; t++)
        {
            const int N = (rand() % 3) + 7;

            cout << N << endl;
            for (int i = 0; i < N; i++)
            {
                cout << ((rand() % 401) + 350);
                if (i != N - 1)
                    cout << " ";
            }
            cout << endl;

        }

        return 0;
    }
    
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
