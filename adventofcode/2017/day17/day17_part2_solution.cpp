#include <iostream>
#include <vector>
#include <algorithm>

#include <cassert>

using namespace std;

int main()
{
    int stepsToMove = -1;
    cin >> stepsToMove;

    int currentPosition = 0;
    int bufferSize = 1;
    int valueAfter0 = -1;
    for (int count = 1; count <= 50'000'000; count++)
    {
        currentPosition = (currentPosition + stepsToMove) % static_cast<int>(bufferSize);
        if (currentPosition == 0)
        {
            std::cout << "inserted: " << count << " after 0" << std::endl;
            valueAfter0 = count;
        }
        bufferSize++;
        currentPosition = (currentPosition + 1) % static_cast<int>(bufferSize);
    }
    std::cout << "result: " << valueAfter0 << std::endl;
    return 0;
}
