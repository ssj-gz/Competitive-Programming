#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

void rotate(vector<vector<int> >& matrix, int row, int col)
{
    const int M = matrix.size();
    if (M == 0)
        return;
    const int N = matrix[0].size();

    // Rotate top row to the left.
    const int lostTopLeft = matrix[row][col];
    for (int i = col + 1; i <= N - 1 - col; i++)
    {
        matrix[row][i - 1] = matrix[row][i];
    }
    // Add missing top-right.
    matrix[row][N - 1 - col] = matrix[row + 1][N - 1 - col];

    // Rotate left row downwards.
    const int lostBottomLeft = matrix[M - 1 - row][col];
    for (int j = M - 1 - row; j >= row + 2; j--)
    {
        matrix[j][col] = matrix[j - 1][col];
    }
    // Fill in.
    matrix[row + 1][col] = lostTopLeft;

    // Rotate bottom row rightwards.
    const int lostBottomRight = matrix[M - 1 - row][N - 1 - col];
    for (int i = N - 1 - col; i >= col + 1; i--)
    {
        matrix[M - 1 - row][i] = matrix[M - 1 - row][i - 1];
    }
    // Fill in.
    matrix[M - 1 - row][col + 1] = lostBottomLeft;

    // Rotate rightmost row upwards. Don't have to worry about losing
    // anything: we took care of that when we shifted the top row left.
    for (int j = row; j <= M - 1 - row - 1; j++)
    {
        matrix[j][N - 1 - col] = matrix[j + 1][N - 1 - col];
    }
    // Fill in.
    matrix[M - 1 - row - 1][N - 1 - col] = lostBottomRight;
}

void printMatrix(const vector<vector<int> >& matrix)
{
    const int M = matrix.size();
    if (M == 0)
        return;
    const int N = matrix[0].size();

    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }

}


int main() {
    int M, N, R;
    cin >> M >> N >> R;
    vector<vector<int> > matrix(M, vector<int>(N));
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            cin >> matrix[i][j];
        }
    }

    int rotationRow = 0;
    int rotationCol = 0;
    for (int i = 0; i < min(M, N) / 2; i++)
    {
        const int innerRows = M - 2 * i;
        const int innerCols = N - 2 * i;
        // If we rotate by perimeter of this inner matrix, we end up
        // where we started!
        const int innerPerimeter =  2 * innerRows + 2 * innerCols - 4;
        for (int j = 0; j < R % innerPerimeter; j++)
        {
            rotate(matrix, rotationRow, rotationCol);
        }

        rotationRow++;
        rotationCol++;
    }
    printMatrix(matrix);
    return 0;
}

