#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;

int main()
{
    string rucksackContents;
    int totalSharedPriority = 0;
    vector<string> rucksacksContents;
    while (std::getline(cin, rucksackContents))
    {
        rucksacksContents.push_back(rucksackContents);
    }

    assert(rucksacksContents.size() % 3 == 0);

    while (!rucksacksContents.empty())
    {
        std::cout << "New batch of three" << std::endl;
        vector<string> threeRucksacksContents{rucksacksContents.begin(), rucksacksContents.begin() + 3};
        assert(threeRucksacksContents.size() == 3);
        rucksacksContents.erase(rucksacksContents.begin(), rucksacksContents.begin() + 3);

        for (int priority = 1; priority <= 52; priority++)
        {
            const char itemWithPriority = (1 <= priority && priority <= 26 ? ('a' + priority - 1) : ('A' + priority - 27));

            int numRacksacksItemAppearsIn = 0;
            std::cout << "Trying: " << itemWithPriority << std::endl;
            for (const auto& rucksackContents : threeRucksacksContents)
            {
                std::cout << " searching: " << rucksackContents << std::endl;
                if (std::find(rucksackContents.begin(), rucksackContents.end(), itemWithPriority) != rucksackContents.end())
                {
                    numRacksacksItemAppearsIn++;
                }
            }
            std::cout << "Item: " << itemWithPriority << " occurs in " << numRacksacksItemAppearsIn << " of the three rucksacks" << std::endl;
            if (numRacksacksItemAppearsIn == 3)
            {
                totalSharedPriority += priority;
                break;
            }

        }
    }

    std::cout << totalSharedPriority << std::endl;
}
