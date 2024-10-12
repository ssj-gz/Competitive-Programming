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


    constexpr int64_t desiredGeneration = 50'000'000'000LL;
    int generation = 1;

    while (true)
    {
        int64_t minPotIndex = std::numeric_limits<int64_t>::max();
        int64_t maxPotIndex = std::numeric_limits<int64_t>::min();
        for (const auto potIndex : state)
        {
            minPotIndex = std::min(minPotIndex, potIndex);
            maxPotIndex = std::max(maxPotIndex, potIndex);
        }

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
            if (matchingNote.has_value() && matchingNote.value().hasPotThisGen)
                nextState.insert(i);
        }

        if (normalisedState(nextState) == normalisedState(state))
        {
            // All basically repetitions (offset by 1 each generation) from here on in.
            assert(*nextState.begin() == *state.begin() + 1);
            const auto firstRepetitionState = nextState;
            const auto genOfFirstRepetition = generation;

            const int minOfRepetitionState = *firstRepetitionState.begin();
            int64_t result = 0;
            for (const int64_t potIndex : normalisedState(firstRepetitionState))
            {
                result += potIndex + minOfRepetitionState + (desiredGeneration - genOfFirstRepetition);
            }
            std::cout << "result: " << result << std::endl;
            break;
        }

        state = nextState;
        generation++;

    }
    
    return 0;
}
