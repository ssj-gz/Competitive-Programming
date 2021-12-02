#include <iostream>
#include <algorithm>
#include <cassert>

using namespace std;

int main()
{
    string boardingPass;
    vector<int> allSeatIds;
    vector<int> mySeatIdPossibilities;
    for (int i = 0; i <= 1023; i++)
        mySeatIdPossibilities.push_back(i);
    //vector<int> permittedMissingSeatIds;
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
            //cout << "letter: " << boardingPass[i] << " new rowMin: " << rowMin << " new rowMax: " << rowMax << endl;
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
            //cout << "letter: " << boardingPass[i] << " new colMin: " << colMin << " new colMax: " << colMax << endl;
        }
        assert(colMax == colMin);
        const int col = colMin;

        const int seatId = row * 8 + col;
        //cout << "seatId: " << seatId << endl;
        if (const auto iter = find(mySeatIdPossibilities.begin(), mySeatIdPossibilities.end(), seatId); iter != mySeatIdPossibilities.end())
        {
            //cout << "erasing: " << seatId << endl;
            mySeatIdPossibilities.erase(iter);
        }
        //if (row == 0 || row == 7)
            //permittedMissingSeatIds.push_back(seatId);
        seatIdsInList.push_back(seatId);
    }
    bool foundMySeat = false;
    for (const auto seatId : mySeatIdPossibilities)
    {
        const int row = seatId / 8;
        if (row == 0 || row == 7)
            continue;
        if ((find(seatIdsInList.begin(), seatIdsInList.end(), seatId - 1) == seatIdsInList.end()) ||
            (find(seatIdsInList.begin(), seatIdsInList.end(), seatId + 1) == seatIdsInList.end()))
        {
            continue;
        }
        assert(!foundMySeat);
        cout << seatId << endl;
        foundMySeat = true;
    }

}
