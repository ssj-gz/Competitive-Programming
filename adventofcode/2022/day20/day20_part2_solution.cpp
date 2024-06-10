#include <iostream>
#include <list>
#include <algorithm>
#include <cassert>

using namespace std;

int main()
{
    struct Number
    {
        int64_t number = 0;
        int originalIndex = -1;
    };
    list<Number> numbers;
    int64_t number;
    while (cin >> number)
    {
        numbers.push_back({number * 811589153, static_cast<int>(numbers.size()) });
    }
    for (int mixNum = 1; mixNum <= 10; mixNum++)
    {

        for (int origIndexToMove = 0; origIndexToMove < numbers.size(); origIndexToMove++)
        {
#if 0
            std::cout << "current: " << std::endl;
            for (const auto& number : numbers)
            {
                std::cout << number.number << " ";
            }
            std::cout << std::endl;
#endif
            const auto numberToMoveIter = std::find_if(numbers.begin(), numbers.end(), [origIndexToMove](const auto& number) { return (number.originalIndex == origIndexToMove); });
            assert(numberToMoveIter != numbers.end()); 

            const auto numberToMoveCopy = *numberToMoveIter;

            const auto stepsToMove = abs(numberToMoveIter->number) % (numbers.size() - 1); // We end up right back where we started every "numbers.size() - 1" movements.
            //std::cout << "Moving " << numberToMoveIter->number << " by " << numberToMoveIter->number << std::endl;
            if (stepsToMove == 0)
                continue;
            enum Direction { Left, Right } directionToMove = (numberToMoveIter->number < 0 ? Left : Right);

            //numberToMoveIter = numbers.erase(numberToMoveIter);
            bool erased = false;
            if (directionToMove == Right)
            {
                auto desiredIterToRight = std::next(numberToMoveIter);
                numbers.erase(numberToMoveIter);
                for (int64_t i = 0; i < stepsToMove; i++)
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
                for (int64_t i = 0; i < stepsToMove; i++)
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

#if 0
            for (int i = 0; i < stepsToMove; i++)
            {
                if (directionToMove == Left)
                {
                    if (numberToMoveIter == numbers.begin() || std::prev(numberToMoveIter) == numbers.begin())
                        numberToMoveIter = std::prev(numbers.end());
                    else
                    {
                        numberToMoveIter--;
                    }
                }
                else if (directionToMove == Right)
                {
                    if (numberToMoveIter == numbers.end() || std::next(numberToMoveIter) == numbers.end())
                        numberToMoveIter = std::next(numbers.begin());
                    else
                        numberToMoveIter++;
                }
                else
                {
                    assert(false);
                }
            }
#endif
#if 0
            if (numberToMoveIter == numbers.end() || std::next(numberToMoveIter) == numbers.end())
                numberToMoveIter = numbers.begin();
            else
                numberToMoveIter++;
#endif

            //numbers.insert(numberToMoveIter, numberToMoveCopy);
        }
        std::cout << "After mix number " << mixNum  << std::endl;
        for (const auto& number : numbers)
        {
            std::cout << number.number << " ";
        }
        std::cout << std::endl;
    }

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
