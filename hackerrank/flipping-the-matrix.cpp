#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;


int main() {
    long Q;
    cin >> Q;
    for (int q = 0; q < Q; q++)
    {
        long n;
        cin >> n;
        vector<vector<long>> matrix(2 *n, vector<long>(2 * n));
        for (int i = 0; i < 2 * n; i++)
        {
            for (int j = 0; j < 2 * n; j++)
            {
                cin >> matrix[i] [j];
            }
        }
        // Basic insight.  In the upper-left quadrant, the highest possible value
        // for cell matrix[i][j] (0 <= i  < n, 0 <= j < n ) is the max of the four 
        // "corners" of the submatrix centred inside the whole matrix and starting
        // at (i,j).  As it happens, with a little bit of ingenuity, it is possible to contrive
        // a sequence of the allowed operations such that each such cell has its maximum
        // value, *without depriving other such cells of their maximum values*,
        // and doing this would give us our maximum upper-left quadrant sum.  We don't 
        // need to actually perform these operations, of course, since we know what the 
        // effect of applying them would be!
        long highestTopLeftQuadrantSum = 0;
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                const long finalRowOrColIndex = 2 * n - 1;
                const long highestCellContent = max({
                        matrix[i][j], 
                        matrix[finalRowOrColIndex - i][j], 
                        matrix[finalRowOrColIndex - i][finalRowOrColIndex - j], 
                        matrix[i][finalRowOrColIndex - j]});
                highestTopLeftQuadrantSum += highestCellContent;
            }
        }
        cout << highestTopLeftQuadrantSum << endl;

    }
    return 0;
}

