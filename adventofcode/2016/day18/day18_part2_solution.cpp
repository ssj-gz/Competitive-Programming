#include <iostream>
#include <algorithm>

#include <cassert>

using namespace std;

int main()
{
    string firstRow;
    getline(cin, firstRow);

    const int width = firstRow.size();

    std::cout << "firstRow: " << firstRow << std::endl;
    string previousRow = firstRow;
    int numSafe = std::count(firstRow.begin(), firstRow.end(), '.');
    constexpr int totalRows = 400000;
    for (int row = 1; row <= totalRows - 1; row++)
    {
        string newRow;
        for (int x = 0; x < width; x++)
        {
            const auto leftIsTrap = (x >= 0 && previousRow[x - 1] == '^');
            const auto centreIsTrap = (previousRow[x] == '^');
            const auto rightIsTrap = (x + 1 < width && previousRow[x + 1] == '^');
            if ((leftIsTrap && centreIsTrap && !rightIsTrap) ||
                (centreIsTrap && rightIsTrap && !leftIsTrap) ||
                (leftIsTrap && !centreIsTrap && !rightIsTrap) ||
                (rightIsTrap && !centreIsTrap && !leftIsTrap))
            {
                newRow += '^';
            }
            else
            {
                newRow += '.';
            }

        }
        std::cout << "newRow  : " << newRow << std::endl;
        numSafe  += std::count(newRow.begin(), newRow.end(), '.');
        previousRow = newRow;
    }


    std::cout << "numSafe: " << numSafe << std::endl;

    return 0;
}
