#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <map>

#include <cassert>

using namespace std;

vector<char> programLineupAfterDance(const vector<char>& initialProgramLineup, const string& danceInstructionsOrig)
{
    const int numPrograms = initialProgramLineup.size();
    auto programLineup = initialProgramLineup;
    auto danceInstructions = danceInstructionsOrig;
    std::replace(danceInstructions.begin(), danceInstructions.end(), ',', ' ');

    istringstream danceInstructionStream(danceInstructions);
    string instruction;
    while (danceInstructionStream >> instruction)
    {
        std::replace(instruction.begin(), instruction.end(), '/', ' ');
        istringstream instructionStream(instruction);
        char instructionType = '\0';
        instructionStream >> instructionType;
        switch (instructionType)
        {
            case 's':
                {
                    int spinAmount = -1;
                    instructionStream >> spinAmount;
                    assert(spinAmount >= 0 && spinAmount < numPrograms);
                    // TODO - use std::rotate instead?
                    const vector<char> endPrograms(programLineup.end() - spinAmount, programLineup.end());
                    programLineup.erase(programLineup.end() - spinAmount, programLineup.end());
                    programLineup.insert(programLineup.begin(), endPrograms.begin(), endPrograms.end());
                }
                break;
            case 'x':
                {
                    int posA = -1;
                    int posB = -1;
                    instructionStream >> posA;
                    instructionStream >> posB;
                    assert(posA >= 0 && posA < numPrograms);
                    assert(posB >= 0 && posB < numPrograms);
                    swap(programLineup[posA], programLineup[posB]);
                }
                break;
            case 'p':
                {
                    char programA = '\0';
                    char programB = '\0';
                    instructionStream >> programA;
                    instructionStream >> programB;
                    assert(programA >= 'a' && programA <= 'a' + numPrograms);
                    assert(programB >= 'a' && programB <= 'a' + numPrograms);
                    const auto programAIter = std::find(programLineup.begin(), programLineup.end(), programA);
                    assert(programAIter != programLineup.end());
                    const auto programBIter = std::find(programLineup.begin(), programLineup.end(), programB);
                    assert(programBIter != programLineup.end());
                    iter_swap(programAIter, programBIter);
                }
                break;
        }
    }
    return programLineup;
}

int main()
{
    constexpr int numPrograms = 16;
    //constexpr int numPrograms = 5;

    vector<char> initialProgramLineup;
    for (int i = 0; i < numPrograms; i++)
        initialProgramLineup.push_back('a' + i);

    string danceInstructions;
    getline(cin, danceInstructions);

#if 0
    const auto programLineupAfterDance = ::programLineupAfterDance(initialProgramLineup, danceInstructions);

    vector<int> dancePermutation;
    for (int i = 0; i < numPrograms; i++)
    {
        dancePermutation.push_back(std::find(programLineupAfterDance.begin(), programLineupAfterDance.end(), static_cast<char>('a' + i)) - programLineupAfterDance.begin());
        std::cout << "program: " << i << " mapped to: " << dancePermutation[i] << " after dance" << std::endl;
    }
    vector<int> dancePeriods;
    for (int i = 0; i < numPrograms; i++)
    {
        int period = 0;
        int pos = i;
        do
        {
            period++;
            pos = dancePermutation[pos];
        } while (pos != i);
        std::cout << "program: " << i << " has period: " << period << std::endl;
        dancePeriods.push_back(period);
    }
#endif

    constexpr int64_t numDances = 1'000'000'000LL; // TODO - reinstate this!
    //constexpr int64_t numDances = 1001;

    map<vector<char>, int> lastSeenAtCount;
    vector<char> programLineup = initialProgramLineup;
    lastSeenAtCount[initialProgramLineup] = 0;
    int period = -1;
    int count = 0;
    while (true)
    {
        programLineup = ::programLineupAfterDance(programLineup, danceInstructions);
        count++;
        if (lastSeenAtCount.contains(programLineup))
        {
            std::cout << "repeat!" << count << " prev seen at: " << lastSeenAtCount[programLineup] << std::endl;
            assert(lastSeenAtCount[programLineup] == 0); // Seems to be the case, and I can't be bothered to handle other cases XD
            period = count;
            break;
        }
        lastSeenAtCount[programLineup] = count;
    }
    const int64_t reducedNumDances = numDances % period;

    programLineup = initialProgramLineup;
    for (int count = 1; count <= reducedNumDances; count++)
    {
        programLineup = ::programLineupAfterDance(programLineup, danceInstructions);
    }
    std::cout << "programLineup: " << std::endl;
    for (const auto x : programLineup) cout << x;
    std::cout << std::endl;

    if (false)
    {
        auto bruteForceLineup = initialProgramLineup;
        for (int count = 1; count <= numDances; count++)
        {
            bruteForceLineup = ::programLineupAfterDance(bruteForceLineup, danceInstructions);
        }
        std::cout << "bruteForceLineup: " << std::endl;
        for (const auto x : bruteForceLineup) cout << x;
        std::cout << std::endl;
    }

#if 0
    vector<char> lineup(numPrograms);
    for (int i = 0; i < numPrograms; i++)
    {
        const int reducedNumDances = numDances % dancePeriods[i];
        std::cout << "program: " << i << " reducedNumDances: " << reducedNumDances << std::endl;
        int pos = i;
        for (int count = 0; count < reducedNumDances; count++)
        {
            pos = dancePermutation[pos];
        }
        assert(lineup[pos] == '\0');
        lineup[pos] = 'a' + i;
    }
    std::cout << "lineup: " << std::endl;
    for (const auto x : lineup) cout << x;
    std::cout << std::endl;
#endif


    return 0;
}
