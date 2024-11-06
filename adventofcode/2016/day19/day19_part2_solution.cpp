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

    auto advanceIter = [&elves](auto& iter)
    {
        iter++;
        if (iter == elves.end())
            iter = elves.begin();

    };


    auto elfIter = elves.begin();
    auto oppositeElfIter = elfIter;
    for (int i = 0; i < elves.size() / 2; i++)
    {
        advanceIter(oppositeElfIter);
    }

    while (elves.size() != 1)
    {
        elfIter->numPresents += oppositeElfIter->numPresents;
        oppositeElfIter = elves.erase(oppositeElfIter);
        if (oppositeElfIter == elves.end())
        {
            oppositeElfIter = elves.begin();
        }

        advanceIter(elfIter);
        if (elves.size() % 2 != 1)
            advanceIter(oppositeElfIter);
    }
    std::cout << "Winning elf: " << elves.front().id << std::endl;

    return 0;
}
