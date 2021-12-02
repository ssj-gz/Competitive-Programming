#include <iostream>
#include <cassert>

using namespace std;

int main()
{
    string boardingPass;
    int maxSeatId = -1;
    while (cin >> boardingPass)
    {
        int rowMin = 0;
        int rowMax = 127;
        for (int i = 0; i < 7; i++)
        {
            if (boardingPass[i] == 'F')
            {
                rowMax = rowMin + (rowMax - rowMin) / 2;
            }
            else if (boardingPass[i] == 'B')
            {
                rowMin = rowMin + (rowMax - rowMin + 1) / 2;
            }
            else
            {
                assert(false);
            }
            cout << "letter: " << boardingPass[i] << " new rowMin: " << rowMin << " new rowMax: " << rowMax << endl;
        }
        assert(rowMax == rowMin);
        const int row = rowMin;

        int colMin = 0;
        int colMax = 7;
        for (int i = 7; i < 10; i++)
        {
            if (boardingPass[i] == 'L')
            {
                colMax = colMin + (colMax - colMin) / 2;
            }
            else if (boardingPass[i] == 'R')
            {
                colMin = colMin + (colMax - colMin + 1) / 2;
            }
            else
            {
                assert(false);
            }
            cout << "letter: " << boardingPass[i] << " new colMin: " << colMin << " new colMax: " << colMax << endl;
        }
        assert(colMax == colMin);
        const int col = colMin;

        const int seatId = row * 8 + col;
        //cout << "seatId: " << seatId << endl;
        maxSeatId = max(maxSeatId, seatId);
    }
    cout << maxSeatId << endl;

}
