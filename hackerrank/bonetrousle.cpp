#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;


int main() {
    long T;
    cin >> T;
    for (long t = 0; t < T; t++)
    {
        long n, k, b;
        cin >> n >> k >> b;
        if (k < b)
        {
            cout << "-1" << endl;
            continue;
        }
        // Start off by purchasing the minimum number of bones.
        // Note that we always maintain purchases[i + 1] > purchases[i]
        // for i = 0, 1, ... b - 2.
        vector<long> purchases(b);
        long totalBonesPurchased = 0;
        for (int i = 0; i < b; i++)
        {
            purchases[i] = i + 1;
            totalBonesPurchased += purchases[i];
        }
        if (k == b && totalBonesPurchased != n)
        {
            cout << "-1" << endl;
            continue;
        }
        if (totalBonesPurchased > n)
        {
            cout << "-1" << endl;
            continue;
        }

        // If we're here, we need to (and are able to) purchase
        // more bones. Basic approach: starting from the last purchase,
        // attempt to change it to a more expensive item that gives us
        // enough bones.  If we can't do this even if we change our purchase
        // to the current most expensive unpurchased one, then consider the
        // next-to-last purchase and repeat, etc.
        long mostExpensiveUnpurchased = k;
        long currentPurchaseIndex = b - 1;
        while (totalBonesPurchased < n && currentPurchaseIndex >= 0)
        {

            const long currentPurchase = purchases[currentPurchaseIndex];
            const long maxIncreaseForThisPurchase = mostExpensiveUnpurchased - currentPurchase;
            const long boneShortfall = n - totalBonesPurchased;
            const long increasePurchaseBy = min(boneShortfall, maxIncreaseForThisPurchase);

            purchases[currentPurchaseIndex] += increasePurchaseBy;
            totalBonesPurchased += increasePurchaseBy;
            currentPurchaseIndex--;
            if (increasePurchaseBy == maxIncreaseForThisPurchase)
                mostExpensiveUnpurchased--;
        }

        if (totalBonesPurchased != n)
        {
            cout << "-1" << endl;
        }
        else
        {
            for (int i = 0; i < b; i++)
            {
                cout << purchases[i];
                if (i != b - 1)
                {
                    cout << " ";
                }
            }
            cout << endl;
        }


    }
    return 0;
}

