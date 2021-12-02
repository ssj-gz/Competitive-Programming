#include <iostream>
#include <algorithm>
#include <cassert>

using namespace std;

int main()
{
    string boardingPass;
    vector<int> seatIdsInList;
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
        }
        assert(colMax == colMin);
        const int col = colMin;

        const int seatId = row * 8 + col;

        seatIdsInList.push_back(seatId);
    }
    bool foundMySeat = false;
    for (int seatId = 0; seatId <= 1023; seatId++)
    {
        const int row = seatId / 8;
        if (row == 0 || row == 7)
        {
            // We know our seat is not in front or back row.
            continue;
        }
        if (find(seatIdsInList.begin(), seatIdsInList.end(), seatId) != seatIdsInList.end())
        {
            // We know our seat is not amongst the list of other passenger's seat ids.
            continue;
        }
        if ((find(seatIdsInList.begin(), seatIdsInList.end(), seatId - 1) == seatIdsInList.end()) ||
            (find(seatIdsInList.begin(), seatIdsInList.end(), seatId + 1) == seatIdsInList.end()))
        {
            // We know the seats with id +/- 1 of ours are in the list.
            continue;
        }
        assert(!foundMySeat);
        cout << seatId << endl;
        foundMySeat = true;
    }

}
