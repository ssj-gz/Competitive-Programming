#include <iostream>
#include <algorithm>
#include <cassert>

using namespace std;

int main()
{
    string rucksackContents;
    int totalSharedPriority = 0;
    while (std::getline(cin, rucksackContents))
    {
        const string firstCompartmentContents{rucksackContents.begin(), rucksackContents.begin() + rucksackContents.size() / 2};
        const string secondCompartmentContents{rucksackContents.begin() + rucksackContents.size() / 2, rucksackContents.end()};

        assert(firstCompartmentContents.size() == secondCompartmentContents.size());

        for (const auto item : secondCompartmentContents)
        {
            if (std::find(firstCompartmentContents.begin(), firstCompartmentContents.end(), item) != firstCompartmentContents.end())
            {
                const int priority = ('a' <= item && item <= 'z') ? (item - 'a' + 1) : (item - 'A' + 27);
                totalSharedPriority += priority;
                break;
            }
        }
    }
    std::cout << totalSharedPriority << std::endl;
}
