#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

int main()
{
    int n, m, k;
    cin >> n >> m >> k;

    vector<vector<int>> a(n, vector<int>(n, -1));

    a[0][0] = m;

    for (int j = 0; j < n; j++)
    {
        for (int i = 0; i < n; i++)
        {
            int& cellValue = a[i][j];
            if (i == j && i != 0)
                cellValue = a[i - 1][j - 1] + k;
            else if (i > j)
                cellValue = a[i - 1][j] - 1;
            else if (i < j && j != 0)
                cellValue = a[i][j - 1] - 1;

            cout << cellValue;
            if (i != n - 1)
                cout << " ";
        }
        cout << endl;
    }

}
