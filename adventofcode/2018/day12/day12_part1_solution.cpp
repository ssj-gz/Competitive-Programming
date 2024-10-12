#include <iostream>
#include <vector>
#include <map>
#include <limits>
#include <optional>

#include <cassert>

using namespace std;

int main()
{

    auto readString = []()
    {
        string str;
        cin >> str;
        return str;
    };

    string initialStateLabel = readString();
    initialStateLabel += " " + readString();
    std::cout << "initialStateLabel: " << initialStateLabel << std::endl;
    assert(initialStateLabel == "initial state:");
    const string initialState = readString();

    struct Note
    {
        string lastGenPattern;
        bool hasPotThisGen = false;
    };
    vector<Note> notes;

    while (true)
    {
        Note note;
        note.lastGenPattern = readString();
        std::cout << "lastGenPattern: " << note.lastGenPattern <<  std::endl;
        assert(note.lastGenPattern.size() == 5 || !cin);
        if (!cin)
            break;
        const auto arrow = readString();
        string hasPotThisGen = readString();
        assert(hasPotThisGen == "." || hasPotThisGen == "#");
        note.hasPotThisGen = (hasPotThisGen == "#");
        notes.push_back(note);
    }
    map<int, bool> state;
    for (int i = 0; i < initialState.size(); i++)
    {
        state[i] = (initialState[i] == '#');
    }

    auto printState = [](const map<int, bool>& state)
    {
        assert(!state.empty());
        int minPotIndex = std::numeric_limits<int>::max();
        int maxPotIndex = std::numeric_limits<int>::min();
        for (const auto& [potIndex, hasPlant] : state)
        {
            minPotIndex = std::min(minPotIndex, potIndex);
            maxPotIndex = std::max(maxPotIndex, potIndex);
        }
        int numSpacesToPrint = 0;
        for (int i = minPotIndex; i <= maxPotIndex; i++)
        {
            if (i % 3 == 0)
            {
                std::cout << i;
                numSpacesToPrint = 3 - std::to_string(i).size();
            }
            else
            {
                if (numSpacesToPrint != 0)
                {
                    std::cout << " ";
                    numSpacesToPrint--;
                }
            }
        }
        std::cout << std::endl;
        for (int i = minPotIndex; i <= maxPotIndex; i++)
        {
            auto hasPlantIter = state.find(i);
            assert(hasPlantIter != state.end());
            if (hasPlantIter->second)
                cout << '#';
            else
                cout << '.';
        }
        std::cout << std::endl;
    };


    for (int generation = 1; generation <= 20; generation++)
    {
        std::cout << "begin generation: " << generation << " state: " << std::endl;
        printState(state);

        int minPotIndex = std::numeric_limits<int>::max();
        int maxPotIndex = std::numeric_limits<int>::min();
        for (const auto& [potIndex, hasPlant] : state)
        {
            minPotIndex = std::min(minPotIndex, potIndex);
            maxPotIndex = std::max(maxPotIndex, potIndex);
        }

        map<int, bool> nextState;
        for (int i = minPotIndex - 3; i <= maxPotIndex + 3; i++)
        {
            string lastGenPattern;
            for (int j = i - 2; j <= i + 2; j++)
            {
                if (auto lastHasPlantIter = state.find(j); lastHasPlantIter != state.end() && lastHasPlantIter->second)
                    lastGenPattern += '#';
                else
                    lastGenPattern += '.';
            }
            //std::cout << "blah: >" << lastGenPattern << "<" << std::endl;
            assert(lastGenPattern.size() == 5);
            std::optional<Note> matchingNote;
            for (const auto& note : notes)
            {
                if (note.lastGenPattern == lastGenPattern)
                {
                    assert(!matchingNote.has_value());
                    matchingNote = note;
                }
            }
            //assert(matchingNote.has_value());
            if (matchingNote.has_value() && matchingNote.value().hasPotThisGen)
                nextState[i] = true;
            else
                nextState[i] = false;
        }

        state = nextState;

    }
    std::cout << "Final state: " << std::endl;
    printState(state);

    int64_t result = 0;
    for (const auto& [potIndex, hasPlant] : state)
    {
        if (hasPlant)
            result += potIndex;
    }
    std::cout << "result: " << result << std::endl;

    
    return 0;
}
