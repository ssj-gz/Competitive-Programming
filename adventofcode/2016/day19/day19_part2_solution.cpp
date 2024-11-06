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

    cout << "numElves: " << numElves << std::endl;
    auto elfIter = elves.begin();
    auto oppositeElfIter = elfIter;
    for (int i = 0; i < elves.size() / 2; i++)
    {
        advanceIter(oppositeElfIter);
    }
    std::cout << " at begin - elfIter: " << elfIter->id << " oppositeElfIter id: " << oppositeElfIter->id << std::endl;
    while (elves.size() != 1)
    {
        //std::cout << "# elves: " << elves.size() << std::endl;
#if 0
        for (const auto x : elves)
            std::cout << x.id << " ";
        std::cout << std::endl;
        std::cout << " current: " << elfIter->id << std::endl;
        auto debugOppositeElfIter = elfIter;
        for (int i = 0; i < elves.size() / 2; i++)
        {
            advanceIter(debugOppositeElfIter);
            std::cout <<  "  advanced debugOppositeElfIter; now " << debugOppositeElfIter->id << std::endl;
        }
        std::cout << "   debugOppositeElfIter id: " << debugOppositeElfIter->id << std::endl;
        std::cout << "   oppositeElfIter id: " << oppositeElfIter->id << std::endl;
        assert(debugOppositeElfIter == oppositeElfIter);
        assert(elfIter != oppositeElfIter);
#endif
        elfIter->numPresents += oppositeElfIter->numPresents;
        //std::cout << "   elft " << elfIter->id << " stealing from elf: " << oppositeElfIter->id << std::endl;
        oppositeElfIter = elves.erase(oppositeElfIter);
        if (oppositeElfIter == elves.end())
        {
            oppositeElfIter = elves.begin();
            //std::cout << "    after deletion, before increment: reset oppositeElfIter to " <<  oppositeElfIter->id << std::endl;
        }
        else
        {
            //std::cout << "    after deletion, before increment: set oppositeElfIter to " <<  oppositeElfIter->id << std::endl;
        }

        advanceIter(elfIter);
        if (elves.size() % 2 != 1)
            advanceIter(oppositeElfIter);
    }
    std::cout << "Winning elf: " << elves.front().id << std::endl;

    return 0;
}
