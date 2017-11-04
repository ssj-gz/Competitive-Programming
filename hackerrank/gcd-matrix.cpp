#include <iostream>
#include <vector>
#include <set>
#include <string>

using namespace std;

int gcd(int a, int b)
{
    while (b != 0)
    {
        int t = b; 
        b = a % b; 
        a = t; 
    }
    return a;
}

int findResultBruteForce(const vector<int>& a, const vector<int>& b, int r1, int c1, int r2, int c2)
{
    set<int> gcds;
    for (int i = r1; i <= r2; i++)
    {
        for (int j = c1; j <= c2; j++)
        {
            gcds.insert(gcd(a[i], b[j]));
        }
    }
    return gcds.size();
}


int main()
{
    int n, m, q;

    cin >> n >> m >> q;

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

    for (int i = 0; i < q; i++)
    {
        int r1, c1, r2, c2;

        cin >> r1 >> c1 >> r2 >> c2;

        const auto resultBruteForce = findResultBruteForce(a, b, r1, c1, r2, c2);
        cout << resultBruteForce << endl;
    }
}

