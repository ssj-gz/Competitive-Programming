#include <iostream>
#include <regex>

#include <cassert>

using namespace std;

int main()
{
    // Disc #1 has 5 positions; at time=0, it is at position 4.
    std::regex discDescrRegex(R"(^Disc #(\d+) has (\d+) positions; at time=(\d+), it is at position (\d+)\.$)");
    string discDescr;

    struct Disc
    {
        int id = -1;
        int numPositions = -1;
        int startPosition = -1;
    };

    vector<Disc> discs;

    while (getline(cin, discDescr))
    {
        std::smatch discDescrMatch;
        const bool matchSuccessful = std::regex_match(discDescr, discDescrMatch, discDescrRegex);
        assert(matchSuccessful);
        Disc disc;
        disc.id = std::stoi(discDescrMatch[1]);;
        disc.numPositions = std::stoi(discDescrMatch[2]);
        disc.startPosition = std::stoi(discDescrMatch[4]);

        discs.push_back(disc);
    }

    const Disc extraDisc = { discs.size() + 1, 11, 0 };
    discs.push_back(extraDisc);

    int timeToWait = 0;
    while (true)
    {
        if (timeToWait % 100'000 == 0)
            std::cout << "Trying with timeToWait: " << timeToWait << std::endl;
        vector<Disc> discsCopy = discs;
        // Fast-forward.
        for (auto& disc : discsCopy)
        {
            disc.startPosition = (disc.startPosition + timeToWait) % disc.numPositions;
        }

        bool passedThroughAllDiscs = true;
        for (auto& disc : discsCopy)
        {
            const int timeToReachDisc = disc.id;
            const int currentDiscPosition = (disc.startPosition + timeToReachDisc) % disc.numPositions;
            if (currentDiscPosition != 0)
            {
                passedThroughAllDiscs = false;
            }
        }
        if (passedThroughAllDiscs)
        {
            break;
        }

        timeToWait++;
    }
    std::cout << "result: " << timeToWait << std::endl;

    return 0;
}
