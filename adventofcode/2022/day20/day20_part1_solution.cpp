#include <iostream>
#include <list>
#include <algorithm>
#include <cassert>

using namespace std;

int main()
{
    struct Number
    {
        int number = 0;
        int originalIndex = -1;
    };
    list<Number> numbers;
    int number;
    while (cin >> number)
    {
        numbers.push_back({number, static_cast<int>(numbers.size()) });
    }

    for (int origIndexToMove = 0; origIndexToMove < static_cast<int>(numbers.size()); origIndexToMove++)
    {
        const auto numberToMoveIter = std::find_if(numbers.begin(), numbers.end(), [origIndexToMove](const auto& number) { return (number.originalIndex == origIndexToMove); });
        assert(numberToMoveIter != numbers.end()); 

        const auto numberToMoveCopy = *numberToMoveIter;

        const int stepsToMove = abs(numberToMoveIter->number);
        if (stepsToMove == 0)
            continue;
        enum Direction { Left, Right } directionToMove = (numberToMoveIter->number < 0 ? Left : Right);

        if (directionToMove == Right)
        {
            auto desiredIterToRight = std::next(numberToMoveIter);
            numbers.erase(numberToMoveIter);
            for (int i = 0; i < stepsToMove; i++)
            {
                if (desiredIterToRight == numbers.end())
                    desiredIterToRight = std::next(numbers.begin());
                else
                    desiredIterToRight++;
            }
            if (desiredIterToRight == numbers.begin())
            {
                numbers.push_back(numberToMoveCopy);
            }
            else
            {
                numbers.insert(desiredIterToRight, numberToMoveCopy);
            }
        }
        else
        {
            auto desiredIterToLeft = (numberToMoveIter == numbers.begin() ? std::prev(numbers.end()) : std::prev(numberToMoveIter));
            numbers.erase(numberToMoveIter);
            for (int i = 0; i < stepsToMove; i++)
            {
                if (desiredIterToLeft == numbers.begin())
                {
                    desiredIterToLeft = std::prev(numbers.end());
                }
                else
                {
                    desiredIterToLeft--;
                }
            }
            if (desiredIterToLeft == std::prev(numbers.end()))
            {
                numbers.push_back(numberToMoveCopy);
            }
            else
            {
                numbers.insert(std::next(desiredIterToLeft), numberToMoveCopy);
            }

        }
    }

    // Mix complete - find the final answer (counting the 1000th, 2000th and 3000th elements after
    // the 0, with wraparound).
    auto iterToCount = std::find_if(numbers.begin(), numbers.end(), [](const auto& number) { return (number.number == 0); });
    assert(iterToCount != numbers.end());
    int64_t total = 0;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 1000; j++)
        {
            iterToCount++;
            if (iterToCount == numbers.end())
                iterToCount = numbers.begin();
        }
        total += iterToCount->number;
    }

    std::cout << "total: " << total << std::endl;



}
