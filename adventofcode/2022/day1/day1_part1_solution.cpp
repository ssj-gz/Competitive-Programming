#include <iostream>
#include <cassert>

using namespace std;

int main()
{
    int64_t currentElfCalories = 0;
    int64_t largestCalories = -1;

    bool processedAllElves = false;

    while (!processedAllElves)
    {
        string line;
        if (!std::getline(cin, line))
            processedAllElves = true;

        if (!line.empty())
        {
            const int64_t numCalories = std::stoll(line);
            currentElfCalories += numCalories;
        }
        else
        {
            std::cout << "Elf had: " << currentElfCalories << " calories" << std::endl;
            // Finished with current elf.
            largestCalories = max(largestCalories, currentElfCalories);

            currentElfCalories = 0;
        }
    }

    assert(currentElfCalories == 0);
    std::cout << "largestCalories: " << largestCalories << std::endl;
}
