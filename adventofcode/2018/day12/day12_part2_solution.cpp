#include <iostream>
#include <vector>
#include <set>
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
    set<int64_t> state;
    for (int i = 0; i < static_cast<int>(initialState.size()); i++)
    {
        if (initialState[i] == '#')
        {
            state.insert(i);
        }
    }

    auto printState = [](const set<int64_t>& state)
    {
        assert(!state.empty());
        int64_t minPotIndex = std::numeric_limits<int64_t>::max();
        int64_t maxPotIndex = std::numeric_limits<int64_t>::min();
        for (const auto& potIndex : state)
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
            if (state.contains(i))
                cout << '#';
            else
                cout << '.';
        }
        std::cout << std::endl;
    };

    auto normalisedState = [](const set<int64_t>& state)
    {
        assert(!state.empty());
        const int64_t minPotIndex = *state.begin();
        set<int64_t> normalisedState;
        for (const auto potIndex : state)
        {
            normalisedState.insert(potIndex - minPotIndex);
        }
        return normalisedState;
    };


    //set<set<int>> seenStates;
    int generation = 1;
    //int genOfFirstRepetition = -1;
    //int repetitionMinPotIndex = -1;
    constexpr int64_t desiredGeneration = 50'000'000'000LL;
    //constexpr int64_t desiredGeneration = 200LL;
    bool resultPrinted = false;
    int64_t genOfFirstRepetition = -1;
    set<int64_t> firstRepetitionState;

    while (true)
    {
        std::cout << "begin generation: " << generation << " state: " << std::endl;
        printState(state);

        int64_t minPotIndex = std::numeric_limits<int64_t>::max();
        int64_t maxPotIndex = std::numeric_limits<int64_t>::min();
        for (const auto potIndex : state)
        {
            minPotIndex = std::min(minPotIndex, potIndex);
            maxPotIndex = std::max(maxPotIndex, potIndex);
        }

#if 0
        if (seenStates.contains(normalisedState))
        {
            std::cout << "woo" << std::endl;
            break;
        }
        seenStates.insert(normalisedState);
#endif

        set<int64_t> nextState;
        for (int i = minPotIndex - 3; i <= maxPotIndex + 3; i++)
        {
            string lastGenPattern;
            for (int j = i - 2; j <= i + 2; j++)
            {
                if (state.contains(j))
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
                nextState.insert(i);
        }

        //if (seenStates.contains(nextState))
        //{
        //    std::cout << "woo" << std::endl;
        //    break;
        //}
        if (normalisedState(nextState) == normalisedState(state) && firstRepetitionState.empty())
        {
            std::cout << "sausage:" << std::endl;
            firstRepetitionState = nextState;
            assert(*nextState.begin() == *state.begin() + 1);
            std::cout << " desiredGeneration: " << desiredGeneration << std::endl;
            genOfFirstRepetition = generation;
            std::cout << "firstRepetitionState: " << std::endl;
            for (const auto& blah : firstRepetitionState)
                std::cout << blah << " ";
            std::cout << std::endl;
            std::cout << "genOfFirstRepetition: " << genOfFirstRepetition << std::endl;
#if 1
            const int minOfRepetitionState = *firstRepetitionState.begin();
            set<int64_t> finalState;
            for (const int64_t potIndex : normalisedState(nextState))
            {
                finalState.insert(potIndex + minOfRepetitionState + (desiredGeneration - genOfFirstRepetition));
            }
            int64_t result = 0;
            for (const int64_t potIndex : finalState)
            {
                result += potIndex;
            }
            std::cout << "result: " << result << std::endl;
            resultPrinted = true;
#endif
            break;
        }
#if 1
        if (!firstRepetitionState.empty())
        {
            const int64_t minOfRepetitionState = *firstRepetitionState.begin();
            std::cout << "minOfRepetitionState: " << minOfRepetitionState << std::endl;
            set<int64_t> optState;
            for (const auto potIndex : normalisedState(firstRepetitionState))
            {
                optState.insert(potIndex + minOfRepetitionState + (generation - genOfFirstRepetition));
            }
            std::cout << "optState: generation: #" << generation << std::endl;
            for (const auto& blah : optState)
                std::cout << blah << " ";
            std::cout << std::endl;
            std::cout << "debugState: " << std::endl;
            for (const auto& blah : nextState)
                std::cout << blah << " ";
            std::cout << std::endl;
            assert(optState == nextState);
        }
#endif

        state = nextState;
        //seenStates.insert(state);
        generation++;
        //previousNormalisedState = normalisedState;

    }
#if 1
    std::cout << "Final state: " << std::endl;
    printState(state);

    int64_t result = 0;
    for (const auto& potIndex : state)
    {
        result += potIndex;
    }
    std::cout << "result: " << result << std::endl;
#endif

    
    return 0;
}
