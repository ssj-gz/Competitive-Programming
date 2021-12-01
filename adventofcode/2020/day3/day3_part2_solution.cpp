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

    const vector<pair<int, int>> slopes = { 
        {1, 1 },
        {3, 1 },
        {5, 1 },
        {7, 1 },
        {1, 2 }
    };


    int64_t answer = 1;
    for (const auto [right, down] : slopes)
    {
        int positionX = 0;
        int treesEncounteredCount = 0;
        for (int row = down; row < numRows; row += down)
        {
            positionX = (positionX + right) % patternRepeatEvery;
            if (pattern[row][positionX] == '#')
                treesEncounteredCount++;
        }
        //cout << treesEncounteredCount << endl;
        answer *= treesEncounteredCount;
    }
    cout << answer << endl;
}
