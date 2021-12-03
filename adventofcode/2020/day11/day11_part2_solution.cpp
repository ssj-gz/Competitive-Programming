#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int countOccupiedVisibleSeat(const vector<string>& seatLayout, int row, int col)
{
    const int layoutHeight = seatLayout.size();
    const int layoutWidth = seatLayout.front().size();

    int result = 0;

    for (int dx = -1; dx <= 1; dx++)
    {
        for (int dy = -1; dy <= 1; dy++)
        {
            if (dx == 0 && dy == 0)
                continue;

            int currentRow = row;
            int currentCol = col;
            while (true)
            {
                currentRow += dy;
                currentCol += dx;
                if (currentRow < 0 || currentRow >= layoutHeight 
                 || currentCol < 0 || currentCol >= layoutWidth)
                {
                    break;
                }
                if (seatLayout[currentRow][currentCol] != '.')
                {
                    if (seatLayout[currentRow][currentCol] == '#')
                        result++;

                    break;
                }
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
                    if (countOccupiedVisibleSeat(seatLayout, row, col) == 0)
                        nextSeatLayout[row][col] = '#';
                }
                else if (state == '#')
                {
                    if (countOccupiedVisibleSeat(seatLayout, row, col) >= 5)
                        nextSeatLayout[row][col] = 'L';
                }
            }
        }

#if 1
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
