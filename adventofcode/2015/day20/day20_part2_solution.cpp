#include <iostream>

#include <cassert>

using namespace std;

int main()
{
    int64_t requiredNumPresents = -1;
    cin >> requiredNumPresents;
    assert(requiredNumPresents > 0);
    int64_t houseNumber = 1;
    while (true)
    {
        int64_t numPresents = 0;
        for (int64_t factor = 1; factor * factor <= houseNumber; factor++)
        {
            if (houseNumber % factor == 0)
            {
                const auto numVisitsFromElf = houseNumber / factor;
                if (numVisitsFromElf <= 50)
                {
                    numPresents += factor * 11;
                }
                const auto otherFactor = houseNumber / factor;
                const auto numVisitsFromOtherElf = houseNumber / otherFactor;
                if (otherFactor != factor && numVisitsFromOtherElf <= 50)
                {
                    numPresents += otherFactor * 11;
                }
            }
            
        }
        std::cout << "House " << houseNumber << " got " << numPresents << " presents" << std::endl;
        if (numPresents >= requiredNumPresents)
        {
            std::cout << "result: " << houseNumber << std::endl;
            break;
        }

        houseNumber++;
    }
    return 0;
}
