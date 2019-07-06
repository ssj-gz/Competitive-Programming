#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main()
{
    int numGames;
    cin >> numGames;

    for (int g = 0; g < numGames; g++)
    {
        int n;
        cin >> n;
        int m;
        cin >> m;
        int x;
        cin >> x;

        vector<int> a(n);
        for (int i = 0; i < n; i++)
        {
            cin >> a[i];
        }
        vector<int> b(m);
        for (int i = 0; i < m; i++)
        {
            cin >> b[i];
        }
        reverse(a.begin(), a.end());
        reverse(b.begin(), b.end());

        int indexTakenFromB = m - 1;
        int64_t sumTakenFromB = 0;
        for (const auto bElement : b)
            sumTakenFromB += bElement;

        a.insert(a.begin(), 0);

        int64_t sumTakenFromA = 0;
        int largestNumMoves = 0;
        int numTakenFromA = 0;
        for (const auto aElement : a)
        {
            sumTakenFromA += aElement;
            while (indexTakenFromB != -1 && (sumTakenFromA + sumTakenFromB > x))
            {
                sumTakenFromB -= b[indexTakenFromB];
                indexTakenFromB--;
            }
            if (sumTakenFromA + sumTakenFromB <= x)
            {
                const int numMoves = numTakenFromA + (indexTakenFromB + 1);
                largestNumMoves = max(largestNumMoves, numMoves);
            }
        }
        cout << largestNumMoves << endl;
    }

    
}
