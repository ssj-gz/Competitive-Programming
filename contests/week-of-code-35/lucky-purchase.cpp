#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include <limits>
using namespace std;


int main() {
    int n;
    cin >> n;

    int64_t lowestValidPrice = numeric_limits<int64_t>::max();
    string lowestCompanyName;
    for (int i = 0; i < n; i++)
    {
        string companyName;
        cin >> companyName;

        int64_t price;
        cin >> price;

        string priceAsString = to_string(price);

        int num7sInPrice = 0;
        int num4sInPrice = 0;
        bool containsNon4Or7 = false;
        for (const auto priceDigit : priceAsString)
        {
            if (priceDigit == '7')
                num7sInPrice++;
            else if (priceDigit == '4')
                num4sInPrice++;
            else
            {
                containsNon4Or7 = true;
                break;
            }
        }
        if (!containsNon4Or7 && num7sInPrice == num4sInPrice)
        {
            if (price < lowestValidPrice)
            {
                lowestValidPrice = price;
                lowestCompanyName = companyName;
            }
        }
    }

    if (lowestCompanyName.empty())
        cout << -1 << endl;
    else
        cout << lowestCompanyName << endl;

    return 0;
}
