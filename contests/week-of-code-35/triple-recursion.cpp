#include <iostream>
#include <vector>

using namespace std;

int main()
{
    int n, m, k;
    cin >> n >> m >> k;

    vector<vector<int>> a(n, vector<int>(n, -1));

    a[0][0] = m;

    for (int diag = 1; diag < n; diag++)
    {
        a[diag][diag] = a[diag - 1][diag - 1] + k;
    }

    for (int j = 0; j < n; j++)
    {
        for (int i = j + 1; i < n; i++)
        {
            a[i][j] = a[i - 1][j] - 1;
        }
    }
    for (int j = 0; j < n; j++)
    {
        for (int i = 0; i < j; i++)
        {
            a[i][j] = a[i][j - 1] - 1;
        }
    }

    for (int j = 0; j < n; j++)
    {
        for (int i = 0; i < n; i++)
        {
            cout << a[i][j];
            if (i != n - 1)
                cout << " ";
        }
        cout << endl;
    }

}
