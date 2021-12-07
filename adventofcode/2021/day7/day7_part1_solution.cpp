#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>
#include <sstream>
#include <cassert>

using namespace std; 

int main()
{
    string positionsLine;
    getline(cin, positionsLine);
    std::replace(positionsLine.begin(), positionsLine.end(), ',', ' ');
    istringstream positionsStream(positionsLine);

    int64_t position = -1;
    vector<int64_t> crabPositions;
    while (positionsStream >> position)
    {
        crabPositions.push_back(position);
    }

    int64_t smallestCodeToAlignAll = numeric_limits<int64_t>::max();
    for (int64_t alignedPosition = *min_element(crabPositions.begin(), crabPositions.end()); 
         alignedPosition <= *max_element(crabPositions.begin(), crabPositions.end());
         alignedPosition++)
    {
        int64_t costToAlignAll = 0;
        for (const auto crabPosition : crabPositions)
        {
            const auto distanceToMove = abs(crabPosition - alignedPosition);
            const auto cost = distanceToMove * (distanceToMove + 1) / 2;
            costToAlignAll += cost;
        }
        smallestCodeToAlignAll = min(smallestCodeToAlignAll, costToAlignAll);
    }
    cout << smallestCodeToAlignAll << endl;

}


