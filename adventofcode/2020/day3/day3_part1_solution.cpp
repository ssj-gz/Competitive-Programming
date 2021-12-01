#include <iostream>
#include <vector>

using namespace std;

int main()
{
    vector<string> pattern;
    string patternRow;
    while (cin >> patternRow)
    {
        pattern.push_back(patternRow);
    }
    const int numRows = pattern.size();
    const int patternRepeatEvery = pattern.front().size();

    int positionX = 0;
    int treesEncounteredCount = 0;
    for (int row = 1; row < numRows; row++)
    {
        positionX = (positionX + 3) % patternRepeatEvery;
        if (pattern[row][positionX] == '#')
            treesEncounteredCount++;
    }
    cout << treesEncounteredCount << endl;
}
