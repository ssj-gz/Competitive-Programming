#include <iostream>
#include <list>

#include <cassert>

using namespace std;

int main()
{
    int numElves = -1;
    cin >> numElves;
    assert(numElves != -1);
    struct Elf
    {
        int id = -1;
        int numPresents = 1;
    };
    list<Elf> elves;
    for (int i = 1; i <= numElves; i++)
    {
        elves.push_back({i, 1});
    }

    auto elfIter = elves.begin();
    while (elves.size() != 1)
    {
        auto nextElfIter = std::next(elfIter);
        if (nextElfIter == elves.end())
            nextElfIter = elves.begin();

        elfIter->numPresents += nextElfIter->numPresents;
        assert(nextElfIter != elfIter);
        std::cout << "erasing elf: " << nextElfIter->id << std::endl;
        elves.erase(nextElfIter);

        elfIter++;
        if (elfIter == elves.end())
            elfIter = elves.begin();
    }
    std::cout << "Winning elf: " << elves.front().id << std::endl;

    return 0;
}
