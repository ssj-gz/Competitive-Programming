#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cassert>

using namespace std;

int main()
{
    int64_t currentElfCalories = 0;

    vector<int64_t> caloriesForElves;

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
            // Finished with current elf.
            std::cout << "Elf had: " << currentElfCalories << " calories" << std::endl;
            caloriesForElves.push_back(currentElfCalories);

            currentElfCalories = 0;
        }
    }

    assert(currentElfCalories == 0);
    assert(caloriesForElves.size() >= 3);
    sort(caloriesForElves.begin(), caloriesForElves.end(), std::greater<>());
    std::cout << "Sum of top three Elves's calories: "  << std::accumulate(caloriesForElves.begin(), caloriesForElves.begin() + 3, static_cast<int64_t>(0)) << std::endl;
}
