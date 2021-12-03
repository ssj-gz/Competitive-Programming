#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int countOccupiedNeighbouringSeat(const vector<string>& seatLayout, int row, int col)
{
    const int layoutHeight = seatLayout.size();
    const int layoutWidth = seatLayout.front().size();

    int result = 0;
    for (int neighbourRow = row - 1; neighbourRow <= row + 1; neighbourRow++)
    {
        for (int neighbourCol = col - 1; neighbourCol <= col + 1; neighbourCol++)
        {
            if (neighbourRow == row && neighbourCol == col)
                continue;
            if (neighbourRow < 0 || neighbourRow >= layoutHeight 
             || neighbourCol < 0 || neighbourCol >= layoutWidth)
            {
                continue;
            }
            if (seatLayout[neighbourRow][neighbourCol] == '#')
            {
                result++;
            }
        }
    }
    return result;
}

int main()
{
    vector<string> seatLayout;
    string layoutRow;
    while (cin >> layoutRow)
        seatLayout.push_back(layoutRow);

    const int layoutHeight = seatLayout.size();
    const int layoutWidth = seatLayout.front().size();

    int numRounds = 0;
    while (true)
    {
        vector<string> nextSeatLayout(seatLayout);

        for (int row = 0; row < layoutHeight; row++)
        {
            for (int col = 0; col < layoutWidth; col++)
            {
                const auto state = seatLayout[row][col];
                if (state == 'L')
                {
                    if (countOccupiedNeighbouringSeat(seatLayout, row, col) == 0)
                        nextSeatLayout[row][col] = '#';
                }
                else if (state == '#')
                {
                    if (countOccupiedNeighbouringSeat(seatLayout, row, col) >= 4)
                        nextSeatLayout[row][col] = 'L';
                }
            }
        }

#if 0
        cout << "After iteration: " << endl;
        for (const auto line : nextSeatLayout)
        {
            cout << line << endl;
        }
#endif

        numRounds++;
        if (nextSeatLayout == seatLayout)
            break;

        seatLayout = nextSeatLayout;
    }
    int numOccupiedSeats = 0;
    for (const auto& line : seatLayout)
    {
        numOccupiedSeats += count(line.begin(), line.end(), '#');
    }
    cout << numOccupiedSeats << endl;
}
