#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

enum CellType { Good, Bad, Occupied};
CellType cell[225][225];
int N, M;

bool tryIncreaseSize(int row, int column, int currentSize)
{
    // "currentSize == 1" means no extent i.e. just a single square.
    if (row - currentSize < 0 || row + currentSize >= N)
        return false;
    if (column - currentSize < 0 || column + currentSize >= M)
        return false;
    if (cell[row + currentSize][column] == Bad) 
        return false;
    if (cell[row - currentSize][column] == Bad) 
        return false;
    if (cell[row][column + currentSize] == Bad) 
        return false;
    if (cell[row][column - currentSize] == Bad) 
        return false;

    return true;
}

// Find the largest size of the plus centred at row, column which does not overlap
// the other plus defined by otherRow, otherColumn, otherSize.
int maxSizeNotOverlapping(int row, int column, int otherRow, int otherColumn, int otherSize)
{
    const int otherLeft = otherColumn - (otherSize - 1);
    const int otherRight = otherColumn + (otherSize - 1);
    const int otherTop = otherRow - (otherSize - 1);
    const int otherBottom = otherRow + (otherSize - 1);
    int maxSize = 1;
    while (true)
    {
        const int myLeft = column - (maxSize - 1) ;
        const int myRight = column + (maxSize - 1) ;
        const int myTop = row - (maxSize - 1) ;
        const int myBottom = row + (maxSize - 1) ;
        if (myLeft < 0)
        {
            break;
        }
        if (myRight >= M)
        {
            break;
        }
        if (myTop < 0)
        {
            break;
        }
        if (myBottom >= N)
        {
            break;
        }

        if (row >= otherTop && row <= otherBottom)
        {
            if (abs(column - otherColumn) < maxSize)
            {
                break;
            }
        }
        if (column >= otherLeft && column <= otherRight)
        {
            if (abs(row - otherRow) < maxSize)
            {
                break;
            }
        }

        if (column == otherColumn)
        {
            if (abs(row - otherRow) <= (maxSize - 1) + (otherSize - 1))
            {
                break;
            }
        }
        if (row == otherRow)
        {
            if (abs(column - otherColumn) <= (maxSize - 1) + (otherSize - 1))
            {
                break;
            }
        }

        maxSize++;
    }
    maxSize--; // Went bust!
    return maxSize;

}

int plusArea(int size)
{
    if (size == 0)
        return 0;
    const int length = ((size - 1) * 2) + 1;
    return length + length - 1; // "-1" ignores the centre point.
}

struct CellMaxSize 
{
    int row;
    int column;
    int maxSize;
};
bool  operator<(const CellMaxSize& lhs, const CellMaxSize& rhs)
{
    // Sort with largest first.
    return lhs.maxSize > rhs.maxSize;
}

int main() {
    // Read in and store all data.
    cin >> N >> M;
    for (int row = 0; row < N; row++)
    {
        string rowString;
        cin >> rowString;
        for (int column = 0; column < M; column++)
        {
            if (rowString[column] == 'G')
                cell[row][column] = Good;
            else
                cell[row][column] = Bad;
        }
    }
    // For each cell, find the maximum size size of a plus centred at that cell.
    // For the moment, we ignore the possibility of overlapping with other plusses.
    vector<CellMaxSize> cellsByMaxSize;
    for (int row = 0; row < N; row++)
    {
        for (int column = 0; column < M; column++)
        {
            if (cell[row][column] == Good) 
            {
                int maxSize = 1;
                while (tryIncreaseSize(row, column, maxSize))
                {
                    maxSize++;
                }
                CellMaxSize cellMaxSize;
                cellMaxSize.row = row;
                cellMaxSize.column = column;
                cellMaxSize.maxSize = maxSize;
                cellsByMaxSize.push_back(cellMaxSize);
            }
        }
    }
    sort(cellsByMaxSize.begin(), cellsByMaxSize.end());
    int maxAreaProduct = 0;
    // Starting with the cells with the largest max cell size, try to 
    // place a pair of pluses at the cells in such a way that they
    // don't overlap; shrink the pluses below their maximum size at their cell if 
    // necessary.  Calculate the area product for this placement, and
    // maintain the maxAreaProduct.
    for (int i = 0; i < cellsByMaxSize.size(); i++)
    {
        CellMaxSize plus1Cell = cellsByMaxSize[i];
        for (int j = i; j < cellsByMaxSize.size(); j++)
        {
            CellMaxSize plus2Cell = cellsByMaxSize[j];

            for (int k = plus1Cell.maxSize; k >= 1; k--)
            {
                int plus2CellMaxSize = maxSizeNotOverlapping(plus2Cell.row, plus2Cell.column, plus1Cell.row, plus1Cell.column, k);
                plus2CellMaxSize = std::min(plus2CellMaxSize, plus2Cell.maxSize);
                const int areaProduct = plusArea(k) * plusArea(plus2CellMaxSize);
                maxAreaProduct = std::max(maxAreaProduct, areaProduct);
            }
        }
    }
    cout << maxAreaProduct;
    return 0;
}

