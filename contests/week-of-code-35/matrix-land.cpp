#include <iostream>
#include <vector>

using namespace std;

int64_t maxSum(int startRow, int startCol, const vector<vector<int>>& A, vector<vector<int64_t>>& lookup)
{
    const int numRows = A.size();
    const int numCols = A[0].size();
    
    if (startRow == numRows)
        return 0;

    if (lookup[startRow][startCol] != -1)
        return lookup[startRow][startCol];

    int64_t best = 0;
    for (int clearToLeftCol = 0; clearToLeftCol <= startCol; clearToLeftCol++)
    {
        for (int clearToRightCol = startCol; clearToRightCol < numCols; clearToRightCol++)
        {
            int64_t scoreAfterClearing = 0;
            for (int i = clearToLeftCol; i <= clearToRightCol; i++)
            {
                scoreAfterClearing += A[startRow][i];
            }
            for (int descendCol = clearToLeftCol; descendCol <= clearToRightCol; descendCol++)
            {
                const auto bestIfClearAndDescendHere = scoreAfterClearing + maxSum(startRow + 1, descendCol, A, lookup);
                best = max(best, bestIfClearAndDescendHere); 
            }
        }
    }

    assert(lookup[startRow][startCol] == -1);
    lookup[startRow][startCol] = best;
    return best;

}

int main()
{
    int n, m;
    cin >> n >> m;

    vector<vector<int>> A(n, vector<int>(m, -1));

    for (int row = 0; row < n; row++)
    {
        for (int col = 0; col < m; col++)
        {
            cin >> A[row][col];
        }
    }

    vector<vector<int64_t>> lookup(n, vector<int64_t>(m, -1));
    int64_t best = 0;
    for (int startCol = 0; startCol < m; startCol++)
    {
        best = max(best, maxSum(0, startCol, A, lookup));
    }
    cout << best << endl;
}
