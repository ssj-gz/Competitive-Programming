#define INTCODE_SILENT

#include "../shared/intcodecomputer.h"

#include <iostream>
#include <vector>
#include <deque>
#include <limits>

using namespace std;

int main()
{
    vector<int64_t> program;
    int64_t programInput;
    while (true)
    {
        cin >> programInput;
        program.push_back(programInput);
        assert(cin);
        char comma;
        cin >> comma;
        if (!cin)
            break;
        assert(comma == ',');
    }

    struct Range
    {
        int xStart = 1; 
        int xEnd = 1;
    };

    auto isAffected = [&program](int x, int y)
    {
        IntCodeComputer intCodeComputer(program);
        const auto status = intCodeComputer.run();
        assert(status == IntCodeComputer::WaitingForInput);

        intCodeComputer.addInput(x);
        intCodeComputer.addInput(y);

        const auto statusAfterInput = intCodeComputer.run();
        assert(statusAfterInput == IntCodeComputer::Terminated);

        const auto output = intCodeComputer.takeOutput();
        assert(output.size() == 1);
        return (output[0] == 1);
    };

    const int width = 150;
    const int height = 150;
    vector<string> blah(height, string(width, ' '));

    // Apart from the origin itself, this is the closest point 
    // to the origin that is Affected.
    int x = 4, y = 5;
    deque<Range> lastUpTo100Ranges = { {x, x} };
    const int desiredSquareSize = 100;
    while (true)
    {
        y++;
        // Find next range.
        const Range previousRange = lastUpTo100Ranges.back();
        std::cout << "Computing range for y: " << y << " previous range: " << previousRange.xStart << " - " << previousRange.xEnd << std::endl;
        int newXStart = previousRange.xStart;
        if (isAffected(newXStart, y))
        {
            do
            {
                newXStart--;
                assert(newXStart >= 0);
            }
            while (isAffected(newXStart, y));
            newXStart++;
        }
        else
        {
            do
            {
                newXStart++;
            }
            while (!isAffected(newXStart, y));
        }
        int newXEnd = previousRange.xEnd;
        if (isAffected(newXEnd, y))
        {
            do
            {
                newXEnd++;
            }
            while (isAffected(newXEnd, y));
            newXEnd--;
        }
        else
        {
            do
            {
                newXEnd--;
            }
            while (newXEnd >= 0 && !isAffected(newXEnd, y));
            if (newXEnd == -1)
            {
                newXEnd = previousRange.xStart;
                do
                {
                    newXEnd++;
                }
                while (!isAffected(newXEnd, y));
                do
                {
                    newXEnd++;
                }
                while (isAffected(newXEnd, y));
                newXEnd--;
            }
            else
            {
                newXEnd++;
            }
        }

        Range newRange(newXStart, newXEnd);
        std::cout << "Range: y: " << y << " newXStart: " << newXStart << " newXEnd: " << newXEnd << std::endl;
        assert(isAffected(newXStart, y));
        assert(!isAffected(newXStart - 1, y));
        assert(isAffected(newXEnd, y)); 
        assert(!isAffected(newXEnd + 1, y));

        lastUpTo100Ranges.push_back(newRange);
        if (lastUpTo100Ranges.size() > desiredSquareSize)
        {
            assert(lastUpTo100Ranges.size() == desiredSquareSize + 1);
            lastUpTo100Ranges.pop_front();
        }
        std::cout << "lastUpTo100Ranges size: " << lastUpTo100Ranges.size() << std::endl;
        if (lastUpTo100Ranges.size() == desiredSquareSize)
        {
            int maxRangeStartX = -1;
            int minRangeEndX = std::numeric_limits<int>::max();
            for (const auto& range : lastUpTo100Ranges)
            {
                maxRangeStartX = max(maxRangeStartX, range.xStart);
                minRangeEndX = min(minRangeEndX, range.xEnd);
            }
            //assert(minRangeEndX >= maxRangeStartX);
            if (minRangeEndX - maxRangeStartX + 1 >= desiredSquareSize)
            {
                const int topX = maxRangeStartX;
                const int topY = y - desiredSquareSize + 1;
                int numSquaresChecked = 0;
                int numSquaresAffected = 0;
                for (int x = topX; x < topX + desiredSquareSize; x++)
                {
                    for (int y = topY; y < topY + desiredSquareSize; y++)
                    {
                        if (isAffected(x, y))
                            numSquaresAffected++;
                        numSquaresChecked++;
                    }

                }
                assert(numSquaresChecked == desiredSquareSize * desiredSquareSize);
                assert(numSquaresAffected == desiredSquareSize * desiredSquareSize);
                //std::cout << "woo! y: " << y << " maxRangeStartX: " << maxRangeStartX << " minRangeEndX: " << minRangeEndX << std::endl;
                std::cout << "woo! topX: " << topX << " topY: " << topY << std::endl;
                cout << "Result: " << (topX * 10'000 + topY) << std::endl;
                break;
            }

        }

    }


}


