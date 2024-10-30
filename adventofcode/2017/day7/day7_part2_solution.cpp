#include <iostream>
#include <regex>
#include <sstream>
#include <map>
#include <algorithm>

#include <cassert>

using namespace std;

struct Program
{
    string name;
    int weight = -1;
    vector<Program*> supportedPrograms;
    vector<string> supportedProgramNames;
    int64_t totalSupportedWeight = -1;
};

int64_t calcTotalSupportedWeight(Program* program, Program** deepestUnbalancedProgram)
{
    int64_t totalSupportedWeight = program->weight;
    for (auto* supportedProgram : program->supportedPrograms)
    {
        totalSupportedWeight += calcTotalSupportedWeight(supportedProgram, deepestUnbalancedProgram);
    }
    // Memo-ise.
    program->totalSupportedWeight = totalSupportedWeight;
    std::cout << "program " << program->name << " totalSupportedWeight: " << program->totalSupportedWeight << std::endl;
    std::cout << " breakdown: " << std::endl;
    // Unbalanced?
    if (!program->supportedPrograms.empty())
    {
        const int64_t weightPerProgram = program->supportedPrograms.front()->totalSupportedWeight;
        for (auto* supportedProgram : program->supportedPrograms)
        {
            std::cout << "  " << supportedProgram->name << " " << supportedProgram->totalSupportedWeight << std::endl;
            if (supportedProgram->totalSupportedWeight != weightPerProgram)
            {
                std::cout << "program: " << program->name << " has unbalanced supported weights!" << std::endl;
                if (*deepestUnbalancedProgram == nullptr)
                {
                    std::cout << " deepest unbalanced program is: " << program->name << std::endl;
                    *deepestUnbalancedProgram = program;
                }
            }
        }
    }
    return totalSupportedWeight;
}

int main()
{
    std::regex listRegex(R"(^(\w+)\s*\((\d+)\)\s*(-> (.*))?$)");
    string listLine;
    map<string, Program> programForName;
    while (getline(cin, listLine))
    {
        std::smatch listMatch;
        const bool matchSuccessful = std::regex_match(listLine, listMatch, listRegex);
        assert(matchSuccessful);
        Program program;
        program.name = listMatch[1];
        std::cout << "name: " << program.name << std::endl;
        program.weight = std::stoi(listMatch[2]);
        std::string supportedProgramDesc = listMatch[4];
        supportedProgramDesc.erase(std::remove(supportedProgramDesc.begin(), supportedProgramDesc.end(), ','), supportedProgramDesc.end());
        std::cout << "supportedProgramDesc: " << supportedProgramDesc << std::endl;
        istringstream supportedProgramsStream(supportedProgramDesc);
        string supportedProgramName;
        while (supportedProgramsStream >> supportedProgramName)
            program.supportedProgramNames.push_back(supportedProgramName);

        programForName[program.name] = program;
    }
    for(auto& [programName, program] : programForName)
    {
        for (const auto& supportedProgramName : program.supportedProgramNames)
        {
            assert(programForName.contains(supportedProgramName));
            program.supportedPrograms.push_back(&programForName[supportedProgramName]);
        }
    }

    Program* bottomProgram = nullptr;
    for(auto& [programName, program] : programForName)
    {
        bool isSupported = false;
        for(auto& [otherProgramName, otherProgram] : programForName)
        {
            if (otherProgramName == programName)
                continue;

            if (std::find(otherProgram.supportedPrograms.begin(), otherProgram.supportedPrograms.end(), &program) != otherProgram.supportedPrograms.end())
                isSupported = true;
        }
        if (!isSupported)
        {
            assert(bottomProgram == nullptr);
            bottomProgram = &program;
        }
    }
    assert(bottomProgram != nullptr);
    std::cout << "bottomProgramName: " << bottomProgram->name << std::endl;
    Program *deepestUnbalancedProgram = nullptr;
    calcTotalSupportedWeight(bottomProgram, &deepestUnbalancedProgram);
    assert(deepestUnbalancedProgram != nullptr);

    const auto supportedPrograms = deepestUnbalancedProgram->supportedPrograms;
    const int numSupportedPrograms = static_cast<int>(supportedPrograms.size());
    Program *programToAdjust = nullptr;
    for (auto* supportedProgram : supportedPrograms)
    {
        int numDifferentTo = 0;
        for (const auto* otherSupportedProgram : supportedPrograms)
        {
            if (otherSupportedProgram->totalSupportedWeight != supportedProgram->totalSupportedWeight)
                numDifferentTo++;
        }
        if (numDifferentTo == numSupportedPrograms - 1)
        {
            assert(programToAdjust == nullptr);
            programToAdjust = supportedProgram;
        }
    }
    assert(programToAdjust != nullptr);
    std::cout << "Need to adjust " << programToAdjust->name << std::endl;
    auto desiredTotalWeight = (*std::find_if(supportedPrograms.begin(), supportedPrograms.end(), [programToAdjust](const auto* program) { return program != programToAdjust;}))->totalSupportedWeight;
    std::cout << "Should be of total weight: " << desiredTotalWeight << " but is: " << programToAdjust->totalSupportedWeight << std::endl;
    assert(desiredTotalWeight != programToAdjust->totalSupportedWeight);
    programToAdjust->weight += (desiredTotalWeight - programToAdjust->totalSupportedWeight);

    // Verify that we are now balanced.
    for(auto& [programName, program] : programForName)
    {
        program.totalSupportedWeight = -1;
    }
    deepestUnbalancedProgram = nullptr;
    calcTotalSupportedWeight(bottomProgram, &deepestUnbalancedProgram);
    assert(deepestUnbalancedProgram == nullptr);

    std::cout << "result: " << programToAdjust->weight << std::endl;

    return 0;
}
